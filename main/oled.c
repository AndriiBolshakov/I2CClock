#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_ssd1306.h"
#include "i2c.h"
#include "oled.h"
#include "render.h"

#define OLED_WIDTH       128
#define OLED_HEIGHT      64
#define OLED_BUFFER_SIZE (OLED_WIDTH * OLED_HEIGHT / 8)

static esp_lcd_panel_io_handle_t s_panel_io;
static esp_lcd_panel_handle_t s_panel;
static uint8_t s_framebuffer[OLED_BUFFER_SIZE];

void oled_init(uint8_t address)
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

    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(
        s_panel_io, 0xA4, NULL, 0));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(
        s_panel_io, 0xA6, NULL, 0));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_panel, true));

    ESP_LOGI(TAG, "SSD1306 initialized at address 0x%02X", address);
}

void oled_show_datetime(
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
        s_framebuffer, OLED_WIDTH, OLED_HEIGHT, 5, date_text, 1);
    draw_text_centered(
        s_framebuffer, OLED_WIDTH, OLED_HEIGHT, 23, "HI:EMRE", 2);
    draw_text_centered(
        s_framebuffer, OLED_WIDTH, OLED_HEIGHT, 51, "DS1307", 1);

    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(
        s_panel,
        0, 0,
        OLED_WIDTH, OLED_HEIGHT,
        s_framebuffer));
}