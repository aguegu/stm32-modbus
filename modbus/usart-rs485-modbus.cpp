/*
 * usart.cpp
 *
 *  Created on: May 28, 2013
 *      Author: agu
 */

#include "usart-rs485-modbus.h"

UsartRs485Modbus::UsartRs485Modbus(USART_TypeDef * usart, uint32_t rcc_apbx_periph,
		void (*rcc_apbx_periph_clock_cmd)(uint32_t, FunctionalState), Gpio & de,
		Gpio & re, Tim & tim, uint8_t buff_size, uint16_t time_out) :
		UsartRs485(usart, rcc_apbx_periph, rcc_apbx_periph_clock_cmd, de, re, buff_size,
				time_out), _tim(tim) {
	_is_receiving = true;
}

UsartRs485Modbus::~UsartRs485Modbus() {

}

void UsartRs485Modbus::init(uint32_t baudrate, uint16_t word_length,
		uint16_t stop_bits, uint16_t parity, uint16_t mode,
		uint16_t hardware_flow_control) {

	this->UsartRs485::init(baudrate, word_length, stop_bits, parity, mode,
			hardware_flow_control);

	// t = 11 * 3.5 / baudrate / (1 / 20000)
	_tim.init(20000, (770000UL / baudrate));
	_tim.configureIT(TIM_IT_Update);
	_tim.configureArrPreload();

	_tim.setCounter(0x0000);
	//_tim.setState(ENABLE);
}

void UsartRs485Modbus::onRXNE() {
	this->UsartRs485::onRXNE();
	_tim.setCounter(0x0000);
	_tim.setState(ENABLE);
}

void UsartRs485Modbus::onTimIrq() {
	if (_tim.getITStatus(TIM_IT_Update) == SET) {
		_tim.clearITPendingBit(TIM_IT_Update);
		_tim.setState(DISABLE);
		_is_receiving = false;
	}
}

bool UsartRs485Modbus::isReceiving() {
	return _is_receiving;
}

void UsartRs485Modbus::setReady() {
	_is_receiving = true;
}
