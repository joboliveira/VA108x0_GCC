/***************************************************************************************
 * @file     reb_main.c
 * @version  V1.0
 * @date     31. March 2016
 *
 * @note
 * VORAGO Technologies 
 *
 * @note
 * Copyright (c) 2013-2016 VORAGO Technologies. 
 *
 * @par
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
 * ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. VORAGO TECHNOLOGIES 
 * SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ****************************************************************************************/
#include <stdio.h>
#include "va108xx.h"
#include "reb_log.h"
#include "reb_board.h"
#include "reb_adt75.h"
#include "driver_common.h"
#include "reb_max11619.h"
#include "reb_timer.h"
#include "gpio_va108xx.h"

volatile uint8_t bounce_cnt = 0; 
volatile uint32_t  ticker = 0 ;  /*  SysTick counter    */
volatile uint32_t TC0[16] , NVIC_CAP[18], IRQSEL_CAP[4], SCB_CAP[6]  ;

void delay (int a)
{
	volatile int i,j;


	for (i=0 ; i < a ; i++)
	{
		j++;
	}

	return;
}

int main()
{
	uint8_t cur_temp = 0; 
	int32_t ret_code = 0;
	uint16_t adc_data = 0;
#ifdef GPIO_DRIVER
	VOR_DRIVER_GPIO *PA10 = &Driver_GPIOA_10; 
#endif 	
	
	/* added CEM to see how code is executing and to get SRAM cleared up.  */
//    volatile uint32_t *pui32TestHead;
////    volatile uint32_t ui32Sum, test_result;
    volatile uint32_t  i,count  ;



	/* Enable clock for all peripherals */ 
	VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE = ( CLK_ENABLE_PORTA | CLK_ENABLE_PORTB | CLK_ENABLE_SPIA | CLK_ENABLE_SPIB | CLK_ENABLE_SPIC |
																					 CLK_ENABLE_UARTA | CLK_ENABLE_UARTB | CLK_ENABLE_I2CA | CLK_ENABLE_I2CB | CLK_ENABLE_IRQSEL |
																					 CLK_ENABLE_IOMGR | CLK_ENABLE_UTILITY | CLK_ENABLE_PORTIO | CLK_ENABLE_SYSTEM ); 
	
	VOR_SYSCONFIG->TIM_CLK_ENABLE = 0xFFFFFF  ;   // enable all timers







	/* Initialize Logging module */ 

/*
  VOR_Log_Init(PORTA_UARTA, LOG_DBG);
 	VOR_Log(LOG_INFO, ANSI_COLOR_GREEN"Welcome to Vorago Technologies REB1 VA108xx software! \n");
 	if( VOR_SYSCONFIG->PROCID == PROC_ID_TORCH )
 		VOR_Log(LOG_INFO, ANSI_COLOR_GREEN"VA10820 MCU ...\n");
    else
      VOR_Log(LOG_INFO, ANSI_COLOR_GREEN"VA10800 MCU ... \n");
      VOR_Log(LOG_INFO, "Restarted... \n");
      VOR_Log(LOG_INFO, "Reset Status: 0x%08x \n", VOR_SYSCONFIG->RST_STAT);
*/
	
	/* Configure PA[10]. This GPIO will continuously blink led D2. */
#ifdef GPIO_DRIVER
	PA10->Initialize(NULL);
	PA10->Control(VOR_GPIO_CONTROL_MASK, VOR_GPIO_CTRL_ENABLE); 
	PA10->Control(VOR_GPIO_CONTROL_DIR, VOR_GPIO_DIR_OUTPUT); 
	PA10->Write(0); //DH
#else 
	VOR_GPIO->BANK[0].DIR |= (1 << PORTA_10_D2); 
	VOR_GPIO->BANK[0].DATAMASK |= (1 << PORTA_10_D2);
	VOR_GPIO->BANK[0].DATAOUT &= ~(1 << PORTA_10_D2); 
#endif 	
	/* Configure PA[11]. Input to blink led D3. */ 
	VOR_GPIO->BANK[0].DIR &= ~(1 << PORTA_11_SW_USER); 
	VOR_GPIO->BANK[0].DATAMASK |= (1 << PORTA_11_SW_USER);
	
	/* Configure PA[7]. This GPIO will turn ON led D3. */ 
	VOR_GPIO->BANK[0].DIR |= (1 << PORTA_7_D3); 
	VOR_GPIO->BANK[0].DATAMASK |= (1 << PORTA_7_D3);
	VOR_GPIO->BANK[0].DATAOUT |= (1 << PORTA_7_D3); 
	
	/* Intialize MSec and Sec Timers */



//	ret_code = VOR_TIM0_MSecInitialize(100);  /* from original code   */
//	ret_code = VOR_TIM1_SecInitialize(1);

	SysTick->LOAD = 0xFF00  ;    /*  enable SysTICK and force SysTick Exception   */
	SysTick->CTRL = 0x3  ;   // enable with  interrupts,  B0=ENABLE, B1=INT_ENABLE
	SysTick->VAL = 0x500  ;



//	__enable_irq()  ; /*    directly clear the PRIMASK   */

//
	VOR_TIM0->RST_VALUE = 0x77777  ;
	VOR_TIM0->CNT_VALUE = 0x500  ;
	VOR_TIM0->CTRL = 0x13   ;   //  IRQ_enable = b4, enable = b0, active = b1

	VOR_IRQSEL->TIM[0] = IRQ_TIM0_IRQn  ;

	NVIC_SetPriority(IRQ_TIM0_IRQn,IRQ_TIM0_PRIORITY);
//	NVIC_EnableIRQ(IRQ_TIM0_IRQn);
	NVIC_EnableIRQ(0);   /*  tim0 IS ASSIGNED TO INT #0   */


      i = 0  ;

		  while (1)
		  {
		     count ++ ;
			 delay(500000)  ;  //  delay 500k counts
		 VOR_GPIO->BANK[0].TOGOUT |= (1 << PORTA_7_D3);

		 TC0[i] = VOR_TIM0->CNT_VALUE  ;
		 i++  ;
		 if(i>9)  { i = 0 ;}

	 		if( VOR_TIM0_MSecExpired() ) {

				VOR_GPIO->BANK[0].DATAOUT ^= (1 << PORTA_10_D2);
				VOR_TIM0_MSecClear();
			}

		  }
	//CEM	if( ret_code )
	//CEM		VOR_Log(LOG_ERROR, "MSec Timer initialization failed! \n");

	ret_code = VOR_TIM1_SecInitialize(1); 	
	//CEM	if( ret_code )
	//CEM		VOR_Log(LOG_ERROR, "Sec Timer initialization failed! \n");
	
	/* Initialize temperature sensor - ADT75 */ 
	ret_code = VOR_ADT75_Initialize(); 
	//CEM	if( ret_code )
	//CEM		VOR_Log(LOG_ERROR, "ADT75 initialization failed! \n");

	/* Initialize SPI interface to Maxim ADC - MAX11619 */ 
	ret_code = VOR_MAX11619_Initialize(); 
	//CEM	if( ret_code )
	//CEM		VOR_Log(LOG_ERROR, "MAX11619 initialization failed! \n");
	
	while(1) {
		if( VOR_TIM1_SecExpired() ) {
			/* Retrieve current temperature */ 
			ret_code = VOR_ADT75_GetTemp(&cur_temp);
			if( ret_code )
				VOR_Log(LOG_ERROR, "ADT75 read failed! \n");
			else
				VOR_Log(LOG_INFO, "\nXXX Temperature: %d(C)\n", cur_temp);
		
			/* Configure Maxim ADC */ 
			ret_code = VOR_MAX11619_Configure();
			if( ret_code )
				VOR_Log(LOG_ERROR, "MAX11619 configuration failed! \n");
		
			/* Retrieve sampled data from ADC */ 
			ret_code = VOR_MAX11619_GetData(&adc_data);	
			if( ret_code )
				VOR_Log(LOG_ERROR, "MAX11619 get data failed! \n"); 
			else
				VOR_Log(LOG_INFO, "\nXXX ADC O/P (0(0V) to 1023(3.3V)): %d\n", adc_data);
			
			VOR_TIM1_SecClear(); 	
		}	
 		if( VOR_TIM0_MSecExpired() ) {
#ifdef GPIO_DRIVER			
			PA10->Write(1); 
#else
			VOR_GPIO->BANK[0].DATAOUT ^= (1 << PORTA_10_D2);		
#endif 			
			if( !(VOR_GPIO->BANK[0].DATAIN & (1 << PORTA_11_SW_USER)) )
				bounce_cnt++; 
			
			if(bounce_cnt >= 2) {
				VOR_GPIO->BANK[0].DATAOUT ^= (1 << PORTA_7_D3); 
				bounce_cnt = 0; 
			}
			VOR_TIM0_MSecClear(); 
		}
	}
}
