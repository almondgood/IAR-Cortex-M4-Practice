#include "stm32f4xx.h"
#include "uart2.h"
#include <stdio.h>
#include <stdlib.h>

extern volatile unsigned char rx2Flag;
extern char rx2Data[50];

static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}


int main()
{
  uint16_t key;
  uint16_t prev_key;
  
  GPIO_InitTypeDef   GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
                                             GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; // C 포트 하위 8비트를 출력으로 선언한다.
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|
                                             GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15; // C 포트 상위 8비트를 입력으로 선언한다.
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  UART2_init();
  printf("Start main()\n\r");
  while(1)
  {
   
    if (rx2Flag) {
      printf("recv :%s\r\n", rx2Data);
      key = atoi(rx2Data);
      GPIO_Write(GPIOC, key);  
      rx2Flag = 0;
  }
  
  key = (GPIO_ReadInputData(GPIOC) & 0xFF00) >> 8;
  if (key != prev_key) {
    if (key != 0) {
      printf("key: 0x%02x \r\n", key);
      GPIO_Write(GPIOC, key);  
      }
    
    prev_key = key;
    }  
  }
}
