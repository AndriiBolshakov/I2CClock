#pragma once

#include <stdbool.h>
#include <stdint.h>

void oled_init(uint8_t address);
void oled_show_datetime(
    int year, int month, int day,
    int hour, int minute, int second,
    bool show_colons);