/**
 ******************************************************************************
 * @file    demo/main.c
 * @author  aGuegu
 * @version V3.5.0
 * @date    May 15, 2013
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "usart/usart.h"
#include "gpio/gpio.h"
#include "tim/tim.h"
#include "nvic/nvic.h"
#include <vector>
#include "stdlib.h"
#include "i2c/i2c.h"

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

Usart usart(USART2, RCC_APB1Periph_USART2, RCC_APB1PeriphClockCmd);
Nvic nvic;

void init() {

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0 );

	usart.init(115200);

	Gpio usart_tx(GPIOA, GPIO_Pin_2,
			RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO );
	usart_tx.init(GPIO_Mode_AF_PP);

	Gpio usart_rx(GPIOA, GPIO_Pin_3,
			RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO );
	usart_rx.init(GPIO_Mode_IN_FLOATING);

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	nvic.init(USART2_IRQn, 0, 2, ENABLE);

	SysTick_Config(SystemCoreClock / 1000);	// Tick per ms
}

int main(void) {

	Gpio ** leds = (Gpio **) malloc(sizeof(Gpio *) * 2);

	leds[0] = new Gpio(GPIOC, GPIO_Pin_9, RCC_APB2Periph_GPIOC );
	leds[1] = new Gpio(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC );

	init();
	leds[0]->init(GPIO_Mode_Out_PP);
	leds[1]->init(GPIO_Mode_Out_PP);

	leds[0]->toggle();

	Gpio scl(GPIOB, GPIO_Pin_6, RCC_APB2Periph_GPIOB );
	Gpio sda(GPIOB, GPIO_Pin_7, RCC_APB2Periph_GPIOB );
	scl.init(GPIO_Mode_AF_OD);
	sda.init(GPIO_Mode_AF_OD);

	I2c i2c(I2C1, RCC_APB1Periph_I2C1 );
	i2c.init();

//	nvic.init(TIM2_IRQn, 0, 3, ENABLE);
//	Tim t2(TIM2, RCC_APB1Periph_TIM2, RCC_APB1PeriphClockCmd);
//	t2.init(1000, 1000);
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//	TIM_Cmd(TIM2, ENABLE);

	///////////////////////////////////////////

	//I2C_LowLevel_Init(400000, 0x00);

	uint8_t w;

	uint8_t cache[] = { 0x0e, 0x9c };
	//w = I2C_Write(I2C1, cache, 2, 0xd0);
	w = i2c.write(0xd0, cache, 2);
	fprintf(stderr, "%02x:\r\n", w);

	uint8_t cache2[] = { 0x0f, 0x00 };
	//w = I2C_Write(I2C1, cache2, 2, 0xd0);
	w = i2c.write(0xd0, cache2, 2);
	fprintf(stderr, "%02x:\r\n", w);

	while (1) {
		static u8 h, m, s = 0;

		uint8_t cmd = 0;
		i2c.write(0xd0, &cmd, 1);
		i2c.read(0xd0, &s, 1);

		cmd = 1;
		i2c.write(0xd0, &cmd, 1);
		i2c.read(0xd0, &m, 1);

		cmd = 2;
		i2c.write(0xd0, &cmd, 1);
		i2c.read(0xd0, &h, 1);

		fprintf(stderr, "%02x:%02x:%02x\r\n", h, m, s);

//		while (usart.available()) {
//			char c = usart.read();
//			fprintf(stdout, "0x%02X\r\n", c);
//		}

//		static char s[128] = { 0 };
//		int len = usart.readBytesUntil('\r', s, 128);
//		if (len && len != -1)
//		{
//			s[len] = 0;
//			fprintf(stderr, "%d %s\r\n", i, s);
//		}

		//i++;
		leds[0]->toggle();
		leds[1]->toggle();
		delay(1000);
	}
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1) {
	}
}
#endif

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
