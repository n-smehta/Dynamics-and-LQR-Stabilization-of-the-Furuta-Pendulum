/*
 * encoder.h
 *
 *  Created on: Nov 10, 2025
 *      Author: shenlong
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#define ENCODER_COUNT 510 //Max count of the encoder
#define M_PI 3.14159265358979323846 //PI value for angle calculation

struct EncoderState
{
    int position;
    int velocity;
    float angle;
    float angle_velocity;
};

void encoder_init();
struct EncoderState read_pendulum_state();
struct EncoderState read_cart_state();



#endif /* ENCODER_H_ */
