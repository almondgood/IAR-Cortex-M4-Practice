#ifndef __UART4_H__
#define __UART4_H__
void UART4_init();
void Serial2_Event();
void Serial2_Send(unsigned char t);
void Serial2_Send_String(char* s);
int putchar(int ch);            //printf() ���� �ʿ�
void print_2d1(double number);
#endif