/*
 * slave-rtu.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#include "slave-rtu.h"

SlaveRtu::SlaveRtu(Usart & usart, Tim & tim, uint8_t address,
		uint8_t * supportted_functions) :
		_usart(usart), _tim(tim), _address(address), _supportted_functions(
				supportted_functions) {
	_is_receiving = true;
}

SlaveRtu::~SlaveRtu() {

}

void SlaveRtu::init() {

	_usart.init(19200, USART_WordLength_9b, USART_StopBits_1,
	USART_Parity_Even);

	_tim.init(20000, (770000UL / 19200));
	//_tim.init(1000, 1000);
	_tim.configureIT(TIM_IT_Update);
	_tim.configureArrPreload();

	_tim.setCounter(0x0000);
	_tim.setState(ENABLE);

	nvic.configureGroup(NVIC_PriorityGroup_1);
	nvic.configure(TIM1_UP_TIM16_IRQn, 0, 3, ENABLE);
	nvic.configure(USART1_IRQn, 1, 2, ENABLE);
}

void SlaveRtu::handler() {
	extern Gpio led_blue;

	static uint16_t index = 0;

	if (_usart.available()) {
		_buff_rx[index++] = _usart.read();
		led_blue.set(Bit_SET);
		_tim.setCounter(0x0000);
		_tim.setState(ENABLE);
	}

	if (_is_receiving == false) {

		uint8_t exception = 0;

		do {
			if (index < 4 || index >= _BUFF_LENGTH) break;
			if (_buff_rx[0] != _address) break;
			if (this->checkFrameCrc(_buff_rx, index) == false) break;

			if (this->isFunctionSupportted(_buff_rx[1]) == false) exception =
					0x01;

			_buff_tx[0] = _address;
			_buff_tx[1] = _buff_rx[1];	// function code

			uint8_t length = 1;
			if (exception) {
				_buff_tx[1] += 0x80;
				_buff_tx[2] = exception;
				length = 3;
			} else {
				_buff_tx[2] = 0x01;
				_buff_tx[3] = 0x01;
				length = 4;
			}
			this->appendCrcAndReply(length);
		} while (false);

		_is_receiving = true;
		index = 0;
	}
}

void SlaveRtu::handleTimIrq() {
	extern Gpio led_blue;

	if (_tim.getITStatus(TIM_IT_Update) == SET) {
		_tim.clearITPendingBit(TIM_IT_Update);
		_tim.setState(DISABLE);
		_is_receiving = false;
		led_blue.set(Bit_RESET);
	}
}

bool SlaveRtu::checkFrameCrc(const uint8_t *p, uint8_t length) {
	uint16_t crc0 = crc.calc(p, length - 2);
	uint16_t crc1 = p[length - 2] | (p[length - 1] << 8);
	return crc0 == crc1;
}

bool SlaveRtu::isFunctionSupportted(uint8_t function) {
	uint8_t *p = _supportted_functions;

	do {
		if (function == *p++) return true;
	} while (*p);

	return false;
}

void SlaveRtu::appendCrcAndReply(uint8_t length) {
	uint16_t v = crc.calc(_buff_tx, length);
	_buff_tx[length] = v;
	_buff_tx[length + 1] = v >> 8;
	_usart.write(_buff_tx, length + 2);
}
