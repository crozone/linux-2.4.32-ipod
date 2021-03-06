/*
 * include/asm-microblaze/irq.h -- Machine interrupt handling
 *
 *  Copyright (C) 2003       John Williams <jwilliams@itee.uq.edu.au>
 *  Copyright (C) 2001,2002  NEC Corporation
 *  Copyright (C) 2001,2002  Miles Bader <miles@gnu.org>
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file COPYING in the main directory of this
 * archive for more details.
 *
 * Written by Miles Bader <miles@gnu.org>
 * Microblaze port by John Williams
 */

#ifndef __MICROBLAZE_IRQ_H__
#define __MICROBLAZE_IRQ_H__

#include <asm/machdep.h>

/* Default NUM_MACH_IRQS.  */
#ifndef NUM_MACH_IRQS
#define NUM_MACH_IRQS	NUM_CPU_IRQS
#endif

/* NMIs have IRQ numbers from FIRST_NMI to FIRST_NMI+NUM_NMIS-1.  */
#define FIRST_NMI	NUM_MACH_IRQS
#define IRQ_NMI(n)	(FIRST_NMI + (n))
/* microblaze processors have 3 non-maskable interrupts.  */
#define NUM_NMIS	3

/* Includes both maskable and non-maskable irqs.  */
#define NR_IRQS		(NUM_MACH_IRQS + NUM_NMIS)


#ifndef __ASSEMBLY__

struct pt_regs;
struct hw_interrupt_type;
struct irqaction;

/* Initialize irq handling for IRQs BASE to BASE+NUM-1 to IRQ_TYPE.
   An IRQ_TYPE of 0 means to use a generic interrupt type.  */
extern void init_irq_handlers (int base_irq, int num,
			       struct hw_interrupt_type *irq_type);

typedef void (*irq_handler_t)(int irq, void *data, struct pt_regs *regs);

/* Handle interrupt IRQ.  REGS are the registers at the time of ther
   interrupt.  */
extern unsigned int handle_irq (int irq, struct pt_regs *regs);

extern void enable_irq(unsigned int);
extern void disable_irq(unsigned int);

#endif /* !__ASSEMBLY__ */

#endif /* __MICROBLAZE_IRQ_H__ */
