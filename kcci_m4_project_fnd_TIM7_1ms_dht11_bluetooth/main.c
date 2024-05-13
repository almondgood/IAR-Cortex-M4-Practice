#include "stm32f4xx.h"
#include "uart2.h"
//#include "uart4.h"
#include "esp8266.h"
#include "fnd.h"
#include "key.h"
#include "lcd.h"
#include "dcmotor.h"
#include "adc1.h"
#include "dht11.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_SIZE 50
#define ARR_CNT 5

extern volatile unsigned char rx2Flag;
extern char rx2Data[50];
//extern volatile unsigned char rx4Flag;
//extern char rx4Data[50];
extern int key;
extern volatile unsigned int tim4_counter;
extern volatile int adc1Flag;;
extern volatile int t_cnt,m_cnt,m_cntFlag;
/*
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0; 
  for(index = (16800 * Count); index != 0; index--);
}
*/
void bluetooth_Event();
void Motor_Left();
void Motor_Right();
void Motor_Stop();
void Motor_Pwm(int dutyrate);

long map(long x, long in_min, long in_max, long out_min, long out_max);

uint8_t rx_buffer[512];

int main()
{

  int displayFlag = 1;
  int size;
  int count = 0;
  int pre_tim4_counter=0;
  int adc_data;
//  int pre_adc_data = 0;
  int cds_val=0;
  FND_init(); 
  UART2_init();
  //UART4_init();
  KEY_init();
  MORTOR_init();
  ADC1_init();
  uart_4_init(38400);
  
  LCD_init();
  disp_ON_OFF(ON,OFF,OFF);
  printf("%s\r\n",__FILE__);
  printf("Start main()\r\n");
  lcd(0,0,"start main()");
  display_fnd(count);
  ADC_SoftwareStartConv(ADC1);

  printf("%s\r\n",__FILE__);
  printf("Start main()\r\n");
  struct dht11 dhtInfo;
  dhtInfo.GPIOx = GPIOB;
  dhtInfo.GPIO_Pin = GPIO_Pin_6;
  DHT11_init(dhtInfo); 
  
  while(1)
  {
//     if(m_cntFlag && !(m_cnt % 5))    //5초 마다 
    if(m_cntFlag)
    {
      ADC_SoftwareStartConv(ADC1);
      m_cntFlag = 0;
      cds_val = map(adc_data,0,1023,0,100);
      if(DHT11_Read(&dhtInfo))
        printf("cds : %d, humi : %d.%d, temp : %d.%d\r\n",cds_val, dhtInfo.data[0],dhtInfo.data[1],dhtInfo.data[2],dhtInfo.data[3]);
      printf("m_cnt : %d\r\n",m_cnt);
    }
       
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
      if(count == 10000)
        count = 0;
      else if(count == -1000)
        count = 0;    
      
      display_fnd(count);
      key = 0;
    }

    if(tim4_counter != pre_tim4_counter)
    {
      printf("tim4_counter : %d\r\n",tim4_counter);
      pre_tim4_counter = tim4_counter;
    }    
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
      count = atoi(rx2Data);
      if(0 <= count && count < 10000)
        display_fnd(count);
      
    }
     if (uart_4_is_interupt()) {
            uart_4_clear_interupt();

            int length = uart_4_get(rx_buffer);

            for (int i = 0; i < length; i++) {
                //USART_(rx_buffer[i]);
           
            }
            printf("uart4 : %s\r\n", rx_buffer);
            bluetooth_Event();
        }

    }
    if(adc1Flag)
    {
      adc1Flag = 0;
      adc_data = ADC_GetConversionValue(ADC1);
//      if(abs(adc_data - pre_adc_data) >= 20)
//      {
//        printf("adc_data : %d, volt : %lf\r\n",adc_data, 3.3/1024 * adc_data);      
//        display_fnd(adc_data);
//        pre_adc_data = adc_data;
//      }
    }

  
}

void bluetooth_Event()
{
  
  int i=0;
  int num = 0;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0};
  char sendBuf[CMD_SIZE]={0}; 
  strcpy(recvBuf,rx_buffer);

  printf("rx4Data : %s\r\n",recvBuf);
     
  pToken = strtok(recvBuf,"[@]");
  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }
//  printf("pArray[0] : %s\r\n",pArray[0]);       
//  printf("pArray[1] : %s\r\n",pArray[1]);
//  printf("pArray[2] : %s\r\n",pArray[2]);
  
  if(!strcmp(pArray[1],"LED"))
  {
    if(pArray[3] != NULL)              
      num = atoi(pArray[3]);
    else
      num = 0;
/*    if(!strcmp(pArray[2],"ON"))
    {
     GPIO_SetBits(GPIOC, 0x01 << num); 
    }
    else 
    {
      GPIO_ResetBits(GPIOC, 0x01 << num); 
*/
    
  } 
  
  else if(!strcmp(pArray[1],"MOTOR"))
  {
    if(!strcmp(pArray[2],"LEFT"))  
    {
      Motor_Pwm(50);
      Motor_Left();
    }
    else if(!strcmp(pArray[2],"RIGHT")) 
    {
      Motor_Pwm(50);
      Motor_Right();
    }
    else if(!strcmp(pArray[2],"STOP")) 
    {
      Motor_Pwm(0);
      Motor_Stop();
    }
    else if(!strcmp(pArray[2],"PWM")) 
    {
      num = atoi(pArray[3]);
      Motor_Pwm(num);
    }
  }
  else if(!strcmp(pArray[1],"FND"))
  {
    num = atoi(pArray[2]);
    display_fnd(num);  
  }
  else if(!strcmp(pArray[1],"LCD"))
  {
    num = strlen(pArray[2]);
    if( num < 17 )
    {
      lcd(0,1,pArray[2]);     
    }    
  }
  else if(!strncmp(pArray[1]," New conn",sizeof(" New conn")))
  {
      return;
  }
  else if(!strncmp(pArray[1]," Already log",sizeof(" Already log")))
  {
      return;
  }    
  else
      return;
      
  sprintf(sendBuf,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
}

void Motor_Left()
{
  GPIO_SetBits(GPIOB,GPIO_Pin_9);               
  GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}
void Motor_Right()
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_9);
  GPIO_SetBits(GPIOB,GPIO_Pin_10);               
}
void Motor_Stop()
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_9);
  GPIO_ResetBits(GPIOB,GPIO_Pin_10);  
}
void Motor_Pwm(int dutyrate)
{
  int pluse = (int)(17700 * (dutyrate / 100.0 )); 
  TIM_SetCompare1(TIM10,pluse);
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x-in_min)*(out_max-out_min) / (in_max-in_min)+out_min;
}
