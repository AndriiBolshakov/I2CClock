
#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bme280.h"
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
static void run_clock_mode(bool has_rtc, bool has_oled, bool has_bme280)
{
    int last_rendered_second = -1;
    int64_t last_second_render_us = 0;
    int64_t last_bme_read_us = 0;
    bool colons_visible = false;
    bool has_bme280_reading = false;
    float temperature_c = 0.0f;
    float pressure_hpa = 0.0f;
    float humidity_percent = 0.0f;

    while (1) {
        int64_t now_us = esp_timer_get_time();
        if (has_bme280 &&
            now_us - last_bme_read_us >= 1000000) {
            float measured_temperature_c;
            float measured_pressure_pa;
            float measured_humidity_percent;

            esp_err_t err = bme280_read(
                &measured_temperature_c, &measured_pressure_pa,
                &measured_humidity_percent);
            if (err == ESP_OK) {
                temperature_c = measured_temperature_c;
                pressure_hpa = measured_pressure_pa / 100.0f;
                humidity_percent = measured_humidity_percent;
                has_bme280_reading = true;
                ESP_LOGI(TAG,
                         "BME280: temperature=%.2f C, pressure=%.2f hPa, humidity=%.2f %%",
                         temperature_c, pressure_hpa,
                         humidity_percent);
            } else {
                ESP_LOGE(TAG, "BME280 read failed: %s",
                         esp_err_to_name(err));
            }
            last_bme_read_us = now_us;
        }

        if (has_rtc) {
            int year, month, day, hour, minute, second;

            esp_err_t err = rtc_get_datetime(
                &year, &month, &day,
                &hour, &minute, &second, s_rtc);

            if (err == ESP_OK) {
                oled_datetime_t datetime = {
                    .year = year,
                    .month = month,
                    .day = day,
                    .hour = hour,
                    .minute = minute,
                    .second = second,
                    .show_colons = colons_visible,
                    .temperature_c = temperature_c,
                    .pressure_hpa = pressure_hpa,
                    .humidity_percent = humidity_percent,
                    .has_bme280_reading = has_bme280_reading,
                };

                if (second != last_rendered_second) {
                    ESP_LOGI(TAG, "%04d-%02d-%02d %02d:%02d:%02d",
                             year, month, day,
                             hour, minute, second);

                    colons_visible = true;
                    datetime.show_colons = colons_visible;
                    last_second_render_us = esp_timer_get_time();
                    if (has_oled) {
                        oled_show_datetime(datetime);
                    }

                    last_rendered_second = second;
                } else if (colons_visible &&
                           esp_timer_get_time() - last_second_render_us >= 500000) {
                    colons_visible = false;
                    datetime.show_colons = colons_visible;
                    if (has_oled) {
                        oled_show_datetime(datetime);
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

    uint8_t bme280_address = ADDR_BME280;
    bool has_bme280 = device_found(ADDR_BME280);
    if (!has_bme280) {
        bme280_address = ADDR_BME280_ALT;
        has_bme280 = device_found(ADDR_BME280_ALT);
    }
    if (has_bme280) {
        esp_err_t err = bme280_init(i2c_bus_get_handle(), bme280_address);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "BME280 initialization failed: %s",
                     esp_err_to_name(err));
            has_bme280 = false;
        }
    }
    ESP_LOGI(TAG, "BME280: %s", has_bme280 ? "initialized" : "not available");

    run_clock_mode(has_rtc, has_oled, has_bme280);
#endif
}
