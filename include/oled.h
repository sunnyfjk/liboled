/*
 * oled.h
 *
 *  Created on: 2020年3月3日
 *      Author: fjk
 */

#ifndef OLED_INCLUDE_OLED_H_
#define OLED_INCLUDE_OLED_H_

typedef enum {
  ON,
  OFF,
} led_screen_state_t;


typedef struct {
  uint8_t addr;
  int (*i2c_write_cmd_fun)(uint8_t, uint8_t *, size_t);

  int (*i2c_write_data_fun)(uint8_t, uint8_t *, size_t);
} oled_t;

int oled_init(oled_t *oled);

int oled_screen_state(oled_t *oled, led_screen_state_t state);

int oled_window_update(oled_t *oled);

void oled_window_bit(uint8_t x, uint8_t y, uint8_t state);

void oled_window_char(uint8_t x, uint8_t y, char data);

void oled_window_string(uint8_t x, uint8_t y, const char *data);

void oled_window_data(uint8_t x, uint8_t y, uint8_t data);

void oled_window_line_clear(uint8_t y);

void oled_window_line_full_data(uint8_t y,uint8_t data);

void oled_window_clear(void);

#endif /* OLED_INCLUDE_OLED_H_ */
