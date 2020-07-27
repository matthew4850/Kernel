.intel_syntax noprefix
.globl switch_to_context

.text

switch_to_context:
    add esp,4
    pop ebx // eip
    pop edx // esp
    pop ecx // ebp
    pop eax // page_dir
    cli
    mov esp, edx
    mov ebp, ecx
    mov cr3, eax
    sti
    jmp ebx

