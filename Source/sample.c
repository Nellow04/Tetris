#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"
#include "led/led.h"
#include "tetris/tetris.h"
#include "adc/adc.h"
#include "music/music.h"

/* I file utilizzati per sviluppare il gioco sono i seguenti:
sample.c
tetris.c
tetris.h
IRQ_timer.c
IRQ_button.c
music.c
*/

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();												/* Inizializzazione Buttons 					*/
  LCD_Initialization();									/* Inizializzazione Display 					*/
	joystick_init();											/* Inizializzazione Joystick 					*/
	LED_init();														/* Inizializzazione LED								*/
	ADC_init();														/* Inizializzazione Potenziometro			*/

	tetris_init();												/* Inizializzazione Tetris						*/
	music_init();													/* Inizializzazione musica						*/

	init_timer(0, 24, 0, 3, 1000000); 			
	enable_timer(0);
	
	init_timer(2, 0, 0, 3, 0xC3501); 			
	enable_timer(2);
	
	
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);
	
  while (1)	
  {
		__ASM("wfi");
  }
}