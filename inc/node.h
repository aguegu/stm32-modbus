/*
 * node.h
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#ifndef NODE_H_
#define NODE_H_

#include "modbus/slave-rtu.h"

class Node: SlaveRtu {
public:
	Node(Usart & usart, Tim & tim, uint8_t address);
	~Node();
};

#endif /* NODE_H_ */
