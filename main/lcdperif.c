#include "main.h"
#ifndef R61529_BITBANG_MODE
#include <string.h>
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "soc/gdma_struct.h"

static const char *TAG = "lcdperif";

int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);

static bool color_trans_cb(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    ar_disp_drv_t *disp_drv = (ar_disp_drv_t *)user_ctx;
    disp_drv->in_tx = 0;
    return true;
}

esp_err_t lcd_init_all(ar_disp_drv_t *disp_drv, esp_lcd_panel_handle_t *panel_handle) {
    // No reads from the display
    setupPin(PIN_RDX, OUTPUT);
    gpio_set_level(PIN_RDX, HIGH);
    ets_delay_us(1000 * 1000);

    ESP_LOGI(TAG, "Initializing Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = PIN_DCX,
        .wr_gpio_num = PIN_WRX,
        .data_gpio_nums = {
            PIN_D0,
            PIN_D1,
            PIN_D2,
            PIN_D3,
            PIN_D4,
            PIN_D5,
            PIN_D6,
            PIN_D7,
        },
        .bus_width = 8,
        // max transfer is whole framebuffer
        .max_transfer_bytes = disp_drv->fb_size * sizeof(uint8_t)
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = PIN_CSX,
        .pclk_hz = LCD_HZ,
        .trans_queue_depth = 1,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .on_color_trans_done = color_trans_cb,
        .user_ctx = disp_drv,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install LCD driver of r61529");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_RESX,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(lcd_new_panel_r61529(io_handle, &panel_config, panel_handle));
    ESP_LOGI(TAG, "%p %p", panel_handle, *panel_handle);

    ESP_LOGI(TAG, "reset");
    esp_lcd_panel_reset(*panel_handle);
    ESP_LOGI(TAG, "init");
    esp_lcd_panel_init(*panel_handle);

    ESP_LOGI(TAG, "after init");

    return ESP_OK;
}

#endif