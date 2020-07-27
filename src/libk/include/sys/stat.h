#ifndef OSDEV_STAT_H
#define OSDEV_STAT_H

/** File types. @{ */
#define S_IFMT      0170000 /**< File type mask. */
#define S_IFIFO     0010000 /**< FIFO file. */
#define S_IFCHR     0020000 /**< Character device file. */
#define S_IFDIR     0040000 /**< Directory. */
#define S_IFBLK     0060000 /**< Block device file. */
#define S_IFREG     0100000 /**< Regular file. */
#define S_IFLNK     0120000 /**< Symbolic link. */
#define S_IFSOCK    0140000 /**< Socket. */
/** @} */
#endif //OSDEV_STAT_H
