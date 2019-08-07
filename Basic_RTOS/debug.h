#ifndef DEBUG_H
#define DEBUG_H

#define MASK(x) (1UL << (x))

// Debug Signals on port B
#define DBG_1 1 	
#define DBG_2 2	  
#define DBG_3 3	
#define DBG_4 8		
#define DBG_5 9		
#define DBG_6 10  
#define DBG_7 11

#define DEBUG_START(channel) {PTB->PSOR = MASK(channel);} 
#define DEBUG_STOP(channel) { PTB->PCOR = MASK(channel); }
#define DEBUG_TOGGLE(channel) { PTB->PTOR = MASK(channel); }

void Init_Debug_Signals(void);

#endif // DEBUG_H
