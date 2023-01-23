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

#if(API_TYPE == POSIX_API)
static msc_pthread_attr_t	Task1_attr;
static msc_pthread_t		Task1ID;

#if(HW_TYPE == HW_VIRTUAL_LINUX)
static msc_pthread_attr_t	Task2_attr;
static msc_pthread_t		Task2ID;
#endif /* HW_VIRTUAL_LINUX */

#else /* POSIX_API */

static OS_TCB        	App_Task1TCB;
static CPU_STK_SIZE		App_Task1Stk[APP_CFG_DEFAULT_TASK_STK_SIZE];

#if(HW_TYPE == HW_VIRTUAL_LINUX)
static  OS_TCB        	App_Task2TCB;
static  CPU_STK_SIZE  	App_Task2Stk[APP_CFG_DEFAULT_TASK_STK_SIZE];
#endif /* HW_VIRTUAL_LINUX */

#endif /* POSIX_API */

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  App_Task1          (void       *p_arg);

#if(HW_TYPE == HW_VIRTUAL_LINUX)
static void App_StartTask2 (void);
static void App_Task2 (void * p_arg);
#endif

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
#if(API_TYPE == POSIX_API)
    int rc1 = 0;
    struct msc_sched_param priority;
    const char *thread1_message = "App 1st Task";
#else
    OS_ERR  err;
#endif

#if(HW_TYPE == HW_EMBEDDED_LPC2129)
	BSP_Init();                             /* Initialize BSP functions */
	LED_Off(0);                             /* Turn OFF all the LEDs    */
	PRINT("\nuC/OS-III running on MCB2100 ...\n");
#else
	PRINT("\nuC/OS-III running on Ubuntu ...\n");
#endif

#if(API_TYPE == POSIX_API)

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
	{
		PRINT("\nErro Pthreads init!!!!");
		while(1);
	}

#else

	PRINT("\nRegular uC/OS-III API ...\n");

	/* Initialize uC/OS-III */
	PRINT("\nOsInit ...");
    OSInit(&err);

	/* Check init errors */
	if(err != OS_ERR_NONE)
	{
		PRINT("\nErro OsInit!!!!");
		while(1);
	}
	else
	{
		PRINT("\nOsInit gone ...");
	}

	/* Create the start task */
	OSTaskCreate((OS_TCB     *)&App_Task1TCB,
                 (CPU_CHAR   *)"App 1st Task",
                 (OS_TASK_PTR )App_Task1,
                 (void       *) 0,
                 (OS_PRIO     )APP_CFG_TASK_1_PRIO,
                 (CPU_STK    *)(CPU_STK *)&App_Task1Stk[0],
                 (CPU_STK     )(APP_CFG_DEFAULT_TASK_STK_SIZE / 10u),
                 (CPU_STK_SIZE)APP_CFG_DEFAULT_TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

	/* Start multitasking (i.e. give control to uC/OS-III) */
	PRINT("\nOSStart ...\n");
	OSStart(&err);

	/* Code should never get here */
	if(err != OS_ERR_NONE)
		PRINT("\nErro Task 1 !!!!");
	else
		PRINT("\nShould never get here!!\n\n");

#endif

	while(1){
    };
}

/*
*********************************************************************************************************
*                                          App_Task1()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_Task1()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

void App_Task1 (void *p_arg)
{
    OS_ERR      os_err;

#if(HW_TYPE == HW_EMBEDDED_LPC2129)
    CPU_INT08U  i;
#endif

    (void)p_arg;                                                /* See Note #1                                          */

#if(HW_TYPE == HW_VIRTUAL_LINUX)
    CPU_Init();
    Mem_Init();                                                 /* Initialize the Memory Management Module              */
    Math_Init();                                                /* Initialize the Mathematical Module                   */
    OS_CPU_SysTickInit();

    /* Cria a 2a Task/Thread */
    App_StartTask2();
#else
	PRINT("\nTask 1 init!");
#endif

    while (1) 
	{
			/* Task body, always written as an infinite loop.       */
    	#if(HW_TYPE == HW_VIRTUAL_LINUX)
    		PRINT("\nuCOS-III Task 1 is running ...");
    		OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
			if(os_err != OS_ERR_NONE)
				PRINT("\nErro 1o delay !!!");
		#else
			for (i = 1; i <= 8; i++)
			{
				PRINT("\nGoing ...");
				LED_On(i);

				OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
				if(os_err != OS_ERR_NONE)
					PRINT("\nErro 1o delay !!!");

				LED_Off(i);
			}

			for (i = 1; i <= 8; i++)
			{
				PRINT("\nComming Back ...");
				LED_On(9 - i);

				OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
				if(os_err != OS_ERR_NONE)
					PRINT("\nErro 2o delay !!!");

				LED_Off(9 - i);
			}
		#endif
    }
}

/*
 * Thread 2
 */
#if(HW_TYPE == HW_VIRTUAL_LINUX)
static void App_StartTask2 (void)
{

#if(API_TYPE == POSIX_API)
    int rc2 = 0;
    struct msc_sched_param priority2;
    const char *thread2_message = "App 2nd Task";
#else
    OS_ERR  err;
#endif


#if(API_TYPE == POSIX_API)

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

#else

    OSTaskCreate((OS_TCB     *)&App_Task2TCB,
                 (CPU_CHAR   *)"App Task 2",
                 (OS_TASK_PTR ) App_Task2,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_2_PRIO,
                 (CPU_STK    *)&App_Task2Stk[0],
                 (CPU_STK     )(APP_CFG_DEFAULT_TASK_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_DEFAULT_TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

#endif

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
#endif

