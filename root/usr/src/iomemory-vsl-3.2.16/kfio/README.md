# kfio object libs
**!!! NON USER SERVICABLE PARTS INSIDE!!!**

**If you don't know what this is you probably shouldn't poke around here.**

This directory contains object libraries that are proprietary and fill the
"gaps" in the open source part of the driver code that handles the NAND,
LEB-MAP, FPGA, and other operations on the cards.

# Goal
The goal of de-mangling the object files is to make sure the driver can be
supported longer term, and make troubleshooting issues with cards more simple
and transparent, hopefully saving cards from the bin.

## orig vs latest vs numbers
The `x86_64_cc##_libkfio.o_shipped` names are based on the `gcc` version used
for linking on the host that compiles the driver, they originally accompanied
the driver. Today we look at which cc version someone has, and copy
the `x86_64_libkfio.o_shipped` object file over to that cc version prior to
compiling the driver. The shipped version is/*should be* a copy from
`x86_64_cc63_libkfio.o_shipped`.

## procedure
The `function_hash.txt` file contains a hash to function mapping, this mapping
is used to translate obscure function names in the object files to humanly
understandable names, allowing for easier debugging and code de-mangling.
`kfio_operations.sh` contains a function called `patch_objs` which gets called
with `-O`. This function takes the `x86_64_libkfio.o_shipped` and copies
it. After copying the file it will munge the contents of `function_hashes.txt`
and rename the functions in the copied file. By default the output file is
`x86_64_latest_libkfio.o_shipped`, which is used when `make`-`ing` the module.

## src
The `src` directory contains a de-compiled version of the library in various
stages.
