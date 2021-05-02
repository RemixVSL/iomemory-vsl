#include <fio/port/kglobal.h>
#include <fio/port/kinfo.h>

extern void kfio_info_remove_node(kfio_info_node_t **dirp);
//extern void kfio_info_remove_node_fio(void);
extern void fio_detach_devices(void);

void of_cleanup_fio_blk(void);
void of_kfio_info_remove_node_fio(void);
