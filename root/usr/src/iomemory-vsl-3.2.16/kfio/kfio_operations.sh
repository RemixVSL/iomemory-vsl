#!/bin/bash -e
#
# Replaced 1379 objects in x86_64_latest_libkfio.o_shipped from 3531
#
# ERROR: modpost: "g_fusion_info_root" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "gv_lock_indirect_read" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "g_config_table_count" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "gv_init_pci_counter" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "g_config_table" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "iodrive_pci_attach_post" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "gv_fio_device_list" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "g_fusion_info_iodrive" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "iodrive_pci_attach_setup" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!
# ERROR: modpost: "gv_fusion_info_fio" [/home/snuf/Documents/Sources/fusion/iomemory-vsl/root/usr/src/iomemory-vsl-3.2.16/iomemory-vsl.ko] undefined!

usage() {
    echo "${0##*/}:
  -s <source object library file>
  -d <destination object library file>
  -O flag: Patch Obj file with hashes
  -h flag: this help
  -D flag: Debug, set -x
"
}

DEBUG=0
OBJ_PATCH=0
SRC_KFIO_LIB=x86_64_libkfio.o_shipped
DST_KFIO_LIB=x86_64_latest_libkfio.o_shipped
while getopts ":s:d:hDO" opt; do
    case ${opt} in
      s )
        SRC_KFIO_LIB=$OPTARG
        ;;
      d )
        DST_KFIO_LIB=$OPTARG
        ;;
      O )
        OBJ_PATCH=1
        ;;
      D )
        DEBUG=1
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

##
# x86_64_cc63_libkfio.o_shipped is the last OG that came with the driver
##
patch_objs() {
    local src_obj_file=$1; shift
    local dst_obj_file=$1
    local TR_COUNT=0
    if [ -f "$src_obj_file" -a "$src_obj_file" != "$dst_obj_file" ]; then
      cp $src_obj_file $dst_obj_file
      obj_file=$dst_obj_file
    else
      echo "Missing source or source same as destination?"
      exit 1
    fi
    function_hash_file=$(dirname $0)/function_hashes.txt
    functions_in_obj=$(objdump -trS $obj_file | perl -ne 'if (m/(ifio_.*.3.2.16.1731|ifio_.*.4.3.7.1205)/) { print $1."\n" }' | uniq)
    function_version=$(objdump -trS x86_64_libkfio.o_shipped  | head -20 | grep ifio | head -1 | perl -ne 'if (m/ifio_.*.(3.2.16.1731|4.3.7.1205)/) { print $1."\n" }')
    # 00122a840b5d7efe1beb3dd8908ae37fe03eac74:lrbtree_externalize
    # hash should become:
    # ifio_00122_a840b5d7efe1beb3dd8908ae37fe03eac74_3_2_16_1731
    # objcopy --redefine-sym ifio_58422......fef60.3.2.16.1731=fio_device_get_write_retard_reason
    if [ -f "$function_hash_file" ]; then
      FH_COUNT=$(cat $function_hash_file | wc -l | awk '{ print $1 }')
      FH=$(cat $function_hash_file)
      for obj_line in $FH; do
        obj_hash=${obj_line//:*}
        obj_name=${obj_line//*:}
        front_hash=${obj_hash:0:5}
        rest_hash=${obj_hash:5}
        # Make ver dynamic later?
        obj_id="ifio_${front_hash}.${rest_hash}.${function_version}"
        set +e
        res=$(echo $functions_in_obj | grep $obj_id)
        set -e
        if [ "$res" != "" ]; then
          echo "Found $obj_id for $obj_line in $dst_obj_file"
          if [ "$DEBUG" == "0" ]; then
            objcopy --redefine-sym $obj_id=$obj_name $obj_file
          else
            echo objcopy --redefine-sym $obj_id=$obj_name $obj_file
          fi
          TR_COUNT=$((TR_COUNT + 1))
        else
          echo "Skipping $obj_id for $obj_line not in $dst_obj_file"
        fi
        # x86_64_cc53_libkfio.o_converted
      done
      echo "Replaced $TR_COUNT objects in $dst_obj_file from $FH_COUNT"
    else
      echo "No function hash replacement file found: $function_hashes"
      exit 1
    fi
}

if [ "$OBJ_PATCH" == "0" ]; then
    usage
    exit 1
elif [ "$OBJ_PATCH" == "1" ]; then
    echo "Patching functions from $SRC_KFIO_LIB to $DST_KFIO_LIB"
    patch_objs $SRC_KFIO_LIB $DST_KFIO_LIB
fi
