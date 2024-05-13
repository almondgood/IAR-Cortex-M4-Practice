#include "esp8266.h"

#include "stm32f4xx_usart.h"

#include "ring_buffer.h"

static bool uart_4_received = false;
static RingBuffer *uart_4_rcv_buffer = 0;
static RingBuffer *uart_4_tx_buffer = 0;

static void config_hardware_uart_4(void);
static void config_dma_1(void);
static void config_driver_uart_4(uint32_t baudrate);

void uart_4_init(uint32_t baudrate)
{
    config_hardware_uart_4();
    config_dma_1();
    config_driver_uart_4(baudrate);
}

bool uart_4_is_interupt(void)
{
    return uart_4_received;
}

void uart_4_clear_interupt(void)
{
    uart_4_received = false;
}

int uart_4_get(uint8_t *data)
{
    int length = ring_buffer_queue_length(uart_4_rcv_buffer);

    if (length > 0) {
        ring_buffer_get(uart_4_rcv_buffer, (char *)data, length);
        ring_buffer_empty(uart_4_rcv_buffer);
    }

    return length;
}

void uart_4_send(uint8_t *data, uint16_t length)
{
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART4->DR;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;

    //Configure Tx DMA
    DMA_InitStructure.DMA_BufferSize = length;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)data;

    DMA_Init(DMA1_Stream6, &DMA_InitStructure);

    //DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Stream6, ENABLE);
    USART_DMACmd(USART4, USART_DMAReq_Tx, ENABLE);

    while (USART_GetFlagStatus(USART4, USART_FLAG_TC) == RESET);
    while (DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6)==RESET);

    USART_ClearFlag(USART4, USART_FLAG_TC);
    DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
}

static void config_hardware_uart_2(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);     //USART4_TX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);    //USART4_RX

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure;

    //인터럽트 enable 및 Priority 설정.
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static void config_dma_1(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void config_driver_uart_4(uint32_t baudrate)
{
     if (uart_4_rcv_buffer == 0) {
        uart_4_rcv_buffer = ring_buffer_create(512);
    }

    if (uart_4_tx_buffer == 0) {
        uart_4_tx_buffer = ring_buffer_create(512);
    }

    ring_buffer_empty(uart_4_rcv_buffer);
    ring_buffer_empty(uart_4_tx_buffer);

    USART_Cmd(USART4, DISABLE);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART4, &USART_InitStructure);

    USART_ITConfig(USART4, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART4, ENABLE);
}

void DMA1_Stream6_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_Stream6, DMA_FLAG_TCIF6)) {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_FLAG_TCIF6);
    }
}

//UART 2 interupt handler
void USART4_IRQHandler(void)
{
    //Rx data interupt
    if(USART_GetITStatus(USART4, USART_IT_RXNE) != RESET) {
        // Received new data
        char c = USART_ReceiveData(USART4);
        ring_buffer_put_1(uart_4_rcv_buffer, c);
        uart_4_received = true;
    }

    //Tx data interupt
    if(USART_GetITStatus(USART4, USART_IT_TXE) != RESET) {
        // Txe Empty
        char c;

        if (ring_buffer_get_1(uart_4_tx_buffer, &c)) {
            USART_SendData(USART4, c);
        } else {
            USART_ITConfig(USART4, USART_IT_TXE, DISABLE);
        }
    }
}