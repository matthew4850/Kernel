.globl idt_flush, read_eip

.text
idt_flush:
   mov 4(%esp), %eax  /* Get the pointer to the IDT, passed as a parameter.*/
   lidt (%eax)        /* Load the IDT pointer.*/
   ret
/*
This is our common ISR stub. It saves the processor state, sets
up for kernel mode segments, calls the C-level fault handler,
and finally restores the stack frame.
*/
isr_common_stub:
    pusha                    // Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov %ds, %ax               // Lower 16-bits of eax = ds.
    push %eax                 // save the data segment descriptor

    push %esp                 // Pushes pointer to registers struct

    mov $0x10, %ax             // load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call isr_handler

    add $4, %esp               // esp contains pointer to registers struct

    pop %eax                  // reload the original data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa                     // Pops edi,esi,ebp...
    add $8, %esp               // Cleans up the pushed error code and pushed ISR number
    iret                     // pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

irq_common_stub:
    pusha                    // Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov %ds, %ax               // Lower 16-bits of eax = ds.
    push %eax                 // save the data segment descriptor

    push %esp                 // Pushes pointer to registers struct

    mov $0x10, %ax             // load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call irq_handler

    add $4, %esp               // esp contains pointer to registers struct

    pop %ebx                  // reload the original data segment descriptor
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs

    popa                     // Pops edi,esi,ebp...
    add $8, %esp               // Cleans up the pushed error code and pushed ISR number
    iret                     // pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

.macro ISR_NOERRCODE arg
    .globl isr\arg
    isr\arg:
        cli
        push $0
        push $\arg
        jmp isr_common_stub
.endm


.macro ISR_ERRCODE arg
    .globl isr\arg
    isr\arg:
        cli
        push $\arg
        jmp isr_common_stub
.endm

.macro IRQ arg arg2
    .globl irq\arg
    irq\arg:
        cli
        push $0
        push $\arg2
        jmp irq_common_stub
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

ISR_NOERRCODE 128

IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47