/*
 * slave-rtu.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#include "slave-rtu.h"

SlaveRtu::SlaveRtu(Usart & usart, Tim & tim) :
		_usart(usart), _tim(tim)
{

}

SlaveRtu::~SlaveRtu()
{

}

void SlaveRtu::init() {

	_usart.init(9600, USART_WordLength_9b, USART_StopBits_1, USART_Parity_Even);

	_tim.init(20000, (770000UL / 9600) + 1);
	_tim.configureIT(TIM_IT_Update);
	_tim.setState();

	_tim.configureArrPreload();

	_tim.configureIT(TIM_IT_Update);
	_tim.setState();

	nvic.configureGroup(NVIC_PriorityGroup_1);
	nvic.configure(TIM1_UP_TIM16_IRQn, 0, 3, ENABLE);
	nvic.configure(USART1_IRQn, 1, 2, ENABLE);
}

