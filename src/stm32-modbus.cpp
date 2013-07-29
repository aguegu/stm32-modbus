#include "stm32-template.h"
#include "usart-rs485/usart-rs485.h"
#include "node.h"

extern UsartRs485 usart;

Gpio led_green(GPIOC, GPIO_Pin_9, RCC_APB2Periph_GPIOC);
Tim t1(TIM1, RCC_APB2Periph_TIM1, RCC_APB2PeriphClockCmd);

Node slave(usart, t1, 0x02);

void setup() {
	led_green.init(GPIO_Mode_Out_PP);

	nvic.configure(TIM2_IRQn, 1, 3, ENABLE);
	Tim t2(TIM2, RCC_APB1Periph_TIM2, RCC_APB1PeriphClockCmd);
	t2.init(1000, 1000);
	t2.configureIT(TIM_IT_Update);
	t2.setState();

	slave.init();
}

void loop() {
	slave.handler();
}

