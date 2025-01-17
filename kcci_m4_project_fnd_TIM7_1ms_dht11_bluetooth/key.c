
/*
사용포트   PORTC12 ~ PORTC15 : Button0 ~ 3 
*/

#include "stm32f4xx.h"
#include "key.h"

volatile int key=0;

void KEY_init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure; 

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 |  GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 
  //포트A의 0번핀을 외부 인터럽트 0 과 연결한다.
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource12);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource15);
  
  //외부 인터럽트0 인터럽트를 허용한다.(우선순위는 가장높게 설정)
  //외부 인터럽트0가 상승엣지에서 발생되게 설정한다.
  EXTI_InitStructure.EXTI_Line = EXTI_Line12|EXTI_Line13|EXTI_Line14|EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
   //인터럽트 enable 및 Priority 설정.
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}


void EXTI15_10_IRQHandler(void)
{

  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  {
    key = 1;
    EXTI_ClearITPendingBit(EXTI_Line12);
  }
  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
    key = 2;
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
  if(EXTI_GetITStatus(EXTI_Line14) != RESET)
  {
    key = 3;
    EXTI_ClearITPendingBit(EXTI_Line14);
  }
  if(EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
    key = 4;    
    EXTI_ClearITPendingBit(EXTI_Line15);
  }
}