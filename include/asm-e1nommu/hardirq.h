#ifndef _HYPERSTONE_NOMMU_HARDIRQ_H_
#define _HYPERSTONE_NOMMU_HARDIRQ_H_

#include <linux/threads.h>

typedef struct {
        unsigned int __softirq_pending;
        unsigned int __local_irq_count;
        unsigned int __local_bh_count;
        unsigned int __syscall_count;
        struct task_struct * __ksoftirqd_task;
} ____cacheline_aligned irq_cpustat_t;

#include <linux/irq_cpustat.h>  /* Standard mappings for irq_cpustat_t above */

/*
 * Are we in an interrupt context? Either doing bottom half
 * or hardware interrupt processing?
 */
#define in_interrupt() (local_irq_count(smp_processor_id()) + local_bh_count(smp_processor_id()) != 0)
#define in_irq() (local_irq_count(smp_processor_id()) != 0)

#define irq_enter(cpu, irq)	(local_irq_count(cpu)++)
#define irq_exit(cpu, irq)	(local_irq_count(cpu)--)

#define hardirq_trylock(cpu)    (local_irq_count(cpu) == 0)
#define hardirq_endlock(cpu)    do { } while (0)

#define synchronize_irq()	barrier()

#endif
