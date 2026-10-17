#include "driver/i2c_master.h"
#include "esp_err.h"


void rtc_clear_clock_halt(i2c_master_dev_handle_t s_rtc);
esp_err_t rtc_set_datetime(
    int year,
    int month,
    int day,
    int hour,
    int minute,
    int second,
    i2c_master_dev_handle_t s_rtc);
esp_err_t rtc_get_datetime(
    int *year,
    int *month,
    int *day,
    int *hour,
    int *minute,
    int *second,
   i2c_master_dev_handle_t s_rtc);