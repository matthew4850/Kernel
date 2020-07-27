.globl read_PIT_count, set_PIT_count

read_PIT_count:
	pushfl
	cli
	mov $0, %al   # al = channel in bits 6 and 7, remaining bits clear
	outb %al, $0x43         # Send the latch command

	inb $0x40, %al          # al = low byte of count
	mov %al, %ah           # ah = low byte of count
	inb $0x40, %al          # al = high byte of count
	rol $8, %ax            # al = low byte, ah = high byte (ax = current count)
	popfl
	ret

set_PIT_count:
    pushfl
    cli
    out %al, $0x40        # Set low byte of reload value
    rol $8, %ax          # al = high byte, ah = low byte
    out %al, $0x40        # Set high byte of reload value
    rol $8, %ax           # al = low byte, ah = high byte (ax = original reload value)
    popfl
    ret