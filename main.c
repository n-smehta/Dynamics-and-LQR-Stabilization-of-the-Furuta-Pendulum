/*
 * main.c
 * Demonstrates reading encoder data and transmitting via UART0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "encoder.h"

// UART0 initialization (PA0 = RX, PA1 = TX)
void UART0_Init(void)
{
    SYSCTL_RCGCUART_R |= (1 << 0);  // Enable UART0 clock
    SYSCTL_RCGCGPIO_R |= (1 << 0);  // Enable Port A clock

    // Configure PA0, PA1 for alternate function (UART)
    GPIO_PORTA_AFSEL_R |= (1 << 0) | (1 << 1);
    GPIO_PORTA_PCTL_R |= (1 << 0) | (1 << 4); // UART0 RX/TX
    GPIO_PORTA_DEN_R  |= (1 << 0) | (1 << 1);

    UART0_CTL_R &= ~(1 << 0);       // Disable UART0 during config
    UART0_IBRD_R = 104;             // 16MHz / (16 * 9600) = 104.166
    UART0_FBRD_R = 11;              // Fractional part (.166 * 64 + 0.5)
    UART0_LCRH_R = (0x3 << 5);      // 8-bit, no parity, 1 stop bit
    UART0_CTL_R = (1 << 0) | (1 << 8) | (1 << 9);  // Enable UART0, TXE, RXE
}

// Send a single character
void UART0_SendChar(char c)
{
    while ((UART0_FR_R & (1 << 5)) != 0); // Wait until TXFF is clear
    UART0_DR_R = c;
}

// Send a string
void UART0_SendString(const char *str)
{
    while (*str)
    {
        UART0_SendChar(*str++);
    }
}

// Delay (approx, not precise)
void delayMs(int n)
{
    volatile int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3180; j++);
}

int main(void)
{
    char buffer[100];

    UART0_Init();
    encoder_init();

    UART0_SendString("\r\nEncoder UART Output Initialized\r\n");

    while (1)
    {
        struct EncoderState cart = read_cart_state();
        struct EncoderState pendulum = read_pendulum_state();

        snprintf(buffer, sizeof(buffer),
                 "Cart Pos: %d  Angle: %.2f rad | Pend Pos: %d  Angle: %.2f rad\r\n",
                 cart.position, cart.angle,
                 pendulum.position, pendulum.angle);

        UART0_SendString(buffer);

        delayMs(200); // 5 Hz refresh rate
    }
}
