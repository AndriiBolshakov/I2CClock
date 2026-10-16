idf_component_register(
    SRCS "main.c" "font.h" "font.c" "i2c.c" "oled.c" "render.c" "rtc.c" "rtc.h"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio
    REQUIRES esp_timer
    REQUIRES esp_adc
    REQUIRES esp_driver_ledc
    REQUIRES esp_driver_i2c
    REQUIRES esp_lcd driver
)