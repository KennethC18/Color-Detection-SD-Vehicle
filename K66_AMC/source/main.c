/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    main.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "Drivers/PWM.h"
#include "Drivers/LED_RGB.h"
#include "Drivers/UART.h"
#include "Drivers/bno055.h"
#include "Drivers/I2C.h"

int16_t pwmservo;
int16_t pwmmotor;

uint16_t pwm_enable;
uint16_t drv_mode;
uint8_t RxActiveRC;

uint8_t lst;

#define AUTO_DRIVING_SPEED 60

#define BNO055_I2C_ADDR 0x28
#define BNO055_ID 0xA0

#define SIN_TABLE_SIZE (sizeof(sin_table) / sizeof(sin_table[0]))

typedef int16_t q15_t;
typedef int16_t angle_t;

angle_t yaw, pitch, roll;
angle_t yaw_offset = 0;
angle_t new_dir = 0;

static const q15_t sin_table[] = {
    0,      // sin(0°) = 0
    2284,   // sin(4°) ≈ 0.0698
    4557,   // sin(8°) ≈ 0.1392
    6802,   // sin(12°) ≈ 0.2079
    9017,   // sin(16°) ≈ 0.2756
    11197,  // sin(20°) ≈ 0.3420
    13328,  // sin(24°) ≈ 0.4067
    15386,  // sin(28°) ≈ 0.4695
    17366,  // sin(32°) ≈ 0.5299
    19261,  // sin(36°) ≈ 0.5878
    21063,  // sin(40°) ≈ 0.6428
    22766,  // sin(44°) ≈ 0.6947
    24363,  // sin(48°) ≈ 0.7431
    25847,  // sin(52°) ≈ 0.7880
    27212,  // sin(56°) ≈ 0.8290
    28453,  // sin(60°) ≈ 0.8660
    29565,  // sin(64°) ≈ 0.8988
    30544,  // sin(68°) ≈ 0.9272
    31385,  // sin(72°) ≈ 0.9511
    32086,  // sin(76°) ≈ 0.9703
    32643,  // sin(80°) ≈ 0.9848
    33056,  // sin(84°) ≈ 0.9945
    33322,  // sin(88°) ≈ 0.9990
    32767   // sin(90°) ≈ 1.0
};

q15_t sin_q15(angle_t angle) {
    int32_t deg = (int32_t)angle / 16;

    deg = deg % 360;
    if (deg < 0) deg += 360;

    int8_t sign = 1;
    if (deg > 180) {
        deg = 360 - deg;
        sign = -1;
    }
    if (deg > 90) {
        deg = 180 - deg;
    }

    uint32_t idx = deg / 4;
    uint32_t frac = deg % 4;
    q15_t sin_val;

    if (idx >= SIN_TABLE_SIZE - 1) {
        sin_val = sin_table[SIN_TABLE_SIZE - 1];
    } else {
        // Linear interpolation: sin_val = sin_table[idx] + (sin_table[idx+1] - sin_table[idx]) * frac / 4
        int32_t delta = sin_table[idx + 1] - sin_table[idx];
        sin_val = sin_table[idx] + (q15_t)((delta * frac) / 4);
    }

    return (q15_t)(sign * sin_val);
}

void Driver_Init(void);
void Drive_MotorsOff(void);

void BNO055_Init(void);
void BNO055_ReadEulerAngles(angle_t *yaw, angle_t *pitch, angle_t *roll);

static void BNO055_RefreshTask(void);

static void DriveMode_Auto(void);
static void DriveMode_Manual(void);
static void DriveMode_Check(void);

TaskHandle_t driveModeCheckHandle = NULL;


int main(void) {
    Driver_Init();
    Drive_MotorsOff();

    if (xTaskCreate(BNO055_RefreshTask, "BNO055_Refresh", configMINIMAL_STACK_SIZE + 100, NULL, 2, NULL) != pdPASS) {
        while (1);
    }

    if (xTaskCreate(DriveMode_Check, "DriveMode_Check", configMINIMAL_STACK_SIZE + 100, NULL, 1, &driveModeCheckHandle) != pdPASS) {
        while (1);
    }

    vTaskStartScheduler();

    while (1);
}

void Driver_Init(void) {
    LED_Init();
    UART_Start();
    I2C_Start();
    PWM_Init(pwm_channel_2);
    PWM_Init(pwm_channel_4);
    BNO055_Init();
    __enable_irq();
}

void Drive_MotorsOff(void) {
    PWM_Update(pwm_channel_2, pwm_duty0);
    PWM_Update(pwm_channel_4, pwm_duty0);
}

void BNO055_Init(void) {
    uint8_t chip_id = 0;

    if (I2C_Read(BNO055_I2C_ADDR, BNO055_CHIP_ID_ADDR, &chip_id, 1) != kStatus_Success || chip_id != BNO055_ID) {
        RED_LED();
        return;
    }

    I2C_Write(BNO055_I2C_ADDR, BNO055_OPR_MODE_ADDR, BNO055_OPERATION_MODE_CONFIG);

    I2C_Write(BNO055_I2C_ADDR, BNO055_OPR_MODE_ADDR, BNO055_OPERATION_MODE_NDOF);

    angle_t initial_yaw, initial_pitch, initial_roll;
    BNO055_ReadEulerAngles(&initial_yaw, &initial_pitch, &initial_roll);
    if (initial_yaw != 0) {
        yaw_offset = initial_yaw;
    }

    GREEN_LED();
}

void BNO055_ReadEulerAngles(angle_t *yaw, angle_t *pitch, angle_t *roll) {
    uint8_t data[6];

    if (I2C_Read(BNO055_I2C_ADDR, BNO055_EULER_H_LSB_ADDR, data, 6) == kStatus_Success) {
        *yaw = (angle_t)((data[1] << 8) | data[0]); // 1/16 degree units
        *pitch = (angle_t)((data[3] << 8) | data[2]);
        *roll = (angle_t)((data[5] << 8) | data[4]);

        *yaw -= yaw_offset;

        // yaw stays within 0 to 360 degrees (0 to 5760 in 1/16 degree units)
        if (*yaw < 0) {
            *yaw += 5760; // 360 degrees = 5760 units
        } else if (*yaw >= 5760) {
            *yaw -= 5760;
        }
    } else {
        YELLOW_LED();
        *yaw = *pitch = *roll = 0;
    }
}

static void BNO055_RefreshTask(void) {
    for (;;) {
        BNO055_ReadEulerAngles(&yaw, &pitch, &roll);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void DriveMode_Auto(void) {
    pwm_enable = (RCBuffer[11] << 8) | (RCBuffer[10]);
    if (pwm_enable == RC_sw_logic_state_ON) {

    	if(CAMBuffer[0] > 200){
    		Drive_MotorsOff();
    		RED_LED();
    	}
    	else if(CAMBuffer[1] > 100){
    		pwmservo = 400;
    		new_dir = -1000;

    		PWM_Update(pwm_channel_4, AUTO_DRIVING_SPEED);
			PWM_Update(pwm_channel_2, pwmservo);

			GREEN_LED();
    	}
    	else if(CAMBuffer[2] > 100){
			pwmservo = -400;
			new_dir = 1000;

			PWM_Update(pwm_channel_4, AUTO_DRIVING_SPEED);
			PWM_Update(pwm_channel_2, pwmservo);
			BLUE_LED();
		}
    	else{
    		if(new_dir != 0){
				yaw_offset = yaw + new_dir;

    			new_dir = 0;
    		}
    		WHITE_LED();

			q15_t sin_val = sin_q15(yaw);
			// Scale by 500 for pwm, shift to integer
			pwmservo = (int16_t)(((int32_t)sin_val * 500) >> 15);

			// Adjust servo direction based on yaw (90 deg to 270 deg: 1440 to 4320 in 1/16 deg)
			if (yaw >= 1440 && yaw <= 4320) {
				pwmservo *= servo_normal_direction;
			} else {
				pwmservo *= servo_invert_direction;
			}

			PWM_Update(pwm_channel_4, AUTO_DRIVING_SPEED);
			PWM_Update(pwm_channel_2, pwmservo);
    	}
    } else {
        GREEN_LED();
        Drive_MotorsOff();
    }

    vTaskResume(driveModeCheckHandle);
    vTaskDelete(NULL);
}

static void DriveMode_Manual(void) {
    pwm_enable = (RCBuffer[11] << 8) | (RCBuffer[10]);
    if (pwm_enable == RC_sw_logic_state_ON) {
        BLUE_LED();
        pwmmotor = (RCBuffer[7] << 8) | (RCBuffer[6]);
        pwmmotor = (pwmmotor - 1000) - pwm_max_duty;
        PWM_Update(pwm_channel_4, pwmmotor);

        pwmservo = (RCBuffer[3] << 8) | (RCBuffer[2]);
        if (pwmmotor >= pwm_max_duty / 2 || pwmmotor <= (-pwm_max_duty) / 2) {
            pwmservo = ((pwmservo - 1000) - pwm_max_duty) / 3;
        } else {
            pwmservo = (pwmservo - 1000) - pwm_max_duty;
        }

        pwmservo *= servo_normal_direction;
        PWM_Update(pwm_channel_2, pwmservo);
    } else {
        GREEN_LED();
        Drive_MotorsOff();
    }

    vTaskResume(driveModeCheckHandle);
    vTaskDelete(NULL);
}

static void DriveMode_Check(void) {
    for (;;) {
        if (UART_Active) {
            RxActiveRC = true;
            UART_Active = false;
            lst = 100;
        } else if ((!UART_Active) && lst > 0) {
            lst--;
        } else if (RxActiveRC && lst == 0) {
            RxActiveRC = false;
        }

        if (RxActiveRC) {
            drv_mode = (RCBuffer[15] << 8) | (RCBuffer[14]);
            if (drv_mode == RC_sw_logic_state_ON) {
                if (xTaskCreate(DriveMode_Auto, "DriveMode_Auto", configMINIMAL_STACK_SIZE + 100, NULL, 1, NULL) != pdPASS) {
                    while (1);
                }
                vTaskSuspend(NULL);
            } else if (drv_mode == RC_sw_logic_state_OFF) {
                if (xTaskCreate(DriveMode_Manual, "DriveMode_Manual", configMINIMAL_STACK_SIZE + 100, NULL, 1, NULL) != pdPASS) {
                    while (1);
                }
                vTaskSuspend(NULL);
            } else {
            	BNO055_Init();

                YELLOW_LED();
                Drive_MotorsOff();
            }
        } else {
            RED_LED();
            Drive_MotorsOff();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
