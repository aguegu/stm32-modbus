/*
 * node.cpp
 *
 *  Created on: Jul 23, 2013
 *      Author: agu
 */

#include "node.h"

const uint8_t Node::_uuid[] = "665247461d1c11e395bc73bf3a254a5f";

const uint8_t Node::_lengths[] = { 0, 0, 2, 6 };

Node::Node(UsartRs485Modbus & usart, uint8_t address) :
		SlaveRtu(usart, address) {

	this->initBitInputs(_lengths[0] * 2);
	this->initCoils(_lengths[1] * 2);
	this->initShortInputs(20 + _lengths[2] * 2);
	this->initHoldings(_lengths[3] * 2);

	_adc_pins = (Gpio **) malloc(_lengths[2] * sizeof(Gpio *));
	_adc_pins[0] = new Gpio(GPIOA, GPIO_Pin_5, RCC_APB2Periph_GPIOA);
	_adc_pins[1] = new Gpio(GPIOA, GPIO_Pin_6, RCC_APB2Periph_GPIOA);

	_adc = new Adc(ADC1, RCC_APB2Periph_ADC1);
	_adc_channels = new uint8_t[2];
	_adc_channels[0] = ADC_Channel_5;
	_adc_channels[1] = ADC_Channel_6;

	_led_pins = (Gpio **) malloc(4 * sizeof(Gpio *));
	_led_pins[0] = new Gpio(GPIOC, GPIO_Pin_0, RCC_APB2Periph_GPIOC);
	_led_pins[1] = new Gpio(GPIOC, GPIO_Pin_1, RCC_APB2Periph_GPIOC);
	_led_pins[2] = new Gpio(GPIOC, GPIO_Pin_2, RCC_APB2Periph_GPIOC);
	_led_pins[3] = new Gpio(GPIOC, GPIO_Pin_3, RCC_APB2Periph_GPIOC);

	_lamp = new Gpio(GPIOC, GPIO_Pin_7, RCC_APB2Periph_GPIOC);

	for (uint8_t i = 0; i < 16; i++) {
		this->setShortInput(i, make16(_uuid[i + i], _uuid[i + i + 1]));
	}

	for (uint8_t i = 0; i < 4; i++)
		this->setShortInput(i + 16, _lengths[i]);

	this->setShortInput(20, 5);
	this->setShortInput(22, 5);

	this->setHolding(0, 1);
	this->setHolding(1, 0);

	this->setHolding(2, 1);
	this->setHolding(4, 1);
	this->setHolding(6, 1);
	this->setHolding(8, 1);

	this->setHolding(3, 0);
	this->setHolding(5, 0);
	this->setHolding(7, 0);
	this->setHolding(9, 0);
}

Node::~Node() {
	for (uint8_t i = 0; i < 2; i++)
		delete _adc_pins[i];
	delete[] _adc_pins;

	delete _adc;
	delete _adc_channels;

	for (uint8_t i = 0; i < 4; i++)
		delete _led_pins[i];
	delete[] _led_pins;
}

void Node::init() {
	this->SlaveRtu::init();

	_adc->init(ADC_Mode_Independent, DISABLE, DISABLE,
	ADC_ExternalTrigConv_None);
	_adc->calibrate();
	_adc->configChannel(ADC_Channel_5, 1);

	_lamp->init(GPIO_Mode_Out_OD);
	_lamp->set(true);

	_led_pins[0]->set(true);
	_led_pins[1]->set(true);
	_led_pins[2]->set(true);
	_led_pins[3]->set(true);

	for (uint8_t i = 0; i < 2; i++)
		_adc_pins[i]->init(GPIO_Mode_AIN);

	for (uint8_t i = 0; i < 4; i++)
		_led_pins[i]->init(GPIO_Mode_Out_PP);
}

//uint8_t Node::updateCoils(uint16_t index, uint16_t length) {
//	for (uint16_t i = 0; i < length; i++) {
//		uint16_t k = index + i;
//		switch(k) {
//		case 1:
//
//		}
//		_coil_pins[index + i]->set(this->getCoil(index + i));
//	}
//	return 0;
//}

uint8_t Node::updateShortInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		uint16_t k = index + i;
		switch (k) {
		case 21:
			_adc->configChannel(_adc_channels[0], 1);
			_adc->startSoftwareConvert();
			this->setShortInput(k, _adc->getValue() * 100 / 4096);
			break;
		case 23:
			_adc->configChannel(_adc_channels[1], 1);
			_adc->startSoftwareConvert();
			this->setShortInput(k, _adc->getValue() * 100 / 4096);
			break;
		}
	}
	return 0;
}

uint8_t Node::updateHoldings(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		uint16_t j = index + i;
		switch (j) {
		case 1:
			_lamp->set(!this->getHolding(j));
			break;
		case 3:
			_led_pins[0]->set(!this->getHolding(j));
			break;
		case 5:
			_led_pins[1]->set(!this->getHolding(j));
			break;
		case 7:
			_led_pins[2]->set(!this->getHolding(j));
			break;
		case 9:
			_led_pins[3]->set(!this->getHolding(j));
			break;
		}
	}

	return 0;
}
