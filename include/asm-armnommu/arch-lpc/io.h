/*
 * linux/include/asm-armnommu/arch-atmel/io.h
 *
 * Copyright (C) 1997-1999 Russell King
 *
 * Modifications:
 *  06-12-1997	RMK	Created.
 *  07-04-1999	RMK	Major cleanup
 *  02-19-2001  gjm     Leveraged for armnommu/dsc21
 */
#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H
#define CONFIG_IO16_BASE    0x00000000

/*
 * kernel/resource.c uses this to initialize the global ioport_resource struct
 * which is used in all calls to request_resource(), allocate_resource(), etc.
 * --gmcnutt
 */
#define IO_SPACE_LIMIT 0xffffffff

/*
 * If we define __io then asm/io.h will take care of most of the inb & friends
 * macros. It still leaves us some 16bit macros to deal with ourselves, though.
 * We don't have PCI or ISA on the dsc21 so I dropped __mem_pci & __mem_isa.
 * --gmcnutt
 */
#define __io(a) (CONFIG_IO16_BASE + (a))
//#define __iob(a) (CONFIG_IO8_BASE + (a))	// byte io address
#define __iob(a) (a)
#define __mem_pci(a)	((unsigned long)(a))	 

#define __arch_getw(a) (*(volatile unsigned short *)(a))
#define __arch_putw(v,a) (*(volatile unsigned short *)(a) = (v))

/*
 * Defining these two gives us ioremap for free. See asm/io.h.
 * --gmcnutt
 */
#define iomem_valid_addr(iomem,sz) (1)
#define iomem_to_phys(iomem) (iomem)

#ifdef CONFIG_CPU_BIG_ENDIAN
#define __io_noswap 1
#endif

#endif

