/*
 * I2C.c
 *
 *  Created on: 7 nov. 2024
 *      Author: kenne
 */
#include "I2C.h"

uint8_t I2C0g_master_rxBuff[I2C0_DATA_LENGTH];
uint8_t I2C0g_master_txBuff[I2C0_DATA_LENGTH];

i2c_master_handle_t I2C0g_m_handle;
volatile bool I2C0g_MasterCompletionFlag = false;

i2c_master_config_t I2C0masterConfig;
i2c_master_transfer_t I2C0masterXfer;


//uint8_t I2C1g_master_rxBuff[I2C1_DATA_LENGTH];
//uint8_t I2C1g_master_txBuff[I2C1_DATA_LENGTH];
//
//i2c_master_handle_t I2C1g_m_handle;
//volatile bool I2C1g_MasterCompletionFlag = false;
//
//i2c_master_config_t I2C1masterConfig;
//i2c_master_transfer_t I2C1masterXfer;

/*!
 * @brief  Converts from a BCD value to an integer.
 *
 * @param  bcd 				BDC value to convert.
 *
 * @return                  Integer conversion.
 * @retval Decimal value    Decimal equivalent of BCD value.
 */
uint8_t BCDtoDecimal(uint8_t bcd) {
    return ((bcd / 16) * 10) + (bcd % 16);
}

/*!
 * @brief  Converts from an integer value to BCD.
 *
 * @param  decimal 		  	BCD value to convert.
 *
 * @return                  BCD conversion.
 * @retval BCD value    	BCD equivalent of decimal value.
 */
uint8_t DecimalToBCD(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

/*!
 * @brief  Retrieves I2C transfer completion flag.
 *
 * @param  base 		I2C base peripheral address.
 * @param  handle 		I2C handle for master.
 * @param  status 		I2C transfer completion status.
 * @param  userData .	I2C user data.
 */
//static void i2c0_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
//{
//    /* Signal transfer success when received success status. */
//    if (status == kStatus_Success)
//    {
//        I2C0g_MasterCompletionFlag = true;
//    }
//}

/*!
 * @brief  Retrieves I2C transfer completion flag.
 *
 * @param  base 		I2C base peripheral address.
 * @param  handle 		I2C handle for master.
 * @param  status 		I2C transfer completion status.
 * @param  userData .	I2C user data.
 */
//static void i2c1_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
//{
//    /* Signal transfer success when received success status. */
//    if (status == kStatus_Success)
//    {
//        I2C1g_MasterCompletionFlag = true;
//    }
//}

/*!
 * @brief  Retrieves data from I2C slave.
 *
 * @return                	Received slave data.
 * @retval I2C0g_master_rxBuff	I2C receive buffer.
 */
status_t I2C_Read(uint8_t deviceAddress, uint8_t regAddress, uint8_t *data, size_t dataSize) {
    i2c_master_transfer_t masterXfer;

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress = deviceAddress;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data = &regAddress;
    masterXfer.dataSize = 1;
    masterXfer.flags = kI2C_TransferNoStopFlag;

    // Escribir la dirección del registro
    status_t result = I2C_MasterTransferBlocking(I2C0_MASTER_BASEADDR, &masterXfer);
    if (result != kStatus_Success) {
        return result;
    }

    // Leer los datos
    masterXfer.direction = kI2C_Read;
    masterXfer.data = data;
    masterXfer.dataSize = dataSize;
    masterXfer.flags = kI2C_TransferRepeatedStartFlag;

    return I2C_MasterTransferBlocking(I2C0_MASTER_BASEADDR, &masterXfer);
}

/*!
 * @brief  Retrieves data from I2C1 slave.
 *
 * @return                	Received slave data.
 * @retval I2C1g_master_rxBuff	I2C1 receive buffer.
 */
//uint8_t I2C1_RX(void){
//	memset(&I2C1masterXfer, 0, sizeof(I2C1masterXfer));
//	I2C1masterXfer.slaveAddress   = I2C1_SLAVE_ADDR_7BIT;
//	I2C1masterXfer.direction      = kI2C_Read;
//	I2C1masterXfer.subaddress     = 0x00;
//	I2C1masterXfer.subaddressSize = 1;
//	I2C1masterXfer.data           = &I2C1g_master_rxBuff;
//	I2C1masterXfer.dataSize       = I2C1_DATA_LENGTH;
//	I2C1masterXfer.flags          = kI2C_TransferDefaultFlag;
//
//	I2C_MasterTransferNonBlocking(I2C1_MASTER_BASEADDR, &I2C1g_m_handle, &I2C1masterXfer);
//
//	I2C1g_MasterCompletionFlag = false;
//	while(!I2C1g_MasterCompletionFlag);
//
//	return I2C1g_master_rxBuff[0];
//}

/*!
 * @brief  Sends data to I2C0 slave.
 *
 * @param  TxBuffer		  	Master transmit buffer.
 */
status_t I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data) {
    i2c_master_transfer_t masterXfer;
    uint8_t buffer[2] = {regAddress, data};

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress = deviceAddress;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data = buffer;
    masterXfer.dataSize = sizeof(buffer);
    masterXfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(I2C0_MASTER_BASEADDR, &masterXfer);
}

/*!
 * @brief  Sends data to I2C0 slave.
 *
 * @param  TxBuffer		  	Master transmit buffer.
 */
//void I2C1_TX(uint8_t TxBuffer){
//	memset(&I2C1masterXfer, 0, sizeof(I2C1masterXfer));
//	I2C1masterXfer.slaveAddress   = I2C1_SLAVE_ADDR_7BIT;
//	I2C1masterXfer.direction      = kI2C_Write;
//	I2C1masterXfer.subaddress     = 0x00;
//	I2C1masterXfer.subaddressSize = 1;
//	I2C1masterXfer.data           = &TxBuffer;
//	I2C1masterXfer.dataSize       = I2C1_DATA_LENGTH;
//	I2C1masterXfer.flags          = kI2C_TransferDefaultFlag;
//
//	I2C1g_MasterCompletionFlag = false;
//
//	I2C_MasterTransferNonBlocking(I2C1_MASTER_BASEADDR, &I2C1g_m_handle, &I2C1masterXfer);
//
//	while(!I2C1g_MasterCompletionFlag);
//}

/*!
 * @brief  Configure and initialise I2C module.
 */
void I2C_Start(void){
	// Habilitar clock para el puerto I2C0 y sus pines
	CLOCK_EnableClock(kCLOCK_PortE);
	CLOCK_EnableClock(kCLOCK_I2c0);

	// Configurar los pines para I2C (PTB2 -> SCL, PTB3 -> SDA)
	PORT_SetPinMux(PORTE, 24U, kPORT_MuxAlt5);
	PORT_SetPinMux(PORTE, 25U, kPORT_MuxAlt5);

	// Configurar resistencias pull-up en los pines I2C
	port_pin_config_t pinConfig = {kPORT_PullUp, kPORT_SlowSlewRate, kPORT_PassiveFilterDisable,
								   kPORT_OpenDrainEnable, kPORT_LowDriveStrength, kPORT_MuxAlt5,
								   kPORT_UnlockRegister};
	PORT_SetPinConfig(PORTE, 24U, &pinConfig);
	PORT_SetPinConfig(PORTE, 25U, &pinConfig);

	// Inicializar el controlador I2C en modo maestro
	I2C_MasterGetDefaultConfig(&I2C0masterConfig);
	I2C0masterConfig.baudRate_Bps = I2C0_BAUDRATE;
	I2C_MasterInit(I2C0_MASTER_BASEADDR, &I2C0masterConfig, I2C0_MASTER_CLK_FREQ);

//	I2C_MasterGetDefaultConfig(&I2C1masterConfig);
//	I2C1masterConfig.baudRate_Bps = I2C1_BAUDRATE;	/* Configure I2C1 @ 400kHz */
//	I2C_MasterInit(I2C1_MASTER_BASEADDR, &I2C1masterConfig, I2C1_MASTER_CLK_FREQ);
//
//	memset(&I2C1g_m_handle, 0, sizeof(I2C1g_m_handle));
//	I2C_MasterTransferCreateHandle(I2C1_MASTER_BASEADDR, &I2C1g_m_handle, i2c1_master_callback, NULL);
}
