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
		IMPORT HardFault_Handler
		MOVS R0, #0
		MSR PRIMASK, R0
		SVC #0
		NOP
		; Actually this function should never return
		; If it does, then branch to HardFault_Handler
		BL HardFault_Handler
		ENDP



; I am using the SVC handler to do the first context switch
SVC_Handler\
		PROC
		EXPORT SVC_Handler
		IMPORT next_task
		IMPORT current_task	
		EXTERN task_status_offset
		
		MOVS R0, #0X02
		MSR CONTROL, R0                     ; use PSP instead of MSP
		ISB                                 ; Flush the pipeline
		
		LDR R0, =next_task                  ;address of the next task
		
		LDR R2, [R0]                        ;Get the structure
		ADDS R2, R2, #4                     ;Point to the enum task_status
		MOVS R1, #1                         ;Move 1 to R1
		STRB R1, [R2]                        ;Store R1 (i.e. 1=Running) to the task_status field
		
		LDR R2, [R0]                        ;Get the struct_task
		LDR R2, [R2]                        ;Get the stack pointer
		
		ADDS R2, R2, #16                    ;Get the location of higher registers
		LDMIA R2!, {R4 - R7}                ;Load the Higher registers first
		MOV R8, R4                          ;
		MOV R9, R5                          ;
		MOV R10, R6                         ;
		MOV R11, R7                         ;
		MSR PSP, R2                         ;Update the PSP location
		SUBS R2, R2, #32                    ;Go back to the original location
		LDMIA R2!, {R4 - R7}		        ;Load the lower registers now
		;MOVS R2, #4
		;MOV R1, LR
		;ORRS R1, R1, R2
		;MOV LR, R1
		MOVS R2, #~0xFFFFFFFD               ;get the value of EXEC_RETURN
		MVNS R1, R2		
		MOV LR, R1                          ;Store it to LR
		BX LR                               ;Branch to next task based on stored context
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
		
		LDR R2, [R1]                        ;Get the struct containg info about the current_task
		ADDS R2, R2, #4                     ;Point to the enum task_status field
		LDRB R3, [R2]                       ;get the (1-byte) value at that location
		CMP R3, #1                          ;If is wasn't in running state (i.e. not preempted)
		BNE Not_Running                     ;      Do nothing
		MOVS R3, #0                         ;Else 
		STRB R3, [R2]                       ;      Store 0 (=Ready, Since preempted) in the task_status
		
Not_Running
Context_Save
		LDR R2, [R1]                        ;Get the address of the current_task
		MRS R3, PSP                         ;Get the PSP value
		SUBS R3, R3, #32                    ;move up (8 register space equivalent)
		STR R3, [R2]                        ;Store this address of stack in the Stack_ptr value of the task
		STMIA R3!, {R4 - R7}                ;Store the lower registers (r4-r7) first
		MOV R4, R8                          ;And ...
		MOV R5, R9                          ;    ...
		MOV R6, R10                         ;    ...
		MOV R7, R11                         ;    ...
		STMIA R3!, {R4 - R7}                ;Save the higher registers (r8-r11)
		
		LDR R2, [R0]                        ;Get the pointer to next_task
		STR R2, [R1]                        ;Save it in current_task (current_task = next_task)
		
		ADDS R3, R2, #4                     ;Get the address of task_status feild
		MOVS R4, #1                         ; and
		STRB R4, [R3]                       ;Store 1 (=Running) at that location
		
Context_Restore
		LDR R2, [R2]                        ;get the stack_ptr value
		ADDS R2, R2, #16                    ;Add 16 (we'll restore higher registers)
		LDMIA R2!, {R4 - R7}                ;Like so...
		MOV R8, R4                          ;        ...
		MOV R9, R5                          ;        ...
		MOV R10, R6                         ;        ...
		MOV R11, R7                         ;        ...
		MSR PSP, R2                         ;Get this stack address to the PSP
		SUBS R2, R2, #32                    ;Adjust the offset we created (+16 and ldmia)
		LDMIA R2!, {R4 - R7}                ;Restore the lower registers
		MOVS R2, #~0xFFFFFFFD ;#0x04        ;Save the EXEC_RETURN value
		MVNS R1, R2                		
		MOV LR, R1                          ;Put that in LR
		
Context_exit
		BX LR                               ;Let hardware figure out where to go 
		                                    ;(according to the PSP we just got using MSR PSP, R2)
		ENDP		
	


PendSV_Handler\
		PROC
		EXPORT PendSV_Handler
		IMPORT os_FindNextTask
		IMPORT next_task
		PUSH {R0, LR}                    ;Store the context
		BL os_FindNextTask               ;Find the next task which is ready to run
		LDR R1, =next_task               ;get the next task pointer
		STR R0, [R1]                     ;Assign the possible task as next_task
		POP {R0, R1}                     ;Restore the context
		MOV LR, R1                       ;Restore LR value (cannot directly pop into LR)
		B Context_Switch                 ;Switch the context
		ALIGN
		ENDP
		



SysTick_Handler\
		PROC
		EXPORT SysTick_Handler
		IMPORT TimerTick
		PUSH {R0, LR}                    ;Save the context
		BL TimerTick                     ; Go to TimerTick function to update timeouts etc
		POP {R0, R1}                     ; Restore the context
		MOV LR, R1                       ; Save LR value (cannot directly pop into LR)
		B Context_Switch                 ; Switch the context
		ALIGN
		ENDP
			
			
	END