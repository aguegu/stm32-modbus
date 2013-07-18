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
	SlaveRtu(Usart & usart, Tim & tim, uint8_t address,
			uint8_t * supportted_functions);
	~SlaveRtu();
	void init();
	void handler();
	void handleTimIrq();

private:
	Usart & _usart;
	Tim & _tim;
	volatile bool _is_receiving;
	static const uint16_t _BUFF_LENGTH = 256;
	uint8_t _buff_rx[_BUFF_LENGTH];
	uint8_t _buff_tx[_BUFF_LENGTH];
	const uint8_t _address;
	uint8_t * _supportted_functions;

	bool checkFrameCrc(const uint8_t *p, uint8_t length);
	bool isFunctionSupportted(uint8_t function);
	void appendCrcAndReply(uint8_t length);
};

#endif /* SLAVE_RTU_H_ */
