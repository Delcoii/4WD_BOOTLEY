#include "hal_plotting.h"

Plotting::Plotting()
{
	// buffer size 32
    for(int i = 0; i < 32; i++) {
        datas[i] = 0;
    }
    data_count = 0;
}

void Plotting::reset()
{
    for(int i = 0; i < 32; i++) {
        datas[i]=0;
    }
    data_count = 0;
}

// put float data into buffer
void Plotting::put(float data, int index)
{
    datas[index]= data;
    data_count++;
}

void Plotting::put(int data, int index)
{
    datas[index] = (float)data;
    data_count++;
}

void Plotting::put(unsigned int data, int index)
{
    datas[index] = (float)data;
    data_count++;
}


void Plotting::send(UART_HandleTypeDef *huart)
{
    uint8_t u8_buffer;

	u8_buffer = 0xAA;
	HAL_UART_Transmit(huart, &u8_buffer, 1, HAL_MAX_DELAY);
	u8_buffer = 0xBB;
	HAL_UART_Transmit(huart, &u8_buffer, 1, HAL_MAX_DELAY);
	u8_buffer = 0xCC;
	HAL_UART_Transmit(huart, &u8_buffer, 1, HAL_MAX_DELAY);
	u8_buffer = (uint8_t)(data_count*4);
	HAL_UART_Transmit(huart, &u8_buffer, 1, HAL_MAX_DELAY);

	for(int i = 0; i < data_count; i++) {
        char *bytePtr = (char *)&(datas[i]);
		HAL_UART_Transmit(huart, (uint8_t*) bytePtr, 4, HAL_MAX_DELAY);
    }

	/*
	port->putc(0xAA);
    port->putc(0xBB);
    port->putc(0xCC);
    port->putc((char)(data_count*4));	// float size
    for(int i = 0; i < data_count; i++) {
        char *bytePtr = (char *)&(datas[i]);

        // reverse the order of these lines if you have endian issues
		port->putc(*bytePtr);
        port->putc(*(bytePtr+1));
        port->putc(*(bytePtr+2));
        port->putc(*(bytePtr+3));
    }*/

}
