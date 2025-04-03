/*
 * UART.c
 *
 *  Created on: 3 Dec 2024
 *      Author: kenneth
 */

#include "UART.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint8_t UART_Active;

static volatile uint16_t rxIndex = 0;
static volatile uint8_t dataIndex = 0;

volatile uint8_t RCBuffer[BUFFER_SIZE];
volatile uint8_t UART4Data;

/*******************************************************************************
 * Code
 ******************************************************************************/
void UART_Start(void)
{
    uart_config_t UART1config;
    uart_config_t UART2config;
    uart_config_t UART4config;

    CLOCK_EnableClock(kCLOCK_PortC);
    CLOCK_EnableClock(kCLOCK_PortD);

    SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC1 |= SIM_SCGC1_UART4_MASK;

    PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt3);
    PORT_SetPinMux(PORTC, 4U, kPORT_MuxAlt3);

    PORT_SetPinMux(PORTC, 14U, kPORT_MuxAlt3);
	PORT_SetPinMux(PORTC, 15U, kPORT_MuxAlt3);

	PORT_SetPinMux(PORTD, 2U, kPORT_MuxAlt3);
	PORT_SetPinMux(PORTD, 3U, kPORT_MuxAlt3);

    UART_GetDefaultConfig(&UART1config);
    UART1config.baudRate_Bps = 115200;
    UART1config.enableTx     = true;
    UART1config.enableRx     = true;

    UART_GetDefaultConfig(&UART2config);
	UART2config.baudRate_Bps = 38400;
	UART2config.enableTx     = true;
	UART2config.enableRx     = true;

	UART_GetDefaultConfig(&UART4config);
	UART4config.baudRate_Bps = 57600;
	UART4config.enableTx     = true;
	UART4config.enableRx     = true;

    UART_Init(UART1, &UART1config, UART1_CLK_FREQ);
    UART_Init(UART2, &UART2config, UART2_CLK_FREQ);
    UART_Init(UART4, &UART4config, UART4_CLK_FREQ);

    /* Habilitar interrupciones de recepción. */
    UART_EnableInterrupts(UART1, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART1_IRQn);

    UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART2_IRQn);

    UART_EnableInterrupts(UART4, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART4_IRQn);
}

void UART1_IRQHandler(void){
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART1))
    {
    	UART_Active = true;
        uint8_t UART1data = UART_ReadByte(UART1);

        if (dataIndex == 0 && UART1data == 0x20)
        {
            RCBuffer[dataIndex++] = UART1data;
        }
        else if (dataIndex > 0 && dataIndex < BUFFER_SIZE)
        {
            RCBuffer[dataIndex++] = UART1data;
            if (dataIndex == BUFFER_SIZE)
            {
                dataIndex = 0;
            }
        }

        if ((((rxIndex + 1) % BUFFER_SIZE) != 0))
        {
            rxIndex = (rxIndex + 1) % BUFFER_SIZE;
        }
    }
    SDK_ISR_EXIT_BARRIER;
}

void UART4_IRQHandler(void){
	if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART4))
	{
		UART4Data = UART_ReadByte(UART4);
	}
    SDK_ISR_EXIT_BARRIER;
}

void UART2_IRQHandler(void){
	if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART2))
	{
		uint8_t UART2data = UART_ReadByte(UART2);
	}
    SDK_ISR_EXIT_BARRIER;
}
