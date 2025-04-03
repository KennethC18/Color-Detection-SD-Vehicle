/*
 * author   : Luis Roberto Vega Campos
 * co-author: Kenneth Eduardo Castillo García
 * date     : 09/November/2023
 * modified : 03/December/2024
 * brief    : This module control pwm of FRDDRONE K66,
 * 		      this pwm is general, for servo, and motor drivers
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include "MK66F18.h"
#include "fsl_ftm.h"
#include "fsl_port.h"
#include "fsl_clock.h"

#define FTM0_IRQ        42
#define PWM_FTM         FTM0
#define NEUTRAL_TIME_US 1500
#define MAX_PWM_TIME_US 2000
#define MIN_PWM_TIME_US 1000

typedef enum _pwm_t
{
	pwm_channel_1 = 0,
	pwm_channel_2,
	pwm_channel_3,
	pwm_channel_4,
	pwm_channel_5,
	pwm_channel_6
} pwm_t;

typedef enum _pwm_duty
{
	pwm_min_duty = -500,
	pwm_duty0 = 0,
	pwm_max_duty = 500
} pwm_duty;

typedef enum _servo_direction
{
	servo_normal_direction = -1,
	servo_invert_direction = 1
} servo_direction;

typedef enum _RC_sw_logic_state
{
	RC_sw_logic_state_ON = 2000,
	RC_sw_logic_state_OFF = 1000
} RC_sw_logic_state;

void PWM_Init(pwm_t pwm);

// duty: duty_cycle, range -> (-500,500).
//       For motor driver -500 is full backward and 500 is full forward
void PWM_Update(pwm_t pwm, int16_t duty);

#endif /* PWM_H_ */
