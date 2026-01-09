/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "../tetris/tetris.h"

int right_activate;
int down_activate;
int left_activate;
int up_activate;
int done;

volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char led_value;					/* defined in funct_led								*/
void TIMER0_IRQHandler (void)
{

	/* Match register 0 interrupt service routine */
	if (LPC_TIM0->IR & 01)
	{
		handle_timer_tick();
		LPC_TIM0->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 02)
  {
	
		LPC_TIM0->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 4)
  {
		
		LPC_TIM0->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM0->IR & 8)
  {
	 
		LPC_TIM0->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
	static int J_select = 0;
	static int J_down = 0;
	static int J_left = 0;
	static int J_right = 0;
	static int J_up = 0;
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick J_Select pressed p1.25*/
		
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
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick J_Down pressed p1.26 --> using J_DOWN due to emulator issues*/
		
		J_down++;
		switch(J_down){
			case 1:
				//code here
				LPC_TIM0->MR0 = 500000; // 0.5s
				LPC_TIM0->TCR = 2;       // Reset
				LPC_TIM0->TCR = 1;       // Enable
			down_activate =1;
				break;
			default:
				break;
		}
	}
	else{
			if(J_down != 0) {
				LPC_TIM0->MR0 = 1000000; // 1s
				LPC_TIM0->TCR = 2;       // Reset
				LPC_TIM0->TCR = 1;       // Enable
			}
			J_down=0;
			down_activate =0;
			//done =0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		/* Joytick J_Left pressed p1.27*/
		J_left++;
		switch(J_left){
			case 1:
				move_left();
				left_activate =1;
				break;
			default:
				break;
		}
	}
	else{
			J_left=0;
			left_activate =0;
		done =0; 
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		/* Joytick J_right pressed p1.28*/
		
		J_right++;
		switch(J_right){
			case 1:
				move_right();
			right_activate =1;
				break;
			default:
				break;
		}
	}
	else{
			J_right=0;
			right_activate =0;

	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	
		/* Joytick J_up pressed p1.29*/
		
		J_up++;
		switch(J_up){
			case 1:
				rotate_piece();
				up_activate =1;
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
	
  /* Match register 0 interrupt service routine */
	if (LPC_TIM2->IR & 01)
	{
		
		LPC_TIM2->IR = 1;			/* clear interrupt flag */
	}
		/* Match register 1 interrupt service routine */
	  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 02)
  {
	
		LPC_TIM2->IR =  2 ;			/* clear interrupt flag */	
	}
	/* Match register 2 interrupt service routine */
  /* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 4)
  {
		
		LPC_TIM2->IR =  4 ;			/* clear interrupt flag */	
	}
		/* Match register 3 interrupt service routine */
  	/* it should be possible to access to both interrupt requests in the same procedure*/
	if(LPC_TIM2->IR & 8)
  {
	 
		LPC_TIM2->IR =  8 ;			/* clear interrupt flag */	
	}
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
