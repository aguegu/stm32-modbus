/*
 * slave-rtu.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#include "slave-rtu.h"

SlaveRtu::SlaveRtu(Usart & usart, Tim & tim, uint8_t address) :
		_usart(usart), _tim(tim), _address(address) {
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
		_buff[index++] = _usart.read();
		led_blue.set(Bit_SET);
		_tim.setCounter(0x0000);
		_tim.setState(ENABLE);
	}

	if (_is_receiving == false) {

		if (index >= 4) {
			uint16_t crc0 = crc.calc(_buff, index - 2);
			uint16_t crc1 = _buff[index - 2] | (_buff[index - 1] << 8);

			if (crc0 == crc1) {
				uint8_t c[6] = { 0x01, 0x01, 0x01, 0x01 };
				uint16_t v = crc.calc(c, 4);
				c[4] = v;
				c[5] = v >> 8;
				_usart.write(c, 6);
			}
		}
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

