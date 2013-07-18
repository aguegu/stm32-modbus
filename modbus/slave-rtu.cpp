/*
 * slave-rtu.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#include "slave-rtu.h"

SlaveRtu::SlaveRtu(Usart & usart, Tim & tim) :
		_usart(usart), _tim(tim) {
	_rx_state = RECEIVING;
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

		uint8_t c = _usart.read();

		if (_rx_state == IDEL) {
			index = 0;
			_buff[index++] = c;
			_rx_state = RECEIVING;
		} else if (_rx_state == RECEIVING) {
			_buff[index++] = c;
		}

		led_blue.set(Bit_SET);

		_tim.setCounter(0x0000);
		_tim.setState(ENABLE);
	}

	if (_rx_state == RECEIVED) {
		_usart.write(0xff);
		for (uint16_t i = 0; i < index; i++)
			_usart.write(_buff[i]);
		_rx_state = IDEL;
	}
}

void SlaveRtu::handleTimIrq() {
	extern Gpio led_blue;

	if (_tim.getITStatus(TIM_IT_Update) == SET) {
		_tim.clearITPendingBit(TIM_IT_Update);
		_tim.setState(DISABLE);
		_rx_state = RECEIVED;
		led_blue.set(Bit_RESET);
	}
}

