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

static void reverse(char* str, int len)
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

static int itoa(int num, char* str)
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
    reverse(str, i);
    return i;
}

void floatToString(float n, char *res) {
    int intPart = (int)n;
    float fractional = n - intPart;

    if (fractional < 0)
        fractional = -fractional;

    // Convert integer part
    int i = itoa(intPart, res);

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

int main(void)
{
    char buffer[100];
    char buffer1[100];

    UART0_Init();
    encoder_init();

    UART0_SendString("\r\nEncoder UART Output Initialized\r\n");

    while (1)
    {
        struct EncoderState cart = read_cart_state();
        struct EncoderState pendulum = read_pendulum_state();


        float angle_c = cart.angle;
        float angle_p = pendulum.angle;

        floatToString(angle_c, buffer);
        floatToString(angle_p, buffer1);

        char dir_0 = cart.direction ? '1' : '0';
        char dir_1 = pendulum.direction ? '1' : '0';

        //Data for plotter
        UART0_SendString(">C:");
        UART0_SendString(buffer);
        UART0_SendString(" Dir:");
        UART0_SendChar(dir_0);
        UART0_SendString(",");
        UART0_SendString("P:");
        UART0_SendString(buffer1);
        UART0_SendString(" Dir:");
        UART0_SendChar(dir_1);
        UART0_SendString("\r\n");

    }

}
