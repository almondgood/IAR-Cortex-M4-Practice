#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include "fnd.h"
#include "key.h"

extern int key;

static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}

volatile int count = 0;

int main()
{
  FND_init();
  KEY_init();
  
  while(1)
  {
    if (count >= 10000 || count <= -1000) 
      count = 0;
   
    if (key != 0) {
      switch (key) {
      case 1: 
        count++; 
        break;
      case 2: 
        count--; 
        break;
      case 3: 
        count = 0; 
        break;
      case 4: {
        display_onoff(0);
        count = 0;
        break;
        }
      }
      key = 0;
    }
    
    display_fnd(count); 
   
  }
}
