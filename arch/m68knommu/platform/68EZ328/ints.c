/*
 * linux/arch/$(ARCH)/platform/$(PLATFORM)/ints.c
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 * Copyright 1996 Roman Zippel
 * Copyright 1999 D. Jeff Dionne <jeff@uClinux.org>
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kernel_stat.h>

#include <asm/system.h>
#include <asm/irq.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/setup.h>

#include <asm/MC68EZ328.h>

#define INTERNAL_IRQS (32)

/* assembler routines */
asmlinkage void buserr(void);
asmlinkage void exception3(void);
asmlinkage void exception4(void);
asmlinkage void exception5(void);
asmlinkage void exception6(void);
asmlinkage void exception7(void);
asmlinkage void exception8(void);
asmlinkage void exception9(void);
asmlinkage void exception10(void);
asmlinkage void exception11(void);
asmlinkage void exception14(void);
asmlinkage void exception15(void);
asmlinkage void bad_interrupt(void);
asmlinkage void inthandler1(void);
asmlinkage void inthandler2(void);
asmlinkage void inthandler3(void);
asmlinkage void inthandler4(void);
asmlinkage void inthandler5(void);
asmlinkage void inthandler6(void);
asmlinkage void inthandler7(void);
asmlinkage void system_call(void);
asmlinkage void trap1(void);
asmlinkage void trap15(void);

// extern void *_ramvec[];
extern e_vector *_ramvec;

/* irq node variables for the 32 (potential) on chip sources */
static irq_node_t *int_irq_list[INTERNAL_IRQS];

static int int_irq_count[INTERNAL_IRQS];
static short int_irq_ablecount[INTERNAL_IRQS];

static void int_badint(int irq, void *dev_id, struct pt_regs *fp)
{
	num_spurious += 1;
}

/*
 * This function should be called during kernel startup to initialize
 * the amiga IRQ handling routines.
 */

void M68EZ328_init_IRQ(void)
{
	int i;

        /* set up the vectors */
        for (i=2; i < 32; ++i)
                _ramvec[i] = bad_interrupt;

        _ramvec[2] = buserr;
        _ramvec[3] = exception3;
        _ramvec[4] = exception4;
        _ramvec[5] = exception5;
        _ramvec[6] = exception6;
        _ramvec[7] = exception7;
        _ramvec[8] = exception8;
        _ramvec[9] = exception9;
        _ramvec[10] = exception10;
        _ramvec[11] = exception11;
        _ramvec[14] = exception14;
        _ramvec[15] = exception15;
        _ramvec[32] = system_call;
        _ramvec[33] = trap1;
        _ramvec[47] = trap15;

        _ramvec[64] = bad_interrupt;
        _ramvec[65] = inthandler1;
        _ramvec[66] = inthandler2;
        _ramvec[67] = inthandler3;
        _ramvec[68] = inthandler4;
        _ramvec[69] = inthandler5;
        _ramvec[70] = inthandler6;
        _ramvec[71] = inthandler7;
 
	IVR = 0x40; /* Set DragonBall IVR (interrupt base) to 64 */

	/* initialize handlers */
	for (i = 0; i < INTERNAL_IRQS; i++) {
		int_irq_list[i] = NULL;

		int_irq_ablecount[i] = 0;
		int_irq_count[i] = 0;
	}
	/* turn off all interrupts */
	IMR = ~0;
}

void M68EZ328_insert_irq(irq_node_t **list, irq_node_t *node)
{
	unsigned long flags;
	irq_node_t *cur;

	if (!node->dev_id)
		printk("%s: Warning: dev_id of %s is zero\n",
		       __FUNCTION__, node->devname);

	save_flags(flags);
	cli();

	cur = *list;

	while (cur) {
		list = &cur->next;
		cur = cur->next;
	}

	node->next = cur;
	*list = node;

	restore_flags(flags);
}

void M68EZ328_delete_irq(irq_node_t **list, void *dev_id)
{
	unsigned long flags;
	irq_node_t *node;

	save_flags(flags);
	cli();

	for (node = *list; node; list = &node->next, node = *list) {
		if (node->dev_id == dev_id) {
			*list = node->next;
			/* Mark it as free. */
			node->handler = NULL;
			restore_flags(flags);
			return;
		}
	}
	restore_flags(flags);
	printk ("%s: tried to remove invalid irq\n", __FUNCTION__);
}

int M68EZ328_request_irq(unsigned int irq, void (*handler)(int, void *, struct pt_regs *),
                         unsigned long flags, const char *devname, void *dev_id)
{
	if (irq >= INTERNAL_IRQS) {
		printk ("%s: Unknown IRQ %d from %s\n", __FUNCTION__, irq, devname);
		return -ENXIO;
	}

	if (!int_irq_list[irq]) {
		int_irq_list[irq] = new_irq_node();
		int_irq_list[irq]->flags   = IRQ_FLG_STD;
	}

	if (!(int_irq_list[irq]->flags & IRQ_FLG_STD)) {
		if (int_irq_list[irq]->flags & IRQ_FLG_LOCK) {
			printk("%s: IRQ %d from %s is not replaceable\n",
			       __FUNCTION__, irq, int_irq_list[irq]->devname);
			return -EBUSY;
		}
		if (flags & IRQ_FLG_REPLACE) {
			printk("%s: %s can't replace IRQ %d from %s\n",
			       __FUNCTION__, devname, irq, int_irq_list[irq]->devname);
			return -EBUSY;
		}
	}
	int_irq_list[irq]->handler = handler;
	int_irq_list[irq]->flags   = flags;
	int_irq_list[irq]->dev_id  = dev_id;
	int_irq_list[irq]->devname = devname;

	/* enable in the IMR */
	if (!int_irq_ablecount[irq])
		*(volatile unsigned long *)0xfffff304 &= ~(1<<irq);

	return 0;
}

void M68EZ328_free_irq(unsigned int irq, void *dev_id)
{
	if (irq >= INTERNAL_IRQS) {
		printk ("%s: Unknown IRQ %d\n", __FUNCTION__, irq);
		return;
	}

	if (int_irq_list[irq]->dev_id != dev_id)
		printk("%s: removing probably wrong IRQ %d from %s\n",
		       __FUNCTION__, irq, int_irq_list[irq]->devname);
	int_irq_list[irq]->handler = int_badint;
	int_irq_list[irq]->flags   = IRQ_FLG_STD;
	int_irq_list[irq]->dev_id  = NULL;
	int_irq_list[irq]->devname = NULL;

	*(volatile unsigned long *)0xfffff304 |= 1<<irq;
}

/*
 * Enable/disable a particular machine specific interrupt source.
 * Note that this may affect other interrupts in case of a shared interrupt.
 * This function should only be called for a _very_ short time to change some
 * internal data, that may not be changed by the interrupt at the same time.
 * int_(enable|disable)_irq calls may also be nested.
 */

void M68EZ328_enable_irq(unsigned int irq)
{
	if (irq >= INTERNAL_IRQS) {
		printk("%s: Unknown IRQ %d\n", __FUNCTION__, irq);
		return;
	}

	if (--int_irq_ablecount[irq])
		return;

	/* enable the interrupt */
	*(volatile unsigned long *)0xfffff304 &= ~(1<<irq);
}

void M68EZ328_disable_irq(unsigned int irq)
{
	if (irq >= INTERNAL_IRQS) {
		printk("%s: Unknown IRQ %d\n", __FUNCTION__, irq);
		return;
	}

	if (int_irq_ablecount[irq]++)
		return;

	/* disable the interrupt */
	*(volatile unsigned long *)0xfffff304 |= 1<<irq;
}

/* The 68k family did not have a good way to determine the source
 * of interrupts until later in the family.  The EC000 core does
 * not provide the vector number on the stack, we vector everything
 * into one vector and look in the blasted mask register...
 * This code is designed to be fast, almost constant time, not clean!
 */
inline int M68EZ328_do_irq(int vec, struct pt_regs *fp)
{
	int irq;
	int mask;

	unsigned long pend = *(volatile unsigned long *)0xfffff30c;

	while (pend) {
		if (pend & 0x0000ffff) {
			if (pend & 0x000000ff) {
				if (pend & 0x0000000f) {
					mask = 0x00000001;
					irq = 0;
				} else {
					mask = 0x00000010;
					irq = 4;
				}
			} else {
				if (pend & 0x00000f00) {
					mask = 0x00000100;
					irq = 8;
				} else {
					mask = 0x00001000;
					irq = 12;
				}
			}
		} else {
			if (pend & 0x00ff0000) {
				if (pend & 0x000f0000) {
					mask = 0x00010000;
					irq = 16;
				} else {
					mask = 0x00100000;
					irq = 20;
				}
			} else {
				if (pend & 0x0f000000) {
					mask = 0x01000000;
					irq = 24;
				} else {
					mask = 0x10000000;
					irq = 28;
				}
			}
		}

		while (! (mask & pend)) {
			mask <<=1;
			irq++;
		}

		if (int_irq_list[irq] && int_irq_list[irq]->handler) {
			int_irq_list[irq]->handler(irq, int_irq_list[irq]->dev_id, fp);
			int_irq_count[irq]++;
		} else {
			printk("unregistered interrupt %d!\nTurning it off in the IMR...\n", irq);
			*(volatile unsigned long *)0xfffff304 |= mask;
		}
		pend &= ~mask;
	}
	return 0;
}

int M68EZ328_get_irq_list(char *buf)
{
	int i, len = 0;
	irq_node_t *node;

	len += sprintf(buf+len, "Internal 68EZ328 interrupts\n");

	for (i = 0; i < INTERNAL_IRQS; i++) {
		if (!(node = int_irq_list[i]))
			continue;
		if (!(node->handler))
			continue;

		len += sprintf(buf+len, " %2d: %10u    %s\n", i,
		               int_irq_count[i], int_irq_list[i]->devname);
	}
	return len;
}

void config_M68EZ328_irq(void)
{
	mach_default_handler = NULL;
	mach_init_IRQ        = M68EZ328_init_IRQ;
	mach_request_irq     = M68EZ328_request_irq;
	mach_free_irq        = M68EZ328_free_irq;
	mach_enable_irq      = M68EZ328_enable_irq;
	mach_disable_irq     = M68EZ328_disable_irq;
	mach_get_irq_list    = M68EZ328_get_irq_list;
	mach_process_int     = M68EZ328_do_irq;
}

void init_irq_proc(void);
void init_irq_proc(void)
{
	/* Insert /proc/irq driver here */
}

