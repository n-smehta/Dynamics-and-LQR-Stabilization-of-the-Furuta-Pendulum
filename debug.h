/*
 * debug.h
 *
 *  Created on: Nov 18, 2025
 *      Author: shenlong
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "encoder.h"

void debug_init(void);
void debug_send_char(char c);
void debug_send_string(const char *str);

static void _reverse(char* str, int len);
static int int_to_string(int num, char* str);
void float_to_string(float n, char *res);

void send_system_state(struct EncoderState, struct EncoderState , float );
#endif /* DEBUG_H_ */
