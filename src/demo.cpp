#include "stm32-template.h"

Gpio ** leds;
I2c i2c(I2C1, RCC_APB1Periph_I2C1 );

void setup() {

	leds = (Gpio **) malloc(sizeof(Gpio *) * 2);

	leds[0] = new Gpio(GPIOC, GPIO_Pin_9, RCC_APB2Periph_GPIOC );
	leds[1] = new Gpio(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC );

	leds[0]->init(GPIO_Mode_Out_PP);
	leds[1]->init(GPIO_Mode_Out_PP);

	Gpio scl(GPIOB, GPIO_Pin_6, RCC_APB2Periph_GPIOB );
	Gpio sda(GPIOB, GPIO_Pin_7, RCC_APB2Periph_GPIOB );
	scl.init(GPIO_Mode_AF_OD);
	sda.init(GPIO_Mode_AF_OD);

	i2c.init();

	//	nvic.init(TIM2_IRQn, 0, 3, ENABLE);
	//	Tim t2(TIM2, RCC_APB1Periph_TIM2, RCC_APB1PeriphClockCmd);
	//	t2.init(1000, 1000);
	//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//	TIM_Cmd(TIM2, ENABLE);

	///////////////////////////////////////////

	uint8_t w;

	uint8_t cache[] = { 0x0e, 0x9c };
	w = i2c.write(0xd0, cache, 2);
	fprintf(stderr, "%02x:\r\n", w);

	uint8_t cache2[] = { 0x0f, 0x00 };
	w = i2c.write(0xd0, cache2, 2);
	fprintf(stderr, "%02x:\r\n", w);

}

void loop() {
	static u8 h, m, s, i = 0;
	uint8_t w, r;

	uint8_t cmd = 0;
	w = i2c.write(0xd0, &cmd, 1);
	r = i2c.read(0xd0, &s, 1);

	cmd = 1;
	i2c.write(0xd0, &cmd, 1);
	i2c.read(0xd0, &m, 1);

	cmd = 2;
	i2c.write(0xd0, &cmd, 1);
	i2c.read(0xd0, &h, 1);

	fprintf(stderr, "[%02x %02x] %02x:%02x:%02x\r\n", w, r, h, m, s);

//	while (usart.available()) {
//		char c = usart.read();
//		fprintf(stdout, "0x%02X\r\n", c);
//	}

	static char str[128] = { 0 };
	if (usart.available()) {
		int len = usart.readBytesUntil('\r', str, 128);
		if (len && len != -1) {
			str[len] = 0;
			fprintf(stderr, "%d %s\r\n", i, str);
		}
	}

	leds[0]->toggle();
	leds[1]->toggle();

	delay(1000);
}

