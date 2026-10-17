
#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c.h"
#include "oled.h"
#include "rtc.h"

#define APP_MODE_CLOCK   0
#define APP_MODE_SET_RTC 1
#define APP_MODE         APP_MODE_CLOCK

#if APP_MODE != APP_MODE_CLOCK && APP_MODE != APP_MODE_SET_RTC
#error "APP_MODE must be APP_MODE_CLOCK or APP_MODE_SET_RTC"
#endif


static i2c_master_dev_handle_t s_rtc;

static void rtc_attach(void)
{
    const i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADDR_RTC,
        .scl_speed_hz = I2C_HZ,
    };
    i2c_master_bus_handle_t bus = i2c_bus_get_handle();

    ESP_ERROR_CHECK(i2c_master_bus_add_device(
        bus, &cfg, &s_rtc));
}

#if APP_MODE == APP_MODE_CLOCK
static void run_clock_mode(bool has_rtc, bool has_oled)
{
    int last_rendered_second = -1;
    int64_t last_second_render_us = 0;
    bool colons_visible = false;

    while (1) {
        if (has_rtc) {
            int year, month, day, hour, minute, second;

            esp_err_t err = rtc_get_datetime(
                &year, &month, &day,
                &hour, &minute, &second, s_rtc);

            if (err == ESP_OK) {
                if (second != last_rendered_second) {
                    ESP_LOGI(TAG, "%04d-%02d-%02d %02d:%02d:%02d",
                             year, month, day,
                             hour, minute, second);

                    colons_visible = true;
                    last_second_render_us = esp_timer_get_time();
                    if (has_oled) {
                        oled_show_datetime(
                            year, month, day,
                            hour, minute, second,
                            colons_visible);
                    }

                    last_rendered_second = second;
                } else if (colons_visible &&
                           esp_timer_get_time() - last_second_render_us >= 500000) {
                    colons_visible = false;
                    if (has_oled) {
                        oled_show_datetime(
                            year, month, day,
                            hour, minute, second,
                            colons_visible);
                    }
                }
            } else {
                ESP_LOGE(TAG, "RTC read failed: %s",
                         esp_err_to_name(err));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif

// -----------------------------------------------------------------------------
// Application
// -----------------------------------------------------------------------------

void app_main(void)
{
    i2c_init();

    bool has_rtc = device_found(ADDR_RTC);
    ESP_LOGI(TAG, "DS1307: %s", has_rtc ? "found" : "not found");

    if (has_rtc) {
        rtc_attach();
#if APP_MODE == APP_MODE_CLOCK
        rtc_clear_clock_halt(s_rtc);
#endif
    }

#if APP_MODE == APP_MODE_SET_RTC
    if (!has_rtc) {
        ESP_LOGE(TAG, "DS1307 not found; check wiring/address");
        return;
    }

    esp_err_t err = rtc_set_datetime(
        2026, 10, 17,
        8, 52, 0,
        s_rtc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "RTC date/time update failed: %s",
                 esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "RTC set to 2026-10-17 08:47:00");
#else
    bool has_oled = device_found(ADDR_OLED);
    ESP_LOGI(TAG, "OLED: %s", has_oled ? "found" : "not found");

    if (has_oled) {
        oled_init(ADDR_OLED);
    } else {
        ESP_LOGE(TAG, "SSD1306 not found; check wiring/address");
    }

    if (!has_rtc) {
        ESP_LOGE(TAG, "DS1307 not found; check wiring/address");
    }

    run_clock_mode(has_rtc, has_oled);
#endif
}
