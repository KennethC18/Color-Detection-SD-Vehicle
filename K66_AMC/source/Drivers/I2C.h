/*
 * I2C.h
 *
 *  Created on: 7 nov. 2024
 *      Author: kenne
 */

#ifndef I2C_H_
#define I2C_H_

#include "pin_mux.h"
#include "board.h"
#include "fsl_i2c.h"

#define I2C0_MASTER_CLK_SRC         I2C0_CLK_SRC
#define I2C0_MASTER_CLK_FREQ        CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C0_MASTER_BASEADDR 		I2C0

#define I2C0_SLAVE_ADDR_7BIT 		0x28		/* bno055 */
#define I2C0_BAUDRATE              	400000U		/* 100 kHz*/
#define I2C0_DATA_LENGTH           	1U

//#define I2C1_MASTER_CLK_SRC         I2C1_CLK_SRC
//#define I2C1_MASTER_CLK_FREQ        CLOCK_GetFreq(I2C1_CLK_SRC)
//#define I2C1_MASTER_BASEADDR 		I2C1
//
//#define I2C1_SLAVE_ADDR_7BIT 		0x42		/* u-blox neo-m8n */
//#define I2C1_BAUDRATE              	400000U		/* 400 kHz*/
//#define I2C1_DATA_LENGTH           	8U

/*!
 * @brief  Converts from a BCD value to an integer.
 *
 * @param  bcd 				BDC value to convert.
 *
 * @return                  Integer conversion.
 * @retval Decimal value    Decimal equivalent of BCD value.
 */
//uint8_t BCDtoDecimal(uint8_t bcd);

/*!
 * @brief  Converts from an integer value to BCD.
 *
 * @param  decimal 		  	BCD value to convert.
 *
 * @return                  BCD conversion.
 * @retval BCD value    	BCD equivalent of decimal value.
 */
//uint8_t DecimalToBCD(uint8_t decimal);

/*!
 * @brief  Retrieves data from I2C slave.
 *
 */
status_t I2C_Read(uint8_t deviceAddress, uint8_t regAddress, uint8_t *data, size_t dataSize);

/*!
 * @brief  Retrieves data from I2C slave.
 *
 */
//uint8_t I2C1_RX(void);

/*!
 * @brief  Sends data to I2C slave.
 *
 * @param  TxBuffer		  	Master transmit buffer.
 */
status_t I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data);

/*!
 * @brief  Sends data to I2C slave.
 *
 * @param  TxBuffer		  	Master transmit buffer.
 */
//void I2C1_TX(uint8_t TxBuffer);

/*!
 * @brief  Configure and initialise I2C module.
 */
void I2C_Start(void);

#endif /* I2C_H_ */
