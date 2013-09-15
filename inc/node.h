/*
 * node.h
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#ifndef NODE_H_
#define NODE_H_

#include <cstdlib>

#include "modbus/slave-rtu.h"
#include "gpio/gpio.h"
#include "adc/adc.h"
#include "dma/dma.h"

class Node: public SlaveRtu {
public:
	Node(UsartRs485Modbus & usart, uint8_t address);
	~Node();
	void init();

private:
	Gpio ** _adc_pins;
	Adc * _adc;

	uint8_t * _adc_channels;

	Gpio * _lamp;
	Gpio ** _led_pins;

	const static uint8_t _uuid[];
	const static uint8_t _lengths[];

	uint8_t updateShortInputs(uint16_t index, uint16_t length);
	uint8_t updateHoldings(uint16_t index, uint16_t length);
};

#endif /* NODE_H_ */
