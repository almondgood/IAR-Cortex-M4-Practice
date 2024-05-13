/******************************************************************************
  ��Ʈ ����:
    1)  ARM Cortex-M4 ����� ��ƮC (PC0 ~ PC7)�� 8�� ���̺� �̿��ؼ�
        Text LCD����� D0 ~ D7�� �����Ѵ�. (D0�� PC0�� ����Ǿ� �Ѵ�.)
    2)  ARM Cortex-M4 ����� ��ƮB (PB0 ~ PB2)�� 4�� ���̺� �̿��ؼ�
        Text LCD����� RS, RW E�� �����Ѵ�. (RS�� PB0�� ����Ǿ� �Ѵ�.)
******************************************************************************/
// stm32f4xx�� �� �������͵��� ������ �������
#include "stm32f4xx.h"
#include "lcd.h"
#include "uart2.h"
#include <string.h>
#include <stdio.h>


extern volatile unsigned char rx2Flag;
extern char rx2Data[50];

// delay �Լ�
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}

int main()
{
  int i;
  int size;
  LCD_init();
  UART2_init();
  
  disp_ON_OFF(ON, OFF, OFF);
  printf("%s\r\n", __FUNCTION__);  // �Լ� �̸�
  printf("%s\r\n", __DATE__);      // ����� ��/��/��
  printf("%s\r\n", __TIME__);      // ����� ��¥
  printf("%s\r\n", __FILE__);      // ������
  
  lcd(0, 0, "start main");
 
  while(1)
  {
    if (rx2Flag) {
      clrscr();
      size = strlen(rx2Data);
      if (size <17) {
        lcd(0, 1, rx2Data);
      } else if (size < 41) {
        lcd(0, 1, rx2Data);
        
        for (i = 0; i < size - 16; i++) {
            move_disp(LEFT);
            Delay(1000);
        }
        
      }
      rx2Flag = 0;
    }
  }
}
