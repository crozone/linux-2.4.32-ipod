/*
 *  linux/arch/arm/lib/csumpartialcopygeneric.S
 *
 *  Copyright (C) 1995-2001 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * - added big endian stuff (Daniel Versick - 2002)
 */

/*
 * unsigned int
 * csum_partial_copy_xxx(const char *src, char *dst, int len, int sum, )
 *  r0 = src, r1 = dst, r2 = len, r3 = sum
 *  Returns : r0 = checksum
 *
 * Note that 'tst' and 'teq' preserve the carry flag.
 */

src	.req	r0
dst	.req	r1
len	.req	r2
sum	.req	r3

.zero:		mov	r0, sum
		load_regs	ea

		/*
		 * Align an unaligned destination pointer.  We know that
		 * we have >= 8 bytes here, so we don't need to check
		 * the length.  Note that the source pointer hasn't been
		 * aligned yet.
		 */
.dst_unaligned:	tst	dst, #1
		beq	.dst_16bit

		load1b	ip
		sub	len, len, #1
#ifndef __ARMEB__
		adcs	sum, sum, ip, lsl #8	@ update checksum
#else
		adcs	sum, sum, ip		@ update checksum
#endif
		strb	ip, [dst], #1
		tst	dst, #2
		moveq	pc, lr			@ dst is now 32bit aligned

.dst_16bit:	load2b	r8, ip
		sub	len, len, #2
#ifndef __ARMEB__
		adcs	sum, sum, r8
		strb	r8, [dst], #1
		adcs	sum, sum, ip, lsl #8
#else
		adcs	sum, sum, r8, lsl #8
		strb	r8, [dst], #1
		adcs	sum, sum, ip
#endif
		strb	ip, [dst], #1
		mov	pc, lr			@ dst is now 32bit aligned

		/*
		 * Handle 0 to 7 bytes, with any alignment of source and
		 * destination pointers.  Note that when we get here, C = 0
		 */
.less8:		teq	len, #0			@ check for zero count
		beq	.zero

		/* we must have at least one byte. */
		tst	dst, #1			@ dst 16-bit aligned
		beq	.less8_aligned

		/* Align dst */
		load1b	ip
		sub	len, len, #1
#ifndef __ARMEB__
		adcs	sum, sum, ip, lsl #8	@ update checksum
#else
		adcs	sum, sum, ip		@ update checksum
#endif
		strb	ip, [dst], #1
		tst	len, #6
		beq	.less8_byteonly

1:		load2b	r8, ip
		sub	len, len, #2
#ifndef __ARMEB__
		adcs	sum, sum, r8
		strb	r8, [dst], #1
		adcs	sum, sum, ip, lsl #8
#else
		adcs	sum, sum, r8, lsl #8
		strb	r8, [dst], #1
		adcs	sum, sum, ip
#endif
		strb	ip, [dst], #1
.less8_aligned:	tst	len, #6
		bne	1b
.less8_byteonly:
		tst	len, #1
		beq	.done
		load1b	r8
#ifndef __ARMEB__
		adcs	sum, sum, r8		@ update checksum
#else
		adcs	sum, sum, r8, lsl #8	@ update checksum
#endif
		strb	r8, [dst], #1
		b	.done

FN_ENTRY
		mov	ip, sp
		save_regs
		sub	fp, ip, #4

		cmp	len, #8			@ Ensure that we have at least
		blo	.less8			@ 8 bytes to copy.

		adds	sum, sum, #0		@ C = 0
		tst	dst, #3			@ Test destination alignment
		blne	.dst_unaligned		@ align destination, return here

		/*
		 * Ok, the dst pointer is now 32bit aligned, and we know
		 * that we must have more than 4 bytes to copy.  Note
		 * that C contains the carry from the dst alignment above.
		 */

		tst	src, #3			@ Test source alignment
		bne	.src_not_aligned

		/* Routine for src & dst aligned */

		bics	ip, len, #15
		beq	2f

1:		load4l	r4, r5, r6, r7
		stmia	dst!, {r4, r5, r6, r7}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		adcs	sum, sum, r6
		adcs	sum, sum, r7
		sub	ip, ip, #16
		teq	ip, #0
		bne	1b

2:		ands	ip, len, #12
		beq	4f
		tst	ip, #8
		beq	3f
		load2l	r4, r5
		stmia	dst!, {r4, r5}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		tst	ip, #4
		beq	4f

3:		load1l	r4
		str	r4, [dst], #4
		adcs	sum, sum, r4

4:		ands	len, len, #3
		beq	.done
		load1l	r4
		tst	len, #2
		beq	.exit
#ifndef __ARMEB__
		adcs	sum, sum, r4, lsl #16
		strb	r4, [dst], #1
		mov	r4, r4, lsr #8
		strb	r4, [dst], #1
		mov	r4, r4, lsr #8
#else
		mov	r5, r4, lsr #16
		mov	r5, r5, lsl #16
		adcs	sum, sum, r5
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
		bic	r4, r4, #255
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
#endif
.exit:
#ifdef	__ARMEB__
		bic	r4, r4, #255
		mov	r4, r4, ror #24
#endif
		tst	len, #1
		strneb	r4, [dst], #1
		andne	r4, r4, #255
#ifndef __ARMEB__
		adcnes	sum, sum, r4
#else
		adcnes	sum, sum, r4, lsl #8	@ TODO: check this
#endif

		/*
		 * If the dst pointer was not 16-bit aligned, we
		 * need to rotate the checksum here to get around
		 * the inefficient byte manipulations in the
		 * architecture independent code.
		 */
.done:		adc	r0, sum, #0
		ldr	sum, [sp, #0]		@ dst
		tst	sum, #1
		movne	sum, r0, lsl #8
		orrne	r0, sum, r0, lsr #24
		load_regs	ea

.src_not_aligned:
		adc	sum, sum, #0		@ include C from dst alignment
		and	ip, src, #3
		bic	src, src, #3
		load1l	r4
		cmp	ip, #2
		beq	.src2_aligned
		bhi	.src3_aligned
		mov	r4, r4, pull #8		@ C = 0
		bics	ip, len, #15
		beq	2f
1:		load4l	r5, r6, r7, r8
		orr	r4, r4, r5, push #24
		mov	r5, r5, pull #8
		orr	r5, r5, r6, push #24
		mov	r6, r6, pull #8
		orr	r6, r6, r7, push #24
		mov	r7, r7, pull #8
		orr	r7, r7, r8, push #24
		stmia	dst!, {r4, r5, r6, r7}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		adcs	sum, sum, r6
		adcs	sum, sum, r7
		mov	r4, r8, pull #8
		sub	ip, ip, #16
		teq	ip, #0
		bne	1b
2:		ands	ip, len, #12
		beq	4f
		tst	ip, #8
		beq	3f
		load2l	r5, r6
		orr	r4, r4, r5, push #24
		mov	r5, r5, pull #8
		orr	r5, r5, r6, push #24
		stmia	dst!, {r4, r5}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		mov	r4, r6, pull #8
		tst	ip, #4
		beq	4f
3:		load1l	r5
		orr	r4, r4, r5, push #24
		str	r4, [dst], #4
		adcs	sum, sum, r4
		mov	r4, r5, pull #8
4:		ands	len, len, #3
		beq	.done
		tst	len, #2
		beq	.exit
#ifndef __ARMEB__
		adcs	sum, sum, r4, lsl #16
		strb	r4, [dst], #1
		mov	r4, r4, lsr #8
		strb	r4, [dst], #1
		mov	r4, r4, lsr #8
#else
		mov	r5, r4, lsr #16
		mov	r5, r5, lsl #16
		adcs	sum, sum, r5
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
		bic	r4, r4, #255
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
#endif
		b	.exit

.src2_aligned:
		mov	r4, r4, pull #16
		adds	sum, sum, #0
		bics	ip, len, #15
		beq	2f
1:		load4l	r5, r6, r7, r8
		orr	r4, r4, r5, push #16
		mov	r5, r5, pull #16
		orr	r5, r5, r6, push #16
		mov	r6, r6, pull #16
		orr	r6, r6, r7, push #16
		mov	r7, r7, pull #16
		orr	r7, r7, r8, push #16
		stmia	dst!, {r4, r5, r6, r7}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		adcs	sum, sum, r6
		adcs	sum, sum, r7
		mov	r4, r8, pull #16
		sub	ip, ip, #16
		teq	ip, #0
		bne	1b
2:		ands	ip, len, #12
		beq	4f
		tst	ip, #8
		beq	3f
		load2l	r5, r6
		orr	r4, r4, r5, push #16
		mov	r5, r5, pull #16
		orr	r5, r5, r6, push #16
		stmia	dst!, {r4, r5}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		mov	r4, r6, pull #16
		tst	ip, #4
		beq	4f
3:		load1l	r5
		orr	r4, r4, r5, push #16
		str	r4, [dst], #4
		adcs	sum, sum, r4
		mov	r4, r5, pull #16
4:		ands	len, len, #3
		beq	.done
		tst	len, #2
		beq	.exit
#ifndef __ARMEB__
		adcs	sum, sum, r4, lsl #16
		strb	r4, [dst], #1
		mov	r4, r4, lsr #8
		strb	r4, [dst], #1
#else
		mov	r5, r4, lsr #16
		mov	r5, r5, lsl #16
		adcs	sum, sum, r5
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
		bic	r4, r4, #255
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
#endif
		tst	len, #1
		beq	.done
		load1b	r4
		b	.exit

.src3_aligned:
		mov	r4, r4, pull #24
		adds	sum, sum, #0
		bics	ip, len, #15
		beq	2f
1:		load4l	r5, r6, r7, r8
		orr	r4, r4, r5, push #8
		mov	r5, r5, pull #24
		orr	r5, r5, r6, push #8
		mov	r6, r6, pull #24
		orr	r6, r6, r7, push #8
		mov	r7, r7, pull #24
		orr	r7, r7, r8, push #8
		stmia	dst!, {r4, r5, r6, r7}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		adcs	sum, sum, r6
		adcs	sum, sum, r7
		mov	r4, r8, pull #24
		sub	ip, ip, #16
		teq	ip, #0
		bne	1b
2:		ands	ip, len, #12
		beq	4f
		tst	ip, #8
		beq	3f
		load2l	r5, r6
		orr	r4, r4, r5, push #8
		mov	r5, r5, pull #24
		orr	r5, r5, r6, push #8
		stmia	dst!, {r4, r5}
		adcs	sum, sum, r4
		adcs	sum, sum, r5
		mov	r4, r6, pull #24
		tst	ip, #4
		beq	4f
3:		load1l	r5
		orr	r4, r4, r5, push #8
		str	r4, [dst], #4
		adcs	sum, sum, r4
		mov	r4, r5, pull #24
4:		ands	len, len, #3
		beq	.done
		tst	len, #2
		beq	.exit
#ifndef __ARMEB__
		adcs	sum, sum, r4, lsl #16
		strb	r4, [dst], #1
		load1l	r4
		strb	r4, [dst], #1
		adcs	sum, sum, r4, lsl #24
		mov	r4, r4, lsr #8
#else
		mov	r5, r4, lsr #16
		mov	r5, r5, lsl #16
		adcs	sum, sum, r5
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
		load1l	r4
		mov	r4, r4, lsl #24
		adcs	sum, sum, r4		/* TODO: check this again */
		mov	r4, r4, ror #24
		strb	r4, [dst], #1
		bic	r4, r4, #255
#endif
		b	.exit
