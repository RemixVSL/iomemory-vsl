#ifndef OPENKFIO_H
#define OPENKFIO_H

#include <fio/port/kglobal.h>
#include <fio/port/kinfo.h>
#include <fio/port/ifio.h>
#include <fio/port/ufio.h>
#include <fio/port/kfio.h>

typedef struct fio_device {
    struct fio_device * fio_next;
    struct fio_device * fio_prev;
    char unknown16[1024];
} fio_device;

extern fio_device *gv_fio_device_list;
extern int32_t gv_init_pci_counter;

extern int kfio_print(const char *format, ...);
extern void kfio_info_remove_node (kfio_info_node_t **dirp);
extern void fio_detach_devices (void);
extern uint32_t fio_detach (fio_device * fio_dev, uint32_t mode);
extern const char *fio_device_get_bus_name (fio_device *dev);
extern void kfio_pci_set_master(kfio_pci_dev_t *pdev);
extern int kfio_pci_set_dma_mask(kfio_pci_dev_t *pdev, uint64_t mask);
extern void kfio_pci_release_regions(kfio_pci_dev_t *pdev);
extern int kfio_pci_enable_device(kfio_pci_dev_t *pdev);
extern void kfio_pci_disable_device(kfio_pci_dev_t *pdev);
extern int kfio_pci_request_regions(kfio_pci_dev_t *pdev, const char *res_name);

void of_cleanup_fio_blk(void);
void of_kfio_info_remove_node_fio(void);

C_ASSERT(sizeof(fio_device) == 4768);

#endif

