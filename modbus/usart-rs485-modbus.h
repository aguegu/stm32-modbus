/*
 * usart.h
 *
 *  Created on: May 28, 2013
 *      Author: agu
 */

#ifndef USART_RS485_MODBUS_H_
#define USART_RS485_MODBUS_H_

#include "usart-rs485/usart-rs485.h"
#include "gpio/gpio.h"
#include "tim/tim.h"

#include <cstdlib>

class UsartRs485Modbus: public UsartRs485 {
public:
	UsartRs485Modbus(USART_TypeDef * usart,
		uint32_t rcc_apbx_periph,
		void (*rcc_apbx_periph_clock_cmd)(uint32_t, FunctionalState),
		Gpio & de, Gpio & re, Tim & tim,
		uint8_t buff_size = 128,
		uint16_t time_out = 4);
	~UsartRs485Modbus();

	void init(uint32_t baudrate = 9600,
		uint16_t word_length = USART_WordLength_8b,
		uint16_t stop_bits = USART_StopBits_1,
		uint16_t parity = USART_Parity_No,
		uint16_t mode = USART_Mode_Rx | USART_Mode_Tx,
		uint16_t hardware_flow_control = USART_HardwareFlowControl_None);

	void onTimIrq();
	bool isReceiving();
	void setReady();

protected:
	void onRXNE();

private:
	Tim & _tim;
	volatile bool _is_receiving;
};

#endif /* USART_RS485_MODBUS_H_ */

