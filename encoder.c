/*
 * encoder.c
 *
 *  Created on: Nov 10, 2025
 *      Author: Aryan
 *
 *  Library to interface 2 Optical Encoders
 *  Makes use of the tm4c QEI interface
 */


#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>
#include "encoder.h"

//Consists of the various functions required to utilize the encoder


void encoder_init()
{
    //Initializes the two encoders
    SYSCTL_RCGCQEI_R = 0x3; // Enable both Encoder Modules
    SYSCTL_RCGCGPIO_R |= (1<<3) | (1<<2); //Enable clock to Port C and Port D

    GPIO_PORTD_LOCK_R = 0x4C4F434B;
    GPIO_PORTD_CR_R  |= (1<<7);


    GPIO_PORTD_AFSEL_R |= (1<<7)|(1<<6); //Enable AFSEL forPD6 and PD7
    GPIO_PORTC_AFSEL_R |= (1<<6)|(1<<5); // Enable AFSEL for PC5 and PC6

    GPIO_PORTD_PCTL_R |= (0x6 << 28)|(0x6 << 24); //Enabling QEI for GPIO
    GPIO_PORTC_PCTL_R |= (0x6 << 24)|(0x6 << 20); //Enabling QEI for GPIO


    GPIO_PORTD_DEN_R |= (1<<6) | (1<<7);
    GPIO_PORTC_DEN_R |= (1<<5) | (1<<6);

    //Filter off Stall disabled

    QEI0_CTL_R = (1<<5); //Enabling velocity capture
    QEI0_CTL_R |= (1 << 4) | (1 << 3);
    QEI0_MAXPOS_R = ENCODER_COUNT-1; //Recheck model once
    QEI0_CTL_R |= (1 << 0);

    QEI1_CTL_R = (1<<5); //Enabling velocity capture
    QEI1_CTL_R |= (1 << 4) | (1 << 3);
    // Velocity capture timer (1ms update)
    QEI0_LOAD_R = 16000;
    QEI1_LOAD_R = 16000;
    QEI1_MAXPOS_R = ENCODER_COUNT-1; //Recheck model once
    QEI1_CTL_R |= (1 << 0);
}

struct EncoderState read_cart_state()
{
    int velocity = QEI0_SPEED_R; //pps
    int position = QEI0_POS_R;
    bool direction = QEI0_STAT_R >> 1;

    struct EncoderState cart_state;
    cart_state.position = position;
    cart_state.velocity = velocity;
    cart_state.direction = direction;
    cart_state.angle = ((float)position/(float)(ENCODER_COUNT-1))*2*M_PI;

//    if (cart_state.angle > M_PI )
//    {
//        cart_state.angle -= 2*M_PI;
//    }

    cart_state.angle_velocity = ((float)velocity/(float)(ENCODER_COUNT-1))*2*M_PI;

    if (cart_state.direction)
    {
        cart_state.angle_velocity *= -1;
    }

    return cart_state;
}

struct EncoderState read_pendulum_state()
{
    int velocity = QEI1_SPEED_R; //pps
    int position = QEI1_POS_R;
    bool direction = QEI1_STAT_R >> 1;

    struct EncoderState pendulum_state;
    pendulum_state.position = position;
    pendulum_state.velocity = velocity;
    pendulum_state.direction = direction;
    pendulum_state.angle = ((float)position/(float)(ENCODER_COUNT-1))*2*M_PI;

//    if (pendulum_state.angle > M_PI)
//    {
//        pendulum_state.angle -= 2 * M_PI;
//    }
    pendulum_state.angle_velocity = ((float)velocity/(float)(ENCODER_COUNT-1))*2*M_PI;

    if (pendulum_state.direction)
    {
        pendulum_state.angle_velocity *= - 1;
    }

    return pendulum_state;
}
