/******************************************************************************
* General-purpose timers TIM9

  ��Ʈ ����:
    1)  ARM Cortex-M4 ����� ��ƮC (PC0 ~ PC7)�� 8�� ���̺� �̿��ؼ�
        Array FND����� SA_A ~ SA_H�� �����Ѵ�. (SA_A�� PC0�� ����Ǿ� �Ѵ�.)
    2)  ARM Cortex-M4 ����� ��ƮC (PC8 ~ PC11)�� 4�� ���̺� �̿��ؼ�
        Array FND����� C0 ~ C3�� �����Ѵ�. (C0�� PC8�� ����Ǿ� �Ѵ�.)
******************************************************************************/
// stm32f4xx�� �� �������͵��� ������ �������
#include "stm32f4xx.h"


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

int t_cnt = 0,m_cnt=0;


void Segment ( int N )  // Segment �Լ� ����
{
  int  i ;
  unsigned char N1000, N100, N10, N1 ;
  int Buff ;

  N1000 = N /1000;  // ���׸�Ʈ���� ����ϴ� õ�� �ڸ��� ����
  Buff = N % 1000 ;
  N100 = Buff / 100 ; // ���׸�Ʈ���� ����ϴ� �����ڸ� ����
  Buff = Buff % 100;
  N10 = Buff /10 ;     // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����
  N1 =  Buff % 10 ;    // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����      

  for( i = 0 ; i < 30; i++ )
  {
    GPIO_Write(GPIOC, Font[N1000]|0x0e00); // ���� ù ��° ���׸�Ʈ�� ON�ϰ�, õ�� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1);

    GPIO_Write(GPIOC, Font[N100]|0x0d00); // ���� �� ��° ���׸�Ʈ�� ON�ϰ�, ���� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1); 

    GPIO_Write(GPIOC, Font[N10]|0x0b00);  // ���� �� ��° ���׸�Ʈ�� ON�ϰ�, ���� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1);

    GPIO_Write(GPIOC, Font[N1]|0x0700);   // ���� �� ��° ���׸�Ʈ�� ON�ϰ�, ���� �ڸ�  ���ڸ� ����� �ش� 
    Delay(1); 
  }
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM9, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
    t_cnt++ ;
    if(t_cnt >= 100)
    {
      t_cnt = 0;
      m_cnt ++ ;
    }
  }
}


int main()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef   NVIC_InitStructure; 
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
                                GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|
                                GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //���ͷ�Ʈ enable �� Priority ����.
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //system_stm32f4xx.c ����
  //TIM9 input clock (TIM9CLK) is set to 2 * APB2 clock (PCLK2),
  //since APB2 prescaler is different from 1.
  //TIM9CLK = 2 * PCLK2
  //PCLK2 = HCLK / 2  => TIM9CLK = HCLK = SystemCoreClock
  //TIM_CounterMode�� TIM_CounterMode_Up�� �����ϸ�, ī��Ʈ���� TIMx_ARR����
  //��������, Update �̺�Ʈ�� �߻��Ѵ�.
  //TIM_Prescaler�� 167��, TIM_ClockDivision�� TIM_CKD_DIV1(1����)��
  //�����ϸ�, Ÿ�̸�9 ī��Ʈ Ŭ���� (168MHz/1) /(167+1) = 1MHz (1us)�̴�.
  //TIM_Period�� 9999���� ����������(TIMx_ARR), ī���Ͱ� �̰��� ��������,
  //(1us x 10000(9999+1) = 10ms)
  TIM_TimeBaseStructure.TIM_Prescaler = 168-1;         //168Mhz/84 = 1MHz(1us)
  TIM_TimeBaseStructure.TIM_Period = 10000-1;          // TOP == 10000
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 5000;
  TIM_OC1Init(TIM9, &TIM_OCInitStructure);

  //Ÿ�̸�9�� ���۽�Ų��.
  TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);
  TIM_Cmd(TIM9, ENABLE);

  while(1)
  {
    Segment(m_cnt);
  }
}
