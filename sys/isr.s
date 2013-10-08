.global isr0
.global isr32
.global isr33
.extern isr_handler
isr0:
	cli
	mov $0,%rdi
	callq generic_handler
	sti
	iretq
isr32:
	cli
	mov $32,%rdi
	callq generic_handler
	sti
	iretq
isr33:
  	cli
	mov $33,%rdi
	callq generic_handler
	sti
	iretq

generic_handler:
	pushq %fs
	pushq %gs
	pushq %rdi
	pushq %rsi
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rsp
	pushq %rbp
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	callq isr_handler
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rbp
	popq %rsp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	popq %rsi
	popq %rdi
	popq %gs
	popq %fs
	retq
