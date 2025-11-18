/*
 * motor_driver.c
 *
 *  Created on: Nov 10, 2025
 *      Author: shenlong
 *  File consisting of functions to drive a motor while using a Motor Driver
 *
 */

#include "motor_driver.h"


void driver_init()
{
    //Initialize required PWM signals
    //PWM signals for PD0 M0PWM7 PD1 M0PWM6
    // Running at 20KHz
    //RCGC0 --> 1 << 20
    //RCGC --> 1<< 3
    int reload_value = 400;
    //GPIOASFEL
    SYSCTL_RCGCPWM_R |= 1<<0; //Enable PWM0
    SYSCTL_RCGC2_R |= 1<< 3 ; //Enabling clock to Port D

    GPIO_PORTD_DEN_R |= (1 << 0) | (1 << 1);     // Enable digital function
    GPIO_PORTD_AFSEL_R |= (1<<0)| (1<<1);

    //Set alternate function to PWM
    GPIO_PORTD_PCTL_R |= 0x4<<4 | 0x4;

    SYSCTL_RCC_R |= 1<<20; //USE PWM DIV
    SYSCTL_RCC_R &= ~(0x7 << 17); // setting clock division to /2 will adjust

    PWM0_CTL_R = 0;

    PWM0_3_GENA_R |= 0x0000008C;
    PWM0_3_GENB_R |= 0x0000080C;

    PWM0_3_LOAD_R = reload_value -1; //20kHz has 800 clock pulses

    //Setting both to 0 duty Cycle

    PWM0_3_CMPA_R = 399;
    PWM0_3_CMPB_R = 0;


    PWM0_3_CTL_R |= 1;

    PWM0_ENABLE_R |= (1<<7) |(1<<6);



}





void drive_motor(int direction, float speed)
{
        if (speed < 0) speed = 0;
        if (speed > 1) speed = 1;

        int cmp_val = RELOAD_VALUE - (int)(speed * RELOAD_VALUE) - 1;

        if (speed == 0) {
            PWM0_ENABLE_R &= ~(1<<6);
            PWM0_ENABLE_R &= ~(1<<7);
            return;
        }


        if (direction > 0) {
            // Forward
            PWM0_ENABLE_R |= (1<<6);
            PWM0_ENABLE_R &= ~(1<<7);
            PWM0_3_CMPA_R = cmp_val;

        } else {
            // Reverse
            PWM0_ENABLE_R |= (1<<7);
            PWM0_ENABLE_R &= ~(1<<6);
            PWM0_3_CMPB_R = cmp_val;

        }
}

int calculate_duty_cycle(float duty_cycle)
{
    return (int)(duty_cycle*RELOAD_VALUE);
}

