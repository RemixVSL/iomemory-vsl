#!/usr/bin/env python3
"""rename-userspace-tools.py -- same-length, content-matched rename of the hardcoded
device and procfs paths inside the VSL3 userspace utilities (fio-status, fio-format,
fio-attach, ...), so they can talk to a driver whose names were renamed to coexist
with iomemory-vsl4. See COEXISTENCE-WITH-VSL4.md.

  usage: rename-userspace-tools.py [--dry-run] BINARY [BINARY ...]

Companion to tools/rename-namespace.py, which applies the same rename inside the
driver's libkfio.o. The two MUST agree:

    /proc/fusion -> /proc/fusio3      fct -> fc3      fio<letter> -> fi3<letter>

NOTE the `/proc/<root>/fio/` subdirectory is deliberately NOT renamed (the driver's
optional `fio` proc node is left alone), so '/proc/fusion/fio/...' becomes
'/proc/fusio3/fio/...' -- only the root changes in that path.

Every replacement is the same length as the original, so nothing in the ELF moves:
no relocations, no offsets, no section resizing. Each binary is rewritten in place
(original kept as <name>.orig) only if every string it does carry is unambiguous;
a binary with no target strings is left untouched.

These utilities are proprietary and are not redistributable, so this rewrites your
own installed copies rather than shipping patched ones. Installing the renamed set
in its own directory lets both generations' tools coexist on one machine.
"""
import sys, os, shutil, hashlib

# exact NUL-delimited string -> same-length replacement.
# count is per-binary and varies, so we only require "at least one of some rule
# matched" per file and that every rule that matches is unambiguous (<=1 hit).
RENAMES = [
    (b'/proc/fusion/fct%u/channels/0/client', b'/proc/fusio3/fc3%u/channels/0/client'),
    (b'/proc/fusion/fio/%s/data_device',      b'/proc/fusio3/fio/%s/data_device'),
    (b'/proc/fusion',                         b'/proc/fusio3'),
    (b'/dev/fct%u',                           b'/dev/fc3%u'),
    (b'/dev/fio%c',                           b'/dev/fi3%c'),
    (b'/dev/fct',                             b'/dev/fc3'),
]

def find_exact(d, s):
    """NUL-delimited exact occurrences (start must follow a NUL or file start)."""
    hits, p = [], 0
    while True:
        p = d.find(s + b'\x00', p)
        if p < 0:
            return hits
        if p == 0 or d[p - 1] == 0:
            hits.append(p)
        p += 1

def main():
    flags = [a for a in sys.argv[1:] if a.startswith('--')]
    files = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not files:
        print(__doc__); sys.exit(2)
    dry = '--dry-run' in flags
    rc = 0
    for path in files:
        d = bytearray(open(path, 'rb').read())
        orig = bytes(d)
        plan, bad = [], False
        for old, new in RENAMES:
            assert len(old) == len(new), (old, new)
            hits = find_exact(orig, old)
            # a longer rule already consumed this region? skip overlaps
            hits = [h for h in hits if not any(h >= p and h < p + len(o) for p, o, _ in plan)]
            if len(hits) > 1:
                print(f"  {path}: AMBIGUOUS {old!r} x{len(hits)} -- refusing"); bad = True
            for h in hits:
                plan.append((h, old, new))
        if bad:
            rc = 1; continue
        if not plan:
            print(f"  {path}: no target strings (nothing to do)"); continue
        for p, old, new in sorted(plan):
            print(f"  {os.path.basename(path):22s} {old.decode():38s} -> {new.decode():38s} @ {p}")
        if dry:
            continue
        shutil.copy2(path, path + '.orig')
        for p, old, new in plan:
            assert d[p:p + len(old)] == old
            d[p:p + len(new)] = new
        open(path, 'wb').write(d)
        nd = open(path, 'rb').read()
        assert len(nd) == len(orig), "size changed"
        diff = [i for i in range(len(orig)) if orig[i] != nd[i]]
        intended = set()
        for p, old, new in plan:
            for k in range(len(old)):
                if old[k] != new[k]:
                    intended.add(p + k)
        assert set(diff) == intended, "unexpected byte differences"
        print(f"  {os.path.basename(path):22s} OK: {len(diff)} bytes differ, all intended; "
              f"sha256 {hashlib.sha256(nd).hexdigest()[:16]}… (orig kept as .orig)")
    sys.exit(rc)

if __name__ == '__main__':
    main()
