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

class SlaveRtu {
public:
	SlaveRtu(Usart & usart, Tim & tim);
	~SlaveRtu();
	void init();

private:
	Usart & _usart;
	Tim & _tim;
};

#endif /* SLAVE_RTU_H_ */
