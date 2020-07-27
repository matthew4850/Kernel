#include <syscalls/sys_write.h>
#include <tty.h>

ssize_t sys_write(int fd, const void *buf, size_t count){
	printk(buf);
	return 0;
}