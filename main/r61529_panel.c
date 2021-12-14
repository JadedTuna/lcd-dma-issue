#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "main.h"

static const char *TAG = "r61529";

int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);

typedef struct {
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;

    int reset_gpio_num;
    bool reset_level;
    unsigned int bits_per_pixel;
    uint8_t color_mode; // color mode as used by the panel
} r61529_panel_t;

esp_err_t panel_r61529_init(esp_lcd_panel_t *panel) {
    r61529_panel_t *r61529 = __containerof(panel, r61529_panel_t, base);
    esp_lcd_panel_io_handle_t io = r61529->io;

    ets_delay_us(1000 * 100);
    esp_lcd_panel_io_tx_param(io, 0x11, NULL, 0);
    ets_delay_us(1000 * 200);
    esp_lcd_panel_io_tx_param(io, 0xB0, (uint8_t []) {
        0x04
    }, 1);
    ets_delay_us(1000 * 1);

    esp_lcd_panel_io_tx_param(io, 0xB3, (uint8_t []) {
        0x02, 0x00, 0x00, 0x00, 0x00
    }, 5);
    ets_delay_us(1000 * 1);

    esp_lcd_panel_io_tx_param(io, 0xB4, (uint8_t []) {
        0x00
    }, 1);

    esp_lcd_panel_io_tx_param(io, 0xC0, (uint8_t []) {
        0x03,
        0xDF,
        0x40,
        0x10,
        0x00,
        0x01,
        0x00,
        0x55
    }, 8);

    esp_lcd_panel_io_tx_param(io, 0xC1, (uint8_t []) {
        0x07,
        0x27,
        0x08,
        0x08,
        0x00
    }, 5);

    esp_lcd_panel_io_tx_param(io, 0xC4, (uint8_t []) {
        0x57,
        0x00,
        0x05,
        0x03
    }, 4);

    esp_lcd_panel_io_tx_param(io, 0xC6, (uint8_t []) {
        0x04
    }, 1);

    // Gamma settings start
    esp_lcd_panel_io_tx_param(io, 0xC8, (uint8_t []) {
        0x03,
        0x12,
        0x1A,
        0x24,
        0x32,
        0x4B,
        0x3B,
        0x29,
        0x1F,
        0x18,
        0x12,
        0x04,

        0x03,
        0x12,
        0x1A,
        0x24,
        0x32,
        0x4B,
        0x3B,
        0x29,
        0x1F,
        0x18,
        0x12,
        0x04,
    }, 24);

    esp_lcd_panel_io_tx_param(io, 0xC9, (uint8_t []) {
        0x03,
        0x12,
        0x1A,
        0x24,
        0x32,
        0x4B,
        0x3B,
        0x29,
        0x1F,
        0x18,
        0x12,
        0x04,

        0x03,
        0x12,
        0x1A,
        0x24,
        0x32,
        0x4B,
        0x3B,
        0x29,
        0x1F,
        0x18,
        0x12,
        0x04
    }, 24);

    esp_lcd_panel_io_tx_param(io, 0xCA, (uint8_t []) {
        0x03,
        0x12,
        0x1A,
        0x24,
        0x32,
        0x4B,
        0x3B,
        0x29,
        0x1F,
        0x18,
        0x12,
        0x04,

        0x03,
        0x12,
        0x1A,
        0x24,
        0x32,
        0x4B,
        0x3B,
        0x29,
        0x1F,
        0x18,
        0x12,
        0x04
    }, 24);
    // Gamma end

    esp_lcd_panel_io_tx_param(io, 0xD0, (uint8_t []) {
        0x99,//DC4~1//A5. Set up clock cycle of the internal step up controller.
        0x06,//BT // Set Voltage step up factor.
        0x08,// default according to the datasheet - does nothing.
        0x20,// VCN step up cycles.
        0x29,//VC1, VC2// VCI3 voltage = 2.70V;  VCI2 voltage = 3.8V.
        0x04,// default 
        0x01,// default 
        0x00,// default 
        0x08,// default
        0x01,// default
        0x00,// default
        0x06,// default
        0x01,// default
        0x00,// default
        0x00,// default
        0x20 // default
    }, 16);

    esp_lcd_panel_io_tx_param(io, 0xD1, (uint8_t []) {
        0x00,
        0x20,
        0x20,
        0x15
    }, 4);

    esp_lcd_panel_io_tx_param(io, 0xE0, (uint8_t []) {
        0x00,
        0x00,
        0x00
    }, 3);

    esp_lcd_panel_io_tx_param(io, 0xE1, (uint8_t []) {
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    }, 6);

    esp_lcd_panel_io_tx_param(io, 0xE2, (uint8_t []) {
        0x00
    }, 1);

    esp_lcd_panel_io_tx_param(io, 0x36, (uint8_t []) {
        0x00
    }, 1);

    esp_lcd_panel_io_tx_param(io, 0x3A, (uint8_t []) {
        r61529->color_mode
    }, 1);

    esp_lcd_panel_io_tx_param(io, 0x2A, (uint8_t []) {
        0x00,
        0x00,
        0x01,
        0x3F
    }, 4);

    esp_lcd_panel_io_tx_param(io, 0x2B, (uint8_t []) {
        0x00,
        0x00,
        0x01,
        0xDF
    }, 4);

    // Tearing
    esp_lcd_panel_io_tx_param(io, 0x35, (uint8_t []) {
        0x00
    }, 1);

    esp_lcd_panel_io_tx_param(io, 0x29, NULL, 0);
    ets_delay_us(1000 * 20);
    return ESP_OK;
}

static esp_err_t panel_r61529_reset(esp_lcd_panel_t *panel) {
    r61529_panel_t *r61529 = __containerof(panel, r61529_panel_t, base);

    if (r61529->reset_gpio_num >= 0) {
        ESP_LOGI(TAG, "reset level: %d", r61529->reset_level);
        gpio_set_level(r61529->reset_gpio_num, r61529->reset_level);
        ets_delay_us(1000 * 200);
        gpio_set_level(r61529->reset_gpio_num, !r61529->reset_level);
        ets_delay_us(1000 * 200);
    }

    return ESP_OK;
}

static esp_err_t panel_r61529_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data) {
    r61529_panel_t *r61529 = __containerof(panel, r61529_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = r61529->io;
    size_t len = (x_end - x_start) * (y_end - y_start) * r61529->bits_per_pixel / 8;
    // Always draw from the beginning of the display
    /*
    esp_lcd_panel_io_tx_param(io, 0x2A, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    esp_lcd_panel_io_tx_param(io, 0x2B, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);
    */
    ESP_LOGI(TAG, "fb: %p, size: 0x%X", color_data, len);
    esp_lcd_panel_io_tx_color(io, 0x2C, color_data, len);

    return ESP_OK;
}

esp_err_t lcd_new_panel_r61529(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    r61529_panel_t *r61529 = NULL;
    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    r61529 = calloc(1, sizeof(r61529_panel_t));
    ESP_GOTO_ON_FALSE(r61529, ESP_ERR_NO_MEM, err, TAG, "no mem for r61529 panel");

    if (panel_dev_config->reset_gpio_num >= 0) {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configuring GPIO for RST line failed");
    }

    switch (panel_dev_config->bits_per_pixel) {
    case 16:
        r61529->color_mode = 0x55;
        break;
    case 24:
        r61529->color_mode = 0x77;
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, err, TAG, "unsupported pixel width");
        break;
    }

    r61529->io = io;
    r61529->bits_per_pixel = panel_dev_config->bits_per_pixel;
    r61529->reset_gpio_num = panel_dev_config->reset_gpio_num;
    r61529->reset_level = panel_dev_config->flags.reset_active_high;
    // r61529->base.del = panel_st7789_del;
    r61529->base.reset = panel_r61529_reset;
    r61529->base.init = panel_r61529_init;
    r61529->base.draw_bitmap = panel_r61529_draw_bitmap;
    *ret_panel = &(r61529->base);
    ESP_LOGD(TAG, "new r61529 panel @%p", r61529);

    return ESP_OK;

err:
    if (r61529) {
        if (panel_dev_config->reset_gpio_num >= 0) {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(r61529);
    }
    return ret;
}
