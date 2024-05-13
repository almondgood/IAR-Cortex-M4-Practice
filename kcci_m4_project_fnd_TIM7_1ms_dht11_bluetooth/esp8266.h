#ifndef __ESP8266_H__
#define __ESP8266_H__

#include <stdint.h>
#include <stdbool.h>

void uart_4_init(uint32_t baudrate);
bool uart_4_is_interupt(void);
void uart_4_clear_interupt(void);
int uart_4_get(uint8_t *data);
void uart_4_send(uint8_t *data, uint16_t length);

#ifdef __cplusplus
extern "C" {
#endif

    void USART4_IRQHandler(void);
    void DMA1_Stream6_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // __UART_1_H__