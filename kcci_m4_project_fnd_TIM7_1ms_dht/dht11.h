#ifndef __DHT_H_
#define __DHT_H_

struct dht11 {
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Pin;
  uint8_t data[5];    /* data from sensor store here */
};


void DHT11_init();
uint8_t DHT11_Read();
static void Delay(const uint32_t Count);
static void Delay_us(const uint32_t Count);

#endif
