#ifndef OSDEV_SYS_WRITE_H
#define OSDEV_SYS_WRITE_H
#include <stddef.h>
ssize_t sys_write(int fd, const void *buf, size_t count);
#endif //OSDEV_SYS_WRITE_H
