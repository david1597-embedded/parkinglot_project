/*
 * ultrasonic.c
 *
 *  Created on: Apr 11, 2025
 *      Author: USER
 */
#include "ultrasonic.h"
//들어오는 쪽 초음파 센서
uint16_t IC_Value_1=0;
uint16_t IC_Value_2=0;
uint16_t echo_time=0;

//나가는 쪽 초음파 센서

void ultra_Init(ULTRASONIC *ultra, GPIO_TypeDef *port, uint16_t pinNumber)
{
    ultra->port=port;
    ultra->pinNumber=pinNumber;
    ultra->capture_flag=0;
    ultra->distance=0;
}
void ultra_Trigger(ULTRASONIC *ultra, uint32_t TIM_IT_CCx )
{
    HAL_GPIO_WritePin(ultra->port, ultra->pinNumber, GPIO_PIN_RESET);
    delay_us_ULTRA(1);

    HAL_GPIO_WritePin(ultra->port, ultra->pinNumber, GPIO_PIN_SET);
    delay_us_ULTRA(10);
    HAL_GPIO_WritePin(ultra->port, ultra->pinNumber, GPIO_PIN_RESET);

    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CCx);
}


