/*
 * dht11.c
 *
 *  Created on: Apr 8, 2025
 *      Author: USER
 */


#include "dht11.h"
#include "stdbool.h"
uint8_t data_to_ras[100]={0,};
void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pinNumber)
{
	//구조체의 포트와 핀번호를 설정
	dht->port = port;
	dht->pinNumber = pinNumber;
}

void dht11GpioMode(DHT11 *dht, uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};  //포트에 대한 구조체 선언 및 초기화

	if(mode == OUTPUT)
	{
		//아웃풋 설정
		GPIO_InitStruct.Pin = dht->pinNumber;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
	}
	if(mode == INPUT)
	{
		//인풋 설정
		GPIO_InitStruct.Pin = dht->pinNumber;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
	}
}

uint8_t dht11Read(DHT11 *dht)
{
        printf("HI \r\n");
	uint8_t ret = 1;

	uint16_t timeTick = 0;         //시간 측정을 위한 변수 초기화
	uint8_t pulse[40] = {0};     // 40비트 데이터를 저장할 배열 및 초기화


	//온습도 데이터 변수
	uint8_t humValue1 = 0, humValue2 = 0;     //습도
	uint8_t temValue1 = 0, temValue2 = 0;     //온도
	uint8_t parityValue = 0;                              //체크섬
	//타이머 시작
	HAL_TIM_Base_Start(&htim1);

	//통신 시작 신호 전송
	dht11GpioMode(dht, OUTPUT);                              //GPIO를 출력 모드로 설정
	HAL_GPIO_WritePin(dht->port, dht->pinNumber, 0);         //dht11에 0 전송
	delay_us_DHT11(18000);                                         //시작 신호(low유지)
	HAL_GPIO_WritePin(dht->port, dht->pinNumber, 1);         //dht11에 1 전송
	delay_us_DHT11(30);                                      //30us 대기
	dht11GpioMode(dht, INPUT);                                //GPIO를 입력 모드로 설정


	//dht11 의 응답신호 대기
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET)
	{
		if (__HAL_TIM_GET_COUNTER(&htim1) > 100)
		{
			printf ("LOW signal not\n\r");        //타임아웃 오류 출력
			break;                                                    //타임아웃 오류가 났으면 while문을 탈출
		}
	}


	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (HAL_GPIO_ReadPin (dht->port, dht->pinNumber) == GPIO_PIN_SET)
	{
		if (__HAL_TIM_GET_COUNTER(&htim1) > 100)
		{
			printf ("HIGH N\n\r");        //타임아웃 오류 출력
			break;                                                    //타임아웃 오류가 났으면 while문을 탈출
		}
	}


	//데이터 수신
	for(uint8_t i = 0; i < 40; i++)
	{
		while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET);

		__HAL_TIM_SET_COUNTER(&htim1,0);

		while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET)
		{
			timeTick = __HAL_TIM_GET_COUNTER(&htim1); //high 신호길이를 측정

			if(timeTick > 20 && timeTick < 30)
			{
				pulse[i] = 0;
			}
			else if(timeTick > 65 && timeTick < 85)
			{
				pulse[i] = 1;
			}
		}
	}
	// 타이머 정지
	HAL_TIM_Base_Stop(&htim1);

	//배열에 저장된 데이터 처리
	for(uint8_t i = 0;     i <  8; i++){humValue1 = (humValue1 << 1) + pulse[i];} //습도 상위 8비트
	for(uint8_t i = 8;     i < 16; i++){humValue2 = (humValue2 << 1) + pulse[i];} //습도 하위 8비트
	for(uint8_t i = 16; i < 24; i++){temValue1  = (temValue1 << 1)  + pulse[i];} //온도 상위 8비트
	for(uint8_t i = 24; i < 32; i++){temValue2  = (temValue2 << 1) + pulse[i];} //온도 하위 8비트
	for(uint8_t i = 32; i < 40; i++){parityValue = (parityValue << 1) + pulse[i];} // 체크섬 8비트


	//구조체에 온습도값을 저장
	dht->temperature = temValue1;
	dht->humidity = humValue1;


	//데이터 무결성 검증
	uint8_t checkSum = humValue1+humValue2+temValue1+temValue2;
	if(checkSum != parityValue)
	{
		printf("checkSum \n\r");
		ret = 0;
	}

        HAL_Delay(10);
	return ret;

}
