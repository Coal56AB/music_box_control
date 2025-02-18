/*
 * oled.h
 *
 *  Created on: Nov 17, 2021
 *      Author: wvv
 */
#ifndef OLED_H
#define OLED_H

#include "stm32f1xx_hal.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 4
#define PIXEL_MODE_PAINT 	0
#define PIXEL_MODE_CLEAR	1
extern uint8_t oled_buf[OLED_HEIGHT * OLED_WIDTH];

void oled_refresh(void);
void oled_init(void);
#endif //OLED_H