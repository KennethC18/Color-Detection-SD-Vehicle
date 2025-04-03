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

#include <Drivers/PWM.h>
#include "Drivers/LED_RGB.h"
#include "Drivers/UART.h"

int16_t pwmservo;
int16_t pwmmotor;

uint16_t pwm_enable;
uint16_t drv_mode;
uint8_t RxActiveRC;

uint8_t lst;

#define AUTO_DRIVING_SPEED 100

void Driver_Init(void);
void Drive_MotorsOff(void);

TaskHandle_t driveModeCheckHandle = NULL;

static void DriveMode_Auto(void *pvParameters);
static void DriveMode_Manual(void *pvParameters);
static void DriveMode_Check(void *pvParameters);

int main(void) {
    Driver_Init();
    Drive_MotorsOff();

    if (xTaskCreate(DriveMode_Check, "DriveMode_Check", configMINIMAL_STACK_SIZE + 100, NULL, 1, &driveModeCheckHandle) != pdPASS) {
        while (1);
    }

    vTaskStartScheduler();

    while (1);
    return 0;
}

void Driver_Init(void) {
    LED_Init();
    UART_Start();
    PWM_Init(pwm_channel_2);
    PWM_Init(pwm_channel_4);
    __enable_irq();
}

void Drive_MotorsOff(void) {
    PWM_Update(pwm_channel_2, pwm_duty0);
    PWM_Update(pwm_channel_4, pwm_duty0);
}

static void DriveMode_Auto(void *pvParameters) {
    pwm_enable = (RCBuffer[11] << 8) | (RCBuffer[10]);
	if (pwm_enable == RC_sw_logic_state_ON) {
		WHITE_LED();
	}
	else{
		GREEN_LED();
        Drive_MotorsOff();
	}

    vTaskResume(driveModeCheckHandle);
    vTaskDelete(NULL);
}

static void DriveMode_Manual(void *pvParameters) {
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

static void DriveMode_Check(void *pvParameters) {
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
