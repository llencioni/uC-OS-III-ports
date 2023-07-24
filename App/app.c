/*
 * Application Software
 * =====================
 *
 * Port to LPC2129 or Ubuntu (see "app_cfg.h")
 *
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

static OS_TCB        	App_Task1TCB;
static CPU_STK_SIZE		App_Task1Stk[APP_CFG_DEFAULT_TASK_STK_SIZE];

#if(HW_TYPE == HW_VIRTUAL_LINUX)
static  OS_TCB        	App_Task2TCB;
static  CPU_STK_SIZE  	App_Task2Stk[APP_CFG_DEFAULT_TASK_STK_SIZE];
#endif /* HW_VIRTUAL_LINUX */

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
    OS_ERR  err;

#if(HW_TYPE == HW_EMBEDDED_LPC2129)
	BSP_Init();                             /* Initialize BSP functions */
	LED_Off(0);                             /* Turn OFF all the LEDs    */
	PRINT("\nuC/OS-III running on MCB2100 ...\n");
#else
	PRINT("\nuC/OS-III running on Ubuntu ...\n");
#endif

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
		PRINT("\n\nOsInit gone ...");
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
	PRINT("\n\nOSStart ...");
	OSStart(&err);

	/* Code should never get here */
	if(err != OS_ERR_NONE)
		PRINT("\nErro Task 1 !!!!");
	else
		PRINT("\nShould never get here!!\n\n");

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
	PRINT("\n");
#else
	PRINT("\nTask 1 init!");
#endif

    while (1) 
	{
			/* Task body, always written as an infinite loop.       */
    	#if(HW_TYPE == HW_VIRTUAL_LINUX)
    		PRINT("\nuCOS-III Task 1 running ...");
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
    OS_ERR  err;

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

