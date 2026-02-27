/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   液晶显示例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./lcd/bsp_ili9806g_lcd.h"
#include "bsp_basic_tim.h"
#include "stm32f4xx_it.h"
#include "bsp_adc.h"
#include "stm32_ub_lin_master.h"

//static void LCD_Test(void);	
static void LCD_Configuration(void);	
static void LCD_Clear(void);	
//static void Delay ( __IO uint32_t nCount );
//static void Printf_Charater(void);
static void LCD_Update(float OCR,float Pressure,float Temp);
static void uart_trans(uint8_t i,float OCR_trans,float Pressure_trans,float Temp_trans);
static void Parameter_Init(void);

// ADC转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[RHEOSTAT_NOFCHANEL];

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal[RHEOSTAT_NOFCHANEL]={0}; 
extern float flyv;
float OCR_to_LED;
extern uint8_t flytime_fromTI[7];
extern uint8_t flytime_Suc;
uint8_t OCR_Trsv[4]={0};
uint8_t Pressure_Trsv[4]={0};
uint8_t Temp_Trsv[4]={0};
uint8_t flytime_Rev[6]={0};

struct Parameter{
 float MIN;
 float MAX;
 float circle_gain;
 uint16_t circle_start;
 uint16_t circle_stop;
 uint16_t Line_long;	
 };
 struct Parameter OCRPara,PressurePara,TempPara;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{
	LIN_FRAME_t myFrame;
  //LIN_ERR_t check;
	
  LED_GPIO_Config();
	
	ILI9806G_Init ();         //LCD 初始化

	Debug_USART_Config();		
	TIMx_Configuration();
  Rheostat_Init();	
  ILI9806G_GramScan ( 3 );

	// init the UART as LIN master
  UB_LIN_Master_Init();
	
	LCD_Configuration();
	//LCD_SetColors(BLACK,BLACK);//NT
	
	Parameter_Init();
	while ( 1 )
	{
		
		//LCD_Test1();
		if(b_500ms_task_flag)
		{		static float ii=0;
			  uint8_t i=0;
			  uint8_t j=0;
		    ii=ii+5;
			if(ii>400)
			{
				ii=0;
			}
			
			//uart_trans()
			b_500ms_task_flag=0;
			//LCD_Test1();
			LCD_Clear();
      LCD_Update(ii/13,ii/6,ii/7);
			uart_trans(i,ii/13,ii/6,ii/7);
			if(flytime_Suc==0x01)
			{
				for(j=0;j<6;j++)
				{
				flytime_Rev[j]=flytime_fromTI[j];
				flytime_fromTI[j]=0x00;
				}
				flytime_Suc=0x00;
			}

		}
		
		if(b_3s_task_flag)
		{
		b_3s_task_flag=0;
		ADC_ConvertedValueLocal[0] =(float) ADC_ConvertedValue[0]/4096*(float)3.3;
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue[1]/4096*(float)3.3; //T
		ADC_ConvertedValueLocal[2] =(float) ADC_ConvertedValue[2]/4096*(float)3.3; //P
	
			OCR_to_LED = 0.257*ADC_ConvertedValueLocal[1]+0.047*flyv-0.025*ADC_ConvertedValueLocal[2]-29.554;
			
//		printf("\r\n CH1_C3 value = %f V \r\n",ADC_ConvertedValueLocal[0]);
//		printf("\r\n CH2_PA4 value = %f V \r\n",ADC_ConvertedValueLocal[1]);
//		printf("\r\n CH3_PA6 value = %f V \r\n",ADC_ConvertedValueLocal[2]);

		}
		
		if(b_5s_task_flag)
		{
		b_5s_task_flag=0;
		myFrame.frame_id=0x1A;
    myFrame.data_len=0x08;
    myFrame.data[0]=0xA1;
    myFrame.data[1]=0xB2;
		myFrame.data[2]=0xB2;
		myFrame.data[3]=0xB2;
		myFrame.data[4]=0xB2;
		myFrame.data[5]=0xB2;
		myFrame.data[6]=0xB2;
		myFrame.data[7]=0xB2;
    UB_LIN_SendData(&myFrame);
		//	LCD_Test1();
		//	LED1_TOGGLE;
		}
		
		if(b_10s_task_flag)
		{
			b_10s_task_flag=0;
		//	LCD_Test1();
		//	LED1_TOGGLE;
		}
		
	}
	
	
}
/**
  * @brief  LCD Configuration
  * @param  nCount ：----
  * @retval 无
  */
void LCD_Configuration(void)
{
	/*演示显示变量*/

	char dispBuff[100];

	
	LCD_SetFont(&Font16x32);
	LCD_SetColors(BLACK,GREY);//NT

  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	//LCD_SetColors(BLACK,WHITE);//NT
	//ILI9806G_DispString_EN_CH
	sprintf(dispBuff,"%%");
	ILI9806G_DispString_EN_CH(700,180,dispBuff);
		sprintf(dispBuff,"MPa");
	ILI9806G_DispString_EN_CH(700,280,dispBuff);
		sprintf(dispBuff,"C");
	ILI9806G_DispString_EN_CH(700,380,dispBuff);
	/********显示字符串示例*******/
	ILI9806G_DrawRectangle(0,0,800,32,1);
	LCD_SetBackColor(BLUE);
	LCD_SetTextColor(WHITE);
	ILI9806G_DispStringLine_EN(LINE(0),"PRODUCT : USSCOOL");
	
	LCD_SetTextColor(BLUE2);
	LCD_SetBackColor(GREY);
	LCD_SetFont(&Font24x48);
  ILI9806G_DispString_EN(80,80,"POE in R454C");
  
	//LCD_SetFont(&Font16x32);
	
	LCD_SetTextColor(RED);
	ILI9806G_DrawLine(50,200,400,200); 
	LCD_SetTextColor(BLACK);
	ILI9806G_DrawCircle(50,200,10,1);
	sprintf(dispBuff,"OCR: 00.0");    //sprintf(dispBuff,"OCR: %.1f ",testper);
	ILI9806G_DispString_EN_CH(420,180,dispBuff);
	
	LCD_SetTextColor(RED);
	ILI9806G_DrawLine(50,300,400,300);
	LCD_SetTextColor(BLACK);
	ILI9806G_DrawCircle(100,300,10,1);
	sprintf(dispBuff," P : ---");
	ILI9806G_DispString_EN_CH(420,280,dispBuff);
	
	LCD_SetTextColor(RED);
  ILI9806G_DrawLine(50,400,400,400);
	LCD_SetTextColor(BLACK);
	ILI9806G_DrawCircle(150,400,10,1);
	sprintf(dispBuff," T : ---");  
	ILI9806G_DispString_EN_CH(420,380,dispBuff);
}

void LCD_Clear()
{
	ILI9806G_Clear(520,180,150,60); //clear data OCR
	ILI9806G_Clear(520,280,150,60); //clear data P
	ILI9806G_Clear(520,380,150,60); //clear data T
	
	ILI9806G_Clear(40,190,375,30);  //clear circle OCR update 40
	ILI9806G_Clear(40,290,375,30);  //clear circle OCR update 40
	ILI9806G_Clear(40,390,375,30);  //clear circle OCR update 40
	
	//OCR_circle_Last=40;
}

void LCD_Update(float OCR,float Pressure,float Temp)
{
	
	char dispBuff[100];
	static uint16_t OCR_Cir_Pos = 0;
	static uint16_t Pre_Cir_Pos = 0;
	static uint16_t Tmp_Cir_Pos = 0;
  LCD_SetBackColor(GREY);
		if(OCR>=OCRPara.MIN&&OCR<=OCRPara.MAX)
		{
			 OCR_Cir_Pos = (OCR - OCRPara.MIN)*OCRPara.circle_gain+OCRPara.circle_start;
			 LCD_SetTextColor(RED);
			 ILI9806G_DrawLine(OCRPara.circle_start,200,OCRPara.circle_stop,200); 
			 LCD_SetTextColor(BLACK);
			 ILI9806G_DrawCircle(OCR_Cir_Pos,200,10,1);
			 sprintf(dispBuff,"OCR: %.1f ",OCR);    //sprintf(dispBuff,"OCR: %.1f ",testper);
			 ILI9806G_DispString_EN_CH(420,180,dispBuff);
		}
		else
		{
			 LCD_SetTextColor(RED);
			 ILI9806G_DrawLine(50,200,400,200); 
			 LCD_SetTextColor(RED);
			 ILI9806G_DrawCircle(400,200,10,1);
			 sprintf(dispBuff,"OCR:Err");    //sprintf(dispBuff,"OCR: %.1f ",testper);
			 ILI9806G_DispString_EN_CH(420,180,dispBuff);
		}
		if(Pressure>=PressurePara.MIN&&Pressure<=PressurePara.MAX)
		{
			  Pre_Cir_Pos = (Pressure - PressurePara.MIN)*PressurePara.circle_gain+PressurePara.circle_start;
				LCD_SetTextColor(RED);
				ILI9806G_DrawLine(PressurePara.circle_start,300,PressurePara.circle_stop,300);
				LCD_SetTextColor(BLACK);
				ILI9806G_DrawCircle(Pre_Cir_Pos,300,10,1);
				sprintf(dispBuff," P : %.2f",Pressure);
				ILI9806G_DispString_EN_CH(420,280,dispBuff);
		}
		else
		{
				LCD_SetTextColor(RED);
				ILI9806G_DrawLine(50,300,400,300);
				LCD_SetTextColor(RED);
				ILI9806G_DrawCircle(400,300,10,1);
				sprintf(dispBuff," P :Err");
				ILI9806G_DispString_EN_CH(420,280,dispBuff);
		}
		if(Temp>=TempPara.MIN&&Temp<=TempPara.MAX)
		{
			  Tmp_Cir_Pos = (Temp - TempPara.MIN)*TempPara.circle_gain+TempPara.circle_start;
				LCD_SetTextColor(RED);
				ILI9806G_DrawLine(TempPara.circle_start,400,TempPara.circle_stop,400);
				LCD_SetTextColor(BLACK);
				ILI9806G_DrawCircle(Tmp_Cir_Pos,400,10,1);
				sprintf(dispBuff," T : %.2f ",Temp);  
				ILI9806G_DispString_EN_CH(420,380,dispBuff);
		}
		
		else
		{
				LCD_SetTextColor(RED);
				ILI9806G_DrawLine(50,400,400,400);
				LCD_SetTextColor(RED);
				ILI9806G_DrawCircle(400,400,10,1);
				sprintf(dispBuff," T :Err");  
				ILI9806G_DispString_EN_CH(420,380,dispBuff);
		}
}


void Parameter_Init()
{

	 OCRPara.MAX=10;
	 OCRPara.MIN=0;
	 OCRPara.circle_start=50;
	 OCRPara.circle_stop=400;
	 OCRPara.Line_long = OCRPara.circle_stop - OCRPara.circle_start;
	 OCRPara.circle_gain = OCRPara.Line_long/(OCRPara.MAX - OCRPara.MIN);
	 
	 PressurePara.MAX=30;
	 PressurePara.MIN=0;
	 PressurePara.circle_start=50;
	 PressurePara.circle_stop=400;
	 PressurePara.Line_long = PressurePara.circle_stop - PressurePara.circle_start; 
	 PressurePara.circle_gain = PressurePara.Line_long/(PressurePara.MAX - PressurePara.MIN);
	 
	 TempPara.MAX=50;
	 TempPara.MIN=10;
	 TempPara.circle_start=50;
	 TempPara.circle_stop=400;
	 TempPara.Line_long =TempPara.circle_stop -TempPara.circle_start;
	 TempPara.circle_gain = TempPara.Line_long/(TempPara.MAX - TempPara.MIN);
}	

void uart_trans(uint8_t i,float OCR_trans,float Pressure_trans,float Temp_trans)
{
				for(i=0;i<4;i++)
			{
				if(i==0)
				{
					OCR_Trsv[0]=0x5A;
				}
				else if(i==1)
				{
					OCR_Trsv[1]=0x3A;
				}
				else if(i==2)
				{
				  OCR_Trsv[2]=(((uint16_t)(OCR_trans*10))&(0xFF00))>>8;
				}
				else if(i==3)
				{
					 OCR_Trsv[3]=(((uint16_t)(OCR_trans*10))&(0x00FF));
				}
				else
				{
					OCR_Trsv[0]=0;
					OCR_Trsv[1]=0;
					OCR_Trsv[2]=0;
					OCR_Trsv[3]=0;
				}
			   Usart_SendByte(DEBUG_USART,OCR_Trsv[i]);
			}
			
						for(i=0;i<4;i++)
			{
				if(i==0)
				{
					Pressure_Trsv[0]=0x5A;
				}
				else if(i==1)
				{
					Pressure_Trsv[1]=0x7B;
				}
				else if(i==2)
				{
				  Pressure_Trsv[2]=(((uint16_t)(Pressure_trans*10))&(0xFF00))>>8;
				}
				else if(i==3)
				{
					 Pressure_Trsv[3]=(((uint16_t)(Pressure_trans*10))&(0x00FF));
				}
				else
				{
					Pressure_Trsv[0]=0;
					Pressure_Trsv[1]=0;
					Pressure_Trsv[2]=0;
					Pressure_Trsv[3]=0;
				}
			   Usart_SendByte(DEBUG_USART,Pressure_Trsv[i]);
			}
			
									for(i=0;i<4;i++)
			{
				if(i==0)
				{
					Temp_Trsv[0]=0x5A;
				}
				else if(i==1)
				{
					Temp_Trsv[1]=0xA9;
				}
				else if(i==2)
				{
				  Temp_Trsv[2]=(((uint16_t)(Temp_trans*10))&(0xFF00))>>8;
				}
				else if(i==3)
				{
					 Temp_Trsv[3]=(((uint16_t)(Temp_trans*10))&(0x00FF));
				}
				else
				{
					Temp_Trsv[0]=0;
					Temp_Trsv[1]=0;
					Temp_Trsv[2]=0;
					Temp_Trsv[3]=0;
				}
			   Usart_SendByte(DEBUG_USART,Temp_Trsv[i]);
			}
}

void uart_rev(float flyspeed)
{
	static uint8_t ii=0;
	while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);
	for(ii=0;ii<6;ii++)
	{
		
	}
	//flyspeed 
}
	
/**
  * @brief  简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
//static void Delay ( __IO uint32_t nCount )
//{
//  for ( ; nCount != 0; nCount -- );
//	
//}


/*********************************************END OF FILE**********************/

