/*
 * stm32-template.h
 *
 *  Created on: May 31, 2013
 *      Author: agu
 */

#ifndef STM32_TEMPLATE_H_
#define STM32_TEMPLATE_H_

#include "stm32f10x.h"

#include <cstdio>
#include <cstdlib>
#include <vector>

#include "usart/usart.h"
#include "gpio/gpio.h"
#include "tim/tim.h"
#include "nvic/nvic.h"
#include "i2c/i2c.h"
#include "adc/adc.h"

#include "modbus/slave-rtu.h"
#include "node.h"

void init();
extern void setup();
extern void loop();
extern void delay(u32 ms);
extern void delayMicroseconds(vu32 us);

extern Usart usart;
extern Node slave;

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#endif /* STM32_TEMPLATE_H_ */
