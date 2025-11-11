/*
 * motor_driver.h
 *
 *  Created on: Nov 11, 2025
 *      Author: shenlong
 */

#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"

// === Configuration constants ===
#define RELOAD_VALUE 400

// === Function prototypes ===
void driver_init(void);
int calculate_duty_cycle(float duty_cycle);
void drive_motor(int direction, float speed);


#endif /* MOTOR_DRIVER_H_ */
