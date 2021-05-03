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

int of_iodrive_pci_probe(kfio_pci_dev_t *pci_dev, void *id)
{
  int32_t rc;
  const char *name;
  char s1[255];
  char s2[255];
  bool include, exclude;

  exclude = true;
  include = false;

  name = kfio_pci_name(pci_dev);
  if (name != (char *)0x0) {
    kfio_snprintf(s2, 255, ",%s,", name);  
    if (kfio_strlen(exclude_devices) != 0) {
      kfio_snprintf(s1, 255, ",%s,", exclude_devices);
      exclude = kfio_strnstr(s1, s2, 255) != (char *)0x0;
    } else {
      exclude = false;
    }
    if (kfio_strlen(include_devices) != 0) {
      kfio_snprintf(s1, 255, ",%s,", include_devices);
      include = kfio_strnstr(s1, s2, 255) != (char *)0x0;
    } else {
      include = true;
    }
  }
  
  if (exclude || !include) {
    kfio_print("<6>fioinf ignoring device %s\n",name);
    return -0x13;  
  }  
      
  kfio_snprintf(s1, 255, "%s %s", "ioDrive", name);

  rc = kfio_pci_enable_device(pci_dev);
  if (rc < 0) {
    kfio_print("<3>fioerr %s: failed to enable pci device\n",s1);
    return rc;
  }
  kfio_pci_set_master(pci_dev);
  rc = kfio_pci_request_regions(pci_dev, "iodrive");
  if (rc < 0) {
    kfio_print("<3>fioerr %s: failed to get memory regions\n", s1);
  } else {
    kfio_pci_set_dma_mask(pci_dev,0xffffffffffffffff);
    rc = iodrive_pci_attach(pci_dev,++gv_init_pci_counter);
    if (rc > -1) {
      return 0;
    }
    kfio_pci_release_regions(pci_dev);
  }
  kfio_pci_disable_device(pci_dev);
  return rc;
}

void of_iodrive_pci_remove(kfio_pci_dev_t *pci_dev)
{
  iodrive_pci_detach(pci_dev);
  kfio_pci_release_regions(pci_dev);
  kfio_pci_disable_device(pci_dev);
}

