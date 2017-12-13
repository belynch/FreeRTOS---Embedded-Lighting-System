#include "lpc24xx.h"
#include <stdio.h>
#include <string.h>
#include "controller.h"

/* Maximum task stack size */
#define controllersSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
#define NO_BUTTON_PRESSED 5
#define PIR_TIMER_MS	10000

xQueueHandle controllerQReference;
xTimerHandle timerReference;
UiState * controllerLcdState;

ControllerState state = SET_LEDS;
unsigned char lastButtonState;

void vStartController( unsigned portBASE_TYPE uxPriority, xQueueHandle xCmdQueue, UiState * uiState)
{
	controllerQReference = xCmdQueue;
	controllerLcdState = uiState;
	timerReference = xTimerCreate("Timer",pdMS_TO_TICKS( PIR_TIMER_MS ),pdFALSE,( void * ) 0,vTimerCallback);
	/* Spawn the controller task . */
	xTaskCreate( vControllerTask, "Controller", controllersSTACK_SIZE, NULL, uxPriority, ( xTaskHandle * ) NULL );
	printf("Controller task started ...\r\n");
}


/* Set I2C LED state */
void putLeds(Command * command)
{
	/* Initialise */
	I20CONCLR =  I2C_AA | I2C_SI | I2C_STA | I2C_STO;
	/* Request send START */
	I20CONSET =  I2C_STA;
	/* Wait for START to be sent */
	while (!(I20CONSET & I2C_SI));

	
	/* Request send PCA9532 ADDRESS and R/W bit and clear SI */		
	I20DAT    =  0xC0;
	I20CONCLR =  I2C_SI | I2C_STA;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));

	
	/* PSC0 subaddress + Auto-IncrementI */		
	I20DAT    =  0x12;
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* PRESCALAR 0 */		
	I20DAT    =  0x00;
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* PWM0 DUTY CYCLE */		
	I20DAT    =  command->pwm0DutyCycle;
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* PRESCALAR 1 */		
	I20DAT    =  0x00;
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* PWM1 DUTY CYCLE */		
	I20DAT    =  command->pwm1DutyCycle;
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* LEDS 0 - 3  ->DONT EXIST<- */		
	I20DAT    =  0x00; 
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* LEDS 4 - 7 ->DONT EXIST<- */		
	I20DAT    =  0x00; 
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* LEDS 8 - 11 */		
	I20DAT    =  command->ledSelector; 
	I20CONCLR =  I2C_SI;
	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));
	
	/* LEDS 12 - 15 		
	I20DAT    =  0x00; 
	I20CONCLR =  I2C_SI;
	 Wait for ADDRESS and R/W to be sent 
	while (!(I20CONSET & I2C_SI));*/
	
	/* Request send NAQ and STOP */
	I20CONSET =  I2C_STO;
	I20CONCLR =  I2C_SI | I2C_AA;
	/* Wait for STOP to be sent */
	while (I20CONSET & I2C_STO);
}

/* Get I2C button status */
unsigned char getButtons()
{
	unsigned char ledData;

	/* Initialise */
	I20CONCLR =  I2C_AA | I2C_SI | I2C_STA | I2C_STO;
	
	/* Request send START */
	I20CONSET =  I2C_STA;

	/* Wait for START to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send PCA9532 ADDRESS and R/W bit and clear SI */		
	I20DAT    =  0xC0;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Send control word to read PCA9532 INPUT0 register */
	I20DAT = 0x00;
	I20CONCLR =  I2C_SI;

	/* Wait for DATA with control word to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send repeated START */
	I20CONSET =  I2C_STA;
	I20CONCLR =  I2C_SI;

	/* Wait for START to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send PCA9532 ADDRESS and R/W bit and clear SI */		
	I20DAT    =  0xC1;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));

	I20CONCLR = I2C_SI;

	/* Wait for DATA to be received */
	while (!(I20CONSET & I2C_SI));

	ledData = I20DAT;

	/* Request send NAQ and STOP */
	I20CONSET =  I2C_STO;
	I20CONCLR =  I2C_SI | I2C_AA;

	/* Wait for STOP to be sent */
	while (I20CONSET & I2C_STO);

	return ledData ^ 0xf;
}

static portTASK_FUNCTION( vControllerTask, pvParameters )
{
	Command cmd;
	int buttonPressed = NO_BUTTON_PRESSED;
	( void ) pvParameters;
	printf("Starting controller ...\r\n");

	/* initialise lastButtonState to all LEDS off */
	lastButtonState = 0;
	
	while( 1 )
	{
		switch(state)
		{
			case SET_LEDS :
				if(xQueueReceive(controllerQReference, &cmd, ( TickType_t ) 100)){
					putLeds(&cmd);
					/* starts the timer after detecting LED activity */
					/* calling this on an already active timer resets the timer */
					xTimerStart( timerReference, 0 );
					printf("TIMER STARTED\r\n");
				}
				/* flip controller state */
				state = POLL_BUTTONS;
				break;
			
			case POLL_BUTTONS :
				buttonPressed = pollButtons();
				if(buttonPressed != NO_BUTTON_PRESSED){						
						printf("BUTTON PRESSED: %u\r\n", buttonPressed);
						/* update the UI and global state if required */
						setCommandByButtonId(&cmd, buttonPressed);
						putLeds(&cmd);							
						/* calling xTimerStart on an already active timer resets the timer */
						xTimerStart( timerReference, 0 );
						printf("TIMER STARTED\r\n");
						buttonPressed = NO_BUTTON_PRESSED;

				}
				/* flip controller state */
				state = SET_LEDS;
				break;
		}
	}
}

int pollButtons()
{
	unsigned char buttonState;
	unsigned char changedState;
	unsigned int i;
	unsigned char mask;
	int buttonId = NO_BUTTON_PRESSED;
		
	/* Read buttons */
	buttonState = getButtons();
		
	changedState = buttonState ^ lastButtonState;
		
	if (buttonState != lastButtonState){
		/* iterate over each of the 4 LS bits looking for changes in state */
		for (i = 0; i <= 3; i++){
				mask = 1 << i;						
				if (changedState & mask){
						buttonId = i;
				}
		}
		/* remember new state */
		lastButtonState = buttonState;
	}
	
	return buttonId;
}

void vTimerCallback( TimerHandle_t xTimer )
{
		Command cmd;
		//printf("TIMER COMPLETE\r\n");
		setCommandForTimerExpiry(&cmd);
		putLeds(&cmd);
		xTimerStop( timerReference, 0 );
}
