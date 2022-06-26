# RUN: llvm-mc -triple=aarch64-unknown-linux-gnu -relax-relocations=false \
# RUN:   -position-independent -filetype=obj -o %t.o %s
# RUN: llvm-jitlink -noexec -abs __tlsdesc_resolver=0xcafef00d -check %s %t.o
        .text

        .globl  main
        .p2align  2
        .type main,@function
main:
        ret

        .size   main, .-main

# Check ThreadPointerNullifier pass is working that
# mrs <dest>, TPIDR_EL0 is replaced with mov <dest>, xzr
#
# jitlink-check: *{4}(tlv_block + 0) = 0xaa1f03e8
#                                      mov x8, xzr
        .globl  tlv_block
        .p2align  2
        .type	tlv_block,@function
tlv_block:
        mrs	x8, TPIDR_EL0
        adrp	x0, :tlsdesc:i
        ldr	x1, [x0, :tlsdesc_lo12:i]
        add	x0, x0, :tlsdesc_lo12:i
        .tlsdesccall i
	      blr	x1
        add	x0, x8, x0
        ret
        .size tlv_block, .-tlv_block

	      .globl	i
	      .p2align	2
i:
	      .word	0
	      .size	i, 4
	
	


