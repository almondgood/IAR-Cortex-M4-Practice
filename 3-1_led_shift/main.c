// stm32f4xx의 각 레지스터들을 정의한 헤더 파일
#include "stm32f4xx.h"

// delay 함수
static void Delay(const uint32_t Count) {
  __IO uint32_t index = 0;
  for(index = (16800 * Count); index != 0; index--); 
}

int main() {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // PORT A에 클럭을 공급
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  // C 포트 하위8비트를 출력으로 선언한다.
  GPIO_InitStructure.GPIO_Pin = 0x00ff;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  while(1) {
    
    
#if 1
    for (int i = 0;i < 8; i++){
      GPIO_Write(GPIOA, 0x01 << i);
      Delay(500);
    }
    
    for (int i = 0;i < 8; i++){
      GPIO_Write(GPIOA, 0x80  >> i);
      Delay(500);
    }
#endif
#if 0
  uint32_t pin = GPIO_Pin_0;
  for (pin = GPIO_Pin_0; pin != GPIO_Pin_8; pin <<= 1) {
    GPIO_Write(GPIOA, pin);
    Delay(500);
  }
#endif    
    
#if 0
  // C 포트 하위 8비트에 ‘high'를 출력한다.
  GPIO_SetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
  
  // 500 ms 지연
  Delay(500);
  // C 포트 하위 8비트에 ‘low'를 출력한다.
  GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
  // 500 ms 지연
  Delay(500); 
#endif
  
#if 0
    // C 포트 하위 8비트에 ‘high'를 출력한다.
  GPIO_SetBits(GPIOA, 0x00ff);
  
  // 500 ms 지연
  Delay(500);
  // C 포트 하위 8비트에 ‘low'를 출력한다.
  GPIO_ResetBits(GPIOA, 0x00ff);
  // 500 ms 지연
  Delay(500); 
#endif
#if 0
  GPIO_ToggleBits(GPIOC, 0xff);
  Delay(1000);
#endif

  
  }
}