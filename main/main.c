
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_ssd1306.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "font.h"
#include "i2c.h"
#include "render.h"
#include "rtc.h"



// Device addresses
#define ADDR_OLED          0x3C
#define ADDR_RTC           0x68

// Display dimensions
#define OLED_WIDTH         128
#define OLED_HEIGHT        64
#define OLED_BUFFER_SIZE   (OLED_WIDTH * OLED_HEIGHT / 8)

static i2c_master_dev_handle_t s_rtc;
static esp_lcd_panel_io_handle_t s_panel_io;
static esp_lcd_panel_handle_t s_panel;

static uint8_t s_framebuffer[OLED_BUFFER_SIZE];


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
// SSD1306 panel initialization
// -----------------------------------------------------------------------------

static void oled_init(uint8_t address)
{
    const esp_lcd_panel_io_i2c_config_t io_cfg = {
        .dev_addr = address,
        .scl_speed_hz = I2C_HZ,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6,
        .transaction_timeout_ms = I2C_TIMEOUT_MS,
    };
    i2c_master_bus_handle_t bus = i2c_bus_get_handle();

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(
        bus, &io_cfg, &s_panel_io));

    const esp_lcd_panel_ssd1306_config_t ssd1306_cfg = {
        .height = OLED_HEIGHT,
        .contrast = 128,
    };

    const esp_lcd_panel_dev_config_t panel_cfg = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
        .vendor_config = (void *)&ssd1306_cfg,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(
        s_panel_io, &panel_cfg, &s_panel));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel));
    

// Use framebuffer contents instead of forcing every pixel on.
ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(
    s_panel_io, 0xA4, NULL, 0));

// Normal (non-inverted) display mode.
ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(
    s_panel_io, 0xA6, NULL, 0));

ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_panel, true));


    ESP_LOGI(TAG, "SSD1306 initialized at address 0x%02X", address);
}



static void oled_show_datetime(
    int year, int month, int day,
    int hour, int minute, int second)
{
    static const char *months[] = {
        "???", "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
        "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
    };

    char date_text[20];
    char time_text[12];

    const char *month_name =
        (month >= 1 && month <= 12) ? months[month] : "???";

    snprintf(date_text, sizeof(date_text),
             "%02d %s %04d", day, month_name, year);

    snprintf(time_text, sizeof(time_text),
             "%02d:%02d:%02d", hour, minute, second);

    memset(s_framebuffer, 0, sizeof(s_framebuffer));

    draw_text_centered(
    s_framebuffer,
    OLED_WIDTH,
    OLED_HEIGHT,
    5,
    date_text,
    1);

draw_text_centered(
    s_framebuffer,
    OLED_WIDTH,
    OLED_HEIGHT,
    23,
    time_text,
    2);

draw_text_centered(
    s_framebuffer,
    OLED_WIDTH,
    OLED_HEIGHT,
    51,
    "DS1307",
    1);

    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(
        s_panel,
        0, 0,
        OLED_WIDTH, OLED_HEIGHT,
        s_framebuffer));
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

                if (s_panel != NULL) {
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
