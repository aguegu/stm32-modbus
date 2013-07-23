/*
 * node.h
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#ifndef NODE_H_
#define NODE_H_

#include "modbus/slave-rtu.h"
#include "gpio/gpio.h"

class Node: public SlaveRtu {
public:
	Node(Usart & usart, Tim & tim, uint8_t address);
	~Node();
	void init();

private:
	Gpio _btn;

	uint8_t updateBitInputs(uint16_t index, uint16_t length);
	uint8_t updateShortInputs(uint16_t index, uint16_t length) {return 0;};
	uint8_t updateCoils(uint16_t index, uint16_t length) {return 0;};
	uint8_t updateHoldings(uint16_t index, uint16_t length) {return 0;};
};

#endif /* NODE_H_ */
