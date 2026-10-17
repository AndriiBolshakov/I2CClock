#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    bool show_colons;
    float temperature_c;
    float pressure_hpa;
    float humidity_percent;
    bool has_bme280_reading;
} oled_datetime_t;

void oled_init(uint8_t address);
void oled_show_datetime(oled_datetime_t datetime);