#ifndef OPENKFIO_H
#define OPENKFIO_H

#include <fio/port/kglobal.h>
#include <fio/port/kinfo.h>
#include <fio/port/ifio.h>
#include <fio/port/ufio.h>
#include <fio/port/kfio.h>
#include <fio/port/kcsr.h>
#include <fio/port/ktime.h>

typedef char * char_ptr;

typedef struct fio_state_t fio_state_t;
struct fio_state_t
{
    volatile uint32_t state;
#if defined(__KERNEL__)
    fusion_condvar_t cv;
    fusion_cv_lock_t lk;
#endif
};


typedef struct fio_device fio_device;
struct fio_device {
    fio_device * fio_next;
    fio_device * fio_prev;
    char unknown16[1024];
};

typedef struct csr_addr_t csr_addr_t;
struct csr_addr_t {
    char * unknown0;
    char * addr0;
    char * unknown16;
    char * unknown24;
    char * addr1;
    char * hdl;
};

typedef struct fusion_nand_device fusion_nand_device;
struct fusion_nand_device {
    fusion_nand_device * next_dev;
    fusion_nand_device * prev_dev;
    char bus_name[256];
    char fct_name[33]; /* fct%d */
    char unknown305[7];
    kfio_pci_dev_t * pci_dev;
    kfio_info_node_t * dir_root_fct;
    char unknown328[8];
    kfio_info_node_t * dir_channels;
    int32_t major;
    char unknown348[120];
    int32_t dev_num;
    uint32_t linked_cnt;
    char unknown476[4];
    void * ptr_480;
    void * ptr_488;
    char unknown496[8];
    uint8_t pci_slot;
    char unknown505[7];
    fusion_spinlock_t lock;
    char unknown704[228];
    uint32_t attach_disabled;
    int32_t errnum;
    kfio_numa_node_t numa_node;
    char unknown944[16];
    csr_addr_t csr;
    bool indirect_read;
    char unknown1009[7];
    kfio_info_node_t * dir_iodrive_fct;
    kfio_info_node_t * dir_powermon;
    kfio_info_node_t * dir_memstats;
    char unknown1040[8];
    int32_t bitfield;
    char unknown1052[36];
    int64_t int64_1088;
    char unknown1096[6];
    char unknown1102[2];
    char unknown1104[190];
    char unknown1294[658];
    fusion_spinlock_t lock_fio_progress;
    char field_0x860[16];
    fio_state_t state;
    int32_t int32_2512;
    char unknown2516[13236];
    char_ptr iomem1[5];
    void * csr_handle1;
    char unknown15800[1576];
    char str_17376[8];
    char unknown17384[488];
    fio_state_t state_exit;
    char unknown18224[53816];
    int64_t * int64ptr_72040;
    char unknown72048[56];
    char * nv_mem;
    char unknown72112[196];
    uint32_t seconds;
    uint32_t int32_72312;
    char unknown72316[22];
    int8_t int8_72338;
    char unknown72339[309];
    uint32_t expected_io_size;
    int32_t dev_nr;
    char dev_name[32];
};

typedef struct fusion_nand_device * fusion_nand_device_ptr;

typedef  struct iodrive_dev_t iodrive_dev_t;
struct iodrive_dev_t {
    kfio_pci_dev_t * pci_dev;
    char * vaddr;
    char * vhdl;
    csr_addr_t csr;
    bool indirect_read;
    char unknown73[7];
    fusion_nand_device_ptr nand_dev[2];
    bool msi;
    char unknown97[7];
    kfio_msix_t msix;
    char unknown112[8];
    uint16_t nr_vecs;
    bool initialized;
    uint8_t fct_cnt;
    char dev_name[256];
    char unknown380[4];
};


extern fio_device *gv_fio_device_list;
extern int32_t gv_init_pci_counter;
extern fusion_spinlock_t gv_lock_indirect_read;

// original functions
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
extern void iodrive_reset_and_wait(iodrive_dev_t * iodrive_dev);
extern int32_t iodrive_pci_attach_setup(fusion_nand_device * nand_dev, int32_t dev_num, uint32_t vector, fusion_nand_device * master_nand_dev);
extern int32_t iodrive_pci_attach_post(fusion_nand_device * nand_dev);
extern void iodrive_clear_all_pci_errors(kfio_pci_dev_t * pci_dev);
extern void iodrive_clear_pci_errors(kfio_pci_dev_t *pci_dev);

// open substitutes
void of_cleanup_fio_blk(void);
void of_kfio_info_remove_node_fio(void);
int32_t of_iodrive_pci_probe(kfio_pci_dev_t *pci_dev, void *id);
void of_iodrive_pci_remove(kfio_pci_dev_t *pci_dev);
uint32_t kfio_csr_read(csr_addr_t *csr,uint64_t index,bool indirect);
int32_t of_iodrive_pci_attach_failed(kfio_pci_dev_t *pci_dev);
int32_t of_iodrive_pci_attach_nand(iodrive_dev_t *iodrive_dev, int32_t dev_num, int32_t nr, kfio_numa_node_t numa_node);
int32_t of_iodrive_pci_attach(kfio_pci_dev_t *pci_dev,int32_t dev_num);
void of_iodrive_reset_and_wait(iodrive_dev_t *iodrive_dev);
void of_iodrive_clear_all_pci_errors(kfio_pci_dev_t *pci_dev);
void of_iodrive_clear_pci_errors(kfio_pci_dev_t *pci_dev);
#endif

