// stm32f4xx�� �� �������͵��� ������ ��� ����
#include "stm32f4xx.h"

// delay �Լ�
static void Delay(const uint32_t Count) {
  __IO uint32_t index = 0;
  for(index = (16800 * Count); index != 0; index--); 
}

int main() {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // PORT A�� Ŭ���� ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  // C ��Ʈ ����8��Ʈ�� ������� �����Ѵ�.
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
  // C ��Ʈ ���� 8��Ʈ�� ��high'�� ����Ѵ�.
  GPIO_SetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
  
  // 500 ms ����
  Delay(500);
  // C ��Ʈ ���� 8��Ʈ�� ��low'�� ����Ѵ�.
  GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
  // 500 ms ����
  Delay(500); 
#endif
  
#if 0
    // C ��Ʈ ���� 8��Ʈ�� ��high'�� ����Ѵ�.
  GPIO_SetBits(GPIOA, 0x00ff);
  
  // 500 ms ����
  Delay(500);
  // C ��Ʈ ���� 8��Ʈ�� ��low'�� ����Ѵ�.
  GPIO_ResetBits(GPIOA, 0x00ff);
  // 500 ms ����
  Delay(500); 
#endif
#if 0
  GPIO_ToggleBits(GPIOC, 0xff);
  Delay(1000);
#endif

  
  }
}