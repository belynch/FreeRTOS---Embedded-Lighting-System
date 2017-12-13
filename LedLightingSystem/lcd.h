#ifndef LCD_H
#define LCD_H

#include "command.h"
#include "lcd_grph.h"

void vStartLcd( unsigned portBASE_TYPE uxPriority, xQueueHandle xCmdQueue, UiState * uiState );
void vSavePresetTimerCallback( TimerHandle_t xTimer );

#endif
