/*
*********************************************************************************************************
*                                              uC/OS-III
*                                        The Real-Time Kernel
*
*                    Copyright 2009-2021 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     Keil compiler and LPC2129 Port (MCB2100 board)
*
* File      : os_cpu_c.c
* Version   : V3.08.01
* LRL		: adapted from an originaly file for uC/OS-III port to POSIX/Linux Ubuntu
*********************************************************************************************************
* For       : ARM7 or ARM9
* Mode      : ARM or Thumb
* Toolchain : Keil uV4
*********************************************************************************************************
*/


#define   OS_CPU_GLOBALS

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_cpu_c__c = "$Id: $";
#endif

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <os.h>
#include <os_cfg_app.h>
#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  ARM_MODE_ARM     0x00000000
#define  ARM_MODE_THUMB   0x00000020

// LRL
// As we are not using IAR compiler but Keil, enable just ARM MODE
//#define  ARM_SVC_MODE    (0x00000013L + ARM_MODE_THUMB)
#define  THUMB_SVC_MODE		(0x00000013u + ARM_MODE_THUMB)
#define  ARM_SVC_MODE    	(0x00000013u + ARM_MODE_ARM)

/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do
*              such things as STOP the CPU to conserve power.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void  OSIdleTaskHook (void)
{

#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppIdleTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppIdleTaskHookPtr)();
    }
#endif

}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : None.
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
void  OSInitHook (void)
{

}

/*
*********************************************************************************************************
*                                         STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-III's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void  OSStatTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppStatTaskHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppStatTaskHookPtr)();
    }
#endif
}

/*
*********************************************************************************************************
*                                         TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void  OSTaskCreateHook (OS_TCB  *p_tcb)
{
	/* Debug */
	PRINT("\nTask created: ");
	PRINT(p_tcb->NamePtr);
}

/*
*********************************************************************************************************
*                                         TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
void  OSTaskDelHook (OS_TCB  *p_tcb)
{
	(void)p_tcb;	/* Prevent compiler warning */
}

/*
*********************************************************************************************************
*                                          TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : p_tcb        Pointer to the task control block of the task that is returning.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void  OSTaskReturnHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskReturnHookPtr != (OS_APP_HOOK_TCB)0) {
        (*OS_AppTaskReturnHookPtr)(p_tcb);
    }
#else
    (void)p_tcb;                                                /* Prevent compiler warning                             */
#endif
}

/*
*********************************************************************************************************
*                                      INITIALIZE A TASK'S STACK
*
* Description: This function is called by OS_Task_Create() or OSTaskCreateExt() to initialize the stack
*              frame of the task being created. This function is highly processor specific.
*
* Arguments  : p_task       Pointer to the task entry point address.
*
*              p_arg        Pointer to a user supplied data area that will be passed to the task
*                               when the task first executes.
*
*              p_stk_base   Pointer to the base address of the stack.
*
*              stk_size     Size of the stack, in number of CPU_STK elements.
*
*              opt          Options used to alter the behavior of OS_Task_StkInit().
*                            (see OS.H for OS_TASK_OPT_xxx).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*********************************************************************************************************
*/
CPU_STK  *OSTaskStkInit (OS_TASK_PTR    p_task,
                         void          *p_arg,
                         CPU_STK       *p_stk_base,
                         CPU_STK       *p_stk_limit,
                         CPU_STK_SIZE   stk_size,
                         OS_OPT         opt)
{
    CPU_STK  *p_stk;

    (void)&p_stk_limit;                                     /* Prevent compiler warning                               */
    (void)&opt;

    p_stk     = &p_stk_base[stk_size];                      /* Load stack pointer                                     */
    *--p_stk  = (CPU_STK)p_task;                         	/* Entry Point                                            */
    *--p_stk  = (CPU_STK)0x14141414u;                     	/* R14 (LR)                                               */
    *--p_stk  = (CPU_STK)0x12121212u;                       /* R12                                                    */
    *--p_stk  = (CPU_STK)0x11111111u;                       /* R11                                                    */
    *--p_stk  = (CPU_STK)0x10101010u;                       /* R10                                                    */
    *--p_stk  = (CPU_STK)0x09090909u;                       /* R9                                                     */
    *--p_stk  = (CPU_STK)0x08080808u;                       /* R8                                                     */
    *--p_stk  = (CPU_STK)0x07070707u;                       /* R7                                                     */
    *--p_stk  = (CPU_STK)0x06060606u;                       /* R6                                                     */
    *--p_stk  = (CPU_STK)0x05050505u;                       /* R5                                                     */
    *--p_stk  = (CPU_STK)0x04040404u;                       /* R4                                                     */
    *--p_stk  = (CPU_STK)0x03030303u;                       /* R3                                                     */
    *--p_stk  = (CPU_STK)0x02020202u;                       /* R2                                                     */
    *--p_stk  = (CPU_STK)0x01010101u;                       /* R1                                                     */
    *--p_stk  = (CPU_STK)p_arg;                             /* R0 : argument                                          */
    *--p_stk  = (CPU_STK)ARM_SVC_MODE;                		/* CPSR  (Enable IRQ and FIQ interrupts, ARM-mode)        */

    return (p_stk);
}

/*
*********************************************************************************************************
*                                          TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : None.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task
*                 that will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points
*                 to the task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/
void  OSTaskSwHook (void)
{
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_TS  ts;
#endif
#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_TS  int_dis_time;
#endif

#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTaskSwHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTaskSwHookPtr)();
    }
#endif

#if OS_CFG_TASK_PROFILE_EN > 0u
    ts = OS_TS_GET();
    if (OSTCBCurPtr != OSTCBHighRdyPtr) {
        OSTCBCurPtr->CyclesDelta  = ts - OSTCBCurPtr->CyclesStart;
        OSTCBCurPtr->CyclesTotal += (OS_CYCLES)OSTCBCurPtr->CyclesDelta;
    }

    OSTCBHighRdyPtr->CyclesStart = ts;
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    int_dis_time = CPU_IntDisMeasMaxCurReset();             /* Keep track of per-task interrupt disable time          */
    if (OSTCBCurPtr->IntDisTimeMax < int_dis_time) {
        OSTCBCurPtr->IntDisTimeMax = int_dis_time;
    }
#endif

#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
                                                            /* Keep track of per-task scheduler lock time             */
    if (OSTCBCurPtr->SchedLockTimeMax < (CPU_TS)OSSchedLockTimeMaxCur) {
        OSTCBCurPtr->SchedLockTimeMax = (CPU_TS)OSSchedLockTimeMaxCur;
    }
    OSSchedLockTimeMaxCur = (CPU_TS)0;                      /* Reset the per-task value                               */
#endif

//    PRINT("\nPassou OSTaskSwHook ...");
}

/*
*********************************************************************************************************
*                                              TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : None.
*
* Note(s)    : 1) This function is assumed to be called from the Tick ISR.
*********************************************************************************************************
*/
void  OSTimeTickHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    if (OS_AppTimeTickHookPtr != (OS_APP_HOOK_VOID)0) {
        (*OS_AppTimeTickHookPtr)();
    }
#endif
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


