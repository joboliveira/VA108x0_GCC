/* File: startup_ARMCM0.S
 * Purpose: startup file for Cortex-M0 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
/* Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

   /*  >>> Vorago notes  <<<
   			modified for VA108xx in Jan 2018
   			-  Not all Interrupts supported in this version

   */
	.thumb
	.syntax	unified
	.cpu cortex-m0
	.fpu softvfp
	.arch	armv6-m

    .section .isr_vector
	.align 2
	.globl __isr_vector 
	
	__isr_vector:
	.long	_stack_start   /*  __StackTop CEM */          /* Top of Stack = 0x10001800  */
	.long	Reset_Handler         /* Reset Handler */
	.long	NMI_Handler           /* NMI Handler */
	.long	HardFault_Handler     /* Hard Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVC_Handler           /* SVCall Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	PendSV_Handler        /* PendSV Handler */
	.long	SysTick_Handler       /* SysTick Handler */

	/* External interrupts */
	
	
	.long	OC0_IRQHandler
	.long	VOR_TIM1_IRQHandler
	.long	OC2_IRQHandler
	.long	VOR_UART0_IRQHandler
	.long	VOR_UART1_IRQHandler
	.long	VOR_GPIO_IRQHandler
	.long	VOR_SPI0_IRQHandler
	.long	VOR_SPI1_IRQHandler
	.long	VOR_SPI2_IRQHandler
	.long	VOR_I2C0_MS_IRQHandler
	.long	VOR_I2C0_SL_IRQHandler
	.long	VOR_I2C1_MS_IRQHandler
	.long	VOR_I2C1_SL_IRQHandler
	.long	OC13_IRQHandler
	.long	OC14_IRQHandler
	.long	OC15_IRQHandler
	.long	OC16_IRQHandler
	.long	OC17_IRQHandler
	.long	OC18_IRQHandler
	.long	OC19_IRQHandler
	.long	OC20_IRQHandler
	.long	OC21_IRQHandler
	.long	OC22_IRQHandler
	.long	OC23_IRQHandler
	.long	OC24_IRQHandler
	.long	OC25_IRQHandler
	.long	OC26_IRQHandler
	.long	OC27_IRQHandler
	.long	OC28_IRQHandler
	.long	OC29_IRQHandler
	.long	OC30_IRQHandler
	.long	OC31_IRQHandler
/*  	.long	Default_Handler   */ 
.size __isr_vector, . - __isr_vector  


 




	.text
	
	.align
	.thumb_func
	
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

/*     cpsid i         removed by CEM Vorago code is expecting PRIMASK (primary mask to be clear)  */
       cpsie i    /*  enable interrupts , clear PRIMASK  */
       cpsie f
   ldr   r0, =_stack_start
   mov  sp, r0
   


   /*  Move UART, I2C, SPI structure pointer Data from ROM space to RAM  */
   /*  Linker defines DATA_START, ROM_AT, DATA_END    */
   movs r1,#0    /* initialize r1 to 0x0  */

/*
   b LABEL_move_struct_info
 LABEL_move_2ROM:
   ldr r3, =__ROM_AT
   ldr r3,[r3,r1]
   str r3,[r0,r1]
   adds r1,r1,#4

 LABEL_move_struct_info:
   ldr r0,=_sdata
   ldr r3,=_edata
   adds r2,r0,r1
   cmp r2,r3
   bcc LABEL_move_2ROM
*/

   /* abide by ARM calling conventions  */
   movs  r0, #0 
   movs  r1, #0 
   movs  r2, #0 
   movs  r3, #0 
   
   bl	main
	
halt:
      b halt  
	  
	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
  ldr r3, NVIC_INT_CTRL_CONST   /*  load int control reg  */
  ldr r2, [r3, #0]
  uxtb r2, r2      /*  int # should be in least sig byte now  */

  ldr r3, SBC_HFSR   /*  load hint for hardfault status int r4  */
  ldr r4, [r3, #0]

  ldr r3, SBC_CFSR   /*  load hint for configurable fault status int r5  */
  ldr r5, [r3, #0]

	b	.   /*  branch to self, infinite loop   */
	.size	Default_Handler, . - Default_Handler
	
NMI_Handler:
	b	.   /*  branch to self, infinite loop   */
	
HardFault_Handler:
	b	.   /*  branch to self, infinite loop   */

SVC_Handler:
	b	.   /*  branch to self, infinite loop   */



/*  SysTick_Handler:  */
/*	b	. */  /*  branch to self, infinite loop   */


.align 4

NVIC_INT_CTRL_CONST:  .word 0xe000ed04
SBC_HFSR:  .word 0xe000ed2c
SBC_CFSR:  .word 0xe000ed28
	/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler


  .weak OC0_IRQHandler
  .thumb_set OC0_IRQHandler,Default_Handler

  .weak VOR_TIM1_IRQHandler
  .thumb_set VOR_TIM1_IRQHandler,Default_Handler

   .weak  OC2_IRQHandler
  .thumb_set OC2_IRQHandler,Default_Handler

     .weak  VOR_UART0_IRQHandler
  .thumb_set VOR_UART0_IRQHandler,Default_Handler

     .weak  VOR_UART1_IRQHandler
  .thumb_set VOR_UART1_IRQHandler,Default_Handler

     .weak  VOR_GPIO_IRQHandler
  .thumb_set VOR_GPIO_IRQHandler,Default_Handler

     .weak  VOR_SPI0_IRQHandler
  .thumb_set VOR_SPI0_IRQHandler,Default_Handler

     .weak  VOR_SPI1_IRQHandler
  .thumb_set VOR_SPI1_IRQHandler,Default_Handler

     .weak  VOR_SPI2_IRQHandler
  .thumb_set VOR_SPI2_IRQHandler,Default_Handler

     .weak  VOR_I2C0_MS_IRQHandler
  .thumb_set VOR_I2C0_MS_IRQHandler,Default_Handler

     .weak  VOR_I2C0_SL_IRQHandler
  .thumb_set VOR_I2C0_SL_IRQHandler,Default_Handler

      .weak  VOR_I2C1_MS_IRQHandler
  .thumb_set VOR_I2C1_MS_IRQHandler,Default_Handler

     .weak  VOR_I2C1_SL_IRQHandler
  .thumb_set VOR_I2C1_SL_IRQHandler,Default_Handler

     .weak  OC13_IRQHandler
  .thumb_set OC13_IRQHandler,Default_Handler

     .weak  OC14_IRQHandler
  .thumb_set OC14_IRQHandler,Default_Handler

     .weak  OC15_IRQHandler
  .thumb_set OC15_IRQHandler,Default_Handler

     .weak  OC16_IRQHandler
  .thumb_set OC16_IRQHandler,Default_Handler

     .weak  OC117_IRQHandler
  .thumb_set OC17_IRQHandler,Default_Handler

     .weak  OC18_IRQHandler
  .thumb_set OC18_IRQHandler,Default_Handler

     .weak  OC19_IRQHandler
  .thumb_set OC19_IRQHandler,Default_Handler

     .weak  OC20_IRQHandler
  .thumb_set OC20_IRQHandler,Default_Handler

     .weak  OC21_IRQHandler
  .thumb_set OC21_IRQHandler,Default_Handler

     .weak  OC22_IRQHandler
  .thumb_set OC22_IRQHandler,Default_Handler

     .weak  OC23_IRQHandler
  .thumb_set OC23_IRQHandler,Default_Handler

     .weak  OC24_IRQHandler
  .thumb_set OC24_IRQHandler,Default_Handler

     .weak  OC25_IRQHandler
  .thumb_set OC25_IRQHandler,Default_Handler

     .weak  OC26_IRQHandler
  .thumb_set OC26_IRQHandler,Default_Handler

     .weak  OC27_IRQHandler
  .thumb_set OC27_IRQHandler,Default_Handler

     .weak  OC28_IRQHandler
  .thumb_set OC28_IRQHandler,Default_Handler

     .weak  OC29_IRQHandler
  .thumb_set OC29_IRQHandler,Default_Handler

       .weak  OC30_IRQHandler
  .thumb_set OC30_IRQHandler,Default_Handler

       .weak  OC31_IRQHandler
  .thumb_set OC31_IRQHandler,Default_Handler

	.end
/****************END OF FILE redirect with weak ****/
