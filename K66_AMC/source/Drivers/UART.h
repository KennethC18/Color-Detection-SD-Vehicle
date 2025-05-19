/*
 * UART.h
 *
 *  Created on: 3 Dec 2024
 *      Author: kenneth
 */

#ifndef DRIVERS_UART_H_
#define DRIVERS_UART_H_

#include "fsl_uart.h"
#include "pin_mux.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART1_CLKSRC     UART1_CLK_SRC
#define UART1_CLK_FREQ   CLOCK_GetFreq(UART1_CLK_SRC)
#define UART1_IRQn       UART1_RX_TX_IRQn
#define UART1_IRQHandler UART1_RX_TX_IRQHandler

#define UART4_CLKSRC     UART4_CLK_SRC
#define UART4_CLK_FREQ   CLOCK_GetFreq(UART4_CLK_SRC)
#define UART4_IRQn       UART4_RX_TX_IRQn
#define UART4_IRQHandler UART4_RX_TX_IRQHandler

#define UART2_CLKSRC     UART2_CLK_SRC
#define UART2_CLK_FREQ   CLOCK_GetFreq(UART2_CLK_SRC)
#define UART2_IRQn       UART2_RX_TX_IRQn
#define UART2_IRQHandler UART2_RX_TX_IRQHandler

#define BUFFER_SIZE      16
#define CAM_BUFFER_SIZE  3

extern volatile uint8_t UART_Active;

extern volatile uint8_t RCBuffer[BUFFER_SIZE];
extern volatile uint8_t CAMBuffer[CAM_BUFFER_SIZE];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Inicializa el módulo UART 1 y UART 4.
 */
void UART_Start(void);

/*!
 * @brief Manejador de interrupciones UART 1.
 */
void UART1_IRQHandler(void);

/*!
 * @brief Manejador de interrupciones UART 2.
 */
void UART2_IRQHandler(void);

/*!
 * @brief Manejador de interrupciones UART 4.
 */
void UART4_IRQHandler(void);

#endif /* DRIVERS_UART_H_ */
