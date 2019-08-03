;   @Author: Aniruddha Kanhere
;
;   @brief: this file is contains the header for functions of machine dependent code
;	@note: In this file definitions of context switching procedures are defines
; 	       The user should not call these routines or use its data structures.

;define area of this file as assembly_code and allow it to be read_only
	AREA    assembly_code, CODE, READONLY
		

Start_Kernel\
		PROC
		EXPORT Start_Kernel
		MOVS R0, #0
		MSR PRIMASK, R0
		SVC #0
		NOP
		; Actually this function should never return
		BX LR
		ENDP

; I am using the SVC handler to do the first context switch
SVC_Handler\
		PROC
		EXPORT SVC_Handler
		IMPORT next_task
		IMPORT current_task	
		
		MOVS R0, #0X02
		MSR CONTROL, R0
		ISB
		
		LDR R0, =next_task                  ;address of the next task		
		LDR R2, [R0]
		LDR R2, [R2]
		
		ADDS R2, R2, #16
		LDMIA R2!, {R4 - R7}
		MOV R8, R4
		MOV R9, R5
		MOV R10, R6
		MOV R11, R7
		MSR PSP, R2
		SUBS R2, R2, #32
		LDMIA R2!, {R4 - R7}		
		MOVS R2, #4
		MOV R1, LR
		ORRS R1, R1, R2
		MOV LR, R1
		BX LR
		ALIGN
		ENDP
	
		
Context_Switch\
		PROC
		THUMB
		EXPORT Context_Switch
		IMPORT next_task
		IMPORT current_task	
		LDR R0, =next_task                  ;address of the next task
		LDR R1, =current_task               ;address of the current task
		CMP R0, R1                          ;compare both addresses and if same, exit
		BEQ Context_exit
		LDR R2, [R1]                        
		MRS R3, PSP
		SUBS R3, R3, #32
		STR R3, [R2]
		STMIA R3!, {R4 - R7}
		MOV R4, R8
		MOV R5, R9
		MOV R6, R10
		MOV R7, R11
		STMIA R3!, {R4 - R7}
		
		LDR R2, [R0]
		STR R2, [R1]
		LDR R2, [R2]
		ADDS R2, R2, #16
		LDMIA R2!, {R4 - R7}
		MOV R8, R4
		MOV R9, R5
		MOV R10, R6
		MOV R11, R7
		MSR PSP, R2
		SUBS R2, R2, #32
		LDMIA R2!, {R4 - R7}
		MOVS R2, #0x04
		MOV R1, LR
		ORRS R1, R1, R2
		MOV LR, R1
		
Context_exit
		BX LR
		ENDP		
		
		
SysTick_Handler\
		PROC
		EXPORT SysTick_Handler
		IMPORT TimerTick
		PUSH {R0, LR}
		BL TimerTick
		POP {R0, R1}
		MOV LR, R1
		B Context_Switch
		ALIGN
		ENDP
	END