
#pragma once
#include "driver/i2c_master.h"
#include "config.h"

// -----------------------------------------------------------------------------
// I2C
// -----------------------------------------------------------------------------

//static i2c_master_bus_handle_t s_bus;
//static i2c_master_dev_handle_t s_rtc;

void i2c_init(void);
bool device_found(uint8_t addr);
//void rtc_attach(void);
i2c_master_bus_handle_t i2c_bus_get_handle(void);
