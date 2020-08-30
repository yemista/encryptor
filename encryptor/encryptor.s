	.file	"encryptor.c"
	.text
	.section	.rodata
.LC0:
	.string	"Function already decrypted"
.LC1:
	.string	"decrypting range %x   %x\n"
	.text
	.globl	decryptRange
	.type	decryptRange, @function
decryptRange:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	-32(%rbp), %rax
	movl	(%rax), %eax
	cmpl	$1, %eax
	jne	.L2
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	jmp	.L3
.L2:
	movq	-32(%rbp), %rax
	movl	$1, (%rax)
	movl	-24(%rbp), %edx
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-20(%rbp), %eax
	movq	%rax, -8(%rbp)
	movl	$0, -12(%rbp)
	jmp	.L4
.L5:
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %ecx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	xorl	$-45, %ecx
	movl	%ecx, %edx
	movb	%dl, (%rax)
	addl	$1, -12(%rbp)
.L4:
	movl	-24(%rbp), %eax
	subl	-20(%rbp), %eax
	movl	%eax, %edx
	movl	-12(%rbp), %eax
	cmpl	%eax, %edx
	jnb	.L5
	movl	$0, %eax
.L3:
	leave
	ret
	.size	decryptRange, .-decryptRange
	.section	.rodata
.LC2:
	.string	"Hello World!"
	.text
	.globl	testFunction1
	.type	testFunction1, @function
testFunction1:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	leaq	isFunctionAlreadyDecrypted.2335(%rip), %rdx
	movl	$1, %esi
	movl	$0, %edi
	call	decryptRange@PLT
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	movl	$4, -8(%rbp)
	movl	-8(%rbp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	leave
	ret
	.size	testFunction1, .-testFunction1
	.section	.rodata
.LC3:
	.string	"Back from dead!"
	.text
	.globl	testFunction2
	.type	testFunction2, @function
testFunction2:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	leaq	isFunctionAlreadyDecrypted.2340(%rip), %rdx
	movl	$1, %esi
	movl	$0, %edi
	call	decryptRange@PLT
	leaq	.LC3(%rip), %rdi
	call	puts@PLT
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	nop
	leave
	ret
	.size	testFunction2, .-testFunction2
	.section	.rodata
.LC4:
	.string	"hello"
	.text
	.globl	main
	.type	main, @function
main:
	endbr64
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	leaq	.LC4(%rip), %rdi
	call	testFunction1@PLT
	movl	$0, %eax
	call	testFunction2@PLT
	movl	$0, %eax
	leave
	ret
	.size	main, .-main
	.local	isFunctionAlreadyDecrypted.2335
	.comm	isFunctionAlreadyDecrypted.2335,4,4
	.local	isFunctionAlreadyDecrypted.2340
	.comm	isFunctionAlreadyDecrypted.2340,4,4
	.ident	"GCC: (Ubuntu 9.3.0-10ubuntu2) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
