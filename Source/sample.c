#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "led/led.h"
#include "tetris/tetris.h"

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();												/* Inizializzazione Buttons 					*/
  LCD_Initialization();									/* Inizializzazione Display 					*/
	joystick_init();											/* Inizializzazione Joystick 					*/
	LED_init();

	tetris_init();

	init_timer(0, 0, 0, 3, 0x7A120); 			
	enable_timer(0);
	
	init_timer(2, 0, 0, 3, 0xC350 ); 	
	enable_timer(2);
	
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);
	
  while (1)	
  {
		__ASM("wfi");
  }
}