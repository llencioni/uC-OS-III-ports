/*
*****************************************************************************
*            ITA - Aeronautics Institute of Technology
*
*         Luiz Rubens Lencioni (luiz.lencioni@gmail.com)
*
*   API to POSIX Pthreads implementation (original from pthread.h)
*
*****************************************************************************
*/

#include "includes.h"
#include "posix-pthread.h"


/* pthread_attr_init()
 *
 * Initializes the thread attributes in the thread attribute
 * object 'attr' to their default values
 *
 */
int msc_pthread_attr_init (msc_pthread_attr_t * attr)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		/* Default values */
		attr->detachstate  = MSC_PTHREAD_CREATE_JOINABLE;
		attr->inheritsched = MSC_PTHREAD_INHERIT_SCHED;
		attr->contentionscope = MSC_PTHREAD_SCOPE_SYSTEM;
		attr->sched = MSC_SCHED_FIFO;
		attr->taskStackSize = MSC_DEFAULT_STACK_SIZE;
	}

	return(status);
}

/*
 * msc_pthread_attr_setdetachstate()
 *
 * The detachstate attribute controls whether the thread is created in a
 * detached state.  * If the thread is created detached,  * then use of
 * the ID of the newly created thread by the pthread_detach() or
 * pthread_join ( ) function is an error.
 */
int msc_pthread_attr_setdetachstate (msc_pthread_attr_t * attr, int detachstate)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		switch(detachstate)
		{
			case MSC_PTHREAD_CREATE_DETACHED:
			case MSC_PTHREAD_CREATE_JOINABLE:
				attr->detachstate = detachstate;
				break;

			default:
				status = MSC_PTHREAD_ERR_INVALID_PARAM;
				break;
		}
	}

	return(status);
}

/*
 * msc_pthread_attr_getdetachstate()
 *
 * The detachstate attribute controls whether the thread is created in a
 * detached state.  * If the thread is created detached,  * then use of
 * the ID of the newly created thread by the pthread_detach() or
 * pthread_join ( ) function is an error.
 */
int msc_pthread_attr_getdetachstate (const msc_pthread_attr_t * attr, int* detachstate)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		*detachstate = attr->detachstate;
	}

	return(status);
}

/*
 * msc_pthread_attr_setinheritsched()
 *
 * Sets the inheritsched attribute in the 'attr' argument
 *
 * The supported values of inheritsched shall be:
 *
 * PTHREAD_INHERIT_SCHED: specifies that the thread scheduling attributes
 * 						  shall be inherited from the creating thread, and
 * 						  the scheduling attributes in this attr argument
 * 						  shall be ignored (only one supported by us).
 *
 * PTHREAD_EXPLICIT_SCHED: specifies that the thread scheduling attributes
 * 						   shall be set to the corresponding values from
 * 						   this attributes object.
 *
 */
int msc_pthread_attr_setinheritsched(msc_pthread_attr_t * attr, int inheritsched)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		switch(inheritsched)
		{
			case MSC_PTHREAD_CREATE_DETACHED:
				attr->detachstate = inheritsched;
				break;

			case MSC_PTHREAD_CREATE_JOINABLE:
				/* we do not support different scheduling modes for different tasks*/
				status = MSC_PTHREAD_ERR_PARAM_NOT_ALLOWED;
				break;

			default:
				status = MSC_PTHREAD_ERR_INVALID_PARAM;
				break;
		}
	}

	return(status);
}


/*
 * msc_pthread_attr_getinheritsched()
 *
 * Gets the inheritsched attribute in the 'attr' argument (only detached is supported by us)
 *
 */
int msc_pthread_attr_getinheritsched(const msc_pthread_attr_t * attr, int * inheritsched)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		*inheritsched = attr->detachstate;
	}

	return(status);
}

/*
 * msc_pthread_attr_setscope()
 *
 *
 */
int msc_pthread_attr_setscope(msc_pthread_attr_t *attr, int contentionscope)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		switch(contentionscope)
		{
			case MSC_PTHREAD_SCOPE_SYSTEM:
				attr->contentionscope = contentionscope;
				break;

			case MSC_PTHREAD_SCOPE_PROCESS:
				/* we do not support this scope */
				status = MSC_PTHREAD_ERR_PARAM_NOT_ALLOWED;
				break;

			default:
				status = MSC_PTHREAD_ERR_INVALID_PARAM;
				break;
		}
	}

	return(status);
}

/*
 * msc_pthread_attr_getscope()
 *
 */
int msc_pthread_attr_getscope(const msc_pthread_attr_t * attr, int * contentionscope)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		*contentionscope = attr->contentionscope;
	}

	return(status);
}

/*
 * pthread_attr_setstacksize()
 *
 * Not supported, because stacks are statically allocated during compiling time
 *
 */
int msc_pthread_attr_setstacksize(msc_pthread_attr_t * attr, unsigned int stacksize)
{
	/* Stacks are only statically allocated */
	return(MSC_PTHREAD_ERR_FUNC_NOT_SUPPORTED);
}


int msc_pthread_attr_getstacksize(const msc_pthread_attr_t * attr, unsigned int * stacksize)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		*stacksize = attr->taskStackSize;
	}

	return(status);
}

/*
 * msc_pthread_attr_setschedpolicy()
 *
 * Sets the 'scheduling policy attribute' of the thread attributes
 * object referred to by attr to the value specified in policy.
 *
 * This attribute determines the scheduling policy of a thread
 * created using the thread attributes object attr.
 *
 * The supported values for policy are SCHED_FIFO, SCHED_RR, and
 * SCHED_OTHER
 */
int msc_pthread_attr_setschedpolicy(msc_pthread_attr_t * attr, int policy)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		switch(policy)
		{
			case MSC_SCHED_FIFO:
			case MSC_SCHED_RR:
			case MSC_SCHED_OTHER:
				attr->sched = policy;
				break;

			case MSC_SCHED_SPORADIC:
				/* Not supported */
				status = MSC_PTHREAD_ERR_PARAM_NOT_ALLOWED;
				break;

			default:
				status = MSC_PTHREAD_ERR_INVALID_PARAM;
				break;
		}
	}

	return(status);
}

int msc_pthread_attr_getschedpolicy(const msc_pthread_attr_t * attr, int * policy)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		*policy = attr->sched;
	}

	return(status);
}



int msc_pthread_attr_setschedparam (msc_pthread_attr_t * attr, const struct msc_sched_param * param)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		attr->param.sched_priority = param->sched_priority;
	}

	return(status);
}

int msc_pthread_attr_getschedparam(const msc_pthread_attr_t * attr, struct msc_sched_param * param)
{
	int status = MSC_PTHREAD_ERR_NONE;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		param->sched_priority = attr->param.sched_priority;
	}

	return(status);
}


int msc_pthread_create (msc_pthread_t * thread_handle,
					    const msc_pthread_attr_t * attr,
					    void *(*start_routine)(void*),
					    void * arg)
{
	int status = MSC_PTHREAD_ERR_NONE;
	static unsigned int first_thread = 1;
    OS_ERR  err;

	if(attr == NULL)
	{
		status = MSC_PTHREAD_ERR_INVALID_PARAM;
	}
	else
	{
		/* The 1st thread must initialize uC/OS-III as there is no 'init' function in Pthreads */
		if(first_thread == 1)
		{
			/* Initialize uC/OS-III */
			PRINT("\nOsInit ...");
			#ifdef LRL_MSC_CHANGES
				OSInit(attr->sched, &err);
			#else
				OSInit(&err);
			#endif

			/* Check init errors */
			if(err != OS_ERR_NONE)
			{
				PRINT("\nErro OsInit !!!!");
				while(1);
			}
			else
			{
				PRINT("\nOsInit gone ...\n");
			}
		}

		OSTaskCreate((OS_TCB     *)&attr->taskTCB,
	                 (CPU_CHAR   *)arg,
	                 (OS_TASK_PTR )start_routine,
	                 (void       *) 0,
	                 (OS_PRIO     )attr->param.sched_priority,
					 (CPU_STK    *)&attr->taskStack[0],
	                 (CPU_STK     )(attr->taskStackSize / 10u),
	                 (CPU_STK_SIZE)attr->taskStackSize,
	                 (OS_MSG_QTY  ) 0,
	                 (OS_TICK     ) 0,
	                 (void       *) 0,
	                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	                 (OS_ERR     *)&err);

		/* Check task create errors */
		if(err != OS_ERR_NONE)
		{
			PRINT("\nErro task creation !!!!");
			while(1);
		}
		else
		{
			PRINT("\nTask creation OK !!!!\n");

			/* The 1st thread must also start uC/OS-III scheduling! */
			if(first_thread == 1)
			{
				first_thread = 0;

				/* Start multitasking (i.e. give control to uC/OS-III) */
				PRINT("\nOSStart ...\n");
				OSStart(&err);

				/* Code should never get here */
				if(err != OS_ERR_NONE)
					PRINT("\nErro OSStart!!!!");
				else
					PRINT("\nShould never get here!!\n\n");
			}
		}
	}

	return(status);
}

