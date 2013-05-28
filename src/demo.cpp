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

void Delay(u32 count) {
	extern vu32 TimingDelay;
	TimingDelay = count;
	while (TimingDelay)
		;
}

Gpio led_green(GPIOC, GPIO_Pin_9, RCC_APB2Periph_GPIOC );
Gpio led_blue(GPIOC, GPIO_Pin_8, RCC_APB2Periph_GPIOC );
Usart usart(USART2, RCC_APB1Periph_USART2, RCC_APB1PeriphClockCmd);

void initUsart() {
	usart.init(115200);

	Gpio usart_tx(GPIOA, GPIO_Pin_2,
			RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO );
	usart_tx.init(GPIO_Mode_AF_PP);

	Gpio usart_rx(GPIOA, GPIO_Pin_3,
			RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO );
	usart_rx.init(GPIO_Mode_IN_FLOATING);

	USART_Cmd(USART2, ENABLE);

	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
}

int main(void) {

	led_green.init(GPIO_Mode_Out_PP);
	led_blue.init(GPIO_Mode_Out_PP);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0 );
	Nvic nvic;
	nvic.init(TIM2_IRQn, 0, 3, ENABLE);

	SysTick_Config(SystemCoreClock / 1000);

	Tim t2(TIM2, RCC_APB1Periph_TIM2, RCC_APB1PeriphClockCmd);
	t2.init(1000, 1000);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	///////////////////////////////////////////

	initUsart();

	while (1) {
		static u8 i = 0;

		fprintf(stdout, "%d %s\r\n", i, "stdout");
		fprintf(stderr, "%d %s\r\n", i, "stderr");

		led_blue.toggle();

		i++;
		Delay(1000);
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
