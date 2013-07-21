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

	_coils_length = 32;
	uint8_t coils_byte_size = (_coils_length + 7) >> 3;
	_coils = (uint8_t *) malloc(coils_byte_size * sizeof(uint8_t));
	memset(_coils, 0, coils_byte_size);

	_dis_length = 16;
	uint8_t dis_byte_size = (_dis_length + 7) >> 3;
	_dis = (uint8_t *) malloc(dis_byte_size * sizeof(uint8_t));
	memset(_dis, 0, dis_byte_size);
}

SlaveRtu::~SlaveRtu() {
	free(_coils);
	free(_dis);
}

void SlaveRtu::init() {

	_usart.init(19200, USART_WordLength_9b, USART_StopBits_1,
	USART_Parity_Even);

	_tim.init(20000, (770000UL / 19200));
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
		memset(_buff_tx, 0, _BUFF_LENGTH);

		do {
			if (index < 4 || index >= _BUFF_LENGTH) break;
			if (_buff_rx[0] && _buff_rx[0] != _address) break;
			if (this->checkFrameCrc(_buff_rx, index) == false) break;

			_buff_tx[0] = _address;
			_buff_tx[1] = _buff_rx[1];	// function code

			uint8_t length = 1;
			uint8_t exception = 0;

			do {
				if (this->isFunctionSupportted(_buff_rx[1]) == false) {
					exception = 0x01;
					break;
				}

				switch (_buff_rx[1]) {
				case 0x01:
					exception = responseReadCoils(&length);
					break;
				case 0x02:
					exception = responseReadDiscreteInputs(&length);
					break;
				case 0x05:
					exception = responseWriteCoils(&length);
					break;
				}

			} while (false);

			if (exception) {
				_buff_tx[1] += 0x80;
				_buff_tx[2] = exception;
				length = 3;
			}

			if (_buff_tx[0]) this->appendCrcAndReply(length);
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
	_buff_tx[length] = lowByte(v);
	_buff_tx[length + 1] = highByte(v);
	_usart.write(_buff_tx, length + 2);
}

void SlaveRtu::setCoil(uint16_t index, BitAction state) {
	assert_param(index < _coils_length);
	bitWrite(_coils[index >> 3], index & 0x07, state == Bit_SET);
}

BitAction SlaveRtu::getCoil(uint16_t index) {
	assert_param(index < _coils_length);
	return bitRead(_coils[index >> 3],index & 0x07) ? Bit_SET : Bit_RESET;
}

uint8_t SlaveRtu::responseReadCoils(uint8_t * length) {

	uint16_t address_length = makeWord(_buff_rx[4], _buff_rx[5]);
	if (address_length == 0 || address_length > 0x07d0) return 0x03;

	uint16_t address_indent = makeWord(_buff_rx[2], _buff_rx[3]);
	if (address_indent + address_length > _coils_length) return 0x02;

	for (uint16_t i = 0; i < address_length; i++) {
		bitWrite(_buff_tx[3 + (i >> 3)], i & 0x07,
				this->getCoil(address_indent + i) == Bit_SET);
	}

	_buff_tx[2] = (address_length + 7) >> 3;

	*length = 3 + _buff_tx[2];
	return 0;
}

void SlaveRtu::setDiscreteInput(uint16_t index, BitAction state) {
	assert_param(index < _dis_length);
	bitWrite(_dis[index >> 3], index & 0x07, state == Bit_SET);
}

BitAction SlaveRtu::getDiscreteInput(uint16_t index) {
	assert_param(index < _dis_length);
	return bitRead(_dis[index >> 3],index & 0x07) ? Bit_SET : Bit_RESET;
}

uint8_t SlaveRtu::responseReadDiscreteInputs(uint8_t * length) {

	uint16_t address_length = makeWord(_buff_rx[4], _buff_rx[5]);
	if (address_length == 0 || address_length > 0x07d0) return 0x03;

	uint16_t address_indent = makeWord(_buff_rx[2], _buff_rx[3]);
	if (address_indent + address_length > _dis_length) return 0x02;

	for (uint16_t i = 0; i < address_length; i++) {
		bitWrite(_buff_tx[3 + (i >> 3)], i & 0x07,
				this->getDiscreteInput(address_indent + i) == Bit_SET);
	}

	_buff_tx[2] = (address_length + 7) >> 3;

	*length = 3 + _buff_tx[2];
	return 0;
}

uint8_t SlaveRtu::responseWriteCoils(uint8_t * length) {
	uint16_t val = makeWord(_buff_rx[4], _buff_rx[5]);
	if (val && val != 0xff00) return 0x03;

	uint16_t address = makeWord(_buff_rx[2], _buff_rx[3]);
	if (address >= _coils_length) return 0x02;

	this->setCoil(address, val ? Bit_SET : Bit_RESET);
	memcpy(_buff_tx + 2, _buff_rx + 2, 4);
	*length = 6;

	return 0;
}
