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

	_bit_input_length = 16;
	uint8_t bit_input_byte_size = (_bit_input_length + 7) >> 3;
	_bit_inputs = (uint8_t *) malloc(bit_input_byte_size * sizeof(uint8_t));
	memset(_bit_inputs, 0, bit_input_byte_size);

	_short_input_length = 8;
	_short_inputs = (uint16_t *) malloc(_short_input_length * sizeof(uint16_t));
	memset(_short_inputs, 0, _short_input_length);
}

SlaveRtu::~SlaveRtu() {
	free(_coils);
	free(_bit_inputs);
	free(_short_inputs);
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

	static uint16_t length_rx = 0;

	if (_usart.available()) {
		_buff_rx[length_rx++] = _usart.read();
		led_blue.set(Bit_SET);
		_tim.setCounter(0x0000);
		_tim.setState(ENABLE);
	}

	if (_is_receiving == false) {
		memset(_buff_tx, 0, _BUFF_LENGTH);

		do {
			if (length_rx < 4 || length_rx >= _BUFF_LENGTH) break;
			if (_buff_rx[0] && _buff_rx[0] != _address) break;
			if (this->checkFrameCrc(_buff_rx, length_rx) == false) break;

			_buff_tx[0] = _address;
			_buff_tx[1] = _buff_rx[1];	// function code

			uint8_t length_tx = 1;
			uint8_t exception = 0;

			do {
				if (this->isFunctionSupportted(_buff_rx[1]) == false) {
					exception = 0x01;
					break;
				}

				switch (_buff_rx[1]) {
				case 0x01:
					exception = responseReadCoils(&length_tx);
					break;
				case 0x02:
					exception = responseReadBitInputs(&length_tx);
					break;
				case 0x03:
					exception = responseReadShortInputs(&length_tx);
					break;
				case 0x05:
					exception = responseWriteSingleCoil(&length_tx);
					break;
				case 0x0f:
					exception = responseWriteMultipleCoils(length_rx,
							&length_tx);
					break;
				}
			} while (false);

			if (exception) {
				_buff_tx[1] += 0x80;
				_buff_tx[2] = exception;
				length_tx = 3;
			}

			if (_buff_tx[0]) this->appendCrcAndReply(length_tx);
		} while (false);

		_is_receiving = true;
		length_rx = 0;
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

void SlaveRtu::appendCrcAndReply(uint8_t length_tx) {
	uint16_t v = crc.calc(_buff_tx, length_tx);
	_buff_tx[length_tx] = lowByte(v);
	_buff_tx[length_tx + 1] = highByte(v);
	_usart.write(_buff_tx, length_tx + 2);
}

void SlaveRtu::setCoil(uint16_t index, BitAction state) {
	assert_param(index < _coils_length);
	bitWrite(_coils[index >> 3], index & 0x07, state == Bit_SET);
}

BitAction SlaveRtu::getCoil(uint16_t index) {
	assert_param(index < _coils_length);
	return bitRead(_coils[index >> 3],index & 0x07) ? Bit_SET : Bit_RESET;
}

uint8_t SlaveRtu::responseReadCoils(uint8_t * p_length_tx) {

	uint16_t address_length = make16(_buff_rx[4], _buff_rx[5]);
	if (address_length == 0 || address_length > 0x07d0) return 0x03;

	uint16_t address_indent = make16(_buff_rx[2], _buff_rx[3]);
	if (address_indent + address_length > _coils_length) return 0x02;

	for (uint16_t i = 0; i < address_length; i++) {
		bitWrite(_buff_tx[3 + (i >> 3)], i & 0x07,
				this->getCoil(address_indent + i) == Bit_SET);
	}

	_buff_tx[2] = (address_length + 7) >> 3;

	*p_length_tx = 3 + _buff_tx[2];
	return 0;
}

void SlaveRtu::setBitInput(uint16_t index, BitAction state) {
	assert_param(index < _bit_input_length);
	bitWrite(_bit_inputs[index >> 3], index & 0x07, state == Bit_SET);
}

BitAction SlaveRtu::getBitInput(uint16_t index) {
	assert_param(index < _bit_input_length);
	return bitRead(_bit_inputs[index >> 3],index & 0x07) ? Bit_SET : Bit_RESET;
}

uint8_t SlaveRtu::responseReadBitInputs(uint8_t * p_length_tx) {

	uint16_t address_length = make16(_buff_rx[4], _buff_rx[5]);
	if (address_length == 0 || address_length > 0x07d0) return 0x03;

	uint16_t address_indent = make16(_buff_rx[2], _buff_rx[3]);
	if (address_indent + address_length > _bit_input_length) return 0x02;

	for (uint16_t i = 0; i < address_length; i++) {
		bitWrite(_buff_tx[3 + (i >> 3)], i & 0x07,
				this->getBitInput(address_indent + i) == Bit_SET);
	}

	_buff_tx[2] = (address_length + 7) >> 3;

	*p_length_tx = 3 + _buff_tx[2];
	return 0;
}

uint8_t SlaveRtu::responseWriteSingleCoil(uint8_t * p_length_tx) {
	uint16_t val = make16(_buff_rx[4], _buff_rx[5]);
	if (val && val != 0xff00) return 0x03;

	uint16_t address = make16(_buff_rx[2], _buff_rx[3]);
	if (address >= _coils_length) return 0x02;

	this->setCoil(address, val ? Bit_SET : Bit_RESET);
	memcpy(_buff_tx + 2, _buff_rx + 2, 4);
	*p_length_tx = 6;

	return 0;
}

uint8_t SlaveRtu::responseWriteMultipleCoils(uint8_t length_rx,
		uint8_t * p_length_tx) {
	uint16_t quantity = make16(_buff_rx[4], _buff_rx[5]);
	if ((quantity && quantity > 0x07b0) || length_rx - 9 != _buff_rx[6])
		return 0x03;

	uint16_t address = make16(_buff_rx[2], _buff_rx[3]);
	if (address + quantity >= _coils_length) return 0x02;

	for (uint16_t i = 0; i < quantity; i++) {
		this->setCoil(address++,
		bitRead(_buff_rx[7 + (i >> 3)], i & 0x07) ? Bit_SET : Bit_RESET);
	}

	memcpy(_buff_tx + 2, _buff_rx + 2, 4);
	*p_length_tx = 6;
	return 0;
}

uint8_t SlaveRtu::responseReadShortInputs(uint8_t * p_length_tx) {

	uint16_t quantity = make16(_buff_rx[4], _buff_rx[5]);
	if (!quantity || quantity > 0x07d) return 0x03;

	uint16_t address = make16(_buff_rx[2], _buff_rx[3]);
	if (address + quantity > _short_input_length) return 0x02;

	for (uint8_t i = 0; i < quantity; i++) {
		_buff_tx[3 + i + i] = highByte(_short_inputs[address + i]);
		_buff_tx[4 + i + i] = lowByte(_short_inputs[address + i]);
	}

	_buff_tx[2] = quantity * 2;
	*p_length_tx = _buff_tx[2] + 3;
	return 0;
}

void SlaveRtu::setShortInput(uint16_t index, uint16_t val) {
	assert_param(index < _short_input_length);
	_short_inputs[index] = val;
}

uint16_t SlaveRtu::getShortInput(uint16_t index) {
	assert_param(index < _short_input_length);
	return _short_inputs[index];
}
