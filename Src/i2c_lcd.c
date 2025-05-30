/*
 * i2c_lcd.c
 *
 *  Created on: Apr 11, 2025
 *      Author: USER
 */
#include "i2c_lcd.h"

uint8_t lcd_flag=0;
void lcd_command(uint8_t command)
{
	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];

	// 상위비트 4bit, 하위비트 4bit 추출
	high_nibble = command & 0xf0;
	low_nibble = (command << 4) & 0xf0;

	i2c_buffer[0] = high_nibble | 0x04 | 0x08;	// en = 1, rs = 0, rw = 0, backlight = 1
	i2c_buffer[1] = high_nibble | 0x00 | 0x08;	// en = 0, rs = 0, rw = 0, backlight = 1
	i2c_buffer[2] = low_nibble  | 0x04 | 0x08;  // en = 1, rs = 0, rw = 0, backlight = 1
	i2c_buffer[3] = low_nibble  | 0x00 | 0x08;  // en = 0, rs = 0, rw = 0, backlight = 1

	// 버퍼 4개를 보내겠다
	while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS, i2c_buffer, 4, 100) != HAL_OK);
}

void lcd_data(uint8_t data)
{
	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];

	// 상위비트 4bit, 하위비트 4bit 추출
	high_nibble = data & 0xf0;
	low_nibble = (data << 4) & 0xf0;

	i2c_buffer[0] = high_nibble | 0x05 | 0x08;	// en = 1, rs = 1, rw = 0, backlight = 1
	i2c_buffer[1] = high_nibble | 0x01 | 0x08;	// en = 0, rs = 1, rw = 0, backlight = 1
	i2c_buffer[2] = low_nibble  | 0x05 | 0x08;  // en = 1, rs = 1, rw = 0, backlight = 1
	i2c_buffer[3] = low_nibble  | 0x01 | 0x08;  // en = 0, rs = 1, rw = 0, backlight = 1

	// 버퍼 4개를 보내겠다
	while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS, i2c_buffer, 4, 100) != HAL_OK);
}

void i2c_lcd_init(void)
{
	//delay_us_LCD(50000);
        HAL_Delay(50);
	lcd_command(0x33);
	//delay_us_LCD(5000);
	HAL_Delay(5);
	lcd_command(0x32);
	//delay_us_LCD(5000);
	HAL_Delay(5);
	lcd_command(0x28);
	//delay_us_LCD(5000);
	lcd_command(DISPLAY_ON);
	HAL_Delay(5);
	//delay_us_LCD(5000);
	lcd_command(0x06);
	HAL_Delay(5);
	//delay_us_LCD(5000);
	lcd_command(CLEAR_DISPLAY);
	HAL_Delay(5);
	//delay_us_LCD(5000);
	lcd_flag=1;
}

void lcd_string(char *str)
{
	while(*str)
	{
		lcd_data(*str++);
	}
}

void move_cursor(uint8_t row, uint8_t col)
{
	lcd_command((0x80) | row << 6 | col);
}

