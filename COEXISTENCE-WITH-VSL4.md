# Running this driver alongside iomemory-vsl4

Some machines carry both an ioDrive2-generation card (this driver, VSL3) and an
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

Nothing on the card is involved and no firmware is touched. Both generations
enumerate their own cards from zero and register them under the same names, so
the second driver to load asks for a name the first one already has:

| what | first card is always |
|------|----------------------|
| procfs directory | `/proc/fusion/fct0` |
| control device | `/dev/fct0` |
| block device | `/dev/fioa` |

Load order only decides which card loses. `fio-status` states the same limit
from the userspace side: *"only one driver package can be installed at a time."*

## What this driver does about it

The namespace is shared, so this driver treats it as shared: it keeps the stock
names and takes a free range of device numbers instead of insisting on starting
at zero. With an ioMemory card already on `fct0`, an ioDrive2 Duo comes up as
`fct1`/`fct2` and `fiob`/`fioc`, inside the same `/proc/fusion` directory.

Nothing is renamed, so nothing downstream has to be taught a new name — udev
rules, `fio-status`, `fio-format` and anything that refers to a card by
`/dev/fio*` all keep working as shipped.

The device numbers are generated inside the prebuilt driver object, which
formats the names from them and hands the finished names to the porting layer.
`kenum.c` is the one place that adds a base to the number inside such a name,
at the three points where the porting layer receives one:

| name | handed to | file |
|------|-----------|------|
| `fct0` (procfs) | `kfio_info_os_create_node()` | `kinfo.c` |
| `fct0` (control device) | `fusion_create_control_device()` | `cdev.c` |
| `fioa` (block device) | `kfio_expose_disk()` | `kblock.c` |

It only rewrites a name it can first reproduce from the number the driver
object supplied alongside it. A name that does not match is passed through
untouched, so an unrecognised naming scheme costs coexistence rather than
producing a wrong device name.

The top level directory is shared the same way. Only the first driver to load
can create `/proc/fusion`; the second finds it already there and hangs its
entries off it by path, because procfs offers no way to get a handle on a
directory this module did not create. A directory that was only borrowed is
left behind on unload for the driver that owns it.

## fio_dev_index_base

```
parm: fio_dev_index_base:First device number to use, for sharing the ioMemory
      namespace with another generation of the driver. -1 detects the first free
      number at load time, 0 always enumerates from fct0. (int)
```

The default, `-1`, looks at `/proc/fusion` while the driver sets up its own top
level directory and before any of its cards have registered, so whatever is
numbered there belongs to another driver. It takes the first free number.

`0` is the behaviour of a driver built without this file: always start at
`fct0`. Set it if you want the stock numbering back on a machine where the
detection would otherwise move the cards.

Whichever it resolves to, the driver says so in the log rather than leaving it
to be inferred:

```
fioinf sharing /proc/fusion with an already loaded ioMemory driver; enumerating this driver's devices from fct1
```

On a machine with only one generation of card nothing is detected, nothing is
renumbered, and nothing is printed.

## Load order

Detection happens once, when this driver loads. It can only see cards that have
already registered, so load `iomemory-vsl4` first and let it settle:

```sh
modprobe iomemory-vsl4
udevadm settle
modprobe iomemory-vsl
```

That is also the safe order: if a number were missed, this driver's probe fails
on the existing `fct0` and the already-attached card is unaffected.

The other order does not work by itself, because `iomemory-vsl4` has no matching
change yet and will still ask for `fct0`. `fio_dev_index_base=<n>` forces a
starting number for a machine that has to load them the other way round, and
the same change in `iomemory-vsl4` would make the order stop mattering.

Unload in the reverse order. The driver that created `/proc/fusion` removes it,
so unloading it while the other is still using the directory would take its
entries with it.

## Userspace tools

Unchanged, and there is nothing to install twice. The names are the stock ones,
so the VSL3 utilities enumerate `/dev/fct*` and find their own cards wherever in
the range they landed.

Both generations' control devices are visible to both tool sets, as they are on
any machine that has seen both; the v3 utilities report an ioMemory card as an
unmanaged device requiring a v4 driver, and vice versa.

## What has been verified

The three rename points are the complete set: an earlier version of this change
separated exactly these names and ran an ioDrive2 Duo 2.41TB and an ioMemory
HHHL card on the same host under 6.12.0, both drivers loaded, both halves of the
Duo attached, with the ioMemory card carrying a live filesystem throughout.

What that version proved is which names have to differ, and that changing them
is sufficient. This version reaches the same end state from the shim instead of
from a patched object, and has not itself been run on that hardware yet. The
parts that want checking on the rig, rather than reasoning about, are the
borrowed `/proc/fusion` directory, the unload path, and what `fio-status` makes
of a control device belonging to the other generation.
