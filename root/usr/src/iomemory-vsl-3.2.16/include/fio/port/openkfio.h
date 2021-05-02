#include <fio/port/kglobal.h>
#include <fio/port/kinfo.h>
#include <fio/port/ifio.h>
#include <fio/port/ufio.h>

typedef struct fio_device {
    struct fio_device * fio_next;
    struct fio_device * fio_prev;
    char unknown16[1024];
} fio_device;

extern fio_device *gv_fio_device_list;

extern int kfio_print(const char *format, ...);
extern void kfio_info_remove_node (kfio_info_node_t **dirp);
extern void fio_detach_devices (void);
extern uint32_t fio_detach (fio_device * fio_dev, uint32_t mode);
extern const char *fio_device_get_bus_name (fio_device *dev);

void of_cleanup_fio_blk(void);
void of_kfio_info_remove_node_fio(void);
