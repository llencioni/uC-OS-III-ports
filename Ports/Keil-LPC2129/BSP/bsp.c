/*
*********************************************************************************************************
*                                               Philips LPC210x
*                                     Keil's MCB2100 Board Support Package
*
*                                    (c) Copyright 2005, Micrium, Weston, FL
*                                              All Rights Reserved
*
*
* File : BSP.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

typedef  void (*BSP_PFNCT)(void);


/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

CPU_INT32U  VIC_SpuriousInt;

/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/

void  VIC_Init(void);
void  VIC_Dummy(void);
void  VIC_DummyWDT(void);
void  VIC_DummyTIMER0(void);
void  VIC_DummyTIMER1(void);
void  VIC_DummyUART0(void);
void  VIC_DummyUART1(void);
void  VIC_DummyPWM0(void);
void  VIC_DummyI2C(void);
void  VIC_DummySPI(void);
void  VIC_DummyRTC(void);
void  VIC_DummyEINT0(void);
void  VIC_DummyEINT1(void);
void  VIC_DummyEINT2(void);

/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

/* Initialize Serial Interface       */
void init_serial (void)
{
	PINSEL0 = 0x00050000;                  /* Enable RxD1 and TxD1              */
	U1LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
	U1DLL = 97;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
	U1LCR = 0x03;                          /* DLAB = 0                          */
}


void  BSP_Init (void)
{
    VIC_Init();        /* Initialize the Vectored Interrupt Controller */

    LED_Init();
    Tmr_TickInit();

	init_serial();
}


/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

void  LED_Init (void)
{
    PINSEL1   = 0;                                /* Use GPIO pins P1.16 to P1.23 for LEDs             */
    IO1DIR   |= 0x00FF0000;                       /* Set GPIO pins as outputs                          */

    LED_Off(0);                                   /* Turn OFF all the LEDs                             */
}

/*
*********************************************************************************************************
*                                             LED ON
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to control
*                      0    indicates that you want ALL the LEDs to be ON
*                      1    turns ON LED1 on the board
*                      .
*                      .
*                      8    turns ON LED8 on the board
*********************************************************************************************************
*/

void  LED_On (CPU_INT08U led)
{
    switch (led) {
        case 0:
             IO1SET = 0x00FF0000;
             break;

        case 1:
             IO1SET = 1 << 16;
             break;

        case 2:
             IO1SET = 1 << 17;
             break;

        case 3:
             IO1SET = 1 << 18;
             break;

        case 4:
             IO1SET = 1 << 19;
             break;

        case 5:
             IO1SET = 1 << 20;
             break;

        case 6:
             IO1SET = 1 << 21;
             break;

        case 7:
             IO1SET = 1 << 22;
             break;

        case 8:
             IO1SET = 1 << 23;
             break;
    }
}

/*
*********************************************************************************************************
*                                             LED OFF
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to turn OFF
*                      0    indicates that you want ALL the LEDs to be OFF
*                      1    turns OFF LED1 on the board
*                      .
*                      .
*                      8    turns OFF LED8 on the board
*********************************************************************************************************
*/

void  LED_Off (CPU_INT08U led)
{
    switch (led) {
        case 0:
             IO1CLR = 0x00FF0000;
             break;

        case 1:
             IO1CLR = 1 << 16;
             break;

        case 2:
             IO1CLR = 1 << 17;
             break;

        case 3:
             IO1CLR = 1 << 18;
             break;

        case 4:
             IO1CLR = 1 << 19;
             break;

        case 5:
             IO1CLR = 1 << 20;
             break;

        case 6:
             IO1CLR = 1 << 21;
             break;

        case 7:
             IO1CLR = 1 << 22;
             break;

        case 8:
             IO1CLR = 1 << 23;
             break;
    }
}


/*
*********************************************************************************************************
*                                           IRQ ISR HANDLER
*
* Description : This function is called by OS_CPU_IRQ_ISR() to determine the source of the interrupt
*               and process it accordingly.
*
* Arguments   : none
*********************************************************************************************************
*/

void  OS_CPU_IRQ_ISR_Handler (void)
{
    BSP_PFNCT  pfnct;


    pfnct = (BSP_PFNCT)VICVectAddr;             /* Read the interrupt vector from the VIC               */
    while (pfnct != (BSP_PFNCT)0) {             /* Make sure we don't have a NULL pointer               */
      (*pfnct)();                               /* Execute the ISR for the interrupting device          */
        pfnct = (BSP_PFNCT)VICVectAddr;         /* Read the interrupt vector from the VIC               */
    }
}


/*
*********************************************************************************************************
*                                           FIQ ISR HANDLER
*
* Description : This function is called by OS_CPU_FIQ_ISR() to determine the source of the interrupt
*               and process it accordingly.
*
* Arguments   : none
*********************************************************************************************************
*/

void  OS_CPU_FIQ_ISR_Handler (void)
{
    BSP_PFNCT  pfnct;


    pfnct = (BSP_PFNCT)VICVectAddr;             /* Read the interrupt vector from the VIC               */
    while (pfnct != (BSP_PFNCT)0) {             /* Make sure we don't have a NULL pointer               */
      (*pfnct)();                               /* Execute the ISR for the interrupting device          */
        pfnct = (BSP_PFNCT)VICVectAddr;         /* Read the interrupt vector from the VIC               */
    }
}


/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*********************************************************************************************************
*/

void  Tmr_TickInit (void)
{
                                                 /* VIC TIMER #0 Initialization                        */
    VICIntSelect &= ~(1 << VIC_TIMER0);          /* Enable interrupts                                  */
    VICVectAddr2  = (CPU_INT32U)Tmr_TickISR_Handler; /* Set the vector address                             */
    VICVectCntl2  = 0x20 | VIC_TIMER0;           /* Enable vectored interrupts                         */
    VICIntEnable  =  (1 << VIC_TIMER0);          /* Enable Interrupts                                  */

    T0TCR         = 0;                           /* Disable timer 0.                                   */
    T0PC          = 0;                           /* Prescaler is set to no division.                   */

// LRL - OS_TICKS_PER_SEC has changed to OS_CFG_TICK_RATE_HZ
//    T0MR0         = CPU_PERIPHERAL_CLK_FREQ / OS_TICKS_PER_SEC;  /* Count up to this value.            */
    T0MR0         = CPU_PERIPHERAL_CLK_FREQ / OS_CFG_TICK_RATE_HZ;  /* Count up to this value.            */

    T0MCR         = 3;                           /* Reset and interrupt on MR0 (match register 0).     */
    T0CCR         = 0;                           /* Capture is disabled.                               */
    T0EMR         = 0;                           /* No external match output.                          */
    T0TCR         = 1;                           /* Enable timer 0                                     */
}


/*
*********************************************************************************************************
*                                         TIMER #0 IRQ HANDLER
*
* Description : This function handles the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Arguments   : none
*********************************************************************************************************
*/

void  Tmr_TickISR_Handler (void)
{
		volatile static unsigned int tmr_count = 0;
	
    T0IR        = 0xFF;                 /* Clear timer #0 interrupt                                    */
    OSTimeTick();                       /* If the interrupt is from the tick source, call OSTimeTick() */
    VICVectAddr = 0;

		tmr_count++;
}


/*
*********************************************************************************************************
*                                        Vectored Interrupt Controller
*********************************************************************************************************
*/

void  VIC_Init (void)
{
		// LRL
		// VICIntEnClear = 0xFFFFFFFF;                  /* Disable ALL interrupts                             */
    VICIntEnClr = 0xFFFFFFFF;                  /* Disable ALL interrupts                             */
	
		VICProtection = 0;                           /* Setup interrupt controller                         */

    VICVectAddr1  = (CPU_INT32U)VIC_DummyWDT;        /* Set the vector address                             */
    VICVectAddr2  = (CPU_INT32U)VIC_DummyTIMER0;
    VICVectAddr3  = (CPU_INT32U)VIC_DummyTIMER1;
    VICVectAddr4  = (CPU_INT32U)VIC_DummyUART0;
    VICVectAddr5  = (CPU_INT32U)VIC_DummyUART1;
    VICVectAddr6  = (CPU_INT32U)VIC_DummyPWM0;
    VICVectAddr7  = (CPU_INT32U)VIC_DummyI2C;
    VICVectAddr8  = (CPU_INT32U)VIC_DummySPI;
    VICVectAddr9  = (CPU_INT32U)VIC_DummyRTC;
    VICVectAddr10 = (CPU_INT32U)VIC_DummyEINT0;
    VICVectAddr11 = (CPU_INT32U)VIC_DummyEINT1;
    VICVectAddr12 = (CPU_INT32U)VIC_DummyEINT2;
}


void  VIC_Dummy (void)
{
    while (1) {
        (void)VIC_SpuriousInt;
    }
}


void  VIC_DummyWDT (void)
{
    VIC_SpuriousInt = VIC_WDT;
    VIC_Dummy();
}


void  VIC_DummyTIMER0 (void)
{
    VIC_SpuriousInt = VIC_TIMER0;
    VIC_Dummy();
}


void  VIC_DummyTIMER1 (void)
{
    VIC_SpuriousInt = VIC_TIMER1;
    VIC_Dummy();
}


void  VIC_DummyUART0 (void)
{
    VIC_SpuriousInt = VIC_UART0;
    VIC_Dummy();
}


void  VIC_DummyUART1 (void)
{
    VIC_SpuriousInt = VIC_UART1;
    VIC_Dummy();
}


void  VIC_DummyPWM0 (void)
{
    VIC_SpuriousInt = VIC_UART1;
    VIC_Dummy();
}


void  VIC_DummyI2C (void)
{
    VIC_SpuriousInt = VIC_I2C;
    VIC_Dummy();
}


void  VIC_DummySPI (void)
{
//	LRL
//		VIC_SpuriousInt = VIC_SPI;
    VIC_SpuriousInt = VIC_SPI0;
    VIC_Dummy();
}


void  VIC_DummyRTC (void)
{
    VIC_SpuriousInt = VIC_RTC;
    VIC_Dummy();
}


void  VIC_DummyEINT0 (void)
{
    VIC_SpuriousInt = VIC_EINT0;
    VIC_Dummy();
}


void  VIC_DummyEINT1 (void)
{
    VIC_SpuriousInt = VIC_EINT1;
    VIC_Dummy();
}


void  VIC_DummyEINT2 (void)
{
    VIC_SpuriousInt = VIC_EINT2;
    VIC_Dummy();
}

/* Write string */
void sendstr (char *p)
{
  while (*p)
	{
    sendchar(*p++);
  }
}

/* implementation of putchar (also used by printf function to output data)    */
/* Write character to Serial Port    */
int sendchar (int ch)
{
  if (ch == '\n')
	{
    while (!(U1LSR & 0x20));
    U1THR = CR;                          /* output CR */
  }
  while (!(U1LSR & 0x20));
  return (U1THR = ch);
}

void sendhex (int hex) {                   /* Write Hex Digit to Serial Port  */
  if (hex > 9) sendchar('A' + (hex - 10));
  else         sendchar('0' +  hex);
}


