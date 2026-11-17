#ifndef INC_steering_wheel_module_26_H
#define INC_steering_wheel_module_26_H

#include "main.h"
#include "GopherCAN.h"
#include <stdio.h>
#include "stm32f4xx_hal_can.h"

#define HBEAT_LED_DELAY_TIME_ms 500
#define NUM_DISPLAY_PAGES 2

void init(CAN_HandleTypeDef* hcan_ptr, UART_HandleTypeDef* uart_ptr);
void can_buffer_handling_loop();
void main_loop();
void hbeat_blink();
void update_button_states();
void update_rotary_dial_states();
void uart_debug_int(int value);
#endif /* INC_steering_wheel_module_26_H */