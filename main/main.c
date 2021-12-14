#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "main.h"

static const char *TAG = "main";
esp_lcd_panel_handle_t panel_handle;

void setupPin(uint8_t pin, uint8_t mode) {
    gpio_config_t conf = {
        .pin_bit_mask = (1ULL<<pin),            /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
        .mode = GPIO_MODE_OUTPUT,               /*!< GPIO mode: set input/output mode                     */
        .pull_up_en = GPIO_PULLUP_DISABLE,      /*!< GPIO pull-up                                         */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,  /*!< GPIO pull-down                                       */
        .intr_type = GPIO_INTR_DISABLE          /*!< GPIO interrupt type                                  */
    };
    gpio_config(&conf);
}

void run_tests(ar_disp_drv_t *disp_drv, int flush_cache) {
    uint8_t colors[4] = {0x3A, 0xA3, 0x00, 0xFF};
    for (int i = 0; i < sizeof(colors)/sizeof(uint8_t); i++) {
        memset(disp_drv->fb, colors[i], disp_drv->fb_size);
        if (flush_cache) {
            ESP_LOGI(TAG, "Flushing cache");
            Cache_WriteBack_Addr((uint32_t)disp_drv->fb, disp_drv->fb_size);
        }

        // A delay just in case, to make sure cache is flushed
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Blitting framebuffer, color: 0x%X", colors[i]);
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 320, 64, disp_drv->fb);

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    static ar_disp_drv_t disp_drv = {
        .fb_size = 320*64*2,
        .in_tx = 0 // whether transmitting
    };
    panel_handle = NULL;

    ESP_LOGI(TAG, "Starting");
    ESP_ERROR_CHECK(lcd_init_all(&disp_drv, &panel_handle));

    ESP_LOGI(TAG, "disp_drv: %p", &disp_drv);

    // Internal
    ESP_LOGI(TAG, "Allocating framebuffer (internal)");
    disp_drv.fb = heap_caps_malloc(disp_drv.fb_size, MALLOC_CAP_8BIT|MALLOC_CAP_INTERNAL);
    assert(disp_drv.fb);
    run_tests(&disp_drv, 0);

    free(disp_drv.fb);

    // heaps_caps_aligned_alloc(MALLOC_CAP_8BIT|MALLOC_CAP_SPIRAM)
    ESP_LOGI(TAG, "Allocating framebuffer (PSRAM, 16-byte aligned)");
    disp_drv.fb = heap_caps_aligned_alloc(16, disp_drv.fb_size, MALLOC_CAP_8BIT|MALLOC_CAP_SPIRAM);
    assert(disp_drv.fb);
    run_tests(&disp_drv, 1);

    free(disp_drv.fb);
}
