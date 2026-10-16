
#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c.h"
#include "oled.h"
#include "rtc.h"


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


// -----------------------------------------------------------------------------
// Application
// -----------------------------------------------------------------------------

void app_main(void)
{
    i2c_init();

    bool has_rtc = device_found(ADDR_RTC);
    bool has_oled = device_found(ADDR_OLED);

    ESP_LOGI(TAG, "DS1307: %s", has_rtc ? "found" : "not found");
    ESP_LOGI(TAG, "OLED: %s", has_oled ? "found" : "not found");

    if (has_rtc) {
        rtc_attach();
        rtc_clear_clock_halt(s_rtc);
    }

    if (has_oled) {
        oled_init(ADDR_OLED);
    } else {
        ESP_LOGE(TAG, "SSD1306 not found; check wiring/address");
    }

    if (!has_rtc) {
        ESP_LOGE(TAG, "DS1307 not found; check wiring/address");
    }

    while (1) {
        if (has_rtc) {
            int year, month, day, hour, minute, second;

            esp_err_t err = rtc_get_datetime(
                &year, &month, &day,
                &hour, &minute, &second, s_rtc);

            if (err == ESP_OK) {
                ESP_LOGI(TAG, "%04d-%02d-%02d %02d:%02d:%02d",
                         year, month, day,
                         hour, minute, second);

                if (has_oled) {
                    oled_show_datetime(
                        year, month, day,
                        hour, minute, second);
                }
            } else {
                ESP_LOGE(TAG, "RTC read failed: %s",
                         esp_err_to_name(err));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
