/*
 * delay.c
 *
 *  Created on: Apr 8, 2025
 *      Author: USER
 */


#include "delay.h"


void delay_us_DHT11(uint16_t us)
{
  //htim1 is used for DHT11
 // HAL_TIM_Base_Start(&htim1);
  __HAL_TIM_SET_COUNTER(&htim1,0);
  while((__HAL_TIM_GET_COUNTER(&htim1))<us);
  //HAL_TIM_Base_Stop(&htim1);
}

void delay_us_ULTRA(uint16_t us)
{
  __HAL_TIM_SET_COUNTER(&htim3,0);
    while((__HAL_TIM_GET_COUNTER(&htim3))<us);
}

void delay_us_LCD(uint16_t us)
{
   __HAL_TIM_SET_COUNTER(&htim9,0);
   while((__HAL_TIM_GET_COUNTER(&htim9))<us);
}
