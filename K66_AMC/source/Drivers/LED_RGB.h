/*
 * LED_RGB.h
 *
 *  Created on: 26 Sept 2024
 *      Author: Kenneth
 */

#ifndef LED_RGB_H_
#define LED_RGB_H_

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

/*!
 * @brief Configuration structure for LED.
 */
typedef struct {
    gpio_pin_config_t config;
} LedConfig;

/*!
 * @brief Initialise board RGB LED with default configurations.
 */
void LED_Init(void);

/*!
 * @brief Turns off the board LED entirely.
 */
void LED_OFF(void);

/*!
 * @brief Turns on only the red board LED.
 */
void RED_LED(void);

/*!
 * @brief Turns on only the green board LED.
 */
void GREEN_LED(void);

/*!
 * @brief Turns on only the blue board LED.
 */
void BLUE_LED(void);

/*!
 * @brief Turns on both the red and blue board LED's to display magenta.
 */
void MAGENTA_LED(void);

/*!
 * @brief Turns on both the red and green board LED's to display yellow.
 */
void YELLOW_LED(void);

/*!
 * @brief Turns on both the green and blue board LED's to display cian.
 */
void CIAN_LED(void);

/*!
 * @brief Turns on all board LED's to display white.
 */
void WHITE_LED(void);

#endif /* LED_RGB_H_ */
