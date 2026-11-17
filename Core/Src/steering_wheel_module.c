#include "steering_wheel_module.h"
#include "gopher_sense.h"
#include "stm32f4xx_hal_gpio.h"

CAN_HandleTypeDef* CAN_CARSIDE;
UART_HandleTypeDef* UART_DEBUG;

// Face Buttons 
U8 sw_button0;
U8 sw_button1;
U8 sw_button2;
U8 sw_button3;
U8 sw_button4;
U8 sw_button5;

// Display Change Button
U8 display_page = 1;
U8 last_displayPageButton_state;

//rotary dial A
U8 rot_a_sw0_in;
U8 rot_a_sw1_in;
U8 rot_a_sw2_in;
U8 rot_a_sw3_in;
U8 rot_a_result;
U8 rot_a_look_up[16] = {2, 1, 0, 15, 10, 9, 8, 7, 14, 13, 12, 11, 6, 5, 4, 3};


U8 rot_b_sw0_in;
U8 rot_b_sw1_in;
U8 rot_b_sw2_in;
U8 rot_b_sw3_in;
U8 rot_b_result;
U8 rot_b_look_up[16] = {8, 7, 6, 5, 0, 15, 14, 13, 4, 3, 2, 1, 12, 11, 10, 9};
// init
//  What needs to happen on startup in order to run GopherCAN
void init(CAN_HandleTypeDef* hcan_ptr, UART_HandleTypeDef* uart_ptr)
{
	CAN_CARSIDE = hcan_ptr;
	init_can(CAN_CARSIDE, GCAN0);

	UART_DEBUG = uart_ptr;
}


// can_buffer_handling_loop
//  This loop will handle CAN RX software task and CAN TX hardware task. Should be
//  called every 1ms or as often as received messages should be handled
void can_buffer_handling_loop()
{
	// handle each RX message in the buffer
	if (service_can_rx_buffer())
	{
		// an error has occurred
	}

	// handle the transmission hardware for each CAN bus
	service_can_tx(CAN_CARSIDE);
}


// main_loop
//  another loop. This includes logic for sending a CAN command. Designed to be
//  called every 10ms
void main_loop()
{
	hbeat_blink();
	update_button_states();
	update_rotary_dial_states();
}

//Heartbeat LED
void hbeat_blink(){
	static uint32_t last_led = 0;
	if(HAL_GetTick() - last_led >= HBEAT_LED_DELAY_TIME_ms) {
		HAL_GPIO_TogglePin(HBEAT_LED_GPIO_Port, HBEAT_LED_Pin);
		last_led = HAL_GetTick();
	}
}

void update_button_states(){
	// Inverted states due to pullups
	sw_button0 = !HAL_GPIO_ReadPin(Face_BTN_0_In_GPIO_Port,Face_BTN_0_In_Pin);
	sw_button1 = !HAL_GPIO_ReadPin(Face_BTN_1_In_GPIO_Port,Face_BTN_1_In_Pin);
	sw_button2 = !HAL_GPIO_ReadPin(Face_BTN_2_In_GPIO_Port,Face_BTN_2_In_Pin);
	sw_button3 = !HAL_GPIO_ReadPin(Face_BTN_3_In_GPIO_Port,Face_BTN_3_In_Pin);
	sw_button4 = !HAL_GPIO_ReadPin(Face_BTN_4_In_GPIO_Port,Face_BTN_4_In_Pin);
	sw_button5 = !HAL_GPIO_ReadPin(Face_BTN_5_In_GPIO_Port,Face_BTN_5_In_Pin);

	update_and_queue_param_u8(&swButon0_state, sw_button0);
	update_and_queue_param_u8(&swButon1_state, sw_button1);
	update_and_queue_param_u8(&swButon2_state, sw_button2);
	update_and_queue_param_u8(&swButon3_state, sw_button3);
	update_and_queue_param_u8(&swButon4_state, sw_button4);
	update_and_queue_param_u8(&swButon5_state, sw_button5);

	U8 new_displayPageButton_state = !HAL_GPIO_ReadPin(Face_BTN_3_In_GPIO_Port, Face_BTN_3_In_Pin);
	if(new_displayPageButton_state > last_displayPageButton_state) {
		display_page = (display_page + 1) % (NUM_DISPLAY_PAGES);
	}
	last_displayPageButton_state = new_displayPageButton_state;

	update_and_queue_param_u8(&displayPage_state, display_page + 1);
}

void update_rotary_dial_states(){
	// Rotary Dial A
	rot_a_sw0_in = HAL_GPIO_ReadPin(Rot_A_0_In_GPIO_Port, Rot_A_0_In_Pin);
	rot_a_sw1_in = HAL_GPIO_ReadPin(Rot_A_1_In_GPIO_Port, Rot_A_1_In_Pin);
	rot_a_sw2_in = HAL_GPIO_ReadPin(Rot_A_2_In_GPIO_Port, Rot_A_2_In_Pin);
	rot_a_sw3_in = HAL_GPIO_ReadPin(Rot_A_3_In_GPIO_Port, Rot_A_3_In_Pin);

	rot_a_result = (rot_a_sw3_in << 2) | (rot_a_sw2_in << 3) | (rot_a_sw1_in << 1) | rot_a_sw0_in;
	rot_a_result = rot_a_look_up[rot_a_result];
	update_and_queue_param_u8(&swDial_a_ul, rot_a_result);

	//Rotary Dial B
	rot_b_sw0_in = HAL_GPIO_ReadPin(Rot_B_0_In_GPIO_Port, Rot_B_0_In_Pin);
	rot_b_sw1_in = HAL_GPIO_ReadPin(Rot_B_1_In_GPIO_Port, Rot_B_1_In_Pin);
	rot_b_sw2_in = HAL_GPIO_ReadPin(Rot_B_2_In_GPIO_Port, Rot_B_2_In_Pin);
	rot_b_sw3_in = HAL_GPIO_ReadPin(Rot_B_3_In_GPIO_Port, Rot_B_3_In_Pin);

	rot_b_result = (rot_b_sw3_in << 2) | (rot_b_sw2_in << 3) | (rot_b_sw1_in << 1) | rot_b_sw0_in;
	rot_b_result = rot_b_look_up[rot_b_result];
	update_and_queue_param_u8(&swDial_b_ul, rot_b_result);
}