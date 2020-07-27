ENTRY(_start)

SECTIONS
{

    . = 1M + 3072M;

    kernel_start = .;
    .text ALIGN(4K) : AT(ADDR(.text) - 3072M)
    {
        *(.multiboot)
        *(.text)
    }

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 3072M)
	{
		*(.rodata*)
	}

    .data ALIGN(4K) : AT(ADDR(.data) - 3072M)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 3072M)
    {
        *(COMMON*)
        *(.bss*)
		*(.bootstrap_stack)
    }
    kernel_end = .;

}