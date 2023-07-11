/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                         uC/OS-III on POSIX
*
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : DC
*                 SB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static msc_pthread_attr_t	Task1_attr;
static msc_pthread_t		Task1ID;

static msc_pthread_attr_t	Task2_attr;
static msc_pthread_t		Task2ID;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  App_Task1          (void       *p_arg);
static void App_StartTask2 (void);
static void App_Task2 (void * p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
    int rc1 = 0;
    struct msc_sched_param priority;
    const char *thread1_message = "App 1st Task";

	PRINT("\nuC/OS-III running on Ubuntu ...\n");
	PRINT("\nPOSIX API ...\n");

	/* Initialize Thread 'attr' params to default values */
	rc1 = msc_pthread_attr_init(&Task1_attr);

	/* Set scheduling policy */
	rc1 |= msc_pthread_attr_setschedpolicy(&Task1_attr, MSC_SCHED_RR);

	/* Set task priority */
	priority.sched_priority = (int) APP_CFG_TASK_1_PRIO;
	rc1 |= msc_pthread_attr_setschedparam(&Task1_attr, &priority);

	/* Create thread */
	rc1 |= msc_pthread_create (&Task1ID,
							   &Task1_attr,
							   (void*)&App_Task1,
							   (void*)thread1_message);

	if(rc1 != MSC_PTHREAD_ERR_NONE)
		PRINT("\nErro Pthreads init!!!!");

	while(1){};
}

/*
* App_Task1
**/

void App_Task1 (void *p_arg)
{
    OS_ERR      os_err;

    (void)p_arg;                                                /* See Note #1                                          */

    CPU_Init();
    Mem_Init();                                                 /* Initialize the Memory Management Module              */
    Math_Init();                                                /* Initialize the Mathematical Module                   */
    OS_CPU_SysTickInit();

    /* Cria a 2a Task/Thread */
    App_StartTask2();

    while (1) 
	{
		/* Task body, always written as an infinite loop.       */
		PRINT("\nuCOS-III Task 1 is running ...");
		OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
		if(os_err != OS_ERR_NONE)
			PRINT("\nErro 1o delay !!!");
    }
}

/*
 * Thread 2
 */
static void App_StartTask2 (void)
{
    int rc2 = 0;
    struct msc_sched_param priority2;
    const char *thread2_message = "App 2nd Task";

	/* Initialize Thread 'attr' params to default values */
	rc2 = msc_pthread_attr_init(&Task2_attr);

	/* Set task priority */
	priority2.sched_priority = (int) APP_CFG_TASK_2_PRIO;
	rc2 |= msc_pthread_attr_setschedparam(&Task2_attr, &priority2);

	/* Create thread */
	rc2 |= msc_pthread_create(&Task2ID,
							  &Task2_attr,
							  (void*)&App_Task2,
							  (void*)thread2_message);

	if(rc2 != MSC_PTHREAD_ERR_NONE)
	{
		PRINT("\nErro Pthreads init!!!!");
		while(1);
	}
}

// LRL
static  void  App_Task2 (void *p_arg)
{
    OS_ERR      os_err;

    (void)p_arg;

    while (1)
	{
		/* Task body, always written as an infinite loop.       */

		OSTimeDlyHMSM(0u, 0u, 5u, 0u,
									OS_OPT_TIME_HMSM_STRICT,
									&os_err);

		#if(HW_TYPE == HW_VIRTUAL_LINUX)
			PRINT("\nuCOS-III Task 2 ....");
		#endif
    }
}


