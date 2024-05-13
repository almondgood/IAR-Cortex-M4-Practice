#ifndef __UART2_H__
#define __UART2_H__
void UART2_init();
void Serial2_Event();
void Serial2_Send2(unsigned char t);
void Serial2_Send_String2(char* s);
//int putchar(int ch);            //printf() 사용시 필요
void print_2d1_2(double number);
#endif