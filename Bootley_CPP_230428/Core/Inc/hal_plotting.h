/*
 * mbed SerialPlot 관련 함수 편집하여 stm32CubeIDE에서 사용할 수 있도록 함
 * 230428
 * sunghoon8585@konkuk.ac.kr
 *
 * 참고 영상
 * https://www.youtube.com/watch?v=j_Q6OvLR2QE
 *
 */


#ifndef HAL_PLOTTING_H
#define HAL_PLOTTING_H

#include "main.h"


class Plotting
{
private:
    float datas[32];			// buffer는 어쨌거나 float형
    int data_count;
public:
    Plotting();

    void reset();
    void put(float data, int index);
    void put(int data, int index);
    void put(unsigned int data, int index);
    void send(UART_HandleTypeDef *huart);
};

#endif	// HAL_PLOTTING_H
