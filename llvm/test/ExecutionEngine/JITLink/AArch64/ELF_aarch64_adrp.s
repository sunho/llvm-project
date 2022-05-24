# RUN: rm -rf %t && mkdir -p %t
# RUN: llvm-mc -triple=aarch64-unknown-linux-gnu -relax-relocations=false -position-independent -filetype=obj -o %t/elf_aarch64_adrp.o %s
# RUN: llvm-jitlink -noexec -check %s %t/elf_aarch64_adrp.o

        .text

# Test Case 1: R_AARCH64_ADR_PREL_PG_HI21
# jitlink-check: decode_operand(main, 1) = (((str + 0) & 0xFFFFFFFFFFFFF000) - (main & 0xFFFFFFFFFFFFF000))[32:12]
#
# Test Case 2: R_AARCH64_ADD_ABS_LO12_NC
# jitlink-check: decode_operand(main + 4, 2) = (str + 0)[11:0]


        .globl    main
	.p2align	2
        .type main,@function
main:
	adrp	x0, str
	add	x0, x0, :lo12:str
	ret
	.size	main, .-main

        .globl  str
	.type	str,@object
str:
	.asciz	"hello world"
	.size	str, 12
