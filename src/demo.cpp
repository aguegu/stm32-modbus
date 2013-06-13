#include "stm32-template.h"

Gpio led_green(GPIOC, GPIO_Pin_9, RCC_APB2Periph_GPIOC );
Gpio led_blue(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC );

void setup() {

	led_green.init(GPIO_Mode_Out_PP);
	led_blue.init(GPIO_Mode_Out_PP);

	nvic.init(TIM2_IRQn, 0, 3, ENABLE);
	Tim t2(TIM2, RCC_APB1Periph_TIM2, RCC_APB1PeriphClockCmd);
	t2.init(1000, 1000);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void loop() {
	while (usart.available()) {
		char c = usart.read();
		fprintf(stdout, "0x%02X\r\n", c);
		led_blue.toggle();
	}

	led_blue.toggle();
	delayMicroseconds(1000000);
}

