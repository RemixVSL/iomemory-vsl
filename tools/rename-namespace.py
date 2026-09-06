#!/usr/bin/env python3
"""rename-namespace.py -- same-length, content-matched rename of the device and procfs
name format strings inside the prebuilt libkfio.o object of iomemory-vsl 3.2.16, so
this driver can be loaded alongside iomemory-vsl4 (which owns /proc/fusion, /dev/fctN
and /dev/fioN). See COEXISTENCE-WITH-VSL4.md.

  usage: rename-namespace.py SRC.o_shipped DST.o_shipped [--with-fio-dir] [--dry-run]

No third-party modules: parses the ELF64 section headers itself.
Refuses to write unless every expected string is found exactly the expected number
of times, inside a SHF_STRINGS section, on a NUL boundary, and every replacement has
the identical length.  After patching it re-reads DST and proves that the section
table, .symtab, .strtab and every .rela.* section are byte-identical to SRC and that
the only differing bytes are the intended ones.
"""
import struct, sys, hashlib

# exact NUL-delimited string -> same-length replacement, expected occurrence count
RENAMES = [
    (b'fusion',     b'fusio3',     1),   # /proc root         (fusion_register_driver -> kfio_info_create_dir(NULL,..))
    (b'fct%d',      b'fc3%d',      2),   # control device     (iodrive_init_start: nand_dev+0x110; iodrive_pci_attach: numa override key)
    (b'fio%c',      b'fi3%c',      1),   # block device a..z  (fio_attach)
    (b'fio%c%c',    b'fi3%c%c',    1),   # block device aa..  (fio_attach)
    (b'fio%c%c%c',  b'fi3%c%c%c',  1),   # block device aaa.. (fio_attach)
]
OPTIONAL = [
    (b'fio',        b'fi3',        1),   # /proc/<root>/fio subdirectory (fio_blk_info_init) -- cosmetic, under the renamed root anyway
]
SHF_STRINGS = 0x20
SHF_MERGE   = 0x10

def sections(d):
    assert d[:4] == b'\x7fELF' and d[4] == 2 and d[5] == 1, "not ELF64 LE"
    e_shoff, = struct.unpack_from('<Q', d, 0x28)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from('<HHH', d, 0x3a)
    secs = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        name, typ, flags, addr, offset, size, link, info, align, entsize = struct.unpack_from('<IIQQQQIIQQ', d, off)
        secs.append(dict(idx=i, name_off=name, type=typ, flags=flags, offset=offset, size=size, entsize=entsize, hdr=d[off:off + e_shentsize]))
    sh = secs[e_shstrndx]
    for s in secs:
        n = d[sh['offset'] + s['name_off']:]
        s['name'] = n[:n.find(b'\x00')].decode()
    return secs

def find_all(d, secs, s):
    """exact NUL-delimited occurrences inside SHF_STRINGS sections"""
    hits = []
    for sec in secs:
        if sec['type'] != 1 or not (sec['flags'] & SHF_STRINGS):
            continue
        lo, hi = sec['offset'], sec['offset'] + sec['size']
        p = lo
        while True:
            p = d.find(s + b'\x00', p, hi)
            if p < 0: break
            if p == lo or d[p - 1] == 0:
                hits.append((sec, p))
            p += 1
    return hits

def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    flags = [a for a in sys.argv[1:] if a.startswith('--')]
    if len(args) != 2:
        print(__doc__); sys.exit(2)
    src, dst = args
    renames = RENAMES + (OPTIONAL if '--with-fio-dir' in flags else [])
    d = bytearray(open(src, 'rb').read())
    orig = bytes(d)
    secs = sections(orig)
    plan = []
    ok = True
    for old, new, want in renames:
        assert len(old) == len(new), (old, new)
        hits = find_all(orig, secs, old)
        status = 'OK' if len(hits) == want else 'MISMATCH'
        if status != 'OK': ok = False
        for sec, p in hits:
            print(f"  {status:8s} {old.decode():12s} -> {new.decode():12s} at fileoff {p:8d} = {sec['name']}+{p - sec['offset']} (flags={sec['flags']:#x} entsize={sec['entsize']})")
            plan.append((p, old, new))
        if not hits:
            print(f"  MISMATCH {old.decode():12s} NOT FOUND (expected {want})")
    if not ok:
        print("refusing: occurrence counts differ from the analysed blob"); sys.exit(1)
    for p, old, new in plan:
        assert d[p:p + len(old)] == old
        d[p:p + len(new)] = new
    if '--dry-run' in flags:
        print("dry run, nothing written"); return
    open(dst, 'wb').write(d)

    # ---- verification ----
    nd = open(dst, 'rb').read()
    assert len(nd) == len(orig), "size changed"
    nsecs = sections(nd)
    assert [s['hdr'] for s in nsecs] == [s['hdr'] for s in secs], "section table changed"
    for s in secs:
        if s['name'] in ('.symtab', '.strtab', '.shstrtab') or s['name'].startswith('.rela') or s['name'] in ('.text', '.text.unlikely', '.data', '.rodata'):
            assert nd[s['offset']:s['offset'] + s['size']] == orig[s['offset']:s['offset'] + s['size']], f"{s['name']} changed"
    diff = [i for i in range(len(orig)) if orig[i] != nd[i]]
    intended = set()
    for p, old, new in plan:
        for k in range(len(old)):
            if old[k] != new[k]: intended.add(p + k)
    assert set(diff) == intended, f"unexpected byte differences: {sorted(set(diff) ^ intended)[:20]}"
    print(f"verified: {len(diff)} bytes differ, all intended; section table, .text, .rodata, .data, .symtab, .strtab and every .rela.* are byte-identical")
    print(f"sha256 src {hashlib.sha256(orig).hexdigest()}")
    print(f"sha256 dst {hashlib.sha256(nd).hexdigest()}")

if __name__ == '__main__':
    main()
