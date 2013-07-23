/*
 * slave-rtu.h
 *
 *  Created on: Jul 17, 2013
 *      Author: agu
 */

#ifndef SLAVE_RTU_H_
#define SLAVE_RTU_H_

#include <cstdlib>
#include <cstring>

#include "stm32f10x.h"

#include "tim/tim.h"
#include "usart/usart.h"
#include "nvic/nvic.h"
#include "gpio/gpio.h"
#include "common/bit_op.h"

#include "crc.h"

class SlaveRtu {
public:
	SlaveRtu(Usart & usart, Tim & tim, uint8_t address);
	~SlaveRtu();
	void init();
	void handler();
	void handleTimIrq();

	void initBitInputs(uint16_t length);
	void initShortInputs(uint16_t length);
	void initCoils(uint16_t length);
	void initHoldings(uint16_t length);

	void setBitInput(uint16_t index, BitAction state);
	BitAction getBitInput(uint16_t index);

	void setShortInput(uint16_t index, uint16_t val);
	uint16_t getShortInput(uint16_t index);

	void setCoil(uint16_t index, BitAction state);
	BitAction getCoil(uint16_t index);

	void setHolding(uint16_t index, uint16_t val);
	uint16_t getHolding(uint16_t index);

private:
	Usart & _usart;
	Tim & _tim;

	volatile bool _is_receiving;
	static const uint16_t _BUFF_LENGTH = 256;
	uint8_t _buff_rx[_BUFF_LENGTH];
	uint8_t _buff_tx[_BUFF_LENGTH];

	const uint8_t _address;

	int16_t _coil_length;
	uint8_t * _coils;

	uint16_t _bit_input_length;	// Discrete Inputs
	uint8_t * _bit_inputs;

	uint16_t _short_input_length;
	uint16_t * _short_inputs;

	uint16_t _holding_length;
	uint16_t * _holdings;

	bool checkFrameCrc(const uint8_t *p, uint8_t length);
	void appendCrcAndReply(uint8_t length_tx);

	uint8_t onReadBitInputs(uint8_t * p_length_tx);
	uint8_t onReadShortInputs(uint8_t * p_length_tx);

	uint8_t onReadCoils(uint8_t * p_length_tx);
	uint8_t onWriteSingleCoil(uint8_t * p_length_tx);
	uint8_t onWriteMultipleCoils(uint8_t length_rx, uint8_t * p_length_tx);

	uint8_t onReadHoldings(uint8_t * p_length_tx);
	uint8_t onWriteSingleHolding(uint8_t * p_length_tx);
	uint8_t onWriteMultipleHoldings(uint8_t length_rx, uint8_t * p_length_tx);
};

#endif /* SLAVE_RTU_H_ */
