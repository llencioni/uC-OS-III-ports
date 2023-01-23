/*
*********************************************************************************************************
*                                               Philips LPC210x
*                                  LPC210x Kick Start Card Board Support Package
*
*                                    (c) Copyright 2004, Micrium, Weston, FL
*                                              All Rights Reserved
*
*
* File : BSP.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/


// Serial debug
#define	 CR     0x0D

#define  CPU_CLK_FREQ               14745600L
#define  CPU_PERIPHERAL_CLK_FREQ   (CPU_CLK_FREQ / 4)

// LRL
//#define OS_TICKS_PER_SEC        100    /* Set the number of ticks in one second                        */

/*
*********************************************************************************************************
*                                          GLOBAL DEFINITIONS
*********************************************************************************************************
*/


/***************************************************************************
**
**  VIC Interrupt channels ----  Copied from EWARM iolpc2194.h
**
***************************************************************************/
#define VIC_WDT          0  /* Watchdog                           */
#define VIC_SW           1  /* Software interrupts                */
#define VIC_DEBUGRX      2  /* Embedded ICE, DbgCommRx            */
#define VIC_DEBUGTX      3  /* Embedded ICE, DbgCommTx            */
#define VIC_TIMER0       4  /* Timer 0 (Match 0-3 Capture 0-3)    */
#define VIC_TIMER1       5  /* Timer 1 (Match 0-3 Capture 0-3)    */
#define VIC_UART0        6  /* UART 0  (RLS, THRE, RDA, CTI)      */
#define VIC_UART1        7  /* UART 1  (RLS, THRE, RDA, CTI, MSI) */
#define VIC_PWM0         8  /* PWM 0   (Match 0-6 Capture 0-3)    */
#define VIC_I2C          9  /* I2C     (SI)                       */
#define VIC_SPI0        10  /* SPI0    (SPIF, MODF)               */
#define VIC_SPI1        11  /* SPI1    (SPIF, MODF)               */
#define VIC_PLL         12  /* PLL lock (PLOCK)                   */
#define VIC_RTC         13  /* RTC     (RTCCIF, RTCALF)           */
#define VIC_EINT0       14  /* External interrupt 0 (EINT0)       */
#define VIC_EINT1       15  /* External interrupt 1 (EINT1)       */
#define VIC_EINT2       16  /* External interrupt 2 (EINT2)       */
#define VIC_EINT3       17  /* External interrupt 3 (EINT3)       */
#define VIC_AD          18  /* Analog to digital converter        */
#define VIC_CAN_AF      19  /* CAN and Acceptance Filter          */
#define VIC_CAN1TX      20  /* CAN1 Tx                            */
#define VIC_CAN2TX      21  /* CAN2 Tx                            */
#define VIC_CAN3TX      22  /* CAN3 Tx                            */
#define VIC_CAN4TX      23  /* CAN4 Tx                            */
#define VIC_RES1       	24  /* Reserved                           */
#define VIC_RES2       	25  /* Reserved                           */
#define VIC_CAN1RX      26  /* CAN1 Rx                            */
#define VIC_CAN2RX      27  /* CAN2 Rx                            */
#define VIC_CAN3RX      28  /* CAN3 Rx                            */
#define VIC_CAN4RX      29  /* CAN4 Rx                            */
#define VIC_RES3       	30  /* Reserved                           */
#define VIC_RES4       	31  /* Reserved                           */

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  BSP_Init(void);
void  BSP_LEDCtrl(CPU_INT08U led, CPU_BOOLEAN state);

/*
*********************************************************************************************************
*                                             LED SERVICES
*********************************************************************************************************
*/

void  LED_Init(void);
void  LED_On(CPU_INT08U led);
void  LED_Off(CPU_INT08U led);
void  LED_Toggle(CPU_INT08U led);

/*
*********************************************************************************************************
*                                             TICK SERVICES
*********************************************************************************************************
*/

void  Tmr_TickInit(void);
void  Tmr_TickISR(void);
void  Tmr_TickISR_Handler(void);
void  sendstr (char *p);
int   sendchar (int ch);
void  sendhex (int hex);


