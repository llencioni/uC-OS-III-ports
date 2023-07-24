/*
*****************************************************************************
*            ITA - Aeronautics Institute of Technology
*
*         Luiz Rubens Lencioni (luiz.lencioni@gmail.com)
*
* OBS:
*
*   1) API to POSIX Pthreads implementation (original from pthread.h)
*
*   2) To avoid conflicts to underlying virtual HW implementation, all POSIX
*   	Pthreads functions and data types begin with a "msc_"
*
*
*****************************************************************************
*/

#ifndef  _POSIX_PTHREAD_H
#define  _POSIX_PTHREAD_H

#include "sched.h"


/******************************************
* Return types
******************************************/
#define MSC_PTHREAD_ERR_NONE				(0)
#define MSC_PTHREAD_ERR_INVALID_PARAM		(1)
#define MSC_PTHREAD_ERR_PARAM_NOT_ALLOWED	(2)
#define MSC_PTHREAD_ERR_FUNC_NOT_SUPPORTED	(4)


/******************************************
* Declarations from other files (time.h)
******************************************/

typedef long msc_time_t;

struct msc_timespec {
   msc_time_t	msc_tv_sec;	 /* The number of seconds. If specifying an absolute time,
   	   	   	   	   	   	   	   this member is the number of seconds since 1970 */
   long     	msc_tv_nsec; /* The number of nanoseconds */
};


/******************************************
* Pthreads interface definition
******************************************/

/* Stack size (in our case is statically defined */
#define MSC_DEFAULT_STACK_SIZE	APP_CFG_DEFAULT_TASK_STK_SIZE

/* General objects */

/* Thread handle */
typedef unsigned int msc_pthread_t;

struct msc_sched_param {
	int sched_priority;
};

/* To inherit or not the scheduling policy of parent thread */
#define MSC_PTHREAD_INHERIT_SCHED		0
#define MSC_PTHREAD_EXPLICIT_SCHED		1

/* Controls whether the thread is created in a detached state */
#define MSC_PTHREAD_CREATE_JOINABLE		0
#define MSC_PTHREAD_CREATE_DETACHED		1

/* contention Scope attribute values */
#define MSC_PTHREAD_SCOPE_SYSTEM    	0	/* Threads are scheduled against all threads in the system */
#define MSC_PTHREAD_SCOPE_PROCESS   	1


typedef struct {
	OS_TCB		 taskTCB;									/* to uC/OS-III compatibility */
	CPU_STK  	 taskStack[APP_CFG_DEFAULT_TASK_STK_SIZE];	/* to uC/OS-III compatibility */
	CPU_STK * 	 taskStack_pt;								/* to uC/OS-III compatibility */
	CPU_STK_SIZE taskStackSize;								/* to uC/OS-III compatibility */
	int contentionscope;
	int inheritsched;
	int detachstate;	/* The default value of the detachstate attribute shall be PTHREAD_CREATE_JOINABLE */
	int sched;
	struct msc_sched_param param;
	struct msc_timespec starttime, deadline, period;
} msc_pthread_attr_t;


/**********************************************************************
* Pthreads API function prototypes (pthread_attr_* family of functions)
*
* See: http://www.qnx.com/developers/docs/qnxcar2/...
**********************************************************************/

/* Attribute administration */

extern int msc_pthread_attr_init (msc_pthread_attr_t * attr);
extern int msc_pthread_attr_destroy (msc_pthread_attr_t * attr);

/* Flags (Boolean characteristics) */

extern int msc_pthread_attr_setdetachstate (msc_pthread_attr_t * attr, int detachstate);
extern int msc_pthread_attr_getdetachstate (const msc_pthread_attr_t * attr, int* detachstate);
extern int msc_pthread_attr_setinheritsched(msc_pthread_attr_t * attr, int inheritsched );
extern int msc_pthread_attr_getinheritsched(const msc_pthread_attr_t * attr, int * inheritsched);
extern int msc_pthread_attr_setscope(msc_pthread_attr_t * attr, int contentionscope);
extern int msc_pthread_attr_getscope(const msc_pthread_attr_t * attr, int * contentionscope);

/* Stack related */

extern int msc_pthread_attr_setstacksize(msc_pthread_attr_t * attr, unsigned int stacksize);
extern int msc_pthread_attr_getstacksize(const msc_pthread_attr_t * attr, unsigned int * stacksize);

/* Scheduling related */

extern int msc_pthread_attr_setschedpolicy(msc_pthread_attr_t * attr, int policy);
extern int msc_pthread_attr_getschedpolicy(const msc_pthread_attr_t * attr, int * policy);
extern int msc_pthread_attr_setschedparam(msc_pthread_attr_t * attr, const struct msc_sched_param * param);
extern int msc_pthread_attr_getschedparam(const msc_pthread_attr_t * attr, struct msc_sched_param * param);

/* Attributes parameters interface */

extern void msc_pthread_init(void);

extern msc_pthread_t msc_pthread_self (void);

extern int msc_pthread_create (msc_pthread_t * thread_handle,
						   	   const msc_pthread_attr_t * attr,
							   void *(*start_routine)(void*),
							   void * arg);

#endif	/* _POSIX_PTHREAD_H */

