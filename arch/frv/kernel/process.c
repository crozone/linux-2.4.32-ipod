/*
 *  linux/arch/m68k/kernel/process.c
 *
 *  Copyright (C) 1995  Hamish Macdonald
 *
 *  68060 fixes by Jesper Skov
 *
 *  uClinux changes Copyright (C) 2000-2002, David McCullough <davidm@snapgear.com>
 */

/*
 * This file handles the architecture-dependent parts of process handling..
 */

#include <linux/config.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/user.h>
#include <linux/elf.h>
#include <linux/reboot.h>

#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/setup.h>
#include <asm/pgtable.h>
#include <asm/gdb-stub.h>
#include <asm/mb-regs.h>

#include "local.h"

/*
 * Initial task structure. Make this a per-architecture thing,
 * because different architectures tend to have different
 * alignment requirements and potentially different initial
 * setup.
 */
static struct fs_struct init_fs = INIT_FS;
static struct files_struct init_files = INIT_FILES;
static struct signal_struct init_signals = INIT_SIGNALS;
struct mm_struct init_mm = INIT_MM(init_mm);

union task_union init_task_union
__attribute__((section(".data.init_task"), aligned(THREAD_SIZE))) = {
	.task	= INIT_TASK(init_task_union.task),
};

asmlinkage void ret_from_fork(void);

/*
 * The idle loop
 */
static void __attribute__((unused)) default_idle(void)
{
	while (1) {
		schedule();
#ifndef NO_MM
		check_pgt_cache();
#endif
	}
}

#include <asm/pgalloc.h>

struct task_struct *alloc_task_struct(void)
{
	struct task_struct *p = kmalloc(THREAD_SIZE, GFP_KERNEL);
	if (p)
		atomic_set((atomic_t *) (p + 1), 1);
	return p;
}

void free_task_struct(struct task_struct *p)
{
	if (atomic_dec_and_test((atomic_t *) (p + 1)))
		kfree(p);
}

void get_task_struct(struct task_struct *p)
{
	atomic_inc((atomic_t *) (p + 1));
}

static void core_sleep_idle(void)
{
	while (1) {
	  	if (frv_dma_inprogress) {
		  	/* If we put the core to sleep, DMA will be suspended */
		  	schedule();
#ifndef NO_MM
			check_pgt_cache();
#endif
		} else {
		  	cli();
			if (need_resched()) {
			  	sti();
			} else {
#ifdef LED_DEBUG_SLEEP
				/* Show that we're sleeping... */
				__set_LEDS(0x55aa);
#endif
				frv_cpu_core_sleep();
#ifdef LED_DEBUG_SLEEP
				/* ... and that we woke up */
				__set_LEDS(0);
#endif
				mb();
			}

			schedule();

#ifndef NO_MM
			check_pgt_cache();
#endif
		}
	}
}



void (*idle)(void) = core_sleep_idle;
//void (*idle)(void) = default_idle;

/*
 * The idle thread. There's no useful work to be
 * done, so just try to conserve power and have a
 * low exit latency (ie sit in a loop waiting for
 * somebody to say that they'd like to reschedule)
 */
void cpu_idle(void)
{
	/* endless idle loop with no priority at all */
	idle();
}

void machine_restart(char * __unused)
{
	unsigned long reset_addr;
#ifdef CONFIG_GDBSTUB
	gdbstub_exit(0);
#endif

	if (PSR_IMPLE(__get_PSR()) == PSR_IMPLE_FR551)
		reset_addr = 0xfefff500;
	else
		reset_addr = 0xfeff0500;
	
	/* Software reset. */
	asm volatile("      dcef @(gr0,gr0),1 ! membar !"
		     "      sti     %1,@(%0,0) !"
		     "      nop ! nop ! nop ! nop ! nop ! "
		     "      nop ! nop ! nop ! nop ! nop ! "
		     "      nop ! nop ! nop ! nop ! nop ! "
		     "      nop ! nop ! nop ! nop ! nop ! "
		     : : "r" (reset_addr), "r" (1) );

	for (;;)
		;
}

void machine_halt(void)
{
#ifdef CONFIG_GDBSTUB
	gdbstub_exit(0);
#endif

	for (;;);
}

void machine_power_off(void)
{
#ifdef CONFIG_GDBSTUB
	gdbstub_exit(0);
#endif

	for (;;);
}

void flush_thread(void)
{
#if 0 //ndef NO_FPU
	unsigned long zero = 0;
#endif
	set_fs(USER_DS);
}

inline unsigned long user_stack(const struct pt_regs *regs)
{
	while (regs->next_frame)
		regs = regs->next_frame;
	return user_mode(regs) ? regs->sp : 0;
}

asmlinkage int sys_fork(void)
{
#ifdef NO_MM
	/* fork almost works, enough to trick you into looking elsewhere:-( */
	return -EINVAL;
#else
	return do_fork(SIGCHLD, user_stack(__frame), __frame, 0);
#endif
}

asmlinkage int sys_vfork(void)
{
	return do_fork(CLONE_VFORK | CLONE_VM | SIGCHLD, user_stack(__frame), __frame, 0);
}

/*****************************************************************************/
/*
 * clone a process
 * - child_tls (in GR12) is accessed directly by copy_thread()
 */
asmlinkage int sys_clone(unsigned long clone_flags, unsigned long newsp,
			 int __user *parent_tidp, int __user *child_tidp,
			 void __user *child_tls)
{
#if 0 //def CONFIG_UCLINUX
	/* uClinux can't or doesn't support all of the CLONE_* options */
	if (clone_flags & (CLONE_SYSVSEM | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID |
			   CLONE_DETACHED | CLONE_UNTRACED | CLONE_CHILD_SETTID | CLONE_STOPPED))
		return -EINVAL;
#endif

	if (!newsp)
		newsp = user_stack(__frame);

	return do_fork(clone_flags, newsp, __frame, 0);
}

/*****************************************************************************/
/*
 * set up the kernel stack and exception frames for a new process
 */
int copy_thread(int nr, unsigned long clone_flags,
		unsigned long usp, unsigned long topstk,
		struct task_struct *p, struct pt_regs *regs)
{
	struct pt_regs *childregs0, *childregs, *regs0;

	regs0 = __kernel_frame0_ptr;
	childregs0 = (struct pt_regs *) ((unsigned long) p + THREAD_SIZE - USER_CONTEXT_SIZE);
	childregs = childregs0;

	/* set up the userspace frame (the only place that the USP is stored) */
	*childregs0 = *regs0;

	childregs0->gr8		= 0;
	childregs0->sp		= usp;
	childregs0->next_frame	= NULL;

#if 0
	if (clone_flags & CLONE_SETTLS)
		/* access sys_clone()'s child_tls argument directly */
		childregs0->gr29 = regs->gr12;
#endif

	/* set up the return kernel frame if called from kernel_thread() */
	if (regs != regs0) {
		childregs--;
		*childregs = *regs;
		childregs->sp = (unsigned long) childregs0;
		childregs->next_frame = childregs0;
		childregs->gr29 = (unsigned long) p;
	}

	p->thread.frame	 = childregs;
	p->thread.curr	 = p;
	p->thread.sp	 = (unsigned long) childregs;
	p->thread.fp	 = 0;
	p->thread.lr	 = 0;
	p->thread.pc	 = (unsigned long) ret_from_fork;
	p->thread.frame0 = childregs0;

	save_user_regs(p->thread.user);

	return 0;
} /* end copy_thread() */

/*
 * fill in the user structure for a core dump..
 */
void dump_thread(struct pt_regs *regs, struct user *dump)
{
	/* changed the size calculations - should hopefully work better. lbt */
	dump->magic = CMAGIC;
	dump->start_code = 0;
	dump->start_stack = user_stack(regs) & ~(PAGE_SIZE - 1);
	dump->u_tsize = ((unsigned long) current->mm->end_code) >> PAGE_SHIFT;
	dump->u_dsize = ((unsigned long) (current->mm->brk + (PAGE_SIZE-1))) >> PAGE_SHIFT;
	dump->u_dsize -= dump->u_tsize;
	dump->u_ssize = 0;

	if (dump->start_stack < TASK_SIZE)
		dump->u_ssize = ((unsigned long) (TASK_SIZE - dump->start_stack)) >> PAGE_SHIFT;

	dump->regs = *(struct user_context *) regs;
}

/*
 * sys_execve() executes a new program.
 */
asmlinkage int sys_execve(char *name, char **argv, char **envp)
{
	int error;
	char * filename;

	lock_kernel();
	filename = getname(name);
	error = PTR_ERR(filename);
	if (IS_ERR(filename))
		goto out;
	error = do_execve(filename, argv, envp, __frame);
	putname(filename);
 out:
	unlock_kernel();
	return error;
}

/*
 * These bracket the sleeping functions..
 */
extern void scheduling_functions_start_here(void);
extern void scheduling_functions_end_here(void);
#define first_sched	((unsigned long) scheduling_functions_start_here)
#define last_sched	((unsigned long) scheduling_functions_end_here)

unsigned long get_wchan(struct task_struct *p)
{
	struct pt_regs *regs0;
	unsigned long fp, pc;
	unsigned long stack_limit;
	int count = 0;
	if (!p || p == current || p->state == TASK_RUNNING)
		return 0;

	stack_limit = (unsigned long) (p + 1);
	fp = p->thread.fp;
	regs0 = p->thread.frame0;

	do {
		if (fp < stack_limit || fp >= (unsigned long) regs0 || fp & 3)
			return 0;

		pc = ((unsigned long *) fp)[2];

		/* FIXME: This depends on the order of these functions. */
		if (pc < first_sched || pc >= last_sched)
			return pc;

		fp = *(unsigned long *) fp;
	} while (count++ < 16);

	return 0;
}

int elf_check_arch(const struct elf32_hdr *hdr)
{
	unsigned long hsr0 = __get_HSR(0);
	unsigned long psr = __get_PSR();

	if (hdr->e_machine != EM_FRV)
		return 0;

	switch (hdr->e_flags & EF_FRV_GPR_MASK) {
	case EF_FRV_GPR64:
		if ((hsr0 & HSR0_GRN) == HSR0_GRN_32)
			return 0;
	case EF_FRV_GPR32:
	case 0:
		break;
	default:
		return 0;
	}

	switch (hdr->e_flags & EF_FRV_FPR_MASK) {
	case EF_FRV_FPR64:
		if ((hsr0 & HSR0_FRN) == HSR0_FRN_32)
			return 0;
	case EF_FRV_FPR32:
	case EF_FRV_FPR_NONE:
	case 0:
		break;
	default:
		return 0;
	}

	if ((hdr->e_flags & EF_FRV_MULADD) == EF_FRV_MULADD)
		if (PSR_IMPLE(psr) != PSR_IMPLE_FR405 &&
		    PSR_IMPLE(psr) != PSR_IMPLE_FR451)
			return 0;

	switch (hdr->e_flags & EF_FRV_CPU_MASK) {
	case EF_FRV_CPU_GENERIC:
		break;
	case EF_FRV_CPU_FR300:
	case EF_FRV_CPU_SIMPLE:
	case EF_FRV_CPU_TOMCAT:
	default:
		return 0;
	case EF_FRV_CPU_FR400:
		if (PSR_IMPLE(psr) != PSR_IMPLE_FR401 &&
		    PSR_IMPLE(psr) != PSR_IMPLE_FR405 &&
		    PSR_IMPLE(psr) != PSR_IMPLE_FR451 &&
		    PSR_IMPLE(psr) != PSR_IMPLE_FR551)
			return 0;
		break;
	case EF_FRV_CPU_FR405:
		if (PSR_IMPLE(psr) != PSR_IMPLE_FR405 &&
		    PSR_IMPLE(psr) != PSR_IMPLE_FR451)
			return 0;
		break;
	case EF_FRV_CPU_FR450:
		if (PSR_IMPLE(psr) != PSR_IMPLE_FR451)
			return 0;
		break;
	case EF_FRV_CPU_FR500:
		if (PSR_IMPLE(psr) != PSR_IMPLE_FR501)
			return 0;
		break;
	case EF_FRV_CPU_FR550:
		if (PSR_IMPLE(psr) != PSR_IMPLE_FR551)
			return 0;
		break;
	}

	return 1;
}
