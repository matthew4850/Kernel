#ifndef _IO_H
#define _IO_H

static inline unsigned char inb (unsigned short int __port)
{
	unsigned char _v;
	__asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (__port));
	return _v;
}
static inline unsigned char inb_p (unsigned short int __port)
{
	unsigned char _v;
	__asm__ __volatile__ ("inb %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (__port));
	return _v;
}
static inline unsigned short int inw (unsigned short int __port)
{
	unsigned short _v;
	__asm__ __volatile__ ("inw %w1,%0":"=a" (_v):"Nd" (__port));
	return _v;
}
static inline unsigned short int inw_p (unsigned short int __port)
{
	unsigned short int _v;
	__asm__ __volatile__ ("inw %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (__port));
	return _v;
}
static inline unsigned int inl (unsigned short int __port)
{
	unsigned int _v;
	__asm__ __volatile__ ("inl %w1,%0":"=a" (_v):"Nd" (__port));
	return _v;
}
static inline unsigned int inl_p (unsigned short int __port)
{
	unsigned int _v;
	__asm__ __volatile__ ("inl %w1,%0\noutb %%al,$0x80":"=a" (_v):"Nd" (__port));
	return _v;
}
static inline void outb (unsigned char __value, unsigned short int __port)
{
	__asm__ __volatile__ ("outb %b0,%w1": :"a" (__value), "Nd" (__port));
}
static inline void outb_p (unsigned char __value, unsigned short int __port)
{
	__asm__ __volatile__ ("outb %b0,%w1\noutb %%al,$0x80": :"a" (__value),
	"Nd" (__port));
}
static inline void outw (unsigned short int __value, unsigned short int __port)
{
	__asm__ __volatile__ ("outw %w0,%w1": :"a" (__value), "Nd" (__port));
}
static inline void outw_p (unsigned short int __value, unsigned short int __port)
{
	__asm__ __volatile__ ("outw %w0,%w1\noutb %%al,$0x80": :"a" (__value),
	"Nd" (__port));
}
static inline void outl (unsigned int __value, unsigned short int __port)
{
	__asm__ __volatile__ ("outl %0,%w1": :"a" (__value), "Nd" (__port));
}
static inline void outl_p (unsigned int __value, unsigned short int __port)
{
	__asm__ __volatile__ ("outl %0,%w1\noutb %%al,$0x80": :"a" (__value),
	"Nd" (__port));
}
static inline void insb (unsigned short int __port, void *__addr, unsigned long int __count)
{
	__asm__ __volatile__ ("cld ; rep ; insb":"=D" (__addr), "=c" (__count)
	:"d" (__port), "0" (__addr), "1" (__count));
}
static inline void insw (unsigned short int __port, void *__addr, unsigned long int __count)
{
	__asm__ __volatile__ ("cld ; rep ; insw":"=D" (__addr), "=c" (__count)
	:"d" (__port), "0" (__addr), "1" (__count));
}
static inline void insl (unsigned short int __port, void *__addr, unsigned long int __count)
{
	__asm__ __volatile__ ("cld ; rep ; insl":"=D" (__addr), "=c" (__count)
	:"d" (__port), "0" (__addr), "1" (__count));
}
static inline void outsb (unsigned short int __port, const void *__addr,
       unsigned long int __count)
{
	__asm__ __volatile__ ("cld ; rep ; outsb":"=S" (__addr), "=c" (__count)
	:"d" (__port), "0" (__addr), "1" (__count));
}
static inline void outsw (unsigned short int __port, const void *__addr,
       unsigned long int __count)
{
	__asm__ __volatile__ ("cld ; rep ; outsw":"=S" (__addr), "=c" (__count)
	:"d" (__port), "0" (__addr), "1" (__count));
}
static inline void outsl (unsigned short int __port, const void *__addr,
       unsigned long int __count)
{
	__asm__ __volatile__ ("cld ; rep ; outsl":"=S" (__addr), "=c" (__count)
	:"d" (__port), "0" (__addr), "1" (__count));
}
static inline void io_wait(void)
{
	__asm__ __volatile__ ("outb %%al, $0x80" : : "a"(0));
}
#endif //_IO_H
