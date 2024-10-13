	.file	"optimized.c"
	.option nopic
	.attribute arch, "rv32i2p1"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	clz32
	.type	clz32, @function
clz32:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	zero,-20(s0)
	lw	a4,-36(s0)
	li	a5,65536
	bgeu	a4,a5,.L2
	li	a5,16
	j	.L3
.L2:
	li	a5,0
.L3:
	sw	a5,-24(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	lw	a4,-36(s0)
	sll	a5,a4,a5
	sw	a5,-36(s0)
	lw	a4,-36(s0)
	li	a5,16777216
	bgeu	a4,a5,.L4
	li	a5,8
	j	.L5
.L4:
	li	a5,0
.L5:
	sw	a5,-24(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	lw	a4,-36(s0)
	sll	a5,a4,a5
	sw	a5,-36(s0)
	lw	a4,-36(s0)
	li	a5,268435456
	bgeu	a4,a5,.L6
	li	a5,4
	j	.L7
.L6:
	li	a5,0
.L7:
	sw	a5,-24(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	lw	a4,-36(s0)
	sll	a5,a4,a5
	sw	a5,-36(s0)
	lw	a4,-36(s0)
	li	a5,1073741824
	bgeu	a4,a5,.L8
	li	a5,2
	j	.L9
.L8:
	li	a5,0
.L9:
	sw	a5,-24(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	lw	a4,-36(s0)
	sll	a5,a4,a5
	sw	a5,-36(s0)
	lw	a5,-36(s0)
	not	a5,a5
	srli	a5,a5,31
	andi	a5,a5,0xff
	sw	a5,-24(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	lw	a4,-36(s0)
	sll	a5,a4,a5
	sw	a5,-36(s0)
	lw	a5,-36(s0)
	seqz	a5,a5
	andi	a5,a5,0xff
	mv	a4,a5
	lw	a5,-20(s0)
	add	a5,a5,a4
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	mv	a0,a5
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	clz32, .-clz32
	.section	.rodata
	.align	2
.LC1:
	.string	"The number of %d's leading zero is udefined\n"
	.align	2
.LC2:
	.string	"The number of %d's leading zero is %d\n"
	.align	2
.LC0:
	.word	0
	.word	837
	.word	-2147483648
	.word	1
	.word	-1035730944
	.word	15728641
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	lui	a5,%hi(.LC0)
	addi	a5,a5,%lo(.LC0)
	lw	a0,0(a5)
	lw	a1,4(a5)
	lw	a2,8(a5)
	lw	a3,12(a5)
	lw	a4,16(a5)
	lw	a5,20(a5)
	sw	a0,-48(s0)
	sw	a1,-44(s0)
	sw	a2,-40(s0)
	sw	a3,-36(s0)
	sw	a4,-32(s0)
	sw	a5,-28(s0)
	sw	zero,-20(s0)
	j	.L12
.L15:
	lw	a5,-20(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-32(a5)
	mv	a0,a5
	call	clz32
	sw	a0,-24(s0)
	lw	a4,-24(s0)
	li	a5,32
	bne	a4,a5,.L13
	lw	a1,-20(s0)
	lui	a5,%hi(.LC1)
	addi	a0,a5,%lo(.LC1)
	call	printf
	j	.L14
.L13:
	lw	a2,-24(s0)
	lw	a1,-20(s0)
	lui	a5,%hi(.LC2)
	addi	a0,a5,%lo(.LC2)
	call	printf
.L14:
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L12:
	lw	a4,-20(s0)
	li	a5,5
	ble	a4,a5,.L15
	li	a5,0
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	main, .-main
	.ident	"GCC: (gc891d8dc2) 13.2.0"
