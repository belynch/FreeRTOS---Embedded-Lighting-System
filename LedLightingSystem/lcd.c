/* 
	Sample task that initialises the EA QVGA LCD display
	with touch screen controller and processes touch screen
	interrupt events.

	Jonathan Dukes (jdukes@scss.tcd.ie)
*/

#include "command.h"
#include "semphr.h"
#include "lcd.h"
#include "lcd_hw.h"
#include <stdio.h>
#include <string.h>

/* Maximum task stack size */
#define lcdSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
#define SAVE_PRESET_TIMER_MS	1000

/* Interrupt handlers */
extern void vLCD_ISREntry( void );
void vLCD_ISRHandler( void );

/* The LCD task. */
static void vLcdTask( void *pvParameters );

/* Semaphore for ISR/task synchronisation */
xSemaphoreHandle xLcdSemphr;
xQueueHandle lcdQReference;
UiState * lcdState;
xTimerHandle savePresetTimer;

void vStartLcd( unsigned portBASE_TYPE uxPriority, xQueueHandle xCmdQueue, UiState * uiState )
{
	vSemaphoreCreateBinary(xLcdSemphr);
	lcdQReference = xCmdQueue;
	savePresetTimer = xTimerCreate("Timer", pdMS_TO_TICKS(SAVE_PRESET_TIMER_MS),pdFALSE,(void *) 0, vSavePresetTimerCallback);
	lcdState = uiState;
	/* Spawn the console task. */
	xTaskCreate( vLcdTask, "Lcd", lcdSTACK_SIZE, &xCmdQueue, uxPriority, ( xTaskHandle * ) NULL );
	printf("LCD task started ...\r\n");
}


static portTASK_FUNCTION( vLcdTask, pvParameters )
{
	unsigned int pressure;
	unsigned int xPos;
	unsigned int yPos;
	Command cmd;
	portTickType xLastWakeTime;
	
	Widget * pressedWidget;

	( void ) pvParameters;

	printf("LCD task running\r\n");

	/* Initialise LCD display */
	/* NOTE: We needed to delay calling lcd_init() until here because it uses
	 * xTaskDelay to implement a delay and, as a result, can only be called from
	 * a task */
	lcd_init();
	lcd_fillScreen(BLACK);
  initialiseState(lcdState);
	initialiseWidgets();
	drawWidgets();
	
	/* Infinite loop blocks waiting for a touch screen interrupt event from
	 * the queue. */
	for( ;; )
	{
		/* Clear TS interrupts (EINT3) */
		/* Reset and (re-)enable TS interrupts on EINT3 */
		EXTINT = 8;						/* Reset EINT3 */

		/* Enable TS interrupt vector (VIC) (vector 17) */
		VICIntEnable = 1 << 17;			/* Enable interrupts on vector 17 */

		/* Block on a queue waiting for an event from the TS interrupt handler */
		xSemaphoreTake(xLcdSemphr, portMAX_DELAY);
				
		/* Disable TS interrupt vector (VIC) (vector 17) */
		VICIntEnClr = 1 << 17;
	
		
		/*#################################################################################*/
		/* +++ This point in the code can be interpreted as a screen button push event +++ */\
		
		/* Measure next sleep interval from this point */
		xLastWakeTime = xTaskGetTickCount();

		/* Start polling the touchscreen pressure and position ( getTouch(...) ) */
		/* Keep polling until pressure == 0 */
		getTouch(&xPos, &yPos, &pressure);
				
		pressedWidget = getPressedWidget(xPos, yPos);
		
		if(pressedWidget && pressedWidget->locked == FALSE){
			
			/***********************************************************************/
			// Start the timer to see if a preset is being held down to save. 
			if(pressedWidget->type == CONFIG)
			{
				xTimerStart(savePresetTimer, 0);
			}
			/***********************************************************************/

			pressedWidget->state = PRESSED;
			while (pressure > 0)
			{
				/* Get current pressure */
				getTouch(&xPos, &yPos, &pressure);
				
				/* re-draw the widget with its updated state */
				lcd_draw_widget(pressedWidget);
				
				/* Delay to give us a 25ms periodic TS pressure sample */
				vTaskDelayUntil( &xLastWakeTime, 25 );
			}
			pressedWidget->state = UNPRESSED;
			
			/***********************************************************************/
			// Identify if the preset/config button has been held down for at least 
			// the duration of the timer
			if(pressedWidget->type == CONFIG)
			{
				if(xTimerIsTimerActive( savePresetTimer ) == pdFALSE){
					xTimerStop( savePresetTimer, 0 );
					// flash the screen to indicate a successful save
					flashWidget(pressedWidget);
					pressedWidget->currentAction = SAVE_PRESET;
				}
				else{
					xTimerStop( savePresetTimer, 0 );
				}
			}
			/***********************************************************************/
			cmd = getCommandFromWidget(pressedWidget);
			/* ADD EVENT TO QUEUE */
			xQueueSendToBack(lcdQReference, &cmd, portMAX_DELAY);
		}
								
		/* +++ This point in the code can be interpreted as a screen button release event +++ */
		drawWidgets();
	}
}

void buttonPushEvent(){
	
}

void vLCD_ISRHandler( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Process the touchscreen interrupt */
	/* We would want to indicate to the task above that an event has occurred */
	xSemaphoreGiveFromISR(xLcdSemphr, &xHigherPriorityTaskWoken);

	EXTINT = 8;					/* Reset EINT3 */
	VICVectAddr = 0;			/* Clear VIC interrupt */

	/* Exit the ISR.  If a task was woken by either a character being received
	or transmitted then a context switch will occur. */
	portEXIT_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void vSavePresetTimerCallback( TimerHandle_t xTimer )
{
		//printf("SAVE PRESET TIMER COMPLETE\r\n");
		xTimerStop( xTimer, 0 );
}

