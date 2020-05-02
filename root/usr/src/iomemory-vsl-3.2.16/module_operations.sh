#!/bin/bash
#
# Script to patch the iomemory-vsl.ko binary. It removes the username
# and appends the first seven characters of the commit hash.
#
set -e

dkms_ver() {
    name=$1
    ver=$(dkms status | grep "$name " | grep -v added | cut -d, -f2 | sed -e s/\ //)
    if [ "$?" != "0" ]; then
        echo "DKMS problem"
        exit 1
    fi
    echo $ver
}

dkms_remove() {
    name=$1
    ver=$2
    echo "Removing $name/$ver from DKMS"
    dkms remove $name/$ver --all
}

dkms_install() {
    name=$1
    ver=$2
    echo "Adding, buidling and installing $name/$ver with DKMS"

    if [ "$(dkms status | grep $name | grep added)" == "" ]; then
      dkms add $name/$ver
    fi
    dkms build $name/$ver
    dkms install $name/$ver --force
}

get_rel_ver() {
    version=$1
    release=$(git describe --tag)
    if [ "$version" == "" ]; then
        tag=$(git rev-parse --short HEAD)
    fi
    if [ "$version" == "" -a "$release" == "" ]; then
        echo "Error: not a release and no git tag, aborting."
        exit 1
    elif [ "$release" != "" ]; then
        echo $release
    elif [ "$version" != "" ]; then
        echo $version
    else
        echo "deadbeef"
    fi
}

check_root() {
    if [[ $EUID -ne 0 ]]; then
        echo "Must be root for DKMS Setup." 1>&2
        exit 1
      fi
}

patchFile() {
    fileToPatch=$1
    tag=$2
    mod_ver=$3

    if [ -f "$fileToPatch" ]; then
        origVer=$(perl -ne 'print $1."\n" if /(\d.\d.\d+.\d+ \w+@[\w\d]+)/' $fileToPatch | head -1)
        if [ ${#origVer} -ne "0" ]; then
            newVer="$tag-${origVer%% *}"
            if [ "${#newVer}" -gt "${#origVer}" ]; then
                tag=$(git rev-parse --short HEAD)
                echo "${#newVer} longer than ${#origVer}, using $tag as version"
                if [ "${#tag}" -gt "${#origVer}" ]; then
                    echo "Unable to revert to $tag for version, longer than $origVer"
                    exit 2
                fi
                newVer=$tag
            fi
            padLength=$((${#origVer}-${#newVer}))
            # 4.3.7.1205 bryce@a7e621e6fb50/
            # v5.6.0-4.3.7.1205            /
            # 85c2e7e-4.3.7.1205           /
            paddedLength=$(perl -se 'print length("$tag"." " x $padd)' -- -tag=$newVer -padd=$padLength)
            replace=$(perl -sne 'print "s/$1/$tag"." " x $padd."/\n" if /((\d.\d.\d+.\d+) (\w+@([\w\d]+)))/' -- -tag=$newVer -padd=$padLength $fileToPatch | head -1)
            if [ "$origVer" == "$newVer" ]; then
                echo "Error: altering $fileToPatch, file already altered it seems, string lengths don't make sense."
                exit 3
            elif [ "${#origVer}" == "${paddedLength}" ]; then
                sed -i "$replace" $fileToPatch
                echo "Ok: patched version for $fileToPatch replaced $origVer with $newVer"
            else
                echo "Error: altering $fileToPatch, ${#origVer} is not ${#newVer}"
                exit 4
            fi
        else
            newVer="$tag-${mod_ver}"
            origVer=$(perl -sne 'print $1."\n" if /($tag)/' -- -tag=$newVer $fileToPatch | head -1)
            if [ "$origVer" == "$newVer" ]; then
                echo "Ok: $fileTAlready already patched with $newVer"
            else
                echo "Warning:  $fileTAlready patched with $origVer not $newVer"
            fi
        fi
    else
        echo "Error; $fileToPatch does not exist!"
        exit 5
    fi
}

patchLicenseVersion() {
    tag=$1
    src=license.c
    echo "Adding module version $tag to source $src"
    set +e
    grep -q MODULE_VERSION $src
    if [ "$?" == "0" ]; then
        replace=$(perl -sne 'print "s/$2/$tag/\n" if /(MODULE_VERSION)\(\"([\d\w\._-]+)\"\)/' -- -tag=$tag $src | head -1)
        sed -i "$replace" $src
    else
        echo "#include \"linux/module.h\"" > $src
        echo "MODULE_LICENSE(\"GPL\");" >> $src
        echo "MODULE_VERSION(\""$tag"\");" >> $src
    fi
    set -e
}

sanity_check() {
    if [ ! -e "Makefile" ]; then
        echo "This script should run from the directory with the Makefile!"
        exit 6
    fi
}

install_libvsl() {
    target_dir="/usr/lib/fio"
    mkdir -p $target_dir
    src_dir=$(git rev-parse --show-toplevel)
    libvsl=$(find ${src_dir}/root/usr/lib/fio -type f| grep libvsl)
    cp $libvsl $target_dir
}

usage() {
    echo "${0##*/}:
  -n <module name>
  -v <version>
  -p flag: Patch module, files, license etc
  -d flag: Install module through DKMS
  -h flag: this help
  -D flag: Debug, set -x
"
}

PATCH=0
DKMS=0
while getopts ":n:v:pdhD" opt; do
    case ${opt} in
      n )
        MODULE_NAME=$OPTARG
        ;;
      v )
        VERSION=$OPTARG
        ;;
      p )
        PATCH=1
        ;;
      d )
        DKMS=1
        ;;
      D )
        set -x
        ;;
      h )
        usage
        exit 0
        ;;
      \? )
        usage
        exit 0
        ;;
    esac
done
shift $((OPTIND -1))

# No matter what ALWAYS tag the license File
sanity_check
MODULE_DIR=${PWD##*/}
MODULE_VER=${MODULE_DIR##*-}
MODULE_FILE=${MODULE_NAME}.ko
RELEASE_VER=$(get_rel_ver $VERSION)

if [ "$MODULE_NAME" == "" ]; then
    usage
    exit 1
fi

# where does this go?

# old behaviour
if [ "$DKMS" == "0" -a "$PATCH" == "0" ]; then
    if [ -f "$MODULE_FILE" ]; then
        patchFile $MODULE_FILE $RELEASE_VER
    else
        patchLicenseVersion ${RELEASE_VER}-${MODULE_VER}
    fi
elif [ "$DKMS" == "1" ]; then
    check_root
    DKMS_DIR="/usr/src/${MODULE_NAME}-${RELEASE_VER}"
    DKMS_VER=$(dkms_ver $MODULE_NAME)

    patchLicenseVersion ${RELEASE_VER}-${MODULE_VER}
    if [ "${DKMS_VER}" != "" ]; then
      dkms_remove $MODULE_NAME $DKMS_VER
    fi
    if [ -d "$DKMS_DIR" ]; then
        echo "$DKMS_DIR already exists."
    else
        cp -r ${PWD} $DKMS_DIR
    fi
    # For DKMS we patch all the library files prior to making/linking
    LIBS=$(ls -1 ${DKMS_DIR}/kfio)
    for LIB in $LIBS; do
        patchFile $DKMS_DIR/kfio/$LIB $RELEASE_VER $MODULE_VER
    done
    dkms_install $MODULE_NAME $RELEASE_VER
    install_libvsl $MODULE_NAME
fi
