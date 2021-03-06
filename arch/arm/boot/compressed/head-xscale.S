/* 
 * linux/arch/arm/boot/compressed/head-xscale.S
 * 
 * XScale specific tweaks.  This is merged into head.S by the linker.
 *
 */

#include <linux/config.h>
#include <linux/linkage.h>
#include <asm/mach-types.h>

		.section        ".start", #alloc, #execinstr

__XScale_start:

		@ Preserve r8/r7 i.e. kernel entry values

		@ Be sure to flush kernel binary out of the cache,
		@ whatever state it is, before it is turned off.
		@ This is done by fetching through currently executed
		@ memory to be sure we hit the same cache.

		bic	r2, pc, #0x1f
		add	r3, r2, #0x10000	@ 64 kb is quite enough...
1:		ldr	r0, [r2], #32
		teq	r2, r3
		bne	1b
		mcr	p15, 0, r0, c7, c10, 4	@ drain WB
		mcr	p15, 0, r0, c7, c7, 0	@ flush I & D caches

		@ disabling MMU and caches
		mrc	p15, 0, r0, c1, c0, 0	@ read control reg
		bic	r0, r0, #0x05		@ clear DC, MMU
		bic	r0, r0, #0x1000		@ clear Icache
		mcr	p15, 0, r0, c1, c0, 0
#ifdef	CONFIG_ARCH_IQ80321
	orr	pc, pc, #0xa0000000
	nop
	mov	r7, #MACH_TYPE_IQ80321
#endif

#ifdef CONFIG_ARCH_IQ80310	

		/*
		 * Crank the CPU up to 733MHz
		 */
		mov r1, #9
		mcr p14, 0, r1, c6, c0, 0	

		/*	
		 * Disable ECC error notification
		 * At some point, we should add an ECC handler to Linux
		 */
		mov	r1, #0x1500
		mov	r0, #0x4
		str	r0, [r1, #0x34]	

		mov	r7, #MACH_TYPE_IQ80310
#endif		

#ifdef CONFIG_ARCH_BRH
		mov	r7, #MACH_TYPE_BRH
#endif

#ifdef CONFIG_ARCH_IXDP425
		mov	r7, #MACH_TYPE_IXDP425
#endif

#ifdef CONFIG_MACH_IXDP465
		mov r7, #MACH_TYPE_IXDP465 & 0xff
		orr r7, r7, #MACH_TYPE_IXDP465 & 0xff00
#endif

#ifdef CONFIG_ARCH_PRPMC1100
		mov	r7, #(MACH_TYPE_PRPMC1100 & 0xff)
		orr	r7, r7, #(MACH_TYPE_PRPMC1100 & 0xff00)
#endif

#ifdef CONFIG_ARCH_ADI_COYOTE
		# Set machine ID into r7 (Coyote ID > 255 hence needs
		# two instructions to build the ID safely).
		mov r7, #MACH_TYPE_ADI_COYOTE & 0xff
		orr r7, r7, #MACH_TYPE_ADI_COYOTE & 0xff00
#endif

#ifdef CONFIG_ARCH_SE4000
		mov r7, #MACH_TYPE_SE4000 & 0xff
		orr r7, r7, #MACH_TYPE_SE4000 & 0xff00
#endif

#ifdef CONFIG_MACH_ESS710
		mov r7, #MACH_TYPE_ESS710 & 0xff
		orr r7, r7, #MACH_TYPE_ESS710 & 0xff00
#endif

#ifdef CONFIG_MACH_MONTEJADE
		mov r7, #MACH_TYPE_MONTEJADE & 0xff
		orr r7, r7, #MACH_TYPE_MONTEJADE & 0xff00
#endif

#ifdef CONFIG_MACH_IVPN
		mov r7, #MACH_TYPE_IVPN & 0xff
		orr r7, r7, #MACH_TYPE_IVPN & 0xff00
#endif

#ifdef CONFIG_MACH_SE5100
		mov r7, #MACH_TYPE_SE5100 & 0xff
		orr r7, r7, #MACH_TYPE_SE5100 & 0xff00
#endif

#ifdef CONFIG_MACH_SG560
		mov r7, #MACH_TYPE_SG560 & 0xff
		orr r7, r7, #MACH_TYPE_SG560 & 0xff00
#endif

#ifdef CONFIG_MACH_SG565
		mov r7, #MACH_TYPE_SG565 & 0xff
		orr r7, r7, #MACH_TYPE_SG565 & 0xff00
#endif

#ifdef CONFIG_MACH_SHIVA1100
		mov r7, #MACH_TYPE_SHIVA1100 & 0xff
		orr r7, r7, #MACH_TYPE_SHIVA1100 & 0xff00
#endif

#ifdef CONFIG_MACH_SG580
		mov r7, #MACH_TYPE_SG580 & 0xff
		orr r7, r7, #MACH_TYPE_SG580 & 0xff00
#endif

