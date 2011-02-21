	.file	"accelerations.c"
	.section	.rodata
	.align 16
.LC0:
	.long	2147483648
	.long	0
	.long	0
	.long	0
	.text
.globl sphericalAccel
	.type	sphericalAccel, @function
sphericalAccel:
.LFB2:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	subq	$80, %rsp
.LCFI2:
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	sqrtss	-20(%rbp), %xmm0
	movss	%xmm0, -60(%rbp)
	movss	-60(%rbp), %xmm3
	ucomiss	-60(%rbp), %xmm3
	jp	.L4
	je	.L2
.L4:
	movl	-20(%rbp), %eax
	movl	%eax, -68(%rbp)
	movss	-68(%rbp), %xmm0
	call	sqrtf
	movss	%xmm0, -60(%rbp)
.L2:
	movl	-60(%rbp), %eax
	movl	%eax, -28(%rbp)
	movq	-48(%rbp), %rax
	movss	8(%rax), %xmm0
	addss	-28(%rbp), %xmm0
	movss	%xmm0, -24(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm2
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movss	.LC0(%rip), %xmm0
	xorps	%xmm0, %xmm1
	movss	-24(%rbp), %xmm0
	mulss	-24(%rbp), %xmm0
	mulss	-28(%rbp), %xmm0
	movaps	%xmm1, %xmm3
	divss	%xmm0, %xmm3
	movaps	%xmm3, %xmm0
	mulss	%xmm2, %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	addq	$4, -16(%rbp)
	addq	$4, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm2
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movss	.LC0(%rip), %xmm0
	xorps	%xmm0, %xmm1
	movss	-24(%rbp), %xmm0
	mulss	-24(%rbp), %xmm0
	mulss	-28(%rbp), %xmm0
	movaps	%xmm1, %xmm3
	divss	%xmm0, %xmm3
	movaps	%xmm3, %xmm0
	mulss	%xmm2, %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	addq	$4, -16(%rbp)
	addq	$4, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm2
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movss	.LC0(%rip), %xmm0
	xorps	%xmm0, %xmm1
	movss	-24(%rbp), %xmm0
	mulss	-24(%rbp), %xmm0
	mulss	-28(%rbp), %xmm0
	movaps	%xmm1, %xmm3
	divss	%xmm0, %xmm3
	movaps	%xmm3, %xmm0
	mulss	%xmm2, %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	leave
	ret
.LFE2:
	.size	sphericalAccel, .-sphericalAccel
	.section	.rodata
	.align 16
.LC1:
	.long	2147483648
	.long	0
	.long	0
	.long	0
	.text
.globl miyamotoNagaiDiskAccel
	.type	miyamotoNagaiDiskAccel, @function
miyamotoNagaiDiskAccel:
.LFB3:
	pushq	%rbp
.LCFI3:
	movq	%rsp, %rbp
.LCFI4:
	subq	$64, %rsp
.LCFI5:
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-48(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -24(%rbp)
	movq	-48(%rbp), %rax
	movl	12(%rax), %eax
	movl	%eax, -20(%rbp)
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	mulss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	call	sqrtf
	movss	%xmm0, -60(%rbp)
	movl	-60(%rbp), %eax
	movl	%eax, -16(%rbp)
	movss	-24(%rbp), %xmm0
	addss	-16(%rbp), %xmm0
	movss	%xmm0, -12(%rbp)
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	movaps	%xmm1, %xmm2
	mulss	%xmm0, %xmm2
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movaps	%xmm2, %xmm1
	addss	%xmm0, %xmm1
	movss	-12(%rbp), %xmm0
	mulss	-12(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-8(%rbp), %xmm0
	mulss	-8(%rbp), %xmm0
	mulss	-8(%rbp), %xmm0
	call	sqrtf
	movss	%xmm0, -60(%rbp)
	movl	-60(%rbp), %eax
	movl	%eax, -4(%rbp)
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movss	.LC1(%rip), %xmm0
	xorps	%xmm0, %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	divss	-4(%rbp), %xmm0
	movq	-40(%rbp), %rax
	movss	%xmm0, (%rax)
	movq	-40(%rbp), %rdx
	addq	$4, %rdx
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movss	.LC1(%rip), %xmm0
	xorps	%xmm0, %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	divss	-4(%rbp), %xmm0
	movss	%xmm0, (%rdx)
	movq	-40(%rbp), %rdx
	addq	$8, %rdx
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movss	.LC1(%rip), %xmm0
	xorps	%xmm0, %xmm1
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	mulss	-12(%rbp), %xmm1
	movss	-16(%rbp), %xmm0
	mulss	-4(%rbp), %xmm0
	movaps	%xmm1, %xmm2
	divss	%xmm0, %xmm2
	movaps	%xmm2, %xmm0
	movss	%xmm0, (%rdx)
	leave
	ret
.LFE3:
	.size	miyamotoNagaiDiskAccel, .-miyamotoNagaiDiskAccel
	.section	.rodata
	.align 16
.LC2:
	.long	2147483648
	.long	0
	.long	0
	.long	0
	.align 8
.LC3:
	.long	0
	.long	1072693248
	.text
.globl exponentialDiskAccel
	.type	exponentialDiskAccel, @function
exponentialDiskAccel:
.LFB4:
	pushq	%rbp
.LCFI6:
	movq	%rsp, %rbp
.LCFI7:
	subq	$80, %rsp
.LCFI8:
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	-64(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -36(%rbp)
	movq	-72(%rbp), %rax
	movss	(%rax), %xmm1
	movq	-72(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-72(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm1
	movq	-72(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-72(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm1
	movq	-72(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -76(%rbp)
	movss	-76(%rbp), %xmm0
	call	sqrtf
	movss	%xmm0, -76(%rbp)
	movl	-76(%rbp), %eax
	movl	%eax, -32(%rbp)
	movss	.LC2(%rip), %xmm0
	movss	-32(%rbp), %xmm1
	xorps	%xmm1, %xmm0
	divss	-36(%rbp), %xmm0
	call	expf
	movaps	%xmm0, %xmm1
	movss	-32(%rbp), %xmm0
	addss	-36(%rbp), %xmm0
	mulss	%xmm1, %xmm0
	divss	-36(%rbp), %xmm0
	movss	%xmm0, -28(%rbp)
	movq	-64(%rbp), %rax
	movss	4(%rax), %xmm0
	cvtss2sd	%xmm0, %xmm2
	cvtss2sd	-28(%rbp), %xmm1
	movsd	.LC3(%rip), %xmm0
	movapd	%xmm1, %xmm3
	subsd	%xmm0, %xmm3
	movapd	%xmm3, %xmm0
	movapd	%xmm2, %xmm1
	mulsd	%xmm0, %xmm1
	movss	-32(%rbp), %xmm0
	mulss	-32(%rbp), %xmm0
	mulss	-32(%rbp), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movapd	%xmm1, %xmm2
	divsd	%xmm0, %xmm2
	movapd	%xmm2, %xmm0
	cvtsd2ss	%xmm0, %xmm0
	movss	%xmm0, -24(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-72(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	addq	$4, -16(%rbp)
	addq	$4, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	addq	$4, -16(%rbp)
	addq	$4, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	leave
	ret
.LFE4:
	.size	exponentialDiskAccel, .-exponentialDiskAccel
	.section	.rodata
	.align 4
.LC4:
	.long	3221225472
	.text
.globl logHaloAccel
	.type	logHaloAccel, @function
logHaloAccel:
.LFB5:
	pushq	%rbp
.LCFI9:
	movq	%rsp, %rbp
.LCFI10:
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	.LC4(%rip), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -24(%rbp)
	movq	-48(%rbp), %rax
	movss	12(%rax), %xmm1
	movq	-48(%rbp), %rax
	movss	12(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-48(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -16(%rbp)
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -12(%rbp)
	movss	-16(%rbp), %xmm0
	movaps	%xmm0, %xmm2
	mulss	-16(%rbp), %xmm2
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	addss	%xmm0, %xmm2
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	addss	%xmm2, %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-12(%rbp), %xmm0
	divss	-20(%rbp), %xmm0
	addss	-8(%rbp), %xmm0
	movss	%xmm0, -4(%rbp)
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	divss	-4(%rbp), %xmm0
	movq	-40(%rbp), %rax
	movss	%xmm0, (%rax)
	movq	-40(%rbp), %rdx
	addq	$4, %rdx
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	divss	-4(%rbp), %xmm0
	movss	%xmm0, (%rdx)
	movq	-40(%rbp), %rdx
	addq	$8, %rdx
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	movaps	%xmm0, %xmm1
	mulss	-24(%rbp), %xmm1
	movss	-20(%rbp), %xmm0
	mulss	-8(%rbp), %xmm0
	addss	-12(%rbp), %xmm0
	movaps	%xmm1, %xmm2
	divss	%xmm0, %xmm2
	movaps	%xmm2, %xmm0
	movss	%xmm0, (%rdx)
	leave
	ret
.LFE5:
	.size	logHaloAccel, .-logHaloAccel
	.section	.rodata
	.align 16
.LC5:
	.long	2147483648
	.long	0
	.long	0
	.long	0
	.align 8
.LC6:
	.long	1408749273
	.long	1070310883
	.text
.globl nfwHaloAccel
	.type	nfwHaloAccel, @function
nfwHaloAccel:
.LFB6:
	pushq	%rbp
.LCFI11:
	movq	%rsp, %rbp
.LCFI12:
	subq	$96, %rsp
.LCFI13:
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	-72(%rbp), %rax
	movss	(%rax), %xmm1
	movq	-72(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-72(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm1
	movq	-72(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-72(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm1
	movq	-72(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	-20(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -84(%rbp)
	movss	-84(%rbp), %xmm0
	call	sqrtf
	movss	%xmm0, -84(%rbp)
	movl	-84(%rbp), %eax
	movl	%eax, -36(%rbp)
	movq	-64(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -32(%rbp)
	movss	-32(%rbp), %xmm0
	addss	-36(%rbp), %xmm0
	movss	%xmm0, -28(%rbp)
	movq	-64(%rbp), %rax
	movss	4(%rax), %xmm1
	movq	-64(%rbp), %rax
	movss	4(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movaps	%xmm0, %xmm1
	mulss	-32(%rbp), %xmm1
	movss	%xmm1, -80(%rbp)
	movss	.LC5(%rip), %xmm0
	movss	-28(%rbp), %xmm1
	movss	%xmm1, -84(%rbp)
	movl	-84(%rbp), %eax
	xorl	$-2147483648, %eax
	movl	%eax, -76(%rbp)
	movss	-36(%rbp), %xmm0
	divss	-32(%rbp), %xmm0
	call	log1pf
	mulss	-76(%rbp), %xmm0
	addss	-36(%rbp), %xmm0
	mulss	-80(%rbp), %xmm0
	cvtss2sd	%xmm0, %xmm2
	movss	-36(%rbp), %xmm0
	mulss	-36(%rbp), %xmm0
	mulss	-36(%rbp), %xmm0
	cvtss2sd	%xmm0, %xmm1
	movsd	.LC6(%rip), %xmm0
	mulsd	%xmm0, %xmm1
	cvtss2sd	-28(%rbp), %xmm0
	mulsd	%xmm1, %xmm0
	movapd	%xmm2, %xmm1
	divsd	%xmm0, %xmm1
	movapd	%xmm1, %xmm0
	cvtsd2ss	%xmm0, %xmm0
	movss	%xmm0, -24(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-72(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	addq	$4, -16(%rbp)
	addq	$4, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	addq	$4, -16(%rbp)
	addq	$4, -8(%rbp)
	movq	-8(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	-24(%rbp), %xmm0
	movq	-16(%rbp), %rax
	movss	%xmm0, (%rax)
	leave
	ret
.LFE6:
	.size	nfwHaloAccel, .-nfwHaloAccel
	.section	.rodata
	.align 16
.LC7:
	.long	2147483648
	.long	0
	.long	0
	.long	0
	.text
.globl triaxialHaloAccel
	.type	triaxialHaloAccel, @function
triaxialHaloAccel:
.LFB7:
	pushq	%rbp
.LCFI14:
	movq	%rsp, %rbp
.LCFI15:
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movq	-48(%rbp), %rax
	movss	12(%rax), %xmm1
	movq	-48(%rbp), %rax
	movss	12(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -32(%rbp)
	movq	-48(%rbp), %rax
	movss	8(%rax), %xmm1
	movq	-48(%rbp), %rax
	movss	8(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -28(%rbp)
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm1
	movq	-48(%rbp), %rax
	movss	4(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movss	.LC7(%rip), %xmm0
	xorps	%xmm1, %xmm0
	movss	%xmm0, -24(%rbp)
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -20(%rbp)
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -16(%rbp)
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movss	%xmm0, -12(%rbp)
	movq	-48(%rbp), %rax
	movss	28(%rax), %xmm0
	mulss	-20(%rbp), %xmm0
	movaps	%xmm0, %xmm2
	addss	-28(%rbp), %xmm2
	movq	-48(%rbp), %rax
	movss	36(%rax), %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm0, %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	movaps	%xmm2, %xmm1
	addss	%xmm0, %xmm1
	movq	-48(%rbp), %rax
	movss	32(%rax), %xmm0
	mulss	-16(%rbp), %xmm0
	addss	%xmm1, %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-12(%rbp), %xmm0
	divss	-32(%rbp), %xmm0
	addss	-8(%rbp), %xmm0
	movss	%xmm0, -4(%rbp)
	cvtss2sd	-24(%rbp), %xmm3
	movq	-48(%rbp), %rax
	movss	28(%rax), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movapd	%xmm0, %xmm1
	addsd	%xmm0, %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movapd	%xmm1, %xmm2
	mulsd	%xmm0, %xmm2
	movq	-48(%rbp), %rax
	movss	36(%rax), %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	cvtss2sd	%xmm0, %xmm0
	addsd	%xmm2, %xmm0
	movapd	%xmm3, %xmm1
	mulsd	%xmm0, %xmm1
	cvtss2sd	-4(%rbp), %xmm0
	movapd	%xmm1, %xmm2
	divsd	%xmm0, %xmm2
	movapd	%xmm2, %xmm0
	cvtsd2ss	%xmm0, %xmm0
	movq	-40(%rbp), %rax
	movss	%xmm0, (%rax)
	movq	-40(%rbp), %rdx
	addq	$4, %rdx
	cvtss2sd	-24(%rbp), %xmm3
	movq	-48(%rbp), %rax
	movss	32(%rax), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movapd	%xmm0, %xmm1
	addsd	%xmm0, %xmm1
	movq	-56(%rbp), %rax
	addq	$4, %rax
	movss	(%rax), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movapd	%xmm1, %xmm2
	mulsd	%xmm0, %xmm2
	movq	-48(%rbp), %rax
	movss	36(%rax), %xmm1
	movq	-56(%rbp), %rax
	movss	(%rax), %xmm0
	mulss	%xmm1, %xmm0
	cvtss2sd	%xmm0, %xmm0
	addsd	%xmm2, %xmm0
	movapd	%xmm3, %xmm1
	mulsd	%xmm0, %xmm1
	cvtss2sd	-4(%rbp), %xmm0
	movapd	%xmm1, %xmm2
	divsd	%xmm0, %xmm2
	movapd	%xmm2, %xmm0
	cvtsd2ss	%xmm0, %xmm0
	movss	%xmm0, (%rdx)
	movq	-40(%rbp), %rdx
	addq	$8, %rdx
	cvtss2sd	-24(%rbp), %xmm0
	movapd	%xmm0, %xmm1
	addsd	%xmm0, %xmm1
	movq	-56(%rbp), %rax
	addq	$8, %rax
	movss	(%rax), %xmm0
	cvtss2sd	%xmm0, %xmm0
	mulsd	%xmm0, %xmm1
	movss	-32(%rbp), %xmm0
	mulss	-8(%rbp), %xmm0
	addss	-12(%rbp), %xmm0
	cvtss2sd	%xmm0, %xmm0
	movapd	%xmm1, %xmm2
	divsd	%xmm0, %xmm2
	movapd	%xmm2, %xmm0
	cvtsd2ss	%xmm0, %xmm0
	movss	%xmm0, (%rdx)
	leave
	ret
.LFE7:
	.size	triaxialHaloAccel, .-triaxialHaloAccel
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	"zR"
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.uleb128 0x1
	.byte	0x3
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.long	.LFB2
	.long	.LFE2-.LFB2
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI0-.LFB2
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.long	.LFB3
	.long	.LFE3-.LFB3
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI3-.LFB3
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI4-.LCFI3
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE3:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.long	.LFB4
	.long	.LFE4-.LFB4
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI6-.LFB4
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI7-.LCFI6
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE5:
.LSFDE7:
	.long	.LEFDE7-.LASFDE7
.LASFDE7:
	.long	.LASFDE7-.Lframe1
	.long	.LFB5
	.long	.LFE5-.LFB5
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI9-.LFB5
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI10-.LCFI9
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE7:
.LSFDE9:
	.long	.LEFDE9-.LASFDE9
.LASFDE9:
	.long	.LASFDE9-.Lframe1
	.long	.LFB6
	.long	.LFE6-.LFB6
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI11-.LFB6
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI12-.LCFI11
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE9:
.LSFDE11:
	.long	.LEFDE11-.LASFDE11
.LASFDE11:
	.long	.LASFDE11-.Lframe1
	.long	.LFB7
	.long	.LFE7-.LFB7
	.uleb128 0x0
	.byte	0x4
	.long	.LCFI14-.LFB7
	.byte	0xe
	.uleb128 0x10
	.byte	0x86
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI15-.LCFI14
	.byte	0xd
	.uleb128 0x6
	.align 8
.LEFDE11:
	.ident	"GCC: (GNU) 4.1.2 20080704 (Red Hat 4.1.2-46)"
	.section	.note.GNU-stack,"",@progbits
