/*
 * node.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#include "node.h"

Node::Node(UsartRs485 & usart, Tim & tim, uint8_t address) :
		SlaveRtu(usart, tim, address) {
	this->initBitInputs(2);
	this->initShortInputs(2);

	this->initCoils(1);
	this->initHoldings(16);

	_bit_input_pins = (Gpio **) malloc(_bit_input_length * sizeof(Gpio *));

	_bit_input_pins[0] = new Gpio(GPIOA, GPIO_Pin_1, RCC_APB2Periph_GPIOA);
	_bit_input_pins[1] = new Gpio(GPIOA, GPIO_Pin_0, RCC_APB2Periph_GPIOA);

	_coil_pins = (Gpio **) malloc(_coil_length * sizeof(Gpio *));
	_coil_pins[0] = new Gpio(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC);

	_short_input_pins = (Gpio **) malloc(_short_input_length * sizeof(Gpio *));
	_short_input_pins[0] = new Gpio(GPIOA, GPIO_Pin_5, RCC_APB2Periph_GPIOA);
	_short_input_pins[1] = new Gpio(GPIOA, GPIO_Pin_6, RCC_APB2Periph_GPIOA);

	_adc = new Adc(ADC1, RCC_APB2Periph_ADC1);
	_adc_channels = new uint8_t[2];
	_adc_channels[0] = ADC_Channel_5;
	_adc_channels[1] = ADC_Channel_6;
}

Node::~Node() {
	for (uint8_t i = 0; i < _bit_input_length; i++)
		delete _bit_input_pins[i];
	delete[] _bit_input_pins;

	for (uint8_t i = 0; i < _coil_length; i++)
		delete _coil_pins[i];
	delete[] _coil_pins;

	for (uint8_t i = 0; i < _short_input_length; i++)
		delete _short_input_pins[i];
	delete[] _short_input_pins;

	delete _adc;
	delete _adc_channels;
}

void Node::init() {
	this->SlaveRtu::init();

	for (uint8_t i = 0; i < _bit_input_length; i++)
		_bit_input_pins[i]->init(GPIO_Mode_IPD);

	for (uint8_t i = 0; i < _coil_length; i++)
		_coil_pins[i]->init(GPIO_Mode_Out_PP);

	for (uint8_t i = 0; i < _short_input_length; i++)
		_short_input_pins[i]->init(GPIO_Mode_AIN);

	_adc->init(ADC_Mode_Independent, DISABLE, DISABLE,
		ADC_ExternalTrigConv_None);
	_adc->calibrate();

	_adc->configChannel(ADC_Channel_5, 1);
}

uint8_t Node::updateBitInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		this->setBitInput(index + i, _bit_input_pins[index + i]->getInput());
	return 0;
}

uint8_t Node::updateCoils(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		_coil_pins[index + i]->set(this->getCoil(index + i));
	return 0;
}

uint8_t Node::updateShortInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		_adc->configChannel(_adc_channels[index+i], 1);
		_adc->startSoftwareConvert();
		this->setShortInput(index + i, _adc->getValue());
	}
	return 0;
}

