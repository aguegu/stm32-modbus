/*
 * node.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#include "node.h"

Node::Node(Usart & usart, Tim & tim, uint8_t address) :
		SlaveRtu(usart, tim, address) {

}

Node::~Node() {

}

