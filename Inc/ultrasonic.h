/*
 * ultrasonic.h
 *
 *  Created on: Apr 11, 2025
 *      Author: USER
 */

#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_

#include "main.h"
#include "tim.h"
#include "delay.h"
typedef struct
{
  GPIO_TypeDef *port;     //ultrasonic sensor port
  uint16_t      pinNumber;//ultrasonic sensor pinNumber;
  uint8_t       capture_flag;
  uint16_t      distance;
}ULTRASONIC;

void ultra_Init(ULTRASONIC *ultra, GPIO_TypeDef *port, uint16_t pinNumber);
void ultra_Trigger(ULTRASONIC *ultra, uint32_t TIM_IT_CCx );


extern uint16_t IC_Value_1;
extern uint16_t IC_Value_2;
extern uint16_t echo_time;



#endif /* INC_ULTRASONIC_H_ */
