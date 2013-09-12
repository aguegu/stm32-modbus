#include "stm32-template.h"
#include "gpio-array/gpio-array.h"
#include "node.h"

int main(void) __attribute__((weak));

Gpio usart_de(GPIOA, GPIO_Pin_8, RCC_APB2Periph_GPIOA);
Gpio usart_re(GPIOA, GPIO_Pin_8, RCC_APB2Periph_GPIOA);

Tim t1(TIM1, RCC_APB2Periph_TIM1, RCC_APB2PeriphClockCmd);
UsartRs485Modbus usart(USART1, RCC_APB2Periph_USART1, RCC_APB2PeriphClockCmd,
		usart_de, usart_re, t1);

Gpio led_green(GPIOC, GPIO_Pin_9, RCC_APB2Periph_GPIOC);
Gpio led_blue(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC);

Node slave(usart, 0x02);

Gpio dip_0(GPIOB, GPIO_Pin_14, RCC_APB2Periph_GPIOB);
Gpio dip_1(GPIOB, GPIO_Pin_13, RCC_APB2Periph_GPIOB);
Gpio dip_2(GPIOB, GPIO_Pin_12, RCC_APB2Periph_GPIOB);
Gpio dip_3(GPIOB, GPIO_Pin_11, RCC_APB2Periph_GPIOB);
Gpio dip_4(GPIOB, GPIO_Pin_10, RCC_APB2Periph_GPIOB);
Gpio dip_5(GPIOB, GPIO_Pin_0, RCC_APB2Periph_GPIOB);
Gpio dip_6(GPIOB, GPIO_Pin_1, RCC_APB2Periph_GPIOB);
GpioArray dips(7);

void setup() {
	dips.setGpio(0, dip_0);
	dips.setGpio(1, dip_1);
	dips.setGpio(2, dip_2);
	dips.setGpio(3, dip_3);
	dips.setGpio(4, dip_4);
	dips.setGpio(5, dip_5);
	dips.setGpio(6, dip_6);

	dips.init(GPIO_Mode_IPU);
	slave.setAddress(dips.getInput());
}


int main(void) {
	init();
	setup();
	slave.init();

	for (;;)
		slave.handler();
}

void init() {
	SysTick_Config(SystemCoreClock / 1000);	// Tick per ms

	Gpio usart_tx(GPIOA, GPIO_Pin_9,
	RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO);
	usart_tx.init(GPIO_Mode_AF_OD);

	Gpio usart_rx(GPIOA, GPIO_Pin_10,
	RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO);
	usart_rx.init(GPIO_Mode_IN_FLOATING);

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	nvic.configureGroup(NVIC_PriorityGroup_1);
	nvic.configure(TIM1_UP_TIM16_IRQn, 0, 3, ENABLE);
	nvic.configure(USART1_IRQn, 1, 2, ENABLE);

	led_green.init(GPIO_Mode_Out_PP);

	nvic.configure(TIM2_IRQn, 1, 3, ENABLE);
	Tim t2(TIM2, RCC_APB1Periph_TIM2, RCC_APB1PeriphClockCmd);
	t2.init(1000, 1000);
	t2.configureIT(TIM_IT_Update);
	t2.setState();
}

void delay(u32 ms) {
	extern vu32 millis;
	uint32_t start = millis;

	while (ms) {
		if (millis - start) {
			ms--;
			start++;
		}
	}
}

// delayMicroseconds ref to libmaple
void delayMicroseconds(vu32 us) {
	us *= SystemCoreClock / 3000000;

	asm volatile(
			"   mov r0, %[us]          \n\t"
			"1: subs r0, #1            \n\t"
			"   bhi 1b                 \n\t"
			:
			: [us] "r" (us)
			: "r0"
	);
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
	fprintf(stderr, "assert failed on %s, line # %ld\r\n", file, line);
	while (1)
		;
}

#endif
