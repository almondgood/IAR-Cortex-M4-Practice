#include "stm32f4xx.h"
#include "fnd.h"
#include "uart2.h"
#include "key.h"
#include "lcd.h"
#include "dcmotor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned char rx2Flag;
extern char rx2Data[50];
extern int key;
extern volatile unsigned int tim4_counter;


int main()
{
    int size;
    int count = 0;
    char buff[16];
    
    int dcmotor_start=0;  //stop
    int dcmotor_dir=0;    //left
    int dutyrate = 50;            //50%
    int pluse = (int)(17700 * (dutyrate / 100.0 ));       //8850
    
    int pre_tim4_counter=0;
    
    UART2_init();
    
    FND_init();
    KEY_init();
    LCD_init();
    
    disp_ON_OFF(ON, OFF, OFF);
    
    printf("%s\r\n", __FILE__);
    lcd(0, 0, "start main()");
    display_fnd(count);   
    
    while(1)
    {
        
#if 0
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
                display_onoff();
                count = 0;
                break;
            }
            }
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
                    printf("Motor Left\r\n");
                    
                    pluse = (int)(17700 * (dutyrate / 100.0 ));  
                    
                    printf("dutyrate: %d, pluse: %d\r\n",dutyrate,pluse);
                    TIM_SetCompare1(TIM10,pluse);
                }
                else if(dcmotor_start == 1)
                {         
                    GPIO_ResetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_10);         
                    dcmotor_start = 0;
                    printf("Motor Stop\r\n");
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
                    printf("Motor Left\r\n");
                }
                else if(dcmotor_dir == 1)       //right
                {
                    dcmotor_dir = 0;
                    dcmotor_start = 1;
                    GPIO_ResetBits(GPIOB,GPIO_Pin_9);
                    GPIO_SetBits(GPIOB,GPIO_Pin_10);
                    printf("Motor Right\r\n");
                }      
            }
            
            else if(key == 3 || key == 4)
            {
                if(key ==3 && dutyrate >= 10)
                    dutyrate -= 10;
                
                else if(key == 4 && dutyrate <= 90)
                    dutyrate += 10;
                
            }       
            key = 0;
        }
        if(tim4_counter != pre_tim4_counter)
        {
            printf("tim4_counter : %d\r\n",tim4_counter);
            pre_tim4_counter = tim4_counter;
        } 
        
        
        if (rx2Flag) {
            printf("recv : %s\r\n", rx2Data);
            rx2Flag = 0;
            
            clrscr();
            size = strlen(rx2Data);
            if (size <17) {
                lcd(0, 0, rx2Data);
            } else if (size < 33) {
                strncpy(buff, rx2Data, 16);
                lcd(0, 0, buff);
                
                strncpy(buff, (rx2Data + 16), 16);
                lcd(0, 1, buff);
            }
            
            count = atoi(rx2Data);
            if (0 <= count && count < 10000)
                display_fnd(count);
        }
    }
}
