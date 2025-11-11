

/**
 * main.c
 */

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "motor_driver.h"



int main(void)
{
    driver_init();
    while(1)
    {
        drive_motor(1, 0.8);

    }
	return 0;
}
