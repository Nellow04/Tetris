/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "../tetris/tetris.h"
#include "../button_EXINT/button.h"

int right_activate = 0;
int down_activate = 0;
int left_activate = 0;
int up_activate = 0;

volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;

void RIT_IRQHandler (void)
{					
	static int J_select = 0;
	static int J_down = 0;
	static int J_left = 0;
	static int J_right = 0;
	static int J_up = 0;
	
	// Joystick Select
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		J_select++;
		switch(J_select){
			case 1:
 				toggle_pause();
				break;
			default:
				break;
		}
	}
	else{
			J_select=0;
	}
	
	// Joystick Down
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		J_down++;
		switch(J_down){
			case 1:
				// Wait for stable signal (debounce)
				break;
			case 2:
				LPC_TIM0->MR0 = 0xBEBC20; // 0.5s (25MHz * 0.5)
				LPC_TIM0->TCR = 2;       // Reset
				LPC_TIM0->TCR = 1;       // Enable
			    down_activate = 1;
				break;
			default:
				break;
		}
	}
	else{
			if(J_down >= 2) {
				LPC_TIM0->MR0 = 0x17D7840; // 1s (25MHz * 1)
				// Non resettiamo il timer qui per evitare blocchi se il segnale rimbalza
				// Il timer continuerà a contare fino al nuovo limite (più alto)
			}
			J_down=0;
			down_activate = 0;
	}
	
	// Joystick Left
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		J_left++;
		switch(J_left){
			case 1:
				move_left();
				left_activate = 1;
				break;
			default:
				break;
		}
	}
	else{
			J_left=0;
			left_activate = 0;
	}
	
	// Joystick Right
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		J_right++;
		switch(J_right){
			case 1:
				move_right();
			    right_activate = 1;
				break;
			default:
				break;
		}
	}
	else{
			J_right=0;
			right_activate = 0;
	}
	
	// Joystick Up
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		J_up++;
		switch(J_up){
			case 1:
				rotate_piece();
				up_activate = 1;
				break;
			default:
				break;
		}
	}
	else{
			J_up=0;
			up_activate = 0;
	}
	
	/* button management */
	if(down_0 != 0){
		down_0++; 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */			
			switch(down_0){
				case 2:				
					//code here
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down_1!=0){ 
		down_1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */			
			switch(down_1){
				case 2:
					toggle_pause();					
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down_2 != 0){
		down_2++;	
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */			
			switch(down_2){
				case 2:	
					hard_drop();					
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
		
	}
	reset_RIT();
	LPC_RIT->RICTRL |= 1;	/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/
