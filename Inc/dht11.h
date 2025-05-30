/*
 * dht11.h
 *
 *  Created on: Apr 8, 2025
 *      Author: USER
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"
#include "usart.h"
#include "delay.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

enum
{
    INPUT,
    OUTPUT
};

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t      pinNumber;
    uint8_t       temperature;
    uint8_t       humidity;
}DHT11;

extern uint8_t data_to_ras[100];
void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pinNumber);
void dht11GpioMode(DHT11 *dht, uint8_t mode);
uint8_t dht11Read(DHT11 *dht);

#endif /* INC_DHT11_H_ */
