#include "stm32f4xx.h"
#include "uart2.h"
#include "fnd.h"
#include "key.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern volatile unsigned char rx2Flag;
extern char rx2Data[50];
extern int key;

/*
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}
*/


int main()
{

  int displayFlag = 1;
  int size;
  int count = 0;
  FND_init(); 
  UART2_init();
  KEY_init();
  LCD_init();
  disp_ON_OFF(ON,OFF,OFF);
  printf("%s\r\n",__FILE__);
  printf("Start main()\r\n");
  lcd(0,0,"start main()");
  display_fnd(count);
  while(1)
  {

    if(count == 10000)
      count = 0;
    else if(count == -1000)
      count = 0;
    
    
    if(key != 0)
    {
      switch(key) 
      {
        case 1:
          count++;
          break;
        case 2:
          count--;
          break;        
        case 3:
          count = 0;
          break;        
         case 4:
          if(displayFlag)
            displayFlag=0;
          else
            displayFlag=1;
          display_onoff(displayFlag);
          break;       
      
      }
      display_fnd(count);
      key = 0;
    }

 
//    count = count + 1;
//    if(count==9999) count = 0;
    
    if(rx2Flag) 
    {
      clrscr();  //  화면 클리어 
      printf("recv2 : %s\r\n",rx2Data);
      rx2Flag = 0;    
      size = strlen(rx2Data);
      if( size < 17 )
      {
        lcd(0,0,rx2Data);     
      }
      else if( size < 33 )
      {
        lcd(0,0,rx2Data);
        lcd(0,1,rx2Data+16);
      }      
    }
  }
}
