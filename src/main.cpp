#include "stm32-template.h"
#include "usart-rs485/usart-rs485.h"

int main(void) __attribute__((weak));

Gpio usart_de(GPIOA, GPIO_Pin_8, RCC_APB2Periph_GPIOA);
Gpio usart_re(GPIOA, GPIO_Pin_8, RCC_APB2Periph_GPIOA);

UsartRs485 usart(USART1, RCC_APB2Periph_USART1, RCC_APB2PeriphClockCmd, usart_de, usart_re);

int main(void) {
	init();
	setup();

	for (;;)
		loop();
}

void init() {

	SysTick_Config(SystemCoreClock / 1000);	// Tick per ms
	delay(1000);

	Gpio usart_tx(GPIOA, GPIO_Pin_9,
	RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO);
	usart_tx.init(GPIO_Mode_AF_OD);

	Gpio usart_rx(GPIOA, GPIO_Pin_10,
	RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO);
	usart_rx.init(GPIO_Mode_IN_FLOATING);

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
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
