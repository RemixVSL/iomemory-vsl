#!/bin/bash
#
# Script to patch the iomemory-vsl.ko binary. It removes the username
# and appends the first seven characters of the commit hash.
#

module=$1
tag=$2

if [ "$tag" == "" ]; then
    echo "patch_version not found, attempting to use git"
    tag=$(git rev-parse --short HEAD)
fi
if [ "$tag" == "" ]; then
    echo "Error: git version tag missing, unable to tag $module"
    exit 1
fi

if [ -e $module ]; then
    maxlength=$(perl -ne 'print length($1)."\n" if /(\d.\d.\d+.\d+ \w+@[\w\d]+)/' $module | head -1)
    newlength=$(perl -sne 'print length("$2 $4 $tag a")."\n" if /((\d.\d.\d+.\d+) (\w+@([\w\d]+)))/' -- -tag=$tag $module | head -1)
    if [ "$maxlength" == "$newlength" -a "$maxlength" == "34" ]; then
        replace=$(perl -sne 'print "s/$1/$2 $4 $tag a/\n" if /((\d.\d.\d+.\d+) (\w+@([\w\d]+)))/' -- -tag=$tag $module | head -1)
        sed -i "$replace" $module
        echo "Ok: altered version for $module appended $tag"
    elif [ "$maxlength" == "$newlength" -a "$maxlength" == "" ]; then
        echo "Error: altering $module, module already altered it seems, string lengths don't make sense."
        exit 1
    else
        echo "Error: altering $module, $maxlength is not $newlength"
        exit 1
    fi
else
    src=license.c
    echo "Adding module version $tag to source $src"
    grep -q MODULE_VERSION $src
    if [ "$?" == "0" ]; then
        replace=$(perl -sne 'print "s/$2/$tag/\n" if /(MODULE_VERSION)\(\"([\d\w]+)\"\)/' -- -tag=$tag $src | head -1)
        sed -i "$replace" $src
    else
        echo "MODULE_VERSION(\""$tag"\");" >> $src
    fi
    exit 1
fi 
