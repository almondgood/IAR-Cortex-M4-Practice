/******************************************************************************
  포트 연결:
    1)  ARM Cortex-M4 모듈의 포트C (PC0 ~ PC7)를 8핀 케이블를 이용해서
        Text LCD모듈의 D0 ~ D7에 연결한다. (D0가 PC0로 연결되야 한다.)
    2)  ARM Cortex-M4 모듈의 포트B (PB0 ~ PB2)를 4핀 케이블를 이용해서
        Text LCD모듈의 RS, RW E에 연결한다. (RS가 PB0로 연결되야 한다.)
******************************************************************************/
// stm32f4xx의 각 레지스터들을 정의한 헤더파일
#include "stm32f4xx.h"
#include "lcd.h"
#include "uart2.h"
#include <string.h>
#include <stdio.h>


extern volatile unsigned char rx2Flag;
extern char rx2Data[50];

// delay 함수
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
  printf("%s\r\n", __FUNCTION__);  // 함수 이름
  printf("%s\r\n", __DATE__);      // 빌드된 월/일/년
  printf("%s\r\n", __TIME__);      // 빌드된 날짜
  printf("%s\r\n", __FILE__);      // 절대경로
  
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
