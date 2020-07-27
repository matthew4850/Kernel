#ifndef OSDEV_VFS_H
#define OSDEV_VFS_H

#include <stddef.h>

typedef struct file_ops file_ops_t;
typedef struct inode_ops inode_ops_t;
typedef struct inode inode_t;
typedef struct file file_t;
typedef struct file_system file_system_t;
typedef struct mount mount_t;

struct file_ops
{
	int (*open) (inode_t*, file_t*);
	void (*release) (inode_t*, file_t*);
	int (*lseek) (inode_t*, file_t*, off_t, int);
	int (*read) (inode_t*, file_t*, char *, int);
	int (*readdir) (inode_t*, file_t*, struct dirent *, int);
	int (*write) (inode_t*, file_t*, char *, int);

	/*
	int (*select) (inode_t*, struct file *, int, select_table *);
	int (*ioctl) (inode_t, struct file *, unsigned int, unsigned long);
	int (*mmap) (inode_t *, struct file *, unsigned long, size_t, int, unsigned long);
	int (*fsync) (inode_t*, struct file *);*/
};


struct inode_ops
{
};

struct inode
{
	ino_t ino;
	off_t size;
	mode_t mode;
	inode_ops_t* ops;
	file_ops_t* f_ops;
};

struct file
{
	inode_t* inode;
	mode_t mode;

	uint32_t flags;

	off_t offset;

	file_ops_t* ops;

	//void *priv_data;
};

struct file_system
{
	const char *name;
	struct super_block* (*read_super_block) (dev_t device);
};

struct mount
{
	char path[256-sizeof(file_system_t*)];
	file_system_t* fs;
};
#endif //OSDEV_VFS_H
