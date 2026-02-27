/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "bsp_basic_tim.h"

uint8_t flytime_fromTI[7]={0};
uint8_t data[5]={0};
uint8_t flytime_Suc=0;
uint8_t b_500ms_task_flag = 0;
uint8_t b_3s_task_flag = 0;
uint8_t b_5s_task_flag = 0;
uint8_t b_10s_task_flag = 0;
float flytime_int = 0;
float flytime_decimal = 0;
float flytime = 0;
float flyv = 0;
/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/
void DEBUG_USART_IRQHandler(void)
{
  static uint8_t i=0;
	
	if(i>5)
	{
		i=0;
	}

		  if(flytime_fromTI[0]!=0xA5)
		{
			i=0;
		}
		
	if(USART_GetITStatus(DEBUG_USART,USART_IT_RXNE)!=RESET)
	{		


		  flytime_fromTI[i] = USART_ReceiveData( DEBUG_USART );
		
		if(flytime_fromTI[0]==0xA5)//&&flytime_fromTI[6]==0x9B
		{
			if(i==5)
			{
			flytime_Suc=0x01;
			}
			data[0] = flytime_fromTI[1];
			data[1] = flytime_fromTI[2];
			data[2] = flytime_fromTI[3];
			data[3] = flytime_fromTI[4];
			data[4] = flytime_fromTI[5];
		}
    //USART_SendData(DEBUG_USART,ucTemp);   
     i++;		
		if(flytime_Suc)
		{
			flytime_decimal = (float)((data[1]<<8&0xFF00)+(data[0]&0xFF))/10000;
			flytime_int = (float)((data[4]&0xFF)+(data[3]<<8&0xFF00)+(data[2]<<16&0xFF0000))/10000;
			flytime = flytime_int + (float)flytime_decimal;
			flyv=flytime;
			flytime_Suc=0;
		}
//	  if(flytime_fromTI[0]!=0xA5)
//		{
//			i=0;
//		}
	}	 
}	

void  BASIC_TIM_IRQHandler (void)
{
	static uint8_t u_3s_task_count=0;
	static uint8_t u_5s_task_count=0;
	static uint8_t u_10s_task_count=0;
	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
	{	
		u_3s_task_count++;
		u_5s_task_count++;
		u_10s_task_count++;
		//LED1_TOGGLE;
		b_500ms_task_flag = 1;
		if(u_3s_task_count>=9)
		{
			b_3s_task_flag = 1;
			u_3s_task_count = 3;
		}
		
		if(u_5s_task_count>=15)
		{
			b_5s_task_flag = 1;
			u_5s_task_count = 5;
		}
		
		if(u_10s_task_count>=27)
		{
			b_10s_task_flag = 1;
			u_10s_task_count=7;
		}
		TIM_ClearITPendingBit(BASIC_TIM , TIM_IT_Update);  		 
	}		 	
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
