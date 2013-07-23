/*
 * node.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#include "node.h"

Node::Node(Usart & usart, Tim & tim, uint8_t address) :
		SlaveRtu(usart, tim, address)
{
	this->initBitInputs(2);
	this->initShortInputs(0);

	this->initCoils(0);
	this->initHoldings(0);

	_btns = (Gpio **) malloc(_bit_input_length * sizeof(Gpio *));

	_btns[0] = new Gpio(GPIOA, GPIO_Pin_1, RCC_APB2Periph_GPIOA);
	_btns[1] = new Gpio(GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA);
}

Node::~Node() {
	for (uint8_t i = 0; i < _bit_input_length; i++)
		delete _btns[i];

	delete[] _btns;
}

void Node::init() {
	this->SlaveRtu::init();

	for (uint8_t i = 0; i < _bit_input_length; i++)
		_btns[i]->init(GPIO_Mode_IPD);
}

uint8_t Node::updateBitInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		this->setBitInput(index + i, _btns[index + i]->getInput());
	return 0;
}
