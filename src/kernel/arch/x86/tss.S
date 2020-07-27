.globl tss_flush, switch_to_user_mode

tss_flush:
    mov $0x2B, %ax
    ltr %ax
    ret

switch_to_user_mode:
    cli
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov %esp, %eax
    pushl $0x23
    pushl %eax
    pushf
    pop %eax
    or $0x200, %eax
    push %eax
    pushl $0x1B
    push $1f
    iret
1:
    ret