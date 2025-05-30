/*
 * gate.c
 *
 *  Created on: Apr 18, 2025
 *      Author: USER
 */


#include "gate.h"


void stepMotor(uint8_t step)
{
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, HALF_STEP_SEQ[step][0]);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, HALF_STEP_SEQ[step][1]);
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, HALF_STEP_SEQ[step][2]);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, HALF_STEP_SEQ[step][3]);
}
void rotateSteps(uint16_t steps, uint8_t direction)
{
	for(uint16_t i = 0; i < steps; i++)
	{
		//스텝 방향에 따라서 패턴 설정































































































	      delay_us_STEPPER(1000); //모터속도 조절
	}
}

void rotateDegrees(uint16_t degrees, uint8_t direction)
{
	//각도에 해당하는 스텝수 계산
	uint16_t steps = (uint16_t)(((uint32_t)degrees * STEPS_PER_REVOLUTION) / 360);

	rotateSteps(steps, direction);		//지정된 방향으로 회전
}
