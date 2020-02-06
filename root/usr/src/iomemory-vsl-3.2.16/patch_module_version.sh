#!/bin/bash
#
# Script to patch the iomemory-vsl.ko binary. It removes the username
# and appends the first seven characters of the commit hash.
#
set -e

module=$1
sumtag=$2

if [ "$tag" == "" ]; then
    echo "patch_version not found, attempting to use git"
    tag=$(git rev-parse --short HEAD)
fi
if [ "$tag" == "" ]; then
    echo "Error: git version tag missing, unable to tag $module"
    exit 1
fi
maxlength=$(perl -ne 'print length($1)."\n" if /(\d.\d.\d+.\d+ \w+@[\w\d]+)/' $module | head -1)
newlength=$(perl -sne 'print length("$2 $4 $tag a")."\n" if /((\d.\d.\d+.\d+) (\w+@([\w\d]+)))/' -- -tag=$tag $module | head -1)
if [ "$maxlength" == "$newlength" -a "$maxlength" == "34" ]; then
    replace=$(perl -sne 'print "s/$1/$2 $4 $tag a/\n" if /((\d.\d.\d+.\d+) (\w+@([\w\d]+)))/' -- -tag=$tag $module | head -1)
    sed -i "$replace" $module
    echo "Ok: altered version for $module appended $tag"
else
    echo "Error: altering $module, $maxlength is not $newlength"
    exit 1
fi
