#include <fio/port/openkfio.h>

void of_kfio_info_remove_node_fio(void)
{
  if (gv_fusion_info_fio != 0) 
    kfio_info_remove_node(&gv_fusion_info_fio);
}

uint32_t of_fio_detach_devices(void)
{
  uint32_t rc;
  const char *errtext;
  const char *errsym;
  const char *bus_name;
  fio_device *fio_dev;
  fio_device *fio_next;
  uint32_t cnt;

  cnt = 0;
  fio_dev = gv_fio_device_list;
  while (fio_dev != (fio_device *)&gv_fio_device_list) {
    fio_next = fio_dev->fio_next;
    rc = fio_detach(fio_dev,0);
    if (rc != 0) {
      cnt = cnt + 1;
      errtext = ifio_strerror(rc);
      errsym = ifio_strerror_sym(rc);
      bus_name = fio_device_get_bus_name(fio_dev);
      kfio_print("<3>fioerr %s: failed to detach: %s: %s\n",bus_name,errsym,errtext);
    }
    fio_dev = fio_next;
  }
  return cnt;
}

void of_cleanup_fio_blk(void)
{
  of_fio_detach_devices();
  of_kfio_info_remove_node_fio();
}

