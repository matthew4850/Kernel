This is a kernel I am creating to learn about low level development and modern operating system design (and for fun!).

Right now it supports
1. GDT
2. Interrupts
3. 8259 PIC
4. PS/22 keyboard
5. Basic scheduling
6. TSS (software context switching is primarily used but the TSS is still required to get out of usermode)
6. Switching to usermode and back to the kernel
