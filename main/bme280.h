#pragma once

#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_err.h"

esp_err_t bme280_init(i2c_master_bus_handle_t bus, uint8_t address);
esp_err_t bme280_read(float *temperature_c, float *pressure_pa,
                      float *humidity_percent);
