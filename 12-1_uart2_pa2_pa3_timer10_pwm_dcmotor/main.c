/******************************************************************************
* General-purpose timers TIM10

  ��Ʈ ����:
    1)  ARM Cortex-M4 ����� ��ƮC (PC0 ~ PC7)�� 8�� ���̺� �̿��ؼ�
        Array FND����� SA_A ~ SA_H�� �����Ѵ�. (SA_A�� PC0�� ����Ǿ� �Ѵ�.)
    2)  ARM Cortex-M4 ����� ��ƮC (PC8 ~ PC11)�� 4�� ���̺� �̿��ؼ�
        Array FND����� C0 ~ C3�� �����Ѵ�. (C0�� PC8�� ����Ǿ� �Ѵ�.)
    3)  ARM Cortex-M4 ����� ��ƮB (PB8)�� 2�� ���̺� �̿��ؼ�
        LED����� LED0�� �����Ѵ�. (LED0�� PB8(TIM10_CH1)�� ����Ǿ� �Ѵ�.)
******************************************************************************/



// stm32f4xx�� �� �������͵��� ������ �������
#include "stm32f4xx.h"
#include "key.h"
#include "uart2.h"
#include <stdio.h>

extern volatile int key;
volatile unsigned int tim4_counter=0;
// delay �Լ�
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}

// 7-���׸�Ʈ ��Ʈ�� �迭�� �����Ѵ�.
unsigned char Font[18] = {0x3F, 0X06, 0X5B, 0X4F, 
                                         0X66, 0X6D, 0X7C, 0X07,
                                         0X7F, 0X67, 0X77, 0X7C, 
                                         0X39, 0X5E, 0X79, 0X71, 
                                         0X08, 0X80};


int tr_cnt = 0,mr_cnt=0;
int tl_cnt =0 ,ml_cnt = 0;


void RSegment ( int N )  // Segment �Լ� ����
{
  int  i ;
  unsigned char N10, N1 ;

  N10 = N /10 ;     // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����
  N1 =  N % 10 ;    // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����      

  for( i = 0 ; i < 5; i++ )
  {
    GPIO_Write(GPIOC, Font[N10]|0x0b00);  // ���� �� ��° ���׸�Ʈ�� ON�ϰ�, ���� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1);

    GPIO_Write(GPIOC, Font[N1]|0x0700);   // ���� �� ��° ���׸�Ʈ�� ON�ϰ�, ���� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1); 
  }
}

void LSegment ( int N )  // Segment �Լ� ����
{
  int  i ;
  unsigned char N10, N1 ;

  N10 = N /10 ;     // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����
  N1 =  N % 10 ;    // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����      

  for( i = 0 ; i < 5; i++ )
  {
    GPIO_Write(GPIOC, Font[N10]|0x0e00);  // ���� �� ��° ���׸�Ʈ�� ON�ϰ�, ���� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1);

    GPIO_Write(GPIOC, Font[N1]|0x0d00);   // ���� �� ��° ���׸�Ʈ�� ON�ϰ�, ���� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1); 
  }   
}

void TIM1_UP_TIM10_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
    tl_cnt++;                   //17.7ms * 28  =  495.6ms  = 0.5Sec
    if(tl_cnt >= 28)
    {
      tl_cnt = 0;
      ml_cnt ++ ;               //0.5Sec
      if(ml_cnt > 99) ml_cnt = 0;
    }  
  }
  if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    tr_cnt++ ;
    if(tr_cnt >= 56)            //17.7ms * 56  =  991.26ms  = 1Sec
    {
      tr_cnt = 0;
      mr_cnt ++ ;       //1Sec
      if(mr_cnt > 99) mr_cnt = 0;
    } 
  }
}

void    TIM4_init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure; 

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;             //TIM4_CH2
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  
  TIM_ClearITPendingBit(TIM4, TIM_IT_Trigger);
  TIM_ITConfig(TIM4, TIM_IT_Trigger, ENABLE);
  TIM_TIxExternalClockConfig(TIM4,TIM_TIxExternalCLK1Source_TI2,TIM_ICPolarity_Rising,0x0f);

  TIM_Cmd(TIM4,ENABLE);
}

void TIM4_IRQHandler()
{
  if(TIM_GetITStatus(TIM4, TIM_IT_Trigger) != RESET)
  {
      tim4_counter++;
      TIM_ClearITPendingBit(TIM4, TIM_IT_Trigger);
  }
}

int main()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 // NVIC_InitTypeDef   NVIC_InitStructure; 
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  int dcmotor_start=0;  //stop
  int dcmotor_dir=0;    //left
  int dutyrate = 50;            //50%
  int pluse = (int)(17700 * (dutyrate / 100.0 ));       //8850
  
  int pre_tim4_counter=0;
  
  KEY_init();
  UART2_init();
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
                                GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|
                                GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;     // TIM10_CH1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  //���ͷ�Ʈ enable �� Priority ����.
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
  //system_stm32f4xx.c ����
  //TIM10 input clock (TIM10CLK) is set to 2 * APB2 clock (PCLK2),
  //since APB2 prescaler is different from 1.
  //TIM10CLK = 2 * PCLK2
  //PCLK2 = HCLK / 2  => TIM10CLK = HCLK = SystemCoreClock
  //TIM_CounterMode�� TIM_CounterMode_Up�� �����ϸ�, ī��Ʈ���� TIMx_ARR����
  //��������, Update �̺�Ʈ�� �߻��Ѵ�.
  //TIM_Prescaler�� 167��, TIM_ClockDivision�� TIM_CKD_DIV1(1����)��
  //�����ϸ�, Ÿ�̸�10 ī��Ʈ Ŭ���� (168MHz/1) /(167+1) = 1MHz (1us)�̴�.
  //TIM_Period�� 17699���� ����������(TIMx_ARR), ī���Ͱ� �̰��� ��������,
  //(1us x 17700(17699+1) = 17.7ms)
  TIM_TimeBaseStructure.TIM_Prescaler = 168-1;         //168Mhz/84 = 1MHz(1us)
  TIM_TimeBaseStructure.TIM_Period = 17700-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = 7080;         //40% duty
  TIM_OCInitStructure.TIM_Pulse = pluse-1;         //pluse=8850, 50% duty
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);

  //Ÿ�̸�10�� ���۽�Ų��.
//  TIM_ClearITPendingBit(TIM10, TIM_IT_CC1|TIM_IT_Update);
//  TIM_ITConfig(TIM10, TIM_IT_CC1|TIM_IT_Update, ENABLE);

  TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM10, ENABLE);

  TIM_Cmd(TIM10, ENABLE);

  
  TIM4_init();
  
  printf("start main()  %d\r\n",pluse);
  printf("dutyrate: %d, pluse: %d\r\n",dutyrate,pluse);
  while(1)
  {
    
    if(key != 0)
    {

      printf("key  %d\r\n",key);
      if(key == 1)     //motor start/stop
      {
        if(dcmotor_start == 0)          //left
        {
          dcmotor_start = 1;
          dcmotor_dir = 1;
          GPIO_SetBits(GPIOB,GPIO_Pin_9);               
          GPIO_ResetBits(GPIOB,GPIO_Pin_10);
          printf("Motor Left\r\n");
        }
        else if(dcmotor_start == 1)
        {         
          GPIO_ResetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_10);         
          dcmotor_start = 0;
          printf("Motor Stop\r\n");
        }       
      }
      if(key == 2)     //motor left/right
      {
        if(dcmotor_dir == 0)            //left
        {
          dcmotor_dir = 1;
          dcmotor_start = 1;
          GPIO_SetBits(GPIOB,GPIO_Pin_9);
          GPIO_ResetBits(GPIOB,GPIO_Pin_10);
          printf("Motor Left\r\n");
        }
        else if(dcmotor_dir == 1)       //right
        {
          dcmotor_dir = 0;
          dcmotor_start = 1;
          GPIO_ResetBits(GPIOB,GPIO_Pin_9);
          GPIO_SetBits(GPIOB,GPIO_Pin_10);
          printf("Motor Right\r\n");
        }      
      }

      else if(key == 3 || key == 4)
      {
        if(key ==3 && dutyrate >= 10)
          dutyrate -= 10;
        
        else if(key == 4 && dutyrate <= 90)
          dutyrate += 10;
        
        pluse = (int)(17700 * (dutyrate / 100.0 ));  
        
        printf("dutyrate: %d, pluse: %d\r\n",dutyrate,pluse);
        TIM_SetCompare1(TIM10,pluse);
      }       
      key = 0;
    }
    if(tim4_counter != pre_tim4_counter)
    {
      printf("tim4_counter : %d\r\n",tim4_counter);
      pre_tim4_counter = tim4_counter;
    }
//    LSegment(ml_cnt);
//    RSegment(mr_cnt);
  }
}
