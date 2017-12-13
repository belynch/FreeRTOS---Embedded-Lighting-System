#include "lcd_grph.h"

#define NUM_WIDGETS 16
#define NUM_LEDS 4
#define LIGHT_1 1
#define LIGHT_2 2
#define LIGHT_3 3
#define LIGHT_4 4
#define PWM_0 0
#define PWM_1 1
#define ALL_ON 0x55
#define ALL_OFF 0x00
#define MAX_DUTY_CYCLE 0x9F
#define MIN_DUTY_CYCLE 0x01
#define DUTY_CYCLE_STEP 0x0A
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define POWER_X1POS 15
#define POWER_Y1POS 70
#define POWER_WIDTH 50
#define POWER_HEIGHT 50
#define POWER_GAP 5
#define PWM_SELECTOR_HEIGHT 40
#define SECTION_GAP 20
#define PWM_Y1POS 185
#define PWM_X1POS 40
#define PWM_HEIGHT 35
#define PRESET_HEIGHT 40
#define MASTER_X1POS 0
#define MASTER_HEIGHT 30
#define PRESET_TEXT_HEIGHT_OFFSET 3
#define PRESET_TEXT_WIDTH_OFFSET 15
#define POWER_TEXT_HEIGHT_OFFSET 3
#define POWER_TEXT_WIDTH_OFFSET 3
#define SELECTOR_TEXT_HEIGHT_OFFSET 3
#define SELECTOR_TEXT_WIDTH_OFFSET 10
#define PWM_TEXT_HEIGHT_OFFSET 3
#define PWM_TEXT_WIDTH_OFFSET 10
#define MASTER_TEXT_HEIGHT_OFFSET 3
#define MASTER_TEXT_WIDTH_OFFSET 15

#define PRESET1_LED_STATE 0x86
#define PRESET1_PWM0_DUTY_CYCLE 0x20
#define PRESET1_PWM1_DUTY_CYCLE 0x01


#define PRESET2_LED_STATE 0xE3
#define PRESET2_PWM0_DUTY_CYCLE 0x10
#define PRESET2_PWM1_DUTY_CYCLE 0x60

/* The current state of the lighting system - I copy the values of the wanted preset into this*/
UiState currentState;
// This points the the preset that has been copied into currentState - need this in order to save
// presets
UiState * currentStatePtr;
/* Used by the master switch to save the state of the system */
UiState previousState;
/* Default state, sleep state, and two presets (given values in the initaliseState() function */
UiState defaultState;
UiState sleepState;
UiState preset1;
UiState preset2;

Widget widgets[NUM_WIDGETS]; 

Widget light1Button;
Widget light1PWMButton;
Widget light2Button;
Widget light2PWMButton;
Widget light3Button;
Widget light3PWMButton;
Widget light4Button;
Widget light4PWMButton;
Widget pwm0Text;
Widget pwm0Dim;
Widget pwm0Brighten;
Widget pwm1Text;
Widget pwm1Dim;
Widget pwm1Brighten;
Widget config1;
Widget master;

/******************************************************************************
** Function name:		initialiseState
**
** Descriptions:		Initialises the values of the leds and PWMs in each state
**
** parameters:			None
** Returned value:	None
** 
******************************************************************************/
void initialiseState(UiState * uiState){

	defaultState.globalLightState = ALL_OFF;
	defaultState.pwm0DutyCycle = MIN_DUTY_CYCLE;
	defaultState.pwm1DutyCycle = MIN_DUTY_CYCLE;
	defaultState.currentPreset = DEFAULT;
	
	copyState(&defaultState, &currentState);
	currentStatePtr = &defaultState;
	
	sleepState.globalLightState = ALL_OFF;
	sleepState.pwm0DutyCycle = MIN_DUTY_CYCLE;
	sleepState.pwm1DutyCycle = MIN_DUTY_CYCLE;
	sleepState.currentPreset = SLEEP;

	preset1.globalLightState = PRESET1_LED_STATE;
	preset1.pwm0DutyCycle = PRESET1_PWM0_DUTY_CYCLE;
	preset1.pwm1DutyCycle = PRESET1_PWM1_DUTY_CYCLE;
	preset1.currentPreset = PRESET1;

	preset2.globalLightState = PRESET2_LED_STATE;
	preset2.pwm0DutyCycle = PRESET2_PWM0_DUTY_CYCLE;
	preset2.pwm1DutyCycle = PRESET2_PWM1_DUTY_CYCLE;
	preset2.currentPreset = PRESET2;

}

void copyState(UiState * src, UiState * dst){
	dst->globalLightState = src->globalLightState;
	dst->pwm0DutyCycle = src->pwm0DutyCycle;
	dst->pwm1DutyCycle = src->pwm1DutyCycle;
	dst->currentPreset = src->currentPreset;
}

/******************************************************************************
** Function name:		initialiseWidgets
**
** Descriptions:		Creates each button in the UI
**
** parameters:			None
** Returned value:	None
** 
******************************************************************************/
void initialiseWidgets()
{	
	/*################# LIGHT 1 ################# */
	/* POWER BUTTON */
	light1Button.x1Pos = POWER_X1POS;
	light1Button.y1Pos = POWER_Y1POS;
	light1Button.x2Pos = POWER_X1POS + POWER_WIDTH;
	light1Button.y2Pos = POWER_Y1POS + POWER_HEIGHT;
	light1Button.unpressedColour = PUMPKIN;
	light1Button.pressedColour = SUNFLOWER;
	light1Button.currentColour = light1Button.unpressedColour;
	light1Button.text = LIGHT_ONE_TEXT;
	light1Button.textX1Pos = light1Button.x1Pos + (POWER_WIDTH/2) - POWER_TEXT_WIDTH_OFFSET;
	light1Button.textY1Pos = light1Button.y1Pos + (POWER_HEIGHT/2) - POWER_TEXT_HEIGHT_OFFSET;
	light1Button.lightId = LIGHT_1;
	light1Button.type = POWER;
	light1Button.locked = FALSE;	
	light1Button.currentPowerState = OFF;
	widgets[0] = light1Button;
	
	/* PWM */
	light1PWMButton.x1Pos = POWER_X1POS;
	light1PWMButton.y1Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP;
	light1PWMButton.x2Pos = POWER_X1POS + POWER_WIDTH;
	light1PWMButton.y2Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP + PWM_SELECTOR_HEIGHT;
	light1PWMButton.currentColour = NEPHRITIS;
	light1PWMButton.defaultSelectorColour = NEPHRITIS;
	light1PWMButton.pwm0SelectorColour = TURQUOISE;
	light1PWMButton.pwm1SelectorColour = PETER_RIVER;
	light1PWMButton.text = PWM_NONE_SELECTED;
	light1PWMButton.textX1Pos = light1PWMButton.x1Pos + (POWER_WIDTH/2) - SELECTOR_TEXT_WIDTH_OFFSET;
	light1PWMButton.textY1Pos = light1PWMButton.y1Pos + (PWM_SELECTOR_HEIGHT/2) - SELECTOR_TEXT_HEIGHT_OFFSET;
	light1PWMButton.lightId = LIGHT_1;
	light1PWMButton.locked = FALSE;	
	light1PWMButton.type = PWM_SELECTOR;
	widgets[1] = light1PWMButton;
	
	
	/*################# LIGHT 2 ################# */
	/* BUTTON 1 */
	light2Button.x1Pos = POWER_X1POS + POWER_WIDTH + POWER_GAP;
	light2Button.y1Pos = POWER_Y1POS;
	light2Button.x2Pos = POWER_X1POS + (POWER_WIDTH * 2) + POWER_GAP;
	light2Button.y2Pos = POWER_Y1POS + POWER_HEIGHT;
	light2Button.unpressedColour = PUMPKIN;
	light2Button.pressedColour = SUNFLOWER;
	light2Button.currentColour = light2Button.unpressedColour;
	light2Button.text = LIGHT_TWO_TEXT;
	light2Button.textX1Pos = light2Button.x1Pos + (POWER_WIDTH/2) - POWER_TEXT_WIDTH_OFFSET;
	light2Button.textY1Pos = light2Button.y1Pos + (POWER_HEIGHT/2) - POWER_TEXT_HEIGHT_OFFSET;
	light2Button.lightId = LIGHT_2;
	light2Button.type = POWER;
	light2Button.locked = FALSE;
	light2Button.currentPowerState = OFF;
	widgets[2] = light2Button;
	
	/* PWM */
	light2PWMButton.x1Pos = POWER_X1POS + POWER_WIDTH + POWER_GAP;
	light2PWMButton.y1Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP;
	light2PWMButton.x2Pos = POWER_X1POS + (POWER_WIDTH * 2) + POWER_GAP;
	light2PWMButton.y2Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP + PWM_SELECTOR_HEIGHT;
	light2PWMButton.currentColour = NEPHRITIS;
	light2PWMButton.defaultSelectorColour = NEPHRITIS;
	light2PWMButton.pwm0SelectorColour = TURQUOISE;
	light2PWMButton.pwm1SelectorColour = PETER_RIVER;
	light2PWMButton.text = PWM_NONE_SELECTED;
	light2PWMButton.textX1Pos = light2PWMButton.x1Pos + (POWER_WIDTH/2) - SELECTOR_TEXT_WIDTH_OFFSET;
	light2PWMButton.textY1Pos = light2PWMButton.y1Pos + (PWM_SELECTOR_HEIGHT/2) - SELECTOR_TEXT_HEIGHT_OFFSET;
	light2PWMButton.lightId = LIGHT_2;
	light2PWMButton.type = PWM_SELECTOR;
	light2PWMButton.locked = FALSE;
	widgets[3] = light2PWMButton;
	
	
	/*################# LIGHT 3 ################# */
	/* BUTTON 1 */
	light3Button.x1Pos = POWER_X1POS + (POWER_WIDTH * 2) + (POWER_GAP * 2);
	light3Button.y1Pos = POWER_Y1POS;
	light3Button.x2Pos = POWER_X1POS + (POWER_WIDTH * 3) + (POWER_GAP * 2);
	light3Button.y2Pos = POWER_Y1POS + POWER_HEIGHT;
	light3Button.unpressedColour = PUMPKIN;
	light3Button.pressedColour = SUNFLOWER;
	light3Button.currentColour = light3Button.unpressedColour;
	light3Button.text = LIGHT_THREE_TEXT;
	light3Button.textX1Pos = light3Button.x1Pos + (POWER_WIDTH/2) - POWER_TEXT_WIDTH_OFFSET;
	light3Button.textY1Pos = light3Button.y1Pos + (POWER_HEIGHT/2) - POWER_TEXT_HEIGHT_OFFSET;
	light3Button.lightId = LIGHT_3;
	light3Button.type = POWER;
	light3Button.currentPowerState = OFF;
	light3Button.locked = FALSE;
	widgets[4] = light3Button;
	
	/* PWM */
	light3PWMButton.x1Pos = POWER_X1POS + (POWER_WIDTH * 2) + (POWER_GAP * 2);
	light3PWMButton.y1Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP;
	light3PWMButton.x2Pos = POWER_X1POS + (POWER_WIDTH * 3) + (POWER_GAP * 2);
	light3PWMButton.y2Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP + PWM_SELECTOR_HEIGHT;
	light3PWMButton.currentColour = NEPHRITIS;
	light3PWMButton.defaultSelectorColour = NEPHRITIS;
	light3PWMButton.pwm0SelectorColour = TURQUOISE;
	light3PWMButton.pwm1SelectorColour = PETER_RIVER;
	light3PWMButton.text = PWM_NONE_SELECTED;
	light3PWMButton.textX1Pos = light3PWMButton.x1Pos + (POWER_WIDTH/2) - SELECTOR_TEXT_WIDTH_OFFSET;
	light3PWMButton.textY1Pos = light3PWMButton.y1Pos + (PWM_SELECTOR_HEIGHT/2) - SELECTOR_TEXT_HEIGHT_OFFSET;
	light3PWMButton.lightId = LIGHT_3;
	light3PWMButton.type = PWM_SELECTOR;
	light3PWMButton.locked = FALSE;
	widgets[5] = light3PWMButton;
	
	
	/*################# LIGHT 4 ################# */
	/* BUTTON 1 */
	light4Button.x1Pos = POWER_X1POS + (POWER_WIDTH * 3) + (POWER_GAP * 3);
	light4Button.y1Pos = POWER_Y1POS;
	light4Button.x2Pos = POWER_X1POS + (POWER_WIDTH * 4) + (POWER_GAP * 3);
	light4Button.y2Pos = POWER_Y1POS + POWER_HEIGHT;
	light4Button.unpressedColour = PUMPKIN;
	light4Button.pressedColour = SUNFLOWER;
	light4Button.currentColour = light4Button.unpressedColour;
	light4Button.text = LIGHT_FOUR_TEXT;
	light4Button.lightId = LIGHT_4;
	light4Button.textX1Pos = light4Button.x1Pos + (POWER_WIDTH/2) - POWER_TEXT_WIDTH_OFFSET;
	light4Button.textY1Pos = light4Button.y1Pos + (POWER_HEIGHT/2) - POWER_TEXT_HEIGHT_OFFSET;
	light4Button.type = POWER;
	light4Button.currentPowerState = OFF;
	light4Button.locked = FALSE;
	widgets[6] = light4Button;
	
	/* PWM */
	light4PWMButton.x1Pos = POWER_X1POS + (POWER_WIDTH * 3) + (POWER_GAP * 3);
	light4PWMButton.y1Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP;
	light4PWMButton.x2Pos = POWER_X1POS + (POWER_WIDTH * 4) + (POWER_GAP * 3);
	light4PWMButton.y2Pos = POWER_Y1POS + POWER_HEIGHT + POWER_GAP + PWM_SELECTOR_HEIGHT;
	light4PWMButton.currentColour = NEPHRITIS;
	light4PWMButton.defaultSelectorColour = NEPHRITIS;
	light4PWMButton.pwm0SelectorColour = TURQUOISE;
	light4PWMButton.pwm1SelectorColour = PETER_RIVER;
	light4PWMButton.text = PWM_NONE_SELECTED;
	light4PWMButton.textX1Pos = light4PWMButton.x1Pos + (POWER_WIDTH/2) - SELECTOR_TEXT_WIDTH_OFFSET;
	light4PWMButton.textY1Pos = light4PWMButton.y1Pos + (PWM_SELECTOR_HEIGHT/2) - SELECTOR_TEXT_HEIGHT_OFFSET;
	light4PWMButton.lightId = LIGHT_4;
	light4PWMButton.type = PWM_SELECTOR;
	light4PWMButton.locked = FALSE;
	widgets[7] = light4PWMButton;
	
	/*################# PWM 0 ################# */
	/* TEXT DISPLAY */
	pwm0Text.x1Pos = PWM_X1POS;
	pwm0Text.y1Pos = PWM_Y1POS;
	pwm0Text.x2Pos = PWM_X1POS + POWER_WIDTH;
	pwm0Text.y2Pos = PWM_Y1POS + PWM_HEIGHT;
	pwm0Text.currentColour = TURQUOISE;
	pwm0Text.text = PWM0_TEXT;
	pwm0Text.type = TEXT_DISPLAY;
	pwm0Text.textX1Pos = pwm0Text.x1Pos + (POWER_WIDTH/2) - PWM_TEXT_WIDTH_OFFSET;
	pwm0Text.textY1Pos = pwm0Text.y1Pos + (PWM_HEIGHT/2) - PWM_TEXT_HEIGHT_OFFSET;
	pwm0Text.pwmId = PWM_0;
	pwm0Text.locked = FALSE;
	widgets[8] = pwm0Text;
	
	/* BRIGHTEN */
	pwm0Brighten.x1Pos = PWM_X1POS + POWER_WIDTH + POWER_GAP;
	pwm0Brighten.y1Pos = PWM_Y1POS;
	pwm0Brighten.x2Pos = PWM_X1POS + (POWER_WIDTH * 2) + POWER_GAP;
	pwm0Brighten.y2Pos = PWM_Y1POS + PWM_HEIGHT;
	pwm0Brighten.unpressedColour = TURQUOISE;
	pwm0Brighten.pressedColour = GREEN_SEA;
	pwm0Brighten.type = BRIGHTEN;
	pwm0Brighten.pwmId = PWM_0;
	pwm0Brighten.locked = FALSE;
	widgets[9] = pwm0Brighten;
	
	/* DIM */
	pwm0Dim.x1Pos = PWM_X1POS + (POWER_WIDTH * 2) + (POWER_GAP * 2);
	pwm0Dim.y1Pos = PWM_Y1POS;
	pwm0Dim.x2Pos = PWM_X1POS + (POWER_WIDTH * 3) + (POWER_GAP * 2);
	pwm0Dim.y2Pos = PWM_Y1POS + PWM_HEIGHT;
	pwm0Dim.unpressedColour = TURQUOISE;
	pwm0Dim.pressedColour = GREEN_SEA;
	pwm0Dim.type = DIM;
	pwm0Dim.pwmId = PWM_0;
	pwm0Dim.locked = FALSE;
	widgets[10] = pwm0Dim;
	
	/*################# PWM 1 ################# */
	
	/* TEXT DISPLAY */
	pwm1Text.x1Pos = PWM_X1POS;
	pwm1Text.y1Pos = PWM_Y1POS + PWM_HEIGHT + POWER_GAP;
	pwm1Text.x2Pos = PWM_X1POS + POWER_WIDTH;
	pwm1Text.y2Pos = PWM_Y1POS + (PWM_HEIGHT * 2) + POWER_GAP;
	pwm1Text.currentColour = PETER_RIVER;
	pwm1Text.text = PWM1_TEXT;
	pwm1Text.textX1Pos = pwm1Text.x1Pos + (POWER_WIDTH/2) - PWM_TEXT_WIDTH_OFFSET;
	pwm1Text.textY1Pos = pwm1Text.y1Pos + (PWM_HEIGHT/2) - PWM_TEXT_HEIGHT_OFFSET;
	pwm1Text.type = TEXT_DISPLAY;
	pwm1Text.pwmId = PWM_1;
	pwm1Text.locked = FALSE;
	widgets[11] = pwm1Text;
	
	/* BRIGHTEN 1 */
	pwm1Brighten.x1Pos = PWM_X1POS + POWER_WIDTH + POWER_GAP;
	pwm1Brighten.y1Pos = PWM_Y1POS + PWM_HEIGHT + POWER_GAP;
	pwm1Brighten.x2Pos = PWM_X1POS + (POWER_WIDTH * 2) + POWER_GAP;
	pwm1Brighten.y2Pos = PWM_Y1POS + (PWM_HEIGHT * 2) + POWER_GAP;;
	pwm1Brighten.unpressedColour = PETER_RIVER;
	pwm1Brighten.pressedColour = BELIZE_HOLE;
	pwm1Brighten.type = BRIGHTEN;
	pwm1Brighten.pwmId = PWM_1;
	pwm1Brighten.locked = FALSE;
	widgets[12] = pwm1Brighten;
	
	/* DIM */
	pwm1Dim.x1Pos = PWM_X1POS + (POWER_WIDTH * 2) + (POWER_GAP * 2);
	pwm1Dim.y1Pos = PWM_Y1POS + PWM_HEIGHT + POWER_GAP;
	pwm1Dim.x2Pos = PWM_X1POS + (POWER_WIDTH * 3) + (POWER_GAP * 2);
	pwm1Dim.y2Pos = PWM_Y1POS + (PWM_HEIGHT * 2) + POWER_GAP;;
	pwm1Dim.unpressedColour = PETER_RIVER;
	pwm1Dim.pressedColour = BELIZE_HOLE;
	pwm1Dim.type = DIM;
	pwm1Dim.pwmId = PWM_1;
	pwm1Dim.locked = FALSE;
	widgets[13] = pwm1Dim;
	
	/*################# BOTTOM MENU ################# */
	
	/* CONFIG 1 */
	config1.x1Pos = 0;
	config1.y1Pos = 0;
	config1.x2Pos = SCREEN_WIDTH;
	config1.y2Pos = PRESET_HEIGHT;
	config1.unpressedColour = EMERALD;
	config1.pressedColour = NEPHRITIS;
	config1.text = NO_PRESET_TEXT;
	config1.textX1Pos = (0 + (SCREEN_WIDTH / 2)) - PRESET_TEXT_WIDTH_OFFSET;
	config1.textY1Pos = (0 + (PRESET_HEIGHT / 2)) - PRESET_TEXT_HEIGHT_OFFSET;
	config1.currentPreset = DEFAULT;
	config1.currentAction = SELECT_PRESET;
	config1.type = CONFIG;
	config1.locked = FALSE;
	widgets[14] = config1;
	
	/* MASTER ON/OFF */
	master.x1Pos = MASTER_X1POS;
	master.y1Pos = SCREEN_HEIGHT - POWER_GAP - MASTER_HEIGHT;
	master.x2Pos = SCREEN_WIDTH - MASTER_X1POS;
	master.y2Pos = SCREEN_HEIGHT - POWER_GAP;
	master.currentColour = NEPHRITIS;
	master.unpressedColour = NEPHRITIS;
	master.pressedColour = LIME;
	master.currentPowerState = OFF;
	master.text = MASTER_BUTTON_TEXT;
	master.textX1Pos = master.x1Pos + (SCREEN_WIDTH / 2) - MASTER_TEXT_WIDTH_OFFSET;
	master.textY1Pos = master.y1Pos + (MASTER_HEIGHT / 2) - MASTER_TEXT_HEIGHT_OFFSET;
	master.type = MASTER;
	master.locked = FALSE;
	widgets[15] = master;
}

/**********************************************
**
** Function name:		drawWidgets
**
** Descriptions:		Draws all widgets in the UI
**
** parameters:			None
** Returned value:	None
** 
******************************************************************************/
void drawWidgets(){
		
	int i;

	for(i = 0; i < NUM_WIDGETS; i++){
		lcd_draw_widget(&widgets[i]);
	}
	
}

/******************************************************************************
** Function name:		getPressedWidget
**
** Descriptions:		Idnetifies whether any of the widgets contain the given point
**
** parameters:			xPos, yPos
** Returned value:	Pointer to press widget or null
** 
******************************************************************************/
Widget * getPressedWidget(unsigned short xPos, unsigned short yPos){
	int i;
	Widget * pressedWidget = NULL;
	for(i = 0; i < NUM_WIDGETS; i++){
			if((xPos > widgets[i].x1Pos && xPos < widgets[i].x2Pos) 
				&& (yPos > widgets[i].y1Pos && yPos < widgets[i].y2Pos))
			{
				pressedWidget = &widgets[i];
			}
	}
	return pressedWidget;
}

/******************************************************************************
** Function name:		getCommandFromWidget
**
** Descriptions:		Creates a command based on the given widget
**
** parameters:			widget
** Returned value:	Command struct containing info based on the widget
** 
******************************************************************************/
Command getCommandFromWidget(Widget * widget){
	Command cmd;
	switch(widget->type) {

		case POWER  :
      setPowerButtonCommand(widget, &cmd);
      break; 	
	 
		case PWM_SELECTOR :
			setPwmSelectorCommand(widget, &cmd);
			break;
		
   case DIM  :
			setDimCommand(widget, &cmd);
      break; 
	 
	 case BRIGHTEN  :
			setBrightenCommand(widget, &cmd);
      break;
	 
	 case CONFIG  :
			setPresetCommand(widget, &cmd);
      break;

	 case MASTER  :
			setMasterCommand(widget, &cmd);
      break;

   default : 
		break;
	}
	return cmd;
}

/******************************************************************************
**
** Function name:		setPowerButtonCommand
**
** Descriptions:		Creates a command for a power button widget
**
** parameters:			widget, cmd
** Returned value:	none
** 
******************************************************************************/
void setPowerButtonCommand(Widget * widget, Command * cmd){
	unsigned short mask;
	switch(widget->currentPowerState)
	{
		case ON :
			/* 00_b in the two bits corresponding to the given LED to turn off */
			mask = ~(0x3 << (widget->lightId - 1) * 2);
			currentState.globalLightState &= mask;
			cmd->ledSelector = currentState.globalLightState;
		  /* update UI */
			widget->currentPowerState = OFF;
			widget->currentColour = widget->unpressedColour;
			break;
		
		case OFF :
			/* 01_b in the two bits corresponding to the given LED to turn on */
			mask = 0x01 << (widget->lightId + widget->lightId - 2);
			currentState.globalLightState |= mask;
			cmd->ledSelector = currentState.globalLightState;
			/* update UI */
			widget->currentPowerState = ON;
			widget->currentColour = widget->pressedColour;
			break;
		
	}
	
	cmd->pwm0DutyCycle = currentState.pwm0DutyCycle;
	cmd->pwm1DutyCycle = currentState.pwm1DutyCycle;
}

/******************************************************************************
**
** Function name:		setPwmSelectorCommand
**
** Descriptions:		Creates a command for a pwmSelector widget
**
** parameters:			widget, cmd
** Returned value:	none
** 
******************************************************************************/
void setPwmSelectorCommand(Widget * widget, Command * cmd){
	unsigned short mask;
	/* move to the next pwm selector state (none, pwm0, pwm1) */
	setNextPwmSelectorState(widget);
	
	/* clear the bits corresponding to the given LED */
	mask = ~(0x3 << (widget->lightId - 1) * 2);
	currentState.globalLightState &= mask;
	
	/* associate LED with PWM0  */
	switch(widget->currentPwmState){
		case NONE :
			/* leave the LED in the ON state if we remove a pwm */
			mask = 0x01 << (widget->lightId + widget->lightId - 2);
			break;		
		case PWM0 :
			mask = 0x2 << (widget->lightId + widget->lightId - 2);
			break;		
		case PWM1 :
			mask = 0x3 << (widget->lightId + widget->lightId - 2);
			break;		
	}
	/* update the global led state and the command */
	currentState.globalLightState |= mask;
	cmd->ledSelector = currentState.globalLightState;
	cmd->pwm0DutyCycle = currentState.pwm0DutyCycle;
	cmd->pwm1DutyCycle = currentState.pwm1DutyCycle;
	updateUI();
}

/******************************************************************************
**
** Function name:		setNextPwmSelectorState
**
** Descriptions:		Sets the next state of a PWM selector button. 
**									Goes from NONE to PWM0 to PWM1 and back to NONE
**
** parameters:			widget
** Returned value:	none
** 
******************************************************************************/
void setNextPwmSelectorState(Widget * widget){
	switch(widget->currentPwmState)
	{
		case NONE :
			widget->currentPwmState = PWM0;
			widget->text = PWM_ZERO_SELECTED;
			widget->currentColour = widget->pwm0SelectorColour;
			break;
		case PWM0 :
			widget->currentPwmState = PWM1;
			widget->text = PWM_ONE_SELECTED;
			widget->currentColour = widget->pwm1SelectorColour;
			break;
		case PWM1 :
			widget->currentPwmState = NONE;
			widget->text = PWM_NONE_SELECTED;
			widget->currentColour = widget->defaultSelectorColour;
			break;
	}
}

/******************************************************************************
**
** Function name:		setDimCommand
**
** Descriptions:		Creates a command for a Dim widget
**
** parameters:			widget, cmd
** Returned value:	none
** 
******************************************************************************/
void setDimCommand(Widget * widget, Command * cmd){
	cmd->ledSelector = currentState.globalLightState;
	
	switch(widget->pwmId){
		case PWM_0 :
			/* only decrease the duty cycle if we are not already at the min */
			if(currentState.pwm0DutyCycle > (MIN_DUTY_CYCLE + DUTY_CYCLE_STEP))
				currentState.pwm0DutyCycle -= DUTY_CYCLE_STEP;
			break;
		
		case PWM_1 :
			/* only decrease the duty cycle if we are not already at the min */
			if(currentState.pwm1DutyCycle > (MIN_DUTY_CYCLE + DUTY_CYCLE_STEP))
				currentState.pwm1DutyCycle -= DUTY_CYCLE_STEP;
			break;
	}
	cmd->pwm1DutyCycle = currentState.pwm1DutyCycle;
	cmd->pwm0DutyCycle = currentState.pwm0DutyCycle;
}

/******************************************************************************
**
** Function name:		setBrightenCommand
**
** Descriptions:		Creates a command for a Brighten widget
**
** parameters:			widget, cmd
** Returned value:	none
** 
******************************************************************************/
void setBrightenCommand(Widget * widget, Command * cmd){
	cmd->ledSelector = currentState.globalLightState;
	
	switch(widget->pwmId){
		case PWM_0 :
			/* only increase the duty cycle if we are not already at the max */
			if(currentState.pwm0DutyCycle < (MAX_DUTY_CYCLE - DUTY_CYCLE_STEP))	
				currentState.pwm0DutyCycle += DUTY_CYCLE_STEP;
			break;
		
		case PWM_1 :
			/* only increase the duty cycle if we are not already at the max */
			if(currentState.pwm1DutyCycle < (MAX_DUTY_CYCLE - DUTY_CYCLE_STEP))
				currentState.pwm1DutyCycle += DUTY_CYCLE_STEP;
			break;
	}
	cmd->pwm1DutyCycle = currentState.pwm1DutyCycle;
	cmd->pwm0DutyCycle = currentState.pwm0DutyCycle;
}


/******************************************************************************
**
** Function name:		setPresetCommand
**
** Descriptions:		Creates a command for a Preset widget
**
** parameters:			widget, cmd
** Returned value:	none
** 
******************************************************************************/
void setPresetCommand(Widget * widget, Command * cmd){

	if(widget->currentAction == SELECT_PRESET){
		
		switch(widget->currentPreset)
		{
			case DEFAULT :
				widget->currentPreset = PRESET1;
				widget->text = PRESET_ONE_TEXT;
				/* update the current state pointer */
				copyState(&preset1, &currentState);
				currentStatePtr = &preset1;
				break;
			
			case PRESET1 :
				widget->currentPreset = PRESET2;
				widget->text = PRESET_TWO_TEXT;
				/* update the current state pointer */
				copyState(&preset2, &currentState);
				currentStatePtr = &preset2;
				break;
			
			case PRESET2 :
				widget->currentPreset = DEFAULT;
				widget->text = NO_PRESET_TEXT;
				/* update the current state pointer */
				copyState(&defaultState, &currentState);
				currentStatePtr = &defaultState;
				break;
			
			default :
				break;	
		}	
	}
	else if(widget->currentAction == SAVE_PRESET)
	{
		saveCurrentPreset();
		widget->currentAction = SELECT_PRESET;
	}
	cmd->ledSelector = currentState.globalLightState;
	cmd->pwm0DutyCycle = currentState.pwm0DutyCycle;
	cmd->pwm1DutyCycle = currentState.pwm1DutyCycle;
	updateUI();
}

/******************************************************************************
**
** Function name:		setMasterCommand
**
** Descriptions:		Creates a command for a Master widget.
**									Master off saves 'currentState' into 'previousState' and then 
**									turns all LEDs off. Master on restores 'previousState' into
**									'currentState'.
**
** parameters:			widget, cmd
** Returned value:	none
** 
******************************************************************************/
void setMasterCommand(Widget * widget, Command * cmd){
	printf("\r\n");
	printf("MASTER STATE:%d\r\n",widget->currentPowerState);
	printf("MASTER LED:%d\r\n",currentState.globalLightState);
	
	switch(widget->currentPowerState)
	{
		case ON :
			/* restore the saved state */
			copyState(&previousState, &currentState);
			printf("RESTORED STATE:%d\r\n",previousState.globalLightState);
			widget->currentPowerState = OFF;
			widget->currentColour = widget->unpressedColour;
			lockUi(FALSE);
			break;
		
		case OFF :
			/* save the current state */
			copyState(&currentState, &previousState);
			printf("SAVED STATE:%d\r\n",previousState.globalLightState);
			/* set the current state to the 'off' state */
			copyState(&sleepState, &currentState);
			widget->currentPowerState = ON;
			widget->currentColour = widget->pressedColour;
			/* lock the UI so that buttons can't be pressed until the master saved state is restored */
			lockUi(TRUE);
			break;
	}
	
	cmd->ledSelector = currentState.globalLightState;
	cmd->pwm0DutyCycle = currentState.pwm0DutyCycle;
	cmd->pwm1DutyCycle = currentState.pwm1DutyCycle;
	updateUI();
	
	printf("UPDATED MASTER STATE:%d\r\n",widget->currentPowerState);
	printf("UPDATED MASTER LED:%d\r\n",currentState.globalLightState);
}

/******************************************************************************
**
** Function name:		setCommandByButtonId
**
** Descriptions:		>>For use when a button press is detected<<
**									updates the UI and creates the cmd to update the global state
**									based on the given button id.							
**									If a button is pressed and the corresponding LED power button 
**									is not in the OFF state - do nothing (the command will be the 
**									current state).
**
** parameters:			cmd, buttonId
** Returned value:	none
** 
******************************************************************************/
void setCommandByButtonId(Command * cmd, int buttonId){
		
	//check if corresponding LED is in off state
	Widget * ledPowerWidget;
	int ledId = buttonId + 1;
	ledPowerWidget = getPowerButtonByLedId(ledId);
	
	//update state if it is off
	if(ledPowerWidget->currentPowerState == OFF){
		//set the LED to ON
		setPowerButtonCommand(ledPowerWidget,cmd);
	}
	//otherwise keep the state as it is
	else{
		cmd->ledSelector = currentState.globalLightState;
		cmd->pwm0DutyCycle = currentState.pwm0DutyCycle;
		cmd->pwm1DutyCycle = currentState.pwm1DutyCycle;
	}
		
	updateUI();
}

/******************************************************************************
**
** Function name:		setCommandForTimerExpiry
**
** Descriptions:		Creates the command to enter a sleeping state when the PIR
**									timer expires.
**
** parameters:			cmd
** Returned value:	none
** 
******************************************************************************/
void setCommandForTimerExpiry(Command * cmd){
		Widget * master = getMaster();
		setMasterCommand(master , cmd);
		lcd_draw_widget(master);
}

/******************************************************************************
**
** Function name:		updateUI
**
** Descriptions:		This function updates the UI given a UiState. 
**									It is used if the state of the system has been updated through
**									means other than the UI (i.e. timers) or if the state isn't
**									properly represented in  the UI after the use of a particular button
**									(i.e. changing preset requires to update the colour of the LED power
**									buttons and corresponding PWM settings).
**
** parameters:			uiState
** Returned value:	none
** 
******************************************************************************/
void updateUI(){
	Widget * powerWidgetToUpdate;
	Widget * pwmWidgetToUpdate;

	unsigned short mask = 0;
	unsigned short ledState = 0;
	unsigned short shiftAmount = 0;
	int ledId = 0;
	
	// iterate over each of the LEDs updating the corresponding power button and PWM selector
	// were appropriate
	for (ledId = 1; ledId <= NUM_LEDS; ledId++)
	{
		shiftAmount = (ledId - 1) * 2;
		mask = 0x03 << shiftAmount;
		// get the two bits for this specific LED
		ledState = (currentState.globalLightState & mask) >> shiftAmount;

		switch(ledState)
		{
			//LED OFF
			case 0x00 :
				//make sure corresponding power button is OFF
				powerWidgetToUpdate = getPowerButtonByLedId(ledId);
				powerWidgetToUpdate->currentPowerState = OFF;
				powerWidgetToUpdate->currentColour = powerWidgetToUpdate->unpressedColour;
			  //make sure corresponding pwm selector is none
				pwmWidgetToUpdate = getPwmSelectorByLedId(ledId);
				pwmWidgetToUpdate->currentPwmState = NONE;
				pwmWidgetToUpdate->text = PWM_NONE_SELECTED;
				pwmWidgetToUpdate->currentColour = pwmWidgetToUpdate->defaultSelectorColour;
				break;
			
			//LED ON
			case 0x01 :
				//make sure corresponding power button is ON
				powerWidgetToUpdate = getPowerButtonByLedId(ledId);
				powerWidgetToUpdate->currentPowerState = ON;
				powerWidgetToUpdate->currentColour = powerWidgetToUpdate->pressedColour;
			  //make sure corresponding pwm selector is none
				pwmWidgetToUpdate = getPwmSelectorByLedId(ledId);
				pwmWidgetToUpdate->currentPwmState = NONE;
				pwmWidgetToUpdate->text = PWM_NONE_SELECTED;
				pwmWidgetToUpdate->currentColour = pwmWidgetToUpdate->defaultSelectorColour;
				break;
			
			//LED set to PWM0 
			case 0x02 :
				//make sure corresponding power button is OFF
				powerWidgetToUpdate = getPowerButtonByLedId(ledId);
				powerWidgetToUpdate->currentPowerState = ON;
				powerWidgetToUpdate->currentColour = powerWidgetToUpdate->pressedColour;
			  //make sure corresponding pwm selector is PWM0
				pwmWidgetToUpdate = getPwmSelectorByLedId(ledId);
				pwmWidgetToUpdate->currentPwmState = PWM0;
				pwmWidgetToUpdate->text = PWM_ZERO_SELECTED;
				pwmWidgetToUpdate->currentColour = pwmWidgetToUpdate->pwm0SelectorColour;
				break;
			
			//LED set to PWM1
			case 0x03 :
				//make sure corresponding power button is OFF
				powerWidgetToUpdate = getPowerButtonByLedId(ledId);
				powerWidgetToUpdate->currentPowerState = ON;
				powerWidgetToUpdate->currentColour = powerWidgetToUpdate->pressedColour;
			  //make sure corresponding pwm selector is PWM1
				pwmWidgetToUpdate = getPwmSelectorByLedId(ledId);
				pwmWidgetToUpdate->currentPwmState = PWM1;
				pwmWidgetToUpdate->text = PWM_ONE_SELECTED;
				pwmWidgetToUpdate->currentColour = pwmWidgetToUpdate->pwm1SelectorColour;
				break;
		}
	}
	// redraw the updated widgets
	drawWidgets();
}

/******************************************************************************
**
** Function name:		getPowerButtonByLedId
**
** Descriptions:		Returns any widget of type POWER that corresponds to the 
**									given led ID.
**
** parameters:			ledId
** Returned value:	powerWidget
** 
******************************************************************************/
Widget * getPowerButtonByLedId(int ledId){
	Widget * powerWidget;
	int i;
	for (i = 0; i < NUM_WIDGETS; i++){
		if(widgets[i].type == POWER && widgets[i].lightId == ledId){
			powerWidget = &widgets[i];
		}
	}
	return powerWidget;
}

/******************************************************************************
**
** Function name:		getPwmSelectorByLedId
**
** Descriptions:		Returns any widget of type LED_SELECTOR that corresponds to the 
**									given led ID.
**
** parameters:			ledId
** Returned value:	pwmSelectorWidget
** 
******************************************************************************/
Widget * getPwmSelectorByLedId(int ledId){
	Widget * pwmSelectorWidget;
	int i;
	for (i = 0; i < NUM_WIDGETS; i++){
		if(widgets[i].type == PWM_SELECTOR && widgets[i].lightId == ledId){
			pwmSelectorWidget = &widgets[i];
		}
	}
	return pwmSelectorWidget;
}

Widget * getMaster(){
	Widget * master;
	int i;
	for (i = 0; i < NUM_WIDGETS; i++){
		if(widgets[i].type == MASTER){
			master = &widgets[i];
		}
	}
	return master;
}

/******************************************************************************
**
** Function name:		saveCurrentPreset
**
** Descriptions:		Saves changes to the current preset by copying the values
**									stored in currentState into the preset pointed to by 
**									currentStatePtr.
**
** parameters:			none
** Returned value:	none
** 
******************************************************************************/
void saveCurrentPreset(){
	copyState(&currentState, currentStatePtr);
}

/******************************************************************************
**
** Function name:		lockUi
**
** Descriptions:		Locks every widget in the UI exceot fir the master widget.
**
** parameters:			lock
** Returned value:	none
** 
******************************************************************************/
void lockUi(int lock){
	int i;
	for (i = 0; i < NUM_WIDGETS; i++){
		if(widgets[i].type != MASTER){
			widgets[i].locked = lock;
		}
	}
}

/******************************************************************************
**
** Function name:		flashWidget
**
** Descriptions:		Not a very nice way of making a widget flash
**
** parameters:			widget
** Returned value:	none
** 
******************************************************************************/
void flashWidget(Widget * widget){
	portTickType xLastWakeTime = xTaskGetTickCount();
	
	lcd_color_t colourHandle = widget->unpressedColour;
	widget->unpressedColour = AMETHYST;
	lcd_draw_widget(widget);
	vTaskDelayUntil( &xLastWakeTime, 80);
	widget->unpressedColour = colourHandle;
	lcd_draw_widget(widget);
	vTaskDelayUntil( &xLastWakeTime, 80);
	widget->unpressedColour = AMETHYST;
	lcd_draw_widget(widget);
	vTaskDelayUntil( &xLastWakeTime, 80);
	widget->unpressedColour = colourHandle;
	lcd_draw_widget(widget);
}
