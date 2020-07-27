#ifndef OSDEV_STDDEF_H
#define OSDEV_STDDEF_H

#include <stdint.h>

#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif
typedef unsigned int    size_t;
typedef int             ssize_t;

typedef int             pid_t;
typedef uint16_t        dev_t;
typedef uint16_t        ino_t;
typedef uint16_t        mode_t;
typedef unsigned int    nlink_t;
typedef unsigned int    uid_t;
typedef unsigned int    gid_t;
typedef unsigned int    id_t;
typedef long int        off_t;
typedef long int        blksize_t;
typedef long int        blkcnt_t;

typedef long ptrdiff_t;
#endif //OSDEV_STDDEF_H
