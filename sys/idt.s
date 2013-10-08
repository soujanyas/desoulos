.text
.extern isr0_handler
.global _x86_64_asm_lidt
.global	_div_by_zero
_x86_64_asm_lidt:
	pushq %fs
	pushq %gs
	lidt (%rdi)
	popq %gs
	popq %fs
	retq

