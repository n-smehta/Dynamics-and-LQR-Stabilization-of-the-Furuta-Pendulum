/*
 * debug.c
 *
 *  Created on: Nov 18, 2025
 *      Author: shenlong
 */

#include "debug.h"

void debug_init(void)
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

void debug_send_char(char c)
{
    while ((UART0_FR_R & (1 << 5)) != 0); // Wait until TXFF is clear
    UART0_DR_R = c;
}


void debug_send_string(const char *str)
{
    while (*str)
    {
        debug_send_char(*str++);
    }
}

static void _reverse(char* str, int len)
{
    int i = 0, j = len - 1;
    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

static int int_to_string(int num, char* str)
{
    int i = 0;
    bool isNegative = false;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    while (num != 0) {
        int rem = num % 10;
        str[i++] = rem + '0';
        num = num / 10;
    }

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';
    _reverse(str, i);
    return i;
}

void float_to_string(float n, char *res) {
    int intPart = (int)n;
    float fractional = n - intPart;

    if (fractional < 0)
        fractional = -fractional;

    // Convert integer part
    int i = int_to_string(intPart, res);

    // Decimal point
    res[i++] = '.';

    // Scale fractional part to 5 digits
    int frac = (int)(fractional * 100000);

    // Ensure exactly 5 digits with leading zeros
    res[i + 4] = (frac % 10) + '0'; frac /= 10;
    res[i + 3] = (frac % 10) + '0'; frac /= 10;
    res[i + 2] = (frac % 10) + '0'; frac /= 10;
    res[i + 1] = (frac % 10) + '0'; frac /= 10;
    res[i + 0] = (frac % 10) + '0';

    res[i + 5] = '\0';
}

