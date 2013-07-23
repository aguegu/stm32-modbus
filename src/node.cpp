/*
* node.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#include "node.h"

Node::Node(Usart & usart, Tim & tim, uint8_t address) :
		SlaveRtu(usart, tim, address),
				_btn(GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA) {

	this->initBitInputs(1);
	this->initShortInputs(0);

	this->initCoils(0);
	this->initHoldings(0);
}

Node::~Node() {

}

void Node::init() {
	this->SlaveRtu::init();
	_btn.init(GPIO_Mode_IN_FLOATING);
}

uint8_t Node::updateBitInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		this->setBitInput(index + i, _btn.getInput());
	return 0;
}
