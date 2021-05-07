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

int32_t of_iodrive_pci_probe(kfio_pci_dev_t *pci_dev, void *id)
{
  int32_t rc, dev_num;
  const char *name;
  char s1[255];
  char s2[255];
  bool include, exclude;

  if (sizeof(iodrive_dev_t) != 0x180 || sizeof(fusion_nand_device) != 0x11bf0) {
    kfio_print("<6>fioerr structure size mismatch");
    return -255;
  }

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
    kfio_pci_set_dma_mask(pci_dev, 0xffffffffffffffff);
    dev_num = gv_init_pci_counter++;
    rc = of_iodrive_pci_attach(pci_dev, dev_num);
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

uint32_t of_kfio_csr_read(csr_addr_t *csr,uint64_t index,bool indirect)
{
  uint32_t val;
  if (indirect == false) {
    val = kfio_csr_read_direct(csr->addr0 + index,csr->hdl);
  } else {
    fusion_spin_lock_irqsave_nested(&gv_lock_indirect_read,1);
    kfio_csr_write(index,csr->addr0 + 0xffc,csr->hdl);
    kfio_udelay(0x32);
    val = kfio_csr_read_direct(csr->addr0 + 0x3c,csr->hdl);
    fusion_spin_unlock_irqrestore(&gv_lock_indirect_read);
  }
  return val;
}

int32_t of_iodrive_pci_attach_failed(kfio_pci_dev_t *pci_dev)
{
  iodrive_dev_t *iodrive_dev;
  iodrive_dev = kfio_pci_get_drvdata(pci_dev);
  kfio_print("<3>fioerr %s: Driver probe failed on all pipelines\n",iodrive_dev->dev_name);
  iodrive_pci_detach(pci_dev);
  return -0x16;
}

int32_t of_iodrive_pci_attach_nand(iodrive_dev_t *iodrive_dev, int32_t dev_num, int32_t nr, kfio_numa_node_t numa_node)
{
  int32_t rc;
  fusion_nand_device * nand_dev;
  char nand_name[80];
  int32_t loop, errnum;

  kfio_snprintf(nand_name,0x1f,"iodrive-%s-%u",kfio_pci_name(iodrive_dev->pci_dev),nr);

  nand_dev = (fusion_nand_device *)kfio_malloc_node(sizeof(fusion_nand_device),numa_node);
  if (nand_dev == (fusion_nand_device *)0x0) {
    kfio_print("<3>fioerr %s: failed to get memory for device control structure\n",iodrive_dev->dev_name);
    return -1;
  }
  kfio_memset(nand_dev,0,sizeof(fusion_nand_device));

  rc = 0;
  if (iodrive_dev->nr_vecs == 0) {
    if (!nr) rc = kfio_request_irq(iodrive_dev->pci_dev, nand_name, iodrive_dev, iodrive_dev->msi);
  } else {
    rc = kfio_request_msix(iodrive_dev->pci_dev, nand_name, nand_dev, &iodrive_dev->msix, nr);
  }
  if (rc<0) {
    kfio_print("<3>fioerr %s: IRQ request failed\n",iodrive_dev->dev_name);
    kfio_free(nand_dev,sizeof(fusion_nand_device));
    return -2;
  }

  iodrive_dev->nand_dev[nr] = nand_dev;
  iodrive_dev->fct_cnt = iodrive_dev->fct_cnt + 1;

  loop = 0;
  do {
    kfio_memset(nand_dev,0,sizeof(fusion_nand_device));
    kfio_strncpy(nand_dev->dev_name,nand_name,0x1f);
    kfio_snprintf(nand_dev->bus_name,0x100,"ioDrive %s.%d",kfio_pci_name(iodrive_dev->pci_dev),0);

    nand_dev->pci_dev = iodrive_dev->pci_dev;
    nand_dev->pci_slot = kfio_pci_get_slot(iodrive_dev->pci_dev);
    nand_dev->numa_node = numa_node;

    (nand_dev->csr).unknown0 = iodrive_dev->vaddr;
    (nand_dev->csr).hdl = iodrive_dev->vhdl;
    if (nr == 0) {
      (nand_dev->csr).unknown16 = iodrive_dev->vaddr;
      (nand_dev->csr).unknown24 = iodrive_dev->vaddr + 0x2000;
      (nand_dev->csr).addr1 = iodrive_dev->vaddr + 0x4000;
      (nand_dev->csr).addr0 = iodrive_dev->vaddr + 0x8000;
    } else {
      (nand_dev->csr).unknown16 = iodrive_dev->vaddr + 0x200;
      (nand_dev->csr).unknown24 = iodrive_dev->vaddr + 0x2100;
      (nand_dev->csr).addr1 = iodrive_dev->vaddr + 0x4100;
      (nand_dev->csr).addr1 = iodrive_dev->vaddr + 0x10000;
    }

    kfio_csr_write(0xffc,iodrive_dev->vaddr + 0x8ffc,iodrive_dev->vhdl);
    rc = of_kfio_csr_read(&nand_dev->csr,0x1208,nand_dev->indirect_read);
    if (rc == 0xcafecafe) nand_dev->indirect_read = true;
    rc = of_kfio_csr_read(&nand_dev->csr,0x120c,nand_dev->indirect_read);
    if (rc == 0) {
      errnum = -21;
      kfio_print("<3>fioerr %s: Failed to communicate with the card. This may be due to incompatible firmware or a hardware failure. Please contact Customer Support.\n",
        nand_dev->dev_name);
      break;
    }
    nand_dev->major = kfio_register_blkdev_pre_init(iodrive_dev->pci_dev);

    errnum = iodrive_pci_attach_setup(nand_dev, dev_num, nr, iodrive_dev->nand_dev[0]);
    if (errnum < 0) break;

    if (   kfio_pci_get_subsystem_vendor(iodrive_dev->pci_dev) == 0x103c
        && kfio_pci_get_subsystem_device(iodrive_dev->pci_dev) == 0x340d
        && nand_dev->pci_slot != 0)
    { 
      kfio_snprintf(nand_dev->bus_name,0x100,"%s Slot %d", nand_dev->str_17376, nand_dev->pci_slot);
    } else {
      kfio_snprintf(nand_dev->bus_name,0x100,"%s %s",nand_dev->str_17376, kfio_pci_name(iodrive_dev->pci_dev));
    }
    kfio_print("<6>fioinf %s: Found device %s (%s) on pipeline %d\n",iodrive_dev->dev_name,nand_dev->fct_name,nand_dev->bus_name,nr);

    errnum = iodrive_pci_attach_post(nand_dev);
    if (errnum != -10) {
      if (errnum < 0) break;
      if (loop != 0) kfio_print("<6>fioinf %s: stuck flush request got better on retry.\n",nand_dev->fct_name);
      kfio_print("<6>fioinf %s: probed %s\n",nand_dev->bus_name,nand_dev->fct_name);
      rc = of_kfio_csr_read(&iodrive_dev->csr,0x1200,iodrive_dev->indirect_read);
      if ((rc & 0x7f) != 3) return -3;
      rc = of_kfio_csr_read(&iodrive_dev->csr,0x1240,iodrive_dev->indirect_read);
      if ((rc == 0xcafecafe) || (rc < 2)) return 0;
      return rc;
    }
    loop = loop + 1;
    kfio_print("<6>fioinf %s: stuck flush request on startup detected, retry iteration %u of %u...\n",
        nand_dev->dev_name,loop,3);
  } while (loop != 4);

  if (iodrive_dev->nand_dev[nr] != (fusion_nand_device *)0x0) {
    iodrive_dev->fct_cnt = iodrive_dev->fct_cnt -1;
    iodrive_dev->nand_dev[nr] = (fusion_nand_device *)0x0;
  }
  if (iodrive_dev->nr_vecs != 0) {
    kfio_free_msix(&iodrive_dev->msix,0,nand_dev);
  }
  kfio_free(nand_dev,sizeof(fusion_nand_device));
  kfio_print("<3>fioerr %s: Driver probe on pipeline %d failed with error %d: %s\n",
    iodrive_dev->dev_name,nr,errnum,ifio_strerror(errnum));
  return -5;
}

int32_t of_iodrive_pci_attach(kfio_pci_dev_t *pci_dev, int32_t dev_num)
{
  iodrive_dev_t *iodrive_dev;
  char fct_name[33];
  kfio_numa_node_t numa_node;
  uint16_t bar;
  uint64_t offset;
  bool ok;
  int32_t nr, maxnr;

  kfio_snprintf(fct_name, 33, "fct%d", dev_num);
  if (kfio_get_numa_node_override(pci_dev,fct_name,&numa_node))
    numa_node = kfio_pci_get_node(pci_dev);

  iodrive_dev = (iodrive_dev_t *)kfio_malloc_node(sizeof(iodrive_dev_t), numa_node);
  if (iodrive_dev == (iodrive_dev_t *)0x0) {
    kfio_print("<3>fioerr ioDrive %s: failed to get memory for control structure\n", kfio_pci_name(pci_dev));
    return -0xc;
  }
  kfio_memset(iodrive_dev, 0, sizeof(iodrive_dev_t));

  kfio_snprintf(iodrive_dev->dev_name, 0xff, "ioDrive %s", kfio_pci_name(pci_dev));
  iodrive_dev->pci_dev = pci_dev;
  kfio_pci_set_drvdata(pci_dev, iodrive_dev);

  bar = 5;
  offset = kfio_pci_resource_start(pci_dev, bar);
  if (offset == 0) {
    bar = 0;
    offset = kfio_pci_resource_start(pci_dev, bar);
  }
  kfio_print("<6>fioinf %s: mapping controller on BAR %d\n", iodrive_dev->dev_name, bar);

  iodrive_dev->vaddr = kfio_ioremap_nocache(offset, kfio_pci_resource_len(pci_dev, bar));
  iodrive_dev->vhdl = (char *)0x0;

  if (iodrive_dev->vaddr == (char *)0x0) {
    kfio_print("<3>fioerr %s: failed to map PCI BAR\n", iodrive_dev->dev_name);
    kfio_print("<3>fioerr %s: failed to get memory regions\n", iodrive_dev->dev_name);
    return of_iodrive_pci_attach_failed(pci_dev);
  }

  ok = false;
  if (disable_msix == 0) {
    iodrive_dev->nr_vecs = kfio_pci_enable_msix(pci_dev, &iodrive_dev->msix, 2);
    if (iodrive_dev->nr_vecs != 0) {
      ok = true;
      iodrive_dev->msi = false;
      kfio_print("<6>fioinf %s: MSI-X enabled (%u vectors)\n", iodrive_dev->dev_name, iodrive_dev->nr_vecs);
      kfio_iodrive_intx(pci_dev, 0);
      kfio_print("<6>fioinf %s: using MSI-X interrupts\n", iodrive_dev->dev_name);
    }
  }
  if (!ok && !disable_msi && !kfio_pci_enable_msi(pci_dev)) {
    ok = true;
    iodrive_dev->msi = true;
    kfio_print("<6>fioinf %s: MSI enabled\n", iodrive_dev->dev_name);
    kfio_iodrive_intx(pci_dev,0);
    kfio_print("<6>fioinf %s: using MSI interrupts\n", iodrive_dev->dev_name);
  }

  if (!ok) {
    kfio_iodrive_intx(pci_dev,1);
    kfio_print("<6>fioinf %s: using legacy interrupts\n", iodrive_dev->dev_name);
  }

  (iodrive_dev->csr).unknown0 = iodrive_dev->vaddr;
  (iodrive_dev->csr).unknown16 = iodrive_dev->vaddr;
  (iodrive_dev->csr).hdl = iodrive_dev->vhdl;
  (iodrive_dev->csr).unknown24 =  iodrive_dev->vaddr + 0x2000;
  (iodrive_dev->csr).addr1 = iodrive_dev->vaddr + 0x4000;
  (iodrive_dev->csr).addr0 = iodrive_dev->vaddr + 0x8000;

  kfio_csr_write(0xffc,iodrive_dev->vaddr + 0x8ffc, iodrive_dev->vhdl);

  iodrive_dev->indirect_read =
    (0xcafecafe == kfio_csr_read_direct((iodrive_dev->csr).addr0 + 0x1208,(iodrive_dev->csr).hdl));

  iodrive_dev->initialized = true;
  iodrive_reset_and_wait(iodrive_dev);

  maxnr = of_iodrive_pci_attach_nand(iodrive_dev,dev_num,0,numa_node);
  if (maxnr < 0) {
    kfio_print("<3>fioerr %s: Master pipeline failed\n",iodrive_dev->dev_name);
    return of_iodrive_pci_attach_failed(pci_dev);
  }

  nr = 1;
  while (nr < maxnr) {
    of_iodrive_pci_attach_nand(iodrive_dev, gv_init_pci_counter++, nr, numa_node);
    nr = nr + 1;
  }
  return 0;
}

