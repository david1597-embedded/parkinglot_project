/*
 * delay.h
 *
 *  Created on: Apr 8, 2025
 *      Author: USER
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "main.h"
#include "tim.h"

void delay_us_DHT11(uint16_t us);
void delay_us_ULTRA(uint16_t us);
void delay_us_LCD(uint16_t us);
void delay_us_STEPPER(uint16_t us);
void delay_us_BUZZER(uint16_t us);
#endif /* INC_DELAY_H_ */
