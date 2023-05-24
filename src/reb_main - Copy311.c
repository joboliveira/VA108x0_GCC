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

//
//    volatile uint32_t *pui32TestISRAM = ((uint32_t volatile *) 0x10006000);
//    count = 64  ;
//    for(i=0; i<count;i++)
//       {
//        pui32TestHead = (8*i) + pui32TestISRAM;
//       *pui32TestHead++ = 0x00000055;
//       *pui32TestHead++ = 0x01FFFFFF;
//       *pui32TestHead++ = 0x02000000;
//       *pui32TestHead++ = 0x03FFFFFF;
//       *pui32TestHead++ = 0x04000000;
//       *pui32TestHead++ = 0x05FFFFFF;
//       *pui32TestHead++ = 0x06000000;
//       *pui32TestHead++ = 0x07FFFFFF;
//       }

	/* Enable clock for all peripherals */ 
	VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE = ( CLK_ENABLE_PORTA | CLK_ENABLE_PORTB | CLK_ENABLE_SPIA | CLK_ENABLE_SPIB | CLK_ENABLE_SPIC |
																					 CLK_ENABLE_UARTA | CLK_ENABLE_UARTB | CLK_ENABLE_I2CA | CLK_ENABLE_I2CB | CLK_ENABLE_IRQSEL |
																					 CLK_ENABLE_IOMGR | CLK_ENABLE_UTILITY | CLK_ENABLE_PORTIO | CLK_ENABLE_SYSTEM ); 
	
	/*   added by CEM  Just want to see real action    ***  */
	/* Configure PA[7]. This GPIO will turn ON led D3. */
	VOR_GPIO->BANK[0].DIR |= (1 << PORTA_7_D3);
	VOR_GPIO->BANK[0].DATAMASK |= (1 << PORTA_7_D3);
	VOR_GPIO->BANK[0].DATAOUT |= (1 << PORTA_7_D3);
	VOR_GPIO->BANK[0].TOGOUT |= (1 << PORTA_7_D3);



//	  while (1)
//	  {
//	     count ++ ;
//		 delay(500000)  ;  //  delay 500k counts
//		 VOR_GPIO->BANK[0].TOGOUT |= (1 << PORTA_7_D3);
//
//	  }


	/* Initialize Logging module */ 


  VOR_Log_Init(PORTA_UARTA, LOG_DBG);
 	VOR_Log(LOG_INFO, ANSI_COLOR_GREEN"Welcome to Vorago Technologies REB1 VA108xx software! \n");
 	if( VOR_SYSCONFIG->PROCID == PROC_ID_TORCH )
 		VOR_Log(LOG_INFO, ANSI_COLOR_GREEN"VA10820 MCU ...\n");
    else
      VOR_Log(LOG_INFO, ANSI_COLOR_GREEN"VA10800 MCU ... \n");
      VOR_Log(LOG_INFO, "Restarted... \n");
      VOR_Log(LOG_INFO, "Reset Status: 0x%08x \n", VOR_SYSCONFIG->RST_STAT);

	
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

	NVIC_CAP[0] = *NVIC->ISER  ;
	NVIC_CAP[1] = *NVIC->ICER  ;
	NVIC_CAP[2] = *NVIC->ISPR  ;
	NVIC_CAP[3] = *NVIC->IP  ;

	SCB_CAP[0] = SCB->ICSR ;



	IRQSEL_CAP[0] = VOR_IRQSEL-> TIM[0]  ;
	ret_code = VOR_TIM0_MSecInitialize(100);  /* from original code   */
	ret_code = VOR_TIM1_SecInitialize(1);

	SysTick->LOAD = 0xFF00  ;    /*  enable SysTICK and force SysTick Exception   */
	SysTick->CTRL = 0x1  ;   // enable without interrupts
	SysTick->VAL = 0x500  ;
	IRQSEL_CAP[1] = VOR_IRQSEL-> TIM[0]  ;
	VOR_TIM0->CNT_VALUE = 0x7700  ;

	NVIC_SetPriority(IRQ_TIM0_IRQn,IRQ_TIM0_PRIORITY);
	NVIC_EnableIRQ(IRQ_TIM0_IRQn);

//	__enable_irq()  ; /*    directly clear the PRIMASK   */

//	VOR_TIM0->RST_VALUE = 0x77777  ;
	VOR_TIM0->CTRL = 0x13   ;

	 TC0[10] = VOR_TIM0->CTRL  ;
	 TC0[11] = VOR_TIM0->RST_VALUE  ;
	 TC0[12] = VOR_TIM0->CNT_VALUE  ;
	 TC0[13] = VOR_TIM0->CNT_VALUE  ;

	NVIC_CAP[10] = *NVIC->ISER  ;
	NVIC_CAP[11] = *NVIC->ICER  ;
	NVIC_CAP[12] = *NVIC->ISPR  ;
	NVIC_CAP[13] = *NVIC->IP  ;
	SCB_CAP[1] = SCB->ICSR ;
   i = 0  ;

//   VOR_Log_Init(PORTA_UARTA, LOG_DBG);


		  while (1)
		  {
		     count ++ ;
			 delay(500000)  ;  //  delay 500k counts
		 VOR_GPIO->BANK[0].TOGOUT |= (1 << PORTA_7_D3);
		 NVIC_SetPendingIRQ(0);

			NVIC_CAP[0] = *NVIC->ISER  ;  // capture NVIC register info
			NVIC_CAP[1] = *NVIC->ICER  ;
			NVIC_CAP[2] = *NVIC->ISPR  ;
			NVIC_CAP[3] = *NVIC->IP  ;
			SCB_CAP[2] = SCB->ICSR ;
            SCB_CAP[3] = SysTick->VAL   ;

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
