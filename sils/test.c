#include "va108xx.h"
#include "stdio.h"
#include "stdint.h"

volatile int msec_tim_timeout = 1 ;

/* Local defines ------------------------------------------------------------*/
#define PORTA_10_D2 			10
#define PORTA_7_D3 				7
#define PORTA_6_D4	 			6
#define PORTA_11_SW_USER 		11


/* Private variables ---------------------------------------------------------*/


/*  function prototypes ------------*/


/* Private functions ---------------------*/
//  void delay (int a);
/**
  * @brief  Main program.   (Blink LED on REB1 brd)
  * @param  None
  * @retval None
  */
int main(void)
{
	 volatile int count = 1  ;  /* counts the # of blinks  */

    printf("Teste de codigo embedded no SILS\r\n");
	/* Enable clock for all peripherals */

		VOR_SYSCONFIG_Type VOR_SYSCONFIG_reg;
		VOR_SYSCONFIG_Type *VOR_SYSCONFIG_p = &VOR_SYSCONFIG_reg;

		VOR_SYSCONFIG_p->PERIPHERAL_CLK_ENABLE = (CLK_ENABLE_PORTA | CLK_ENABLE_PORTB |\
      CLK_ENABLE_SPIA | CLK_ENABLE_SPIB | CLK_ENABLE_SPIC | CLK_ENABLE_UARTA |\
      CLK_ENABLE_UARTB | CLK_ENABLE_I2CA | CLK_ENABLE_I2CB | CLK_ENABLE_IRQSEL |\
      CLK_ENABLE_IOMGR | CLK_ENABLE_UTILITY | CLK_ENABLE_PORTIO | CLK_ENABLE_SYSTEM );

/*
		VOR_SYSCONFIG_p->PERIPHERAL_CLK_ENABLE = (CLK_ENABLE_PORTA | CLK_ENABLE_PORTB |\
      CLK_ENABLE_SPIA | CLK_ENABLE_SPIB | CLK_ENABLE_SPIC | CLK_ENABLE_UARTA |\
      CLK_ENABLE_UARTB | CLK_ENABLE_I2CA | CLK_ENABLE_I2CB | CLK_ENABLE_IRQSEL |\
      CLK_ENABLE_IOMGR | CLK_ENABLE_UTILITY | CLK_ENABLE_PORTIO | CLK_ENABLE_SYSTEM );
*/

    printf("Passou VOR_SYSCONFIG_Type->PERIPHERAL_CLK_ENABLE\r\n");

/* Configure PA[7]. This GPIO will turn ON LED D3. */
		VOR_GPIO_PERIPHERAL_BANK_Type VOR_GPIO_reg;
		VOR_GPIO_PERIPHERAL_BANK_Type *VOR_GPIO_p = &VOR_GPIO_reg;

    VOR_GPIO_p->BANK[0].DIR       |= (1 << PORTA_7_D3);
	  VOR_GPIO_p->BANK[0].DATAMASK  |= (1 << PORTA_7_D3);
    VOR_GPIO_p->BANK[0].DATAOUT   |= (1 << PORTA_7_D3);

    printf("Passou VOR_GPIO_p->BANK\r\n");

      /* Systick added for debug purposes  5/29  */

//      	SysTick -> CTRL = 0  ;
//      	SysTick -> LOAD = 0x800000  ;
//      	SysTick -> VAL = 0x00   ;
//      	SysTick -> CTRL = 7  ;    /* enable systick with ints enabled */

      //	ret_code = VOR_TIM0_MSecInitialize(100);
      	msec_tim_timeout = 99   ;
      	while(1)
      	{

       		//if(msec_tim_timeout!=0)
      				{
      					count ++  ;
      					msec_tim_timeout = 0 ;
								//Observação: Enquanto o TOGOUT estiver em 1,
								//            a saída deve se comportar como um tipo FF do tipo T
      					VOR_GPIO_p->BANK[0].TOGOUT |= (1 << PORTA_7_D3);
      				}
              fprintf(stdout,"Count %d\r\n",count);
							fprintf(stdout,"Porta_7_D3 %d\r\n",VOR_GPIO_p->BANK[0].TOGOUT);
      	}


//  while (1)    /*  loop forever here   */
//  {
//     count ++ ;
//	 delay(500000)  ;  /*  delay 500k counts  */
//	 VOR_GPIO->BANK[0].TOGOUT |= (1 << PORTA_7_D3);
    
//  }
  return 0;
}   /* **************  foot of main loop  ****************   */
//void delay (int a)
//{
//	volatile int i,j;
	
//	for (i=0 ; i < a ; i++)
//	{
//		j++;
//	}
	
//	return;
// }



/*   added to show SysTick Exception firing     */
void SysTick_Handler(void)
{
	msec_tim_timeout = 1  ;

}

/********** *****END OF FILE****/
