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
#include "tim/tim.h"

class Node: public SlaveRtu {
public:
	Node(UsartRs485 & usart, Tim & tim, uint8_t address);
	~Node();
	void init();

private:
	Gpio ** _pins;
	TimOc ** _ocs;
	uint8_t updateHoldings(uint16_t index, uint16_t length);
};

#endif /* NODE_H_ */
