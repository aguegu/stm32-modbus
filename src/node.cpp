/*
 * node.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#include "node.h"

Node::Node(UsartRs485 & usart, Tim & tim, uint8_t address) :
		SlaveRtu(usart, tim, address) {
	this->initBitInputs(0);
	this->initShortInputs(0);
	this->initCoils(0);
	this->initHoldings(4);

	_pins = (Gpio **) malloc(_holding_length * sizeof(Gpio *));

	_pins[0] = new Gpio(GPIOB, GPIO_Pin_6, RCC_APB2Periph_GPIOB);
	_pins[1] = new Gpio(GPIOB, GPIO_Pin_7, RCC_APB2Periph_GPIOB);
	_pins[2] = new Gpio(GPIOB, GPIO_Pin_8, RCC_APB2Periph_GPIOB);
	_pins[3] = new Gpio(GPIOB, GPIO_Pin_9, RCC_APB2Periph_GPIOB);

	_ocs = (TimOc **) malloc(_holding_length * sizeof(TimOc *));

	_ocs[0] = new TimOc(TIM4, TIM_OC1Init, TIM_SetCompare1);
	_ocs[1] = new TimOc(TIM4, TIM_OC2Init, TIM_SetCompare2);
	_ocs[2] = new TimOc(TIM4, TIM_OC3Init, TIM_SetCompare3);
	_ocs[3] = new TimOc(TIM4, TIM_OC4Init, TIM_SetCompare4);
}

Node::~Node() {
	for (uint8_t i = 0; i < _holding_length; i++) {
		delete _pins[i];
		delete _ocs[i];
	}

	delete[] _pins;
	delete[] _ocs;
}

void Node::init() {

	this->SlaveRtu::init();

	Tim t4(TIM4, RCC_APB1Periph_TIM4, RCC_APB1PeriphClockCmd);
	t4.init(1000000, 20000);

	for (uint8_t i = 0; i < _holding_length; i++) {
		_pins[i]->init(GPIO_Mode_AF_PP);
		_ocs[i]->init(TIM_OCMode_PWM1, TIM_OutputState_Enable,
				TIM_OutputNState_Disable);
		this->setHolding(i, 1000);
	}

	this->updateHoldings(0, _holding_length);
}

uint8_t Node::updateHoldings(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		_ocs[index+i]->setCompare(this->getHolding(index + i));
	}
	return 0;
}

