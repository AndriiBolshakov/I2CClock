
#pragma once
// I2C configuration
#define I2C_PORT           I2C_NUM_0
#define I2C_SDA_GPIO       GPIO_NUM_8
#define I2C_SCL_GPIO       GPIO_NUM_9
#define I2C_HZ             100000
#define I2C_TIMEOUT_MS     200

// Device addresses
#define ADDR_OLED          0x3C
#define ADDR_OLED_ALT      0x3D
#define ADDR_RTC           0x68

static const char *TAG = "rtc_clock";
