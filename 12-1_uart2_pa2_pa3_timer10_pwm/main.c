/******************************************************************************
* General-purpose timers TIM10

��Ʈ ����:
1)  ARM Cortex-M4 ����� ��ƮC (PC0 ~ PC7)�� 8�� ���̺��� �̿��ؼ�
Array FND����� SA_A ~ SA_H�� �����Ѵ�. (SA_A�� PC0�� ����Ǿ� �Ѵ�.)
2)  ARM Cortex-M4 ����� ��ƮC (PC8 ~ PC11)�� 4�� ���̺��� �̿��ؼ�
Array FND����� C0 ~ C3�� �����Ѵ�. (C0�� PC8�� ����Ǿ� �Ѵ�.)
3)  ARM Cortex-M4 ����� ��ƮB (PB8)�� 2�� ���̺��� �̿��ؼ�
LED����� LED0�� �����Ѵ�. (LED0�� PB8(TIM10_CH1)�� ����Ǿ� �Ѵ�.)
******************************************************************************/
// stm32f4xx�� �� �������͵��� ������ �������
#include "stm32f4xx.h"
#include "key.h"
#include "uart2.h"
#include <stdio.h>


extern volatile int key;

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
        tl_cnt++;
        if(tl_cnt >= 28)            // 17.7ms * 29 = 495.6ms = 0.5s
        {
            tl_cnt = 0;
            ml_cnt ++ ;
            if(ml_cnt > 99) ml_cnt = 0;
        }  
    }
    if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
        tr_cnt++ ;
        if(tr_cnt >= 56)            // 17.7ms * 56 = 991.2ms = 1s
        {
            tr_cnt = 0;
            mr_cnt ++ ; 
            if(mr_cnt > 99) mr_cnt = 0;
        } 
    }
}


int main()
{
    int dutyRate = 50;
    int pulse = 17700 * (dutyRate / 100.0);
    
    KEY_init();
    UART2_init();
    
    GPIO_InitTypeDef   GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef   NVIC_InitStructure; 
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
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
    
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //���ͷ�Ʈ enable �� Priority ����.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
    //TIM_OCInitStructure.TIM_Pulse = 7080;         //40% duty
    TIM_OCInitStructure.TIM_Pulse = pulse;         //10% duty
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM10, &TIM_OCInitStructure);
    
    //Ÿ�̸�10�� ���۽�Ų��.
    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1|TIM_IT_Update);
    TIM_ITConfig(TIM10, TIM_IT_CC1|TIM_IT_Update, ENABLE);
    
    TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM10, ENABLE);
    
    TIM_Cmd(TIM10, ENABLE);


    printf("start main()\r\n");
    printf("dutyRate : %d\r\n", dutyRate);
    printf("pulse : %d\r\n", pulse);
    GPIO_Write(GPIOB, GPIO_Pin_9);
    
    
    while(1)
    {
        if (key != 0) {
            
            if (key == 1 && dutyRate < 100)
                dutyRate += 10;
            else if (key == 2 && dutyRate > 0)
                dutyRate -= 10;
            
            pulse = 17700 * (dutyRate / 100.0);
            
            if (pulse != 0)
                pulse -= 1;
            
            printf("dutyRate : %d\r\n", dutyRate);
            printf("pulse : %d\r\n", pulse);
            TIM_SetCompare1(TIM10, pulse);
            key = 0;
        }
        LSegment(ml_cnt);
        RSegment(mr_cnt);
    }
}