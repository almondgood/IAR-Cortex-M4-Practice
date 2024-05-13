#include "stm32f4xx.h"
#include "uart2.h"
#include "fnd.h"
#include "key.h"
#include "lcd.h"
#include "dcmotor.h"
#include "adc1.h"
#include "dht11.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern volatile unsigned char rx2Flag;
extern char rx2Data[50];
extern int key;
extern volatile unsigned int tim4_counter;
extern volatile int adc1Flag;   
extern volatile int t_cnt, m_cnt, m_cntFlag;

/*
static void Delay(const uint32_t Count)
{
__IO uint32_t index = 0; 
for(index = (16800 * Count); index != 0; index--);
}
*/

long map(long x, long in_min, long in_max, long out_min, long out_max);

int main()
{
    int displayFlag = 1;
    int size;
    int count = 0;
    int dcmotor_start=0;  //stop
    int dcmotor_dir=0;    //left
    int dutyrate = 50;            //50%
    int pluse = (int)(17700 * (dutyrate / 100.0 ));       //8850
    int pre_tim4_counter = 0;
    int adc_data;
    int pre_adc_data = 0;
    int cds_val = 0;
    
    FND_init();
    UART2_init();
    KEY_init();
    MOTOR_init();
    ADC1_init();
    
    LCD_init();
    disp_ON_OFF(ON,OFF,OFF);
    printf("%s\r\n",__FILE__);
    printf("Start main()\n\r");
    lcd(0,0,"start main()");
    display_fnd(count);
    ADC_SoftwareStartConv(ADC1);
    
    struct dht11 dhtInfo;
    dhtInfo.GPIOx = GPIOB;
    dhtInfo.GPIO_Pin = GPIO_Pin_6;
    DHT11_init(dhtInfo);
    
    
    while(1)
    {
        //if (m_cntFlag && !(m_cnt % 5)) {
        if (m_cntFlag) {    // 1s
            ADC_SoftwareStartConv(ADC1);
            m_cntFlag = 0;
            cds_val = map(adc_data, 0, 1023, 0, 100);
            if (DHT11_Read(&dhtInfo))
                printf("humi : %d.%d, temp : %d.%d\r\n",dhtInfo.data[0],dhtInfo.data[1],dhtInfo.data[2],dhtInfo.data[3]);
            
            printf("m_cnt : %d\r\n", m_cnt);
        }
#if 0
        if(key != 0)
        {
            switch (key)
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
#endif
        
        if(key != 0)
        {
            
            printf("key  %d\r\n",key);
            if(key == 1)     //motor start/stop
            {
                if(dcmotor_start == 0)          //left
                {
                    dcmotor_start = 1;
                    dcmotor_dir = 1;
                    GPIO_SetBits(GPIOB,GPIO_Pin_9);               
                    GPIO_ResetBits(GPIOB,GPIO_Pin_10);
                    
                    pluse = (int)(17700 * (dutyrate / 100.0 ));
                    TIM_SetCompare1(TIM10,pluse);
                    printf("dutyrate: %d, pluse: %d\r\n",dutyrate,pluse);
                }       
                else if(dcmotor_start == 1)
                {         
                    GPIO_ResetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_10);         
                    dcmotor_start = 0;
                }       
            }
            if(key == 2)     //motor left/right
            {
                if(dcmotor_dir == 0)            //left
                {
                    dcmotor_dir = 1;
                    dcmotor_start = 1;
                    GPIO_SetBits(GPIOB,GPIO_Pin_9);
                    GPIO_ResetBits(GPIOB,GPIO_Pin_10);
                }
                else if(dcmotor_dir == 1)       //right
                {
                    dcmotor_dir = 0;
                    dcmotor_start = 1;
                    GPIO_ResetBits(GPIOB,GPIO_Pin_9);
                    GPIO_SetBits(GPIOB,GPIO_Pin_10);
                }      
            }
            
            else if(key == 3 || key == 4)
            {
                if(key ==3 && dutyrate >= 10)
                    dutyrate -= 10;
                
                else if(key == 4 && dutyrate <= 90)
                    dutyrate += 10;
                
                pluse = (int)(17700 * (dutyrate / 100.0 ));  
                
                printf("dutyrate: %d, pluse: %d\r\n",dutyrate,pluse);
                TIM_SetCompare1(TIM10,pluse);
            }       
            key = 0;
        }
        if(tim4_counter != pre_tim4_counter)
        {
            printf("tim4_counter : %d\r\n", tim4_counter);
            pre_tim4_counter = tim4_counter ;
        }
        
        if(rx2Flag)
        {
            clrscr();
            printf("recv2 : %s\r\n",rx2Data);
            rx2Flag = 0;
            size = strlen(rx2Data);
            if( size < 17 )
            {
                lcd(0,0,rx2Data);
            }
            else if(size < 33)
            {
                lcd(0,0,rx2Data);
                lcd(0,1,rx2Data+16);
            }
            count = atoi(rx2Data);
            if(0 <= count && count < 10000)
                display_fnd(count);
        }
        if(adc1Flag)
        {
            adc1Flag = 0;
            adc_data = ADC_GetConversionValue(ADC1);
            //if(abs(adc_data - pre_adc_data) >= 20)
            {
                printf("adc_data : %d, volt : %lf\r\n",adc_data, 3.3/1024 * adc_data);
                display_fnd(adc_data);
            }
            pre_adc_data = adc_data;
        }
       
    }
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x-in_min)*(out_max-out_min) / (in_max-in_min)+out_min;
}