/*
 *  linux/include/asm-arm/proc-armv/pgtable.h
 *
 *  Copyright (C) 1995-2001 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  12-Jan-1997	RMK	Altered flushing routines to use function pointers
 *			now possible to combine ARM6, ARM7 and StrongARM versions.
 *  17-Apr-1999	RMK	Now pass an area size to clean_cache_area and
 *			flush_icache_area.
 *  03-Oct-2003 JNZ     Modified for FASS, based on Adam Wiggin's patches
 */
#ifndef __ASM_PROC_PGTABLE_H
#define __ASM_PROC_PGTABLE_H

/*
 * entries per page directory level: they are two-level, so
 * we don't really have any PMD directory.
 */
#define PTRS_PER_PTE		256
#define PTRS_PER_PMD		1
#define PTRS_PER_PGD		4096

/*
 * Hardware page table definitions.
 *
 * + Level 1 descriptor (PMD)
 *   - common
 */
#define PMD_TYPE_MASK		(3 << 0)
#define PMD_TYPE_FAULT		(0 << 0)
#define PMD_TYPE_TABLE		(1 << 0)
#define PMD_TYPE_SECT		(2 << 0)
#define PMD_UPDATABLE		(1 << 4)
#define PMD_DOMAIN(x)		((x) << 5)
#define PMD_PROTECTION		(1 << 9)	/* v5 */
/*
 *   - section
 */
#define PMD_SECT_BUFFERABLE	(1 << 2)
#define PMD_SECT_CACHEABLE	(1 << 3)
#define PMD_SECT_AP_WRITE	(1 << 10)
#define PMD_SECT_AP_READ	(1 << 11)
#define PMD_SECT_TEX(x)		((x) << 12)	/* v5 */
/*
 *   - coarse table
 */

/*
 * + Level 2 descriptor (PTE)
 *   - common
 */
#define PTE_TYPE_MASK		(3 << 0)
#define PTE_TYPE_FAULT		(0 << 0)
#define PTE_TYPE_LARGE		(1 << 0)
#define PTE_TYPE_SMALL		(2 << 0)
#define PTE_TYPE_EXT		(3 << 0)	/* v5 */
#define PTE_BUFFERABLE		(1 << 2)
#define PTE_CACHEABLE		(1 << 3)

/*
 *   - extended small page/tiny page
 */
#define PTE_EXT_AP_UNO_SRO	(0 << 4)
#define PTE_EXT_AP_UNO_SRW	(1 << 4)
#define PTE_EXT_AP_URO_SRW	(2 << 4)
#define PTE_EXT_AP_URW_SRW	(3 << 4)
#define PTE_EXT_TEX(x)		((x) << 6)	/* v5 */

/*
 *   - small page
 */
#define PTE_SMALL_AP_UNO_SRO	(0x00 << 4)
#define PTE_SMALL_AP_UNO_SRW	(0x55 << 4)
#define PTE_SMALL_AP_URO_SRW	(0xaa << 4)
#define PTE_SMALL_AP_URW_SRW	(0xff << 4)
#define PTE_AP_READ		PTE_SMALL_AP_URO_SRW
#define PTE_AP_WRITE		PTE_SMALL_AP_UNO_SRW

/*
 * "Linux" PTE definitions.
 *
 * We keep two sets of PTEs - the hardware and the linux version.
 * This allows greater flexibility in the way we map the Linux bits
 * onto the hardware tables, and allows us to have YOUNG and DIRTY
 * bits.
 *
 * The PTE table pointer refers to the hardware entries; the "Linux"
 * entries are stored 1024 bytes below.
 */
#define L_PTE_PRESENT		(1 << 0)
#define L_PTE_YOUNG		(1 << 1)
#define L_PTE_BUFFERABLE	(1 << 2)	/* matches PTE */
#define L_PTE_CACHEABLE		(1 << 3)	/* matches PTE */
#define L_PTE_USER		(1 << 4)
#define L_PTE_WRITE		(1 << 5)
#define L_PTE_EXEC		(1 << 6)
#define L_PTE_DIRTY		(1 << 7)

#ifndef __ASSEMBLY__

#include <asm/proc/cpd.h>
#include <asm/proc/domain.h>
#include <asm/arch/vmalloc.h>

#define _PAGE_USER_TABLE	(PMD_TYPE_TABLE | PMD_DOMAIN(DOMAIN_USER))
#define _PAGE_KERNEL_TABLE	(PMD_TYPE_TABLE | PMD_DOMAIN(DOMAIN_KERNEL))

#define pmd_bad(pmd)		(pmd_val(pmd) & 2)
#ifdef CONFIG_ARM_FASS
#define set_pmd(pmdp,pmd)	cpd_set_pmd(pmdp, pmd)
#else
#define set_pmd(pmdp,pmd)	cpu_set_pmd(pmdp, pmd)
#endif

static inline pmd_t __mk_pmd(pte_t *ptep, unsigned long prot)
{
	unsigned long pte_ptr = (unsigned long)ptep;
	pmd_t pmd;

	pte_ptr -= PTRS_PER_PTE * sizeof(void *);

	/*
	 * The pmd must be loaded with the physical
	 * address of the PTE table
	 */
	pmd_val(pmd) = __virt_to_phys(pte_ptr) | prot;

	return pmd;
}

static inline unsigned long pmd_page(pmd_t pmd)
{
	unsigned long ptr;

	ptr = pmd_val(pmd) & ~(PTRS_PER_PTE * sizeof(void *) - 1);

	ptr += PTRS_PER_PTE * sizeof(void *);

	return __phys_to_virt(ptr);
}

#define set_pte(ptep, pte)	cpu_set_pte(ptep,pte)

/*
 * The following macros handle the cache and bufferable bits...
 */
#define _L_PTE_DEFAULT	L_PTE_PRESENT | L_PTE_YOUNG
#define _L_PTE_READ	L_PTE_USER | L_PTE_CACHEABLE | L_PTE_BUFFERABLE

#define PAGE_NONE       __pgprot(_L_PTE_DEFAULT)
#define PAGE_COPY       __pgprot(_L_PTE_DEFAULT | _L_PTE_READ)
#define PAGE_SHARED     __pgprot(_L_PTE_DEFAULT | _L_PTE_READ | L_PTE_WRITE)
#define PAGE_READONLY   __pgprot(_L_PTE_DEFAULT | _L_PTE_READ)
#define PAGE_KERNEL     __pgprot(_L_PTE_DEFAULT | L_PTE_CACHEABLE | L_PTE_BUFFERABLE | L_PTE_DIRTY | L_PTE_WRITE)

#define _PAGE_CHG_MASK	(PAGE_MASK | L_PTE_DIRTY | L_PTE_YOUNG)


/*
 * The following only work if pte_present() is true.
 * Undefined behaviour if not..
 */
#define pte_present(pte)		(pte_val(pte) & L_PTE_PRESENT)
#define pte_read(pte)			(pte_val(pte) & L_PTE_USER)
#define pte_write(pte)			(pte_val(pte) & L_PTE_WRITE)
#define pte_exec(pte)			(pte_val(pte) & L_PTE_EXEC)
#define pte_dirty(pte)			(pte_val(pte) & L_PTE_DIRTY)
#define pte_young(pte)			(pte_val(pte) & L_PTE_YOUNG)

#define PTE_BIT_FUNC(fn,op)			\
static inline pte_t pte_##fn(pte_t pte) { pte_val(pte) op; return pte; }

/*PTE_BIT_FUNC(rdprotect, &= ~L_PTE_USER);*/
/*PTE_BIT_FUNC(mkread,    |= L_PTE_USER);*/
PTE_BIT_FUNC(wrprotect, &= ~L_PTE_WRITE);
PTE_BIT_FUNC(mkwrite,   |= L_PTE_WRITE);
PTE_BIT_FUNC(exprotect, &= ~L_PTE_EXEC);
PTE_BIT_FUNC(mkexec,    |= L_PTE_EXEC);
PTE_BIT_FUNC(mkclean,   &= ~L_PTE_DIRTY);
PTE_BIT_FUNC(mkdirty,   |= L_PTE_DIRTY);
PTE_BIT_FUNC(mkold,     &= ~L_PTE_YOUNG);
PTE_BIT_FUNC(mkyoung,   |= L_PTE_YOUNG);

/*
 * Mark the prot value as uncacheable and unbufferable.
 */
#define pgprot_noncached(prot)	__pgprot(pgprot_val(prot) & ~(L_PTE_CACHEABLE | L_PTE_BUFFERABLE))

#endif /* __ASSEMBLY__ */

#endif /* __ASM_PROC_PGTABLE_H */
