/*
 * gate.h
 *
 *  Created on: Apr 18, 2025
 *      Author: USER
 */

#ifndef INC_GATE_H_
#define INC_GATE_H_
#include "main.h"
#include "delay.h"

#define STEPS_PER_REVOLUTION  4096 // half step 방식의 계산한 수
#define DIR_CW								0    // 시계 방향 위
#define DIR_CCW                                                         1
#define IN1_GPIO_Port  GPIOA
#define IN1_Pin			   GPIO_PIN_12
#define IN2_GPIO_Port  GPIOA
#define IN2_Pin			   GPIO_PIN_11
#define IN3_GPIO_Port  GPIOB
#define IN3_Pin			   GPIO_PIN_15
#define IN4_GPIO_Port  GPIOB
#define IN4_Pin			   GPIO_PIN_14

static const uint8_t HALF_STEP_SEQ[8][4] =
		{
				{1, 0, 0, 0},
				{1, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 1, 1},
				{0, 0, 0, 1},
				{1, 0, 0, 1}
		};


void stepMotor(uint8_t step);
void rotateSteps(uint16_t steps, uint8_t direction);
void rotateDegrees(uint16_t degrees, uint8_t direction);
#endif /* INC_GATE_H_ */
