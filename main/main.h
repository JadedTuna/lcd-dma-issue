#ifndef _R61529_MAIN_H
#define _R61529_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#define LOW 0
#define HIGH 1
#define OUTPUT GPIO_MODE_OUTPUT

// Pin definitions
#define PIN_CSX 4
#define PIN_DCX 5
#define PIN_WRX 7
#define PIN_RDX 36
#define PIN_RESX 37
#define PIN_D0 15
#define PIN_D1 16
#define PIN_D2 17
#define PIN_D3 18
#define PIN_D4 8
#define PIN_D5 3
#define PIN_D6 46
#define PIN_D7 9

#define LCD_H_RES 320
#define LCD_V_RES 480
// doesn't work with higher freqs @240MHz
#define LCD_HZ (4 * 1000 * 1000)
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

/* Structs */
typedef struct {
    uint32_t fb_size;
    uint8_t *fb;
    volatile int in_tx;
} ar_disp_drv_t;

/* Functions */
void setupPin(uint8_t pin, uint8_t mode);

esp_err_t lcd_init_all(ar_disp_drv_t *disp_drv, esp_lcd_panel_handle_t *panel_handle);

esp_err_t panel_r61529_init(esp_lcd_panel_t *panel);
esp_err_t lcd_new_panel_r61529(const esp_lcd_panel_io_handle_t io,
    const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);
// defined elsewhere
int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);

#endif