#ifndef _WIDGETS_

#define _WIDGETS_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "StringResources.h"

typedef enum {UNPRESSED, PRESSED} WidgetState;
typedef enum {ON, OFF} WidgetPowerState;
typedef enum {POWER, DIM, BRIGHTEN, PWM_SELECTOR, TEXT_DISPLAY, CONFIG, MASTER} WidgetType;
typedef enum {NONE, PWM0, PWM1} PwmSelectorState;
typedef enum {DEFAULT, PRESET1, PRESET2, SLEEP} PresetState;
typedef enum {SELECT_PRESET, SAVE_PRESET} PresetAction;

typedef unsigned short lcd_color_t;

typedef struct UiStates {
	unsigned short globalLightState;
	unsigned short pwm0DutyCycle;
	unsigned short pwm1DutyCycle;
	PresetState currentPreset;
} UiState;

typedef struct Widgets {
	WidgetType type;
	int locked;
	//LOCATION
	unsigned short x1Pos;
	unsigned short y1Pos;
	unsigned short x2Pos;
	unsigned short y2Pos;
	//TEXT DETAILS
	char *text;
	unsigned short textX1Pos;
	unsigned short textY1Pos;
	//POWER BUTTON
	WidgetState state;
	WidgetPowerState currentPowerState;
	unsigned short lightId;
	//PWM SELECTOR
	unsigned short pwmId;
	PwmSelectorState currentPwmState;
	lcd_color_t defaultSelectorColour;
	lcd_color_t pwm0SelectorColour;
	lcd_color_t pwm1SelectorColour;
	//PRESET BUTTON
	PresetState currentPreset;
	PresetAction currentAction;
	//SHARED COLOUR STORAGE
	lcd_color_t currentColour;
	lcd_color_t unpressedColour;
	lcd_color_t pressedColour;
} Widget;

void initialiseState(UiState * uiState);
void initialiseWidgets(void);
void drawWidgets(void);
Widget * getPressedWidget(unsigned short xPos, unsigned short yPos);
Command getCommandFromWidget(Widget * widget);
void setPowerButtonCommand(Widget * widget, Command * cmd);
void setPwmSelectorCommand(Widget * widget, Command * cmd);
void setNextPwmSelectorState(Widget * widget);
void setDimCommand(Widget * widget, Command * cmd);
void setBrightenCommand(Widget * widget, Command * cmd);
void setPresetCommand(Widget * widget, Command * cmd);
void copyState(UiState * src, UiState * dst);
void setMasterCommand(Widget * widget, Command * cmd);
void updateUI(void);
Widget * getPowerButtonByLedId(int ledId);
Widget * getPwmSelectorByLedId(int ledId);
Widget * getMaster(void);
void setCommandByButtonId(Command * cmd, int buttonId);
void setCommandForTimerExpiry(Command * cmd);
void saveCurrentPreset(void);
void lockUi(int lock);
void flashWidget(Widget * widget);

#endif /* _WIDGETS_ */
