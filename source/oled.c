/*
 * oled.c
 *
 *  Created on: 2020年3月3日
 *      Author: fjk
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oled.h"
#include "oled_font.h"
#include "oled_screen.h"

int oled_init(oled_t *oled) {
	uint8_t init_cmd[] = { 0x00, //---set low column address
			0x10, //---set high column address
			0x40, //--set start line address
			0xB0, //--set page address
			0x81, // contract control
			0xFF, //--128
			0xA1, // set segment remap
			0xA6, //--normal / reverse
			0xA8, //--set multiplex ratio(1 to 64)
			0x3F, //--1/32 duty
			0xC8, // Com scan direction
			0xD3, //-set display offset
			0x00, //

			0xD5, // set osc division
			0x80, //

			0xD8, // set area color mode off
			0x05, //

			0xD9, // Set Pre-Charge Period
			0xF1, //

			0xDA, // set com pin configuartion
			0x12, //

			0xDB, // set Vcomh
			0x30, //

			0x8D, // set charge pump enable
			0x14, //

			0xAF, //--turn on oled panel
			};
	if (oled == NULL
			&& (oled->i2c_write_cmd_fun == NULL
					|| oled->i2c_write_data_fun == NULL)) {
		return -1;
	}
	return oled->i2c_write_cmd_fun(oled->addr, init_cmd, sizeof(init_cmd));
}

int oled_screen_state(oled_t *oled, led_screen_state_t state) {
	uint8_t oled_screen_state_cmd[3] = { 0X8D };
	switch ((int) state) {
	case ON:
		oled_screen_state_cmd[1] = 0X14;
		oled_screen_state_cmd[2] = 0XAF;
		break;
	case OFF:
	default:
		oled_screen_state_cmd[1] = 0X10;
		oled_screen_state_cmd[2] = 0XAE;
		break;
	}
	if (oled == NULL
			&& (oled->i2c_write_cmd_fun == NULL
					|| oled->i2c_write_data_fun == NULL)) {
		return -1;
	}
	return oled->i2c_write_cmd_fun(oled->addr, oled_screen_state_cmd,
			sizeof(oled_screen_state_cmd));
}

int oled_window_update(oled_t *oled) {
	int ret = 0;
	uint8_t row = 0;
#if defined(OLED_0_9_6)
	uint8_t oled_pos_cmd[] = {0xb0, 0x00, 0x10};
#elif defined(OLED_1_3_0)
	uint8_t oled_pos_cmd[] = { 0xb0, 0x02, 0x10 };
#else
#error "You need to define the OLED screen size，OLED_0_9_6 or OLED_1_3_0"
#endif
	if (oled == NULL
			&& (oled->i2c_write_cmd_fun == NULL
					|| oled->i2c_write_data_fun == NULL)) {
		return -1;
	}
	for (row = 0; row < 8; row++) {
		oled_pos_cmd[0] = 0xb0 + row;
		ret = oled->i2c_write_cmd_fun(oled->addr, oled_pos_cmd,
				sizeof(oled_pos_cmd));
		if (ret) {
			return ret;
		}
		ret = oled->i2c_write_data_fun(oled->addr, &(oled_buffer[row * 128]),
				128);
		if (ret) {
			return ret;
		}
	}
	return ret;
}

void oled_window_bit(uint8_t x, uint8_t y, uint8_t state) {
	x = x > 127 ? 127 : x;
	y = y > 63 ? 63 : y;
	if (state)
		oled_buffer[(y / 8) * 128 + x] |= (1 << (y % 8));
	else
		oled_buffer[(y / 8) * 128 + x] &= ~(1 << (y % 8));
}

void oled_window_char(uint8_t x, uint8_t y, char data) {

	uint8_t pos = data - ' ';

	x = x > 119 ? 119 : x;
	y = y > 6 ? 6 : y;

	memcpy(&oled_buffer[y * 128 + x], &F8X16[pos * 16], 8);
	memcpy(&oled_buffer[(y + 1) * 128 + x], &F8X16[pos * 16 + 8], 8);
}

void oled_window_string(uint8_t x, uint8_t y, const char *data) {
	int len = strnlen(data, 16);
	int col = 0;
	len = ((len * 8) > 128) ? ((128 - x) / 8) : len;
	for (col = 0; col < len; col++) {
		oled_window_char(x + col * 8, y, data[col]);
	}
}

void oled_window_data(uint8_t x, uint8_t y, uint8_t data) {
	x = x > 127 ? 127 : x;
	y = y > 7 ? 7 : y;
	memcpy(&oled_buffer[y * 128 + x], &data, sizeof(data));
}
void oled_window_line_clear(uint8_t y) {

	oled_window_line_full_data(y, 0);
}

void oled_window_line_full_data(uint8_t y, uint8_t data) {
	y = y > 6 ? 6 : y;
	memset(&oled_buffer[y * 128], data, 128);
	memset(&oled_buffer[(y + 1) * 128], data, 128);
}

void oled_window_clear(void) {
	memset(oled_buffer, 0x00, sizeof(oled_buffer));
}
