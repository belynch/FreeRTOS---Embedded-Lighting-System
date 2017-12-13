#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "command.h"
#include "widgets.h"

#define I2C_AA      	0x00000004
#define I2C_SI      	0x00000008
#define I2C_STO     	0x00000010
#define I2C_STA     	0x00000020
#define I2C_I2EN    	0x00000040

#define LS2    				0x08
#define PWM0    			0x03
#define PWM1    			0x05

typedef enum {POLL_BUTTONS, SET_LEDS} ControllerState;

static void vControllerTask( void *pvParameters );
void vStartController(unsigned portBASE_TYPE uxPriority, xQueueHandle xCmdQueue, UiState * uiState);
void putLEDs(Command * command);
unsigned char getButtons(void);
int pollButtons(void);
void vTimerCallback( TimerHandle_t xTimer );

#endif
