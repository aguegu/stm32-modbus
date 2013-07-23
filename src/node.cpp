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

	this->initCoils(1);
	this->initHoldings(0);

	_dis = (Gpio **) malloc(_bit_input_length * sizeof(Gpio *));

	_dis[0] = new Gpio(GPIOA, GPIO_Pin_1, RCC_APB2Periph_GPIOA);
	_dis[1] = new Gpio(GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA);

	_dos = (Gpio **) malloc(_coil_length * sizeof(Gpio *));
	_dos[0] = new Gpio(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC);
}

Node::~Node() {
	for (uint8_t i = 0; i < _bit_input_length; i++)
		delete _dis[i];

	delete[] _dis;

	for (uint8_t i = 0; i < _coil_length; i++)
		delete _dos[i];

	delete[] _dos;
}

void Node::init() {
	this->SlaveRtu::init();

	for (uint8_t i = 0; i < _bit_input_length; i++)
		_dis[i]->init(GPIO_Mode_IPD);

	for (uint8_t i = 0; i < _coil_length; i++)
		_dos[i]->init(GPIO_Mode_Out_PP);
}

uint8_t Node::updateBitInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		this->setBitInput(index + i, _dis[index + i]->getInput());
	return 0;
}

uint8_t Node::updateCoils(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		_dos[index + i]->set(this->getCoil(index + i));
	return 0;
}
