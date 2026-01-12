/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "LPC17xx.h"
#include "adc.h"

/* Speed variable accessible from other files */
volatile uint32_t standard_speed_mr0 = 1000000; 

unsigned short AD_current;   
unsigned short AD_last = 0xFF; // Ultimo valore convertito

void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);
  
	if(AD_current != AD_last){
		standard_speed_mr0 = (1000000UL * 4095UL) / (4095UL + AD_current * 4UL);
		
		AD_last = AD_current;
  }
}
