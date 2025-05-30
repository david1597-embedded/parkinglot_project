/*
 * buzzer.c
 *
 *  Created on: Apr 18, 2025
 *      Author: USER
 */

#include "buzzer.h"
// 계이름
#define Do  383
#define Re  340
#define Mi  304
#define Fa  287
#define Sol 255
#define Ra  227
#define Si  203
#define Do1 191

// 음의 길이
#define qNote 600
#define wNote 1800

void BUZZER_Open()
{
	uint16_t song[] = {Do, Re, Mi};
	uint32_t time[] = {qNote,qNote,qNote};

	for(uint16_t i=0; i < sizeof(song)/sizeof(song[0]); i++)
	{
		TIM2->PSC = song[i];
		TIM2->CCR1 = 500;
		HAL_Delay(time[i]);
		TIM2->CCR1 = 0;
		HAL_Delay(5);

	}
}

void BUZZER_Close()
{
	uint16_t song[] = {Mi, Re, Do};
	uint16_t time[] = {qNote,qNote,qNote};

	for(uint16_t i=0; i < sizeof(song)/sizeof(song[0]); i++)
	{
		TIM2->PSC = song[i];
		TIM2->CCR1 = 500;
		HAL_Delay(time[i]);
		TIM2->CCR1 = 0;
		HAL_Delay(5);
	}
}

void BUZZER_Temp()
{
	uint16_t song[] = {Re, Fa, Re, Fa};

	for(uint16_t i=0; i < sizeof(song)/sizeof(song[0]); i++)
	{
		TIM2->PSC = song[i];
		TIM2->CCR1 = 500;
		HAL_Delay(300);
		TIM2->CCR1 = 0;
		HAL_Delay(5);
	}
}
