/*
 * slave-rtu.h
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#ifndef SLAVE_RTU_H_
#define SLAVE_RTU_H_

#include "tim/tim.h"
#include "usart/usart.h"
#include "nvic/nvic.h"
#include "gpio/gpio.h"

class SlaveRtu {
public:
	SlaveRtu(Usart & usart, Tim & tim);
	~SlaveRtu();
	void init();

	enum State {
		IDEL,
		RECEIVING,
		TRANSMITTING
	};

	void handler();
	void handleTimIrq();
private:
	Usart & _usart;
	Tim & _tim;
	State _state;
};

#endif /* SLAVE_RTU_H_ */
