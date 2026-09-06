# Running this driver alongside iomemory-vsl4

Some machines have both an ioDrive2-generation card (this driver, VSL3) and an
ioMemory/ioScale3-generation card (`iomemory-vsl4`). The two drivers bind
different PCI IDs, so they look like they ought to coexist. Loading both leaves
one card unusable:

```
proc_dir_entry 'fusion/fct0' already registered
fioinf <card> 0000:04:00.0: Failed to setup proc.
fioerr <card> 0000:04:00.0: 12314 Device is unavailable.
fioerr ioDrive 0000:04:00.0: 12295 Driver start failed with error -5: I/O error
iomemory-vsl4 0000:04:00.0: probe with driver iomemory-vsl4 failed with error -5
```

Both drivers create the same procfs root (`/proc/fusion`) and both number their
own devices from zero, so whichever loads second collides on `fct0` and its card
fails to attach. Load order only changes which card loses. `fio-status` states
the same limitation from the userspace side: *"only one driver package can be
installed at a time."*

Nothing on the card is involved, and no firmware is touched. The conflict is
purely host-side naming.

## What has to change

Three identifier namespaces are shared and must be separated:

| what | stock | example rename |
|------|-------|----------------|
| procfs root | `fusion` | `fusio3` |
| control device | `fct%d` | `fc3%d` |
| block device | `fio%c` | `fi3%c` |

The module names already differ (`iomemory_vsl` vs `iomemory_vsl4`), so nothing
is needed there.

The strings are not in the GPL sources. `UFIO_KINFO_ROOT`,
`UFIO_CONTROL_DEVICE_PREFIX` and `UFIO_BLOCK_DEVICE_PREFIX` in
`include/fio/port/common-linux/` have no usage sites at all. The names live in
the prebuilt `kfio/*_libkfio.o_shipped` object, which generates them and passes
them out through `kfio_expose_disk()` and the misc-device registration.

## tools/rename-namespace.py

`tools/rename-namespace.py` rewrites those strings in your own local copy of the
shipped object. Every replacement is the same length as the original, so no
relocation, symbol or section moves. The patched object differs from the input
only in the handful of bytes that spell the names.

```sh
python3 tools/rename-namespace.py --dry-run \
    root/usr/src/iomemory-vsl-3.2.16/kfio/x86_64_cc63_libkfio.o_shipped \
    root/usr/src/iomemory-vsl-3.2.16/kfio/x86_64_latest_libkfio.o_shipped
```

Drop `--dry-run` to write. It refuses to touch anything unless every expected
string is found exactly the expected number of times, inside a `SHF_STRINGS`
section, on a NUL boundary. Afterwards it re-reads the result and asserts that
the section table, `.text`, `.rodata`, `.data`, `.symtab`, `.strtab` and every
`.rela.*` section are byte-identical to the input.

Patch `x86_64_latest_libkfio.o_shipped` specifically. `TARGET` in the Makefile is
`x86_64_cc$(CCMAJOR)$(CCMINOR)`, so on any recent compiler there is no matching
`kfio/x86_64_ccNN_libkfio.o` and `check_n_fix_kfio_ccver` copies
`x86_64_latest_libkfio.o_shipped` into place. For the same reason, remove any
stale non-`_shipped` `kfio/x86_64_cc*_libkfio.o` first, or the copy is skipped
and the unpatched object is used.

The section flags argue against doing this. `.rodata.str1.1` is marked `SHF_MERGE|SHF_STRINGS`, which normally implies the
linker may have merged literals so that one string is a suffix of another;
rewriting bytes in place would then corrupt something unrelated. In this object
the merge never happened. It is an `ld -r` of many translation units that were
left unmerged (74 separate copies of `fct??` survive), and auditing every
NUL-terminated string against every relocation shows each one referenced only at
its first byte, with no interior references.

## Source-side changes

The changes in the tree are small and independent of the object:

* `kblock.c`, the `register_blkdev()`/`unregister_blkdev()` name.
* `tools/udev/rules.d/60-persistent-fio.rules`, the `KERNEL==` matches plus the
  `disk/by-id/` symlink prefix, so it cannot collide with the other driver's.
* `include/fio/port/common-linux/{ufio.h,commontypes.h}` and `kscsi_host.c`,
  kept consistent with the new names even though they are not compiled in on a
  default build.

If you use the by-partlabel/by-partuuid udev rules that many deployments add for
iomemory partitions, extend their `KERNEL==` patterns too. udev skips these
devices by default, so a missed pattern silently leaves partitions without
symlinks.

## Result

Tested on 6.12.0 with an ioDrive2 Duo 2.41TB beside an ioMemory HHHL card, both
drivers loaded at once, the ioMemory card carrying a live filesystem throughout:

```
iomemory_vsl         1302528  0
iomemory_vsl4        1302528  27

/proc/fusion  -> fct0, vsl4
/proc/fusio3  -> fc30, fc31, fio, iodrive, version

/dev/fct0  /dev/fioa1..4     (vsl4 card, untouched)
/dev/fc30  /dev/fc31         (Duo control devices)
/dev/fi3a  /dev/fi3b         (Duo block devices, major 251 vs vsl4's 253)

fioinf Fusion-io ioDrive2 Duo 2.41TB 0000:0a:00.0: Attach succeeded.
fioinf Fusion-io ioDrive2 Duo 2.41TB 0000:0b:00.0: Attach succeeded.
```

Load the renamed driver while `iomemory_vsl4` is already up. That order is the
safe one: if a name were missed, the new driver's probe fails on the existing
`fct0` and the already-attached card is unaffected.

## Userspace tools

The VSL3 utilities hardcode the same paths, so a renamed driver is invisible to
them until they are given matching names. `fio-status` reports the cards as
"unmanaged ioMemory devices requiring a v3.x driver" even while they are attached
and working. The strings are again same-length replaceable:

```
/dev/fct                              /dev/fct%u              /dev/fio%c
/proc/fusion/fct%u/channels/0/client  /proc/fusion/fio/%s/data_device
```

`tools/rename-userspace-tools.py` applies them with the same guarantees as the
driver-side script: identical lengths, nothing in the ELF moves, and it refuses
to touch a binary where a string is ambiguous.

```sh
python3 tools/rename-userspace-tools.py --dry-run /usr/bin/fio-*
```

Only `fio-status` carries all five (6 bytes change); the other utilities have a
single `/dev/fct` (1 byte). The `/proc/<root>/fio/` subdirectory keeps its name,
so only the root changes in that path.

The script rewrites your own installed copies rather than shipping patched
binaries. Install the renamed set in its own directory, alongside the vsl4 tools
rather than replacing them, and both generations' tool sets live on the same
machine, each seeing its own cards.

That last part is what makes the card usable in practice: `fio-status` then
reports the Duo's temperature, reserves and endurance, and `fio-format` can set
the sector size, which otherwise stays at whatever the card shipped with.
