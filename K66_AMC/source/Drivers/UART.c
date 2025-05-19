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
volatile uint8_t CAMBuffer[CAM_BUFFER_SIZE];

static volatile uint8_t uart4FrameBuffer[12]; // Buffer for frame up to "255,255,255\n"
static volatile uint8_t uart4FrameIndex = 0;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
static bool ParseUART4Frame(const uint8_t *frame, uint8_t length, uint8_t *outBuffer);

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
    UART4config.baudRate_Bps = 115200;
    UART4config.enableTx     = true;
    UART4config.enableRx     = true;

    UART_Init(UART1, &UART1config, UART1_CLK_FREQ);
    UART_Init(UART2, &UART2config, UART2_CLK_FREQ);
    UART_Init(UART4, &UART4config, UART4_CLK_FREQ);

    /* Enable receive interrupts */
    UART_EnableInterrupts(UART1, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART1_IRQn);

    UART_EnableInterrupts(UART2, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART2_IRQn);

    UART_EnableInterrupts(UART4, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    EnableIRQ(UART4_IRQn);
}

void UART1_IRQHandler(void)
{
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

void UART4_IRQHandler(void)
{
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART4))
    {
        uint8_t data = UART_ReadByte(UART4);

        if (uart4FrameIndex < 12)
        {
            uart4FrameBuffer[uart4FrameIndex] = data;
            uart4FrameIndex++;

            // Check for end of frame
            if (data == '\n')
            {
            	ParseUART4Frame(uart4FrameBuffer, uart4FrameIndex, CAMBuffer);
                uart4FrameIndex = 0; // next frame
            }
        }
        else
        {
            // Buffer overflow, reset
            uart4FrameIndex = 0;
        }
    }
    SDK_ISR_EXIT_BARRIER;
}

static bool ParseUART4Frame(const uint8_t *frame, uint8_t length, uint8_t *outBuffer)
{
    if (length < 7 || frame[length - 1] != '\n')
    {
        return false;
    }

    uint16_t values[3] = {0, 0, 0};
    uint8_t valueIndex = 0;
    uint8_t digitCount = 0;
    uint8_t i = 0;

    while (i < length - 1) // Exclude '\n'
    {
        if (frame[i] >= '0' && frame[i] <= '9')
        {
            // Accumulate digit
            values[valueIndex] = values[valueIndex] * 10 + (frame[i] - '0');
            digitCount++;
            if (digitCount > 3) // Too many digits
            {
                return false;
            }
        }
        else if (frame[i] == ',')
        {
            // End of number
            if (digitCount == 0 || values[valueIndex] > 255) // No digits or value too large
            {
                return false;
            }
            valueIndex++;
            digitCount = 0;
            if (valueIndex > 2) // Too many commas
            {
                return false;
            }
        }
        else
        {
            // Invalid character
            return false;
        }
        i++;
    }

    // Check final number
    if (valueIndex != 2 || digitCount == 0 || values[2] > 255)
    {
        return false;
    }

    // Store valid values in CAMBuffer
    if (CAM_BUFFER_SIZE >= 3)
    {
        outBuffer[0] = (uint8_t)values[0];
        outBuffer[1] = (uint8_t)values[1];
        outBuffer[2] = (uint8_t)values[2];
        return true;
    }

    return false;
}

void UART2_IRQHandler(void)
{
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART2))
    {
        // uint8_t UART2data = UART_ReadByte(UART2);
    }
    SDK_ISR_EXIT_BARRIER;
}
