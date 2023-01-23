;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2005, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           Generic ARM Port
;
; File      : OS_CPU_A.ASM
; Version   : V1.61
; By        : Jean J. Labrosse
;
; For       : ARM7 or ARM9
; Mode      : ARM or Thumb
; Toolchain : IAR's EWARM V4.11a and higher
;********************************************************************************************************

            IMPORT  OSRunning                    ; External references
            IMPORT  OSPrioCur
            IMPORT  OSPrioHighRdy
            IMPORT  OSTCBCurPtr					 ; LRL - name changed from uC/OS-II to uC/OS-III
            IMPORT  OSTCBHighRdyPtr				 ; LRL - name changed from uC/OS-II to uC/OS-III
            IMPORT  OSIntNestingCtr				 ; LRL - name changed from uC/OS-II to uC/OS-III
            IMPORT  OSIntExit
            IMPORT  OSTaskSwHook
            IMPORT  OS_CPU_IRQ_ISR_Handler
            IMPORT  OS_CPU_FIQ_ISR_Handler

			; Functions declared in this file
            EXPORT  OS_CPU_SR_Save        		; protoype in 'cpu.h'
            EXPORT  OS_CPU_SR_Restore			; protoype in 'cpu.h'
            EXPORT  OSStartHighRdy				; protoype in 'os_cpu.h'
            EXPORT  OSCtxSw						; protoype in 'os_cpu.h'
            EXPORT  OSIntCtxSw					; protoype in 'os_cpu.h'
            EXPORT  OS_CPU_IRQ_ISR				; protoype in 'os_cpu.h'
            EXPORT  OS_CPU_FIQ_ISR				; protoype in 'os_cpu.h'


OS_CPU_ARM_CONTROL_INT_DIS        EQU  0xC0                     ; Disable both FIQ and IRQ.
OS_CPU_ARM_MODE_SVC               EQU  0x13
NO_INT      EQU     0xC0                         ; Mask used to disable interrupts (Both FIR and IRQ)
SVC32_MODE  EQU     0x13
FIQ32_MODE  EQU     0x11
IRQ32_MODE  EQU     0x12


;*********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
;                     CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
;                          :
;                          :
;                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
;                          :
;                          :
;                 }
;
;              2) OS_CPU_SaveSR() is implemented as recommended by Atmel's application note:
;
;                    "Disabling Interrupts at Processor Level"
;*********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)
;        CODE32

		REQUIRE8
		PRESERVE8

		AREA CODE, CODE, READONLY
		CODE32

OS_CPU_SR_Save
        MRS     R0,CPSR                     ; Set IRQ and FIQ bits in CPSR to disable all interrupts
        ORR     R1,R0,#NO_INT
        MSR     CPSR_c,R1
        MRS     R1,CPSR                     ; Confirm that CPSR contains the proper interrupt disable flags
        AND     R1,R1,#NO_INT
        CMP     R1,#NO_INT
        BNE     OS_CPU_SR_Save              ; Not properly disabled (try again)
        BX      LR                          ; Disabled, return the original CPSR contents in R0

OS_CPU_SR_Restore
        MSR     CPSR_c,R0
        BX      LR


;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) OSStartHighRdy() MUST:
;              a) Call OSTaskSwHook() then,
;              b) Set OSRunning to TRUE,
;              c) Switch to the highest priority task.
;*********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)
;        CODE32

OSStartHighRdy

    MSR     CPSR_cxsf, #0xD3        ; Switch to SVC mode with IRQ and FIQ disabled

    LDR     R0, =OSTaskSwHook                                   ; OSTaskSwHook();
    MOV     LR, PC
    BX      R0

                                                                ; SWITCH TO HIGHEST PRIORITY TASK:
    LDR     R0, =OSTCBHighRdyPtr                                ;    Get highest priority task TCB address,
    LDR     R0, [R0]                                            ;    Get stack pointer,
    LDR     SP, [R0]                                            ;    Switch to the new stack,

    LDR     R0, [SP], #4                                        ;    Pop new task's CPSR,
    MSR     SPSR_cxsf, R0

    LDMFD   SP!, {R0-R12, LR, PC}^                              ;    Pop new task's context.




;*********************************************************************************************************
;                         PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
;
; Note(s) : 1) OSCtxSw() is called in SYS mode with BOTH FIQ and IRQ interrupts DISABLED
;
;           2) The pseudo-code for OSCtxSw() is:
;              a) Save the current task's context onto the current task's stack
;              b) OSTCBCurPtr->OSTCBStkPtr = SP;
;              c) OSTaskSwHook();
;              d) OSPrioCur             = OSPrioHighRdy;
;              e) OSTCBCurPtr              = OSTCBHighRdyPtr;
;              f) SP                    = OSTCBHighRdyPtr->OSTCBStkPtr;
;              g) Restore the new task's context from the new task's stack
;              h) Return to new task's code
;
;           3) Upon entry:
;              OSTCBCurPtr      points to the OS_TCB of the task to suspend
;              OSTCBHighRdyPtr  points to the OS_TCB of the task to resume
;*********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)
;        CODE32

OSCtxSw
                                        ; SAVE CURRENT TASK'S CONTEXT
        STMFD   SP!, {LR}               ;     Push return address
        STMFD   SP!, {LR}
        STMFD   SP!, {R0-R12}           ;     Push registers
        MRS     R4,  CPSR               ;     Push current CPSR
        TST     LR, #1                  ;     See if called from Thumb mode
        ORRNE   R4,  R4, #0x20          ;     If yes, Set the T-bit
        STMFD   SP!, {R4}

;        LDR     R4, ??OS_TCBCur         ; OSTCBCurPtr->OSTCBStkPtr = SP;
        LDR     R4, =OSTCBCurPtr         ; OSTCBCurPtr->OSTCBStkPtr = SP;
        LDR     R5, [R4]
        STR     SP, [R5]

;        LDR     R0, ??OS_TaskSwHook     ; OSTaskSwHook();
        LDR     R0, =OSTaskSwHook     ; OSTaskSwHook();
        MOV     LR, PC
        BX      R0

;        LDR     R4, ??OS_PrioCur        ; OSPrioCur = OSPrioHighRdy
;        LDR     R5, ??OS_PrioHighRdy
        LDR     R4, =OSPrioCur        ; OSPrioCur = OSPrioHighRdy
        LDR     R5, =OSPrioHighRdy
        LDRB    R6, [R5]
        STRB    R6, [R4]

;        LDR     R4, ??OS_TCBCur         ; OSTCBCurPtr  = OSTCBHighRdyPtr;
;        LDR     R6, ??OS_TCBHighRdy
        LDR     R4, =OSTCBCurPtr         ; OSTCBCurPtr  = OSTCBHighRdyPtr;
        LDR     R6, =OSTCBHighRdyPtr
        LDR     R6, [R6]
        STR     R6, [R4]

        LDR     SP, [R6]                ; SP = OSTCBHighRdyPtr->OSTCBStkPtr;

                                        ; RESTORE NEW TASK'S CONTEXT
        LDMFD   SP!, {R4}               ;    Pop new task's CPSR
        MSR     SPSR_cxsf, R4

        LDMFD   SP!, {R0-R12,LR,PC}^    ;    Pop new task's context


;*********************************************************************************************************
;                   PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
;
; Note(s) : 1) OSIntCtxSw() is called in SYS mode with BOTH FIQ and IRQ interrupts DISABLED
;
;           2) The pseudo-code for OSCtxSw() is:
;              a) OSTaskSwHook();
;              b) OSPrioCur             = OSPrioHighRdy;
;              c) OSTCBCurPtr              = OSTCBHighRdyPtr;
;              d) SP                    = OSTCBHighRdyPtr->OSTCBStkPtr;
;              e) Restore the new task's context from the new task's stack
;              f) Return to new task's code
;
;           3) Upon entry:
;              OSTCBCurPtr      points to the OS_TCB of the task to suspend
;              OSTCBHighRdyPtr  points to the OS_TCB of the task to resume
;*********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)
;        CODE32

OSIntCtxSw
;        LDR     R0, ??OS_TaskSwHook     ; OSTaskSwHook();
        LDR     R0, =OSTaskSwHook     ; OSTaskSwHook();
        MOV     LR, PC
        BX      R0

;        LDR     R4,??OS_PrioCur         ; OSPrioCur = OSPrioHighRdy
;        LDR     R5,??OS_PrioHighRdy
        LDR     R4,=OSPrioCur         ; OSPrioCur = OSPrioHighRdy
        LDR     R5,=OSPrioHighRdy
        LDRB    R6,[R5]
        STRB    R6,[R4]

;        LDR     R4,??OS_TCBCur          ; OSTCBCurPtr  = OSTCBHighRdyPtr;
;        LDR     R6,??OS_TCBHighRdy
        LDR     R4,=OSTCBCurPtr          ; OSTCBCurPtr  = OSTCBHighRdyPtr;
        LDR     R6,=OSTCBHighRdyPtr
        LDR     R6,[R6]
        STR     R6,[R4]

        LDR     SP,[R6]                 ; SP = OSTCBHighRdyPtr->OSTCBStkPtr;

                                        ; RESTORE NEW TASK'S CONTEXT
        LDMFD   SP!, {R4}               ;    Pop new task's CPSR
        MSR     SPSR_cxsf, R4

        LDMFD   SP!, {R0-R12,LR,PC}^    ;    Pop new task's context


;*********************************************************************************************************
;                                      IRQ Interrupt Service Routine
;*********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)
;        CODE32

OS_CPU_IRQ_ISR

        STMFD   SP!, {R1-R3}                   ; PUSH WORKING REGISTERS ONTO IRQ STACK

        MOV     R1, SP                         ; Save   IRQ stack pointer

        ADD     SP, SP,#12                     ; Adjust IRQ stack pointer

        SUB     R2, LR,#4                      ; Adjust PC for return address to task

        MRS     R3, SPSR                       ; Copy SPSR (i.e. interrupted task's CPSR) to R3

; LRL
;		MSR     CPSR_c, #(NO_INT | SVC32_MODE) ; Change to SVC mode
        MSR     CPSR_c, #(NO_INT :OR: SVC32_MODE) ; Change to SVC mode

                                               ; SAVE TASK'S CONTEXT ONTO TASK'S STACK
        STMFD   SP!, {R2}                      ;    Push task's Return PC
        STMFD   SP!, {LR}                      ;    Push task's LR
        STMFD   SP!, {R4-R12}                  ;    Push task's R12-R4

        LDMFD   R1!, {R4-R6}                   ;    Move task's R1-R3 from IRQ stack to SVC stack
        STMFD   SP!, {R4-R6}
        STMFD   SP!, {R0}                      ;    Push task's R0    onto task's stack
        STMFD   SP!, {R3}                      ;    Push task's CPSR (i.e. IRQ's SPSR)

                                               ; HANDLE NESTING COUNTER
;        LDR     R0, ??OS_IntNesting            ; OSIntNestingCtr++;
        LDR     R0, =OSIntNestingCtr            ; OSIntNestingCtr++;
        LDRB    R1, [R0]
        ADD     R1, R1,#1
        STRB    R1, [R0]

        CMP     R1, #1                         ; if (OSIntNestingCtr == 1) {
        BNE     OS_CPU_IRQ_ISR_1

;        LDR     R4, ??OS_TCBCur                ;     OSTCBCurPtr->OSTCBStkPtr = SP
        LDR     R4, =OSTCBCurPtr                ;     OSTCBCurPtr->OSTCBStkPtr = SP
        LDR     R5, [R4]
        STR     SP, [R5]                       ; }

OS_CPU_IRQ_ISR_1
; LRL
;        MSR     CPSR_c, #(NO_INT | IRQ32_MODE) ; Change to IRQ mode (to use the IRQ stack to handle interrupt)
        MSR     CPSR_c, #(NO_INT :OR: IRQ32_MODE) ; Change to IRQ mode (to use the IRQ stack to handle interrupt)

;        LDR     R0, ??OS_CPU_IRQ_ISR_Handler   ; OS_CPU_IRQ_ISR_Handler();
        LDR     R0, =OS_CPU_IRQ_ISR_Handler   ; OS_CPU_IRQ_ISR_Handler();
        MOV     LR, PC
        BX      R0

; LRL
;        MSR     CPSR_c, #(NO_INT | SVC32_MODE) ; Change to SVC mode
        MSR     CPSR_c, #(NO_INT :OR: SVC32_MODE) ; Change to SVC mode

;        LDR     R0, ??OS_IntExit               ; OSIntExit();
        LDR     R0, =OSIntExit               ; OSIntExit();
        MOV     LR, PC
        BX      R0

                                               ; RESTORE NEW TASK'S CONTEXT
        LDMFD   SP!, {R4}                      ;    Pop new task's CPSR
        MSR     SPSR_cxsf, R4

        LDMFD   SP!, {R0-R12,LR,PC}^           ;    Pop new task's context


;*********************************************************************************************************
;                                      FIQ Interrupt Service Routine
;*********************************************************************************************************

;        RSEG CODE:CODE:NOROOT(2)
;        CODE32

OS_CPU_FIQ_ISR

        STMFD   SP!, {R1-R3}                   ; PUSH WORKING REGISTERS ONTO FIQ STACK

        MOV     R1, SP                         ; Save   FIQ stack pointer

        ADD     SP, SP,#12                     ; Adjust FIQ stack pointer

        SUB     R2, LR,#4                      ; Adjust PC for return address to task

        MRS     R3, SPSR                       ; Copy SPSR (i.e. interrupted task's CPSR) to R3

; LRL
;        MSR     CPSR_c, #(NO_INT | SVC32_MODE) ; Change to SVC mode
        MSR     CPSR_c, #(NO_INT :OR: SVC32_MODE) ; Change to SVC mode

                                               ; SAVE TASK'S CONTEXT ONTO TASK'S STACK
        STMFD   SP!, {R2}                      ;    Push task's Return PC
        STMFD   SP!, {LR}                      ;    Push task's LR
        STMFD   SP!, {R4-R12}                  ;    Push task's R12-R4

        LDMFD   R1!, {R4-R6}                   ;    Move task's R1-R3 from FIQ stack to SVC stack
        STMFD   SP!, {R4-R6}
        STMFD   SP!, {R0}                      ;    Push task's R0    onto task's stack
        STMFD   SP!, {R3}                      ;    Push task's CPSR (i.e. FIQ's SPSR)

                                               ; HANDLE NESTING COUNTER
;        LDR     R0, ??OS_IntNesting            ; OSIntNestingCtr++;
        LDR     R0, =OSIntNestingCtr            ; OSIntNestingCtr++;
        LDRB    R1, [R0]
        ADD     R1, R1,#1
        STRB    R1, [R0]

        CMP     R1, #1                         ; if (OSIntNestingCtr == 1) {
        BNE     OS_CPU_FIQ_ISR_1

;        LDR     R4, ??OS_TCBCur                ;     OSTCBCurPtr->OSTCBStkPtr = SP
        LDR     R4, =OSTCBCurPtr                ;     OSTCBCurPtr->OSTCBStkPtr = SP
        LDR     R5, [R4]
        STR     SP, [R5]                       ; }

OS_CPU_FIQ_ISR_1
; LRL
;		MSR     CPSR_c, #(NO_INT | FIQ32_MODE) ; Change to FIQ mode (to use the FIQ stack to handle interrupt)
        MSR     CPSR_c, #(NO_INT :OR: FIQ32_MODE) ; Change to FIQ mode (to use the FIQ stack to handle interrupt)

;        LDR     R0, ??OS_CPU_FIQ_ISR_Handler   ; OS_CPU_FIQ_ISR_Handler();
        LDR     R0, =OS_CPU_FIQ_ISR_Handler   ; OS_CPU_FIQ_ISR_Handler();
        MOV     LR, PC
        BX      R0

; LRL
;        MSR     CPSR_c, #(NO_INT | SVC32_MODE) ; Change to SVC mode
        MSR     CPSR_c, #(NO_INT :OR: SVC32_MODE) ; Change to SVC mode

;        LDR     R0, ??OS_IntExit               ; OSIntExit();
        LDR     R0, =OSIntExit               ; OSIntExit();
        MOV     LR, PC
        BX      R0

                                               ; RESTORE NEW TASK'S CONTEXT
        LDMFD   SP!, {R4}                      ;    Pop new task's CPSR
        MSR     SPSR_cxsf, R4

        LDMFD   SP!, {R0-R12,LR,PC}^           ;    Pop new task's context


        END
