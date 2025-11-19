#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "encoder.h"
#include "debug.h"
#include "motor_driver.h"


//System parameters
#define MOTOR_R 3
#define MOTOR_Kt 3.37
#define SUPPLY_VOLTAGE 12
const float TORQUE_2_DUTY = MOTOR_R/(MOTOR_Kt*SUPPLY_VOLTAGE);

//Controller parameters
const float K[4] = {100, 1, 10, 1};
#define CONTROLLER_FREQ 10 //Hz

void delayMs(int);
float controller(int *x);

void controller_init();
void controller_handler();
void disable_controller();

volatile float duty_cycle = 0;
volatile struct EncoderState cart;
volatile struct EncoderState pendulum;
int main(void)
{

    debug_init();


    debug_send_string("\r\nPendulum initialized\r\n");
    debug_send_string("\rInitializing Peripherals\r\n");
    encoder_init();
    debug_send_string("\rInitialized encoder successfully\n");
    driver_init();
    debug_send_string("\rInitialized motor driver successfully\n");

    while (1)
    {
        cart = read_cart_state();
        pendulum = read_pendulum_state();

        if(abs(pendulum.angle) > 0.174533)
        {
            //Not in permissible limits stop
            debug_send_string("\rPendulum angle limit reached stopping controller\n");
            disable_controller();
        }
        if(abs(cart.angle) > 1.5708)
        {
            debug_send_string("\rCart angle limit reached stopping controller\n");
            //Not in permissible limits stop
            disable_controller();
        }



    }


}
// Delay (approx, not precise)
void delayMs(int n)
{
    volatile int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3180; j++);
}




void controller_init()
{
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_RELOAD_R  = (int)((16000000 /CONTROLLER_FREQ )-1);
    NVIC_ST_CTRL_R  = 7;
    NVIC_EN0_R |= (1<< 30); //Enable interrupt

}

void controller_handler()
{
    float x[4] = {pendulum.angle, cart.angle, pendulum.angle_velocity, cart.angle_velocity};

    duty_cycle = TORQUE_2_DUTY*(K[0]*x[0] + K[1]*x[1] + K[2]*x[2] + K[3]*x[3]); //Calculate Duty Cycle

    if(duty_cycle < 0)
    {
        drive_motor(-1, -duty_cycle);
    }
    else
    {
        drive_motor(1, duty_cycle);
    }
    send_system_state(cart, pendulum, duty_cycle);

}

void disable_controller()
{
    debug_send_string("Disabled Controller");
    NVIC_ST_CTRL_R &= ~(1); //Disable systick timer
}


void SysTickHandler(void)
{
    controller_handler();
}
