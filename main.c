#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "encoder.h"
#include "debug.h"



//System parameters
#define MOTOR_R 3
#define MOTOR_Kt 3.37
#define SUPPLY_VOLTAGE 12
const float TORQUE_2_DUTY = MOTOR_R/(MOTOR_Kt*SUPPLY_VOLTAGE)

//Controller parameters
const float K[4] = {100, 1, 10, 1};
#define CONTROLLER_FREQ 10 //Hz

void delayMs(int);
float controller(int *x);



int main(void)
{
    char buffer[100];
    char buffer1[100];

    debug_init();
    encoder_init();

    debug_send_string("\r\nEncoder UART Output Initialized\r\n");

    while (1)
    {
        struct EncoderState cart = read_cart_state();
        struct EncoderState pendulum = read_pendulum_state();


        float angle_c = cart.angle;
        float angle_p = pendulum.angle;

        float_to_string(angle_c, buffer);
        float_to_string(angle_p, buffer1);

        char dir_0 = cart.direction ? '1' : '0';
        char dir_1 = pendulum.direction ? '1' : '0';

        //Data for plotter
        debug_send_string(">C:");
        debug_send_string(buffer);
        debug_send_string(" Dir:");
        debug_send_char(dir_0);
        debug_send_string(",");
        debug_send_string("P:");
        debug_send_string(buffer1);
        debug_send_string(" Dir:");
        debug_send_char(dir_1);
        debug_send_string("\r\n");

    }


}
// Delay (approx, not precise)
void delayMs(int n)
{
    volatile int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3180; j++);
}

float controller(int* x)
{
    return TORQUE_2_DUTY*(K[0]*x[0] + K[1]*x[1] + K[2]*x[2] + K[3]*x[3]);
}
