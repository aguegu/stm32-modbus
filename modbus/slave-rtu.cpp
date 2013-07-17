/*
 * slave-rtu.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#include "slave-rtu.h"

SlaveRtu::SlaveRtu(Usart & usart, Tim & tim) :
		_usart(usart), _tim(tim) {
	_state = IDEL;
}

SlaveRtu::~SlaveRtu() {

}

void SlaveRtu::init() {

	_usart.init(9600, USART_WordLength_9b, USART_StopBits_1, USART_Parity_Even);

	_tim.init(2000, (770000UL / 9600));
	//_tim.init(1000, 1000);
	_tim.configureIT(TIM_IT_Update);
	_tim.setState(DISABLE);

	_tim.configureArrPreload();

	_tim.configureIT(TIM_IT_Update);

	nvic.configureGroup(NVIC_PriorityGroup_1);
	nvic.configure(TIM1_UP_TIM16_IRQn, 0, 3, ENABLE);
	nvic.configure(USART1_IRQn, 1, 2, ENABLE);
}

void SlaveRtu::handler() {

	extern Gpio led_blue;

	if (_usart.available()) {
		//if (_state == IDEL) _state = RECEIVING;

		_tim.setCounter(0x0000);
		_tim.setState();

		led_blue.set(Bit_SET);

		_usart.read();
	}
}

void SlaveRtu::handleTimIrq() {
	extern Gpio led_blue;

	if (_tim.getITStatus(TIM_IT_Update) == SET) {
		_tim.clearITPendingBit(TIM_IT_Update);

		led_blue.set(Bit_RESET);
		_tim.setState(DISABLE);
	}
}

