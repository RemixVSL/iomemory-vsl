#ifndef NBD_FUNCS_H_
# define NBD_FUNCS_H_

# include <sys/types.h>
# include <unistd.h>

typedef int (* open_fn)(const char *, int, ...);                                                                                      
typedef int (* close_fn)(int fd);
typedef ssize_t (* read_fn)(int fd, void *buf, size_t count);
typedef ssize_t (* write_fn)(int fd, const void *buf, size_t count);
typedef int (* ioctl_fn)(int fd, unsigned long int request, ...);
typedef int64_t (* lseek64_fn)(int fd, int64_t offset, int whence);
typedef int (* dup_fn)(int fd);
typedef int (* dup2_fn)(int oldfd, int newfd);

int nbd_path_is_ours(const char *path);
int nbd_fd_is_ours(int fd);

int nbd_open(open_fn stdopen, const char *file, int flags, ...);
int nbd_close(close_fn stdclose, int fd);
uint64_t nbd_lseek64(lseek64_fn stdseek, int fd, uint64_t offset, int whence);
ssize_t nbd_read(read_fn stdread, int fd, void *buf, size_t count);
ssize_t nbd_write(write_fn stdwrite, int fd, const void *buf, size_t count);
int nbd_ioctl(ioctl_fn stdioctl, int fd, unsigned long int request, ...);
int nbd_dup(dup_fn stddup, int fd);
int nbd_dup2(dup2_fn stddup2, int oldfd, int newfd);

#endif
