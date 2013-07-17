#include "stm32-template.h"
#include "modbus/crc.h"

Gpio led_green(GPIOC, GPIO_Pin_9, RCC_APB2Periph_GPIOC);
Gpio led_blue(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC);

void setup() {

	led_green.init(GPIO_Mode_Out_PP);
	led_blue.init(GPIO_Mode_Out_PP);

	nvic.init(TIM2_IRQn, 0, 3, ENABLE);
	Tim t2(TIM2, RCC_APB1Periph_TIM2, RCC_APB1PeriphClockCmd);
	t2.init(1000, 1000);
	t2.configureIT(TIM_IT_Update);
	t2.setState();
}

void loop() {
	while (usart.available()) {
		uint8_t c = usart.read();
		fprintf(stdout, "0x%02X\r\n", c);

		fprintf(stdout, "0x%02X\r\n", crc.calc(&c, 1));
		led_blue.toggle();
	}

	led_blue.toggle();
	delayMicroseconds(1000000);
}

