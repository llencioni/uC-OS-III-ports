
#ifndef  _SCHED_H_
#define  _SCHED_H_

/* Scheduling policy */

#define MSC_SCHED_FIFO		0	/* fixed-priority scheduler, between 1 (lowest)
					   	   	   and 99 (highest), in which the highest
					   	   	   priority ready thread runs until it blocks or
					   	   	   is preempted by a higher priority thread */

#define MSC_SCHED_RR		1 	/* Similar to SCHED_FIFO, except that threads at the
					   	   	   same priority level timeslice (round robin)
					   	   	   every 4 × the clock period (see ClockPeriod()) */

#define MSC_SCHED_OTHER		2 /* Currently the same as SCHED_RR */

#define MSC_SCHED_SPORADIC	3 /* sporadic scheduling */


#endif /* _SCHED_H_ */
