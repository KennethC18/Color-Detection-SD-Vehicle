/*
 * LED_RGB.c
 *
 *  Created on: 26 Sept 2024
 *      Author: Kenneth
 */
#include "LED_RGB.h"

/*!
 * @brief Initialise board RGB LED with default configurations.
 */
void LED_Init(void){
	// Configuration for LED pins
	LedConfig led_config = {
	    .config = { kGPIO_DigitalOutput, 0 }  // Pin direction as output, Initial logic state = 1 (off)
	};

	CLOCK_EnableClock(kCLOCK_PortC);
	CLOCK_EnableClock(kCLOCK_PortD);

	PORT_SetPinMux(PORTD, 1U, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTC, 9U, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTC, 8U, kPORT_MuxAsGpio);

	/* Init output LED GPIO. */
	GPIO_PinInit(GPIOD, 1U, &led_config);
	GPIO_PinInit(GPIOC, 8U, &led_config);
	GPIO_PinInit(GPIOC, 9U, &led_config);
}

/*!
 * @brief Turns off the board LED entirely.
 */
void LED_OFF(void){
	GPIO_PinWrite(GPIOD, 1U, 0);
	GPIO_PinWrite(GPIOC, 9U, 0);
	GPIO_PinWrite(GPIOC, 8U, 0);
}

/*!
 * @brief Turns on only the red board LED.
 */
void RED_LED(void){
	LED_OFF();
	GPIO_PinWrite(GPIOD, 1U, 1);
}

/*!
 * @brief Turns on only the green board LED.
 */
void GREEN_LED(void){
	LED_OFF();
	GPIO_PinWrite(GPIOC, 9U, 1);
}

/*!
 * @brief Turns on only the blue board LED.
 */
void BLUE_LED(void){
	LED_OFF();
	GPIO_PinWrite(GPIOC, 8U, 1);
}

/*!
 * @brief Turns on both the red and blue board LED's to display magenta.
 */
void MAGENTA_LED(void){
	LED_OFF();
	GPIO_PinWrite(GPIOD, 1U, 1);
	GPIO_PinWrite(GPIOC, 8U, 1);
}

/*!
 * @brief Turns on both the red and green board LED's to display yellow.
 */
void YELLOW_LED(void){
	LED_OFF();
	GPIO_PinWrite(GPIOD, 1U, 1);
	GPIO_PinWrite(GPIOC, 9U, 1);
}

/*!
 * @brief Turns on both the green and blue board LED's to display cian.
 */
void CIAN_LED(void){
	LED_OFF();
	GPIO_PinWrite(GPIOC, 9U, 1);
	GPIO_PinWrite(GPIOC, 8U, 1);
}

/*!
 * @brief Turns on all board LED's to display white.
 */
void WHITE_LED(void){
	LED_OFF();
	GPIO_PinWrite(GPIOD, 1U, 1);
	GPIO_PinWrite(GPIOC, 9U, 1);
	GPIO_PinWrite(GPIOC, 8U, 1);
}
