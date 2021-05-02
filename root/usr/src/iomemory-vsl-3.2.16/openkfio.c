#include <fio/port/openkfio.h>

void of_kfio_info_remove_node_fio(void)
{
  if (gv_fusion_info_fio != 0) 
    kfio_info_remove_node(&gv_fusion_info_fio);
}

void of_cleanup_fio_blk(void)
{
  fio_detach_devices();
  of_kfio_info_remove_node_fio();
}
