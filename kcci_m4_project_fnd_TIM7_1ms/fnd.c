/*
�����Ʈ : PORTC0 ~ PORTC7 : FND A ~ G 
        PORTC8 ~ PORTC11 : FND C0 ~ C3 
*/

#include "stm32f4xx.h"
#include "fnd.h"

// 7-���׸�Ʈ ��Ʈ�� �迭�� �����Ѵ�.
unsigned char Font[19] = {0x3F, 0X06, 0X5B, 0X4F, 
                                         0X66, 0X6D, 0X7C, 0X07,
                                         0X7F, 0X67, 0X77, 0X7C, 
                                         0X39, 0X5E, 0X79, 0X71, 
                                         0X08, 0X80, 0x40};

int fndFlag = 1;
volatile unsigned char arrayNum[4];
void FND_init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
                                                  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|
                                                  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11; // C ��Ʈ ���� 12��Ʈ�� ������� �����Ѵ�.
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  TIM7_init();
}

void TIM7_init()
{
   TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   NVIC_InitTypeDef   NVIC_InitStructure; 
   
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
      //���ͷ�Ʈ enable �� Priority ����.
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1;         // 84Mz /84  = 1us
  TIM_TimeBaseStructure.TIM_Period = 1000-1;           //       1us* 1000  = 1ms
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  //Ÿ�̸�7�� ���۽�Ų��.
  TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM7, ENABLE);
  
}
void TIM7_IRQHandler(void)      //1ms
{
  static int digit = 0;
  if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    display_digit(digit,arrayNum[digit]);  
    digit++;
    if(digit == 4)
      digit = 0;
  }
}
/*
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}
*/

void display_digit(int pos,int num)
{
 
  if(fndFlag)
  {
    GPIO_Write(GPIOC,GPIO_ReadInputData(GPIOC) | 0x0f00); //fnd all off
    GPIO_Write(GPIOC,GPIO_ReadInputData(GPIOC) & ~(GPIO_Pin_8 << pos));
    GPIO_Write(GPIOC,(GPIO_ReadInputData(GPIOC) & 0xff00 )| Font[num]);
//    Delay(1);
  }

}
void display_fnd( int N )  // Segment �Լ� ����
{
//  int  i ;
  int Buff ;
// unsigned char arrayNum[4];

   if(N < 0)
  {
    N=-N;
    arrayNum[0] = 18;
  }
  else
    arrayNum[0] = N /1000;  // ���׸�Ʈ���� ����ϴ� õ�� �ڸ��� ����
  
  Buff = N % 1000 ;
  arrayNum[1] = Buff / 100 ; // ���׸�Ʈ���� ����ϴ� �����ڸ� ����
  Buff = Buff % 100;
  arrayNum[2] = Buff /10 ;     // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����
  arrayNum[3] =  Buff % 10 ;    // ���׸�Ʈ���� ����ϴ� ���� �ڸ� ����      

//  for(i=0;i<4;i++)
//   display_digit(i,arrayNum[i]);
}

void display_onoff(int flag)
{

  fndFlag = flag;
  
  if(!fndFlag)
  {
    GPIO_Write(GPIOC,GPIO_ReadInputData(GPIOC) | 0x0f00); //fnd all off
  }
  
}