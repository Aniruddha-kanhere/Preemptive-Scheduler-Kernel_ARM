*****************************************************
Author: Aniruddha Kanhere

Copyright: Aniruddha Kanhere (2019)

This code is provided "as is" and without any warranty, express or implied, in any form whatsoever. This code has been developed for the FREEDOM dev-board for educational purposes. If any damage occurs to any property due to improper or unsolicited use of this code, then the author cannot be held accountable.
*****************************************************


**Basic RTOS Scheduler**


This section of the repository has the most basic scheduler with round-robin context switching without priority checking. It goes through all the tasks in a circular fashion. In this scheduler, the user has to create the stack space for the tasks. Refer to [main.c](https://github.com/Aniruddha-kanhere/Preemptive-Scheduler-myKernelRTOS/blob/master/Basic_RTOS/main.c) for examples and use.

The device dependent code is in the os_Kernel.s file. Whereas the generic code is kept in the os_Kernel.c file with prototypes in os_Kernel.h file. Include this header file into the project to use the OS related functions.

***Note:-***

This code is develped to be used with FREEDOM development board which has ARM Cortex M0+ processor. If you want to use some other dev-board please change the LED ports or you may end up damaging the controller. Also, if you are using a controller with some different processor, the [os_Kernel.s](https://github.com/Aniruddha-kanhere/Preemptive-Scheduler-myKernelRTOS/blob/master/Basic_RTOS/os_kernel.s) should be tweaked accordingly.
