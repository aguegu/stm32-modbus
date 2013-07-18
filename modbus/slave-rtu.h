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

#include "crc.h"

class SlaveRtu {
public:
	SlaveRtu(Usart & usart, Tim & tim, uint8_t address);
	~SlaveRtu();
	void init();
	void handler();
	void handleTimIrq();
private:
	Usart & _usart;
	Tim & _tim;
	volatile bool _is_receiving;
	static const uint16_t _BUFF_LENGTH = 256;
	uint8_t _buff[_BUFF_LENGTH];
	const uint8_t _address;
};

#endif /* SLAVE_RTU_H_ */
