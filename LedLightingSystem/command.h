#ifndef COMMANDS_H
#define COMMANDS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define TRUE 1
#define FALSE 0

typedef struct Command
{
		unsigned short lightId;
		unsigned short ledSelector;
		unsigned short pwm0DutyCycle;
		unsigned short pwm1DutyCycle;		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
} Command;

#endif /* COMMANDS_H */
