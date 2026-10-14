
#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "config.h"

// -----------------------------------------------------------------------------
// DS1307 RTC
// -----------------------------------------------------------------------------

uint8_t bcd_to_int(uint8_t value)
{
    return (uint8_t)(((value >> 4) * 10) + (value & 0x0F));
}

esp_err_t rtc_read_registers(uint8_t *data, size_t length, i2c_master_dev_handle_t s_rtc)
{
    const uint8_t first_register = 0x00;

    return i2c_master_transmit_receive(
        s_rtc,
        &first_register,
        1,
        data,
        length,
        I2C_TIMEOUT_MS);
}

void rtc_clear_clock_halt(i2c_master_dev_handle_t s_rtc)
{
    uint8_t seconds;

        if (rtc_read_registers(&seconds, 1, s_rtc) != ESP_OK) {
        ESP_LOGE(TAG, "Could not read RTC seconds register");
        return;
    }

    // DS1307 CH bit: bit 7 of the seconds register.
    if ((seconds & 0x80) != 0) {
        const uint8_t packet[2] = {
            0x00,
            (uint8_t)(seconds & 0x7F)
        };

        ESP_ERROR_CHECK(i2c_master_transmit(
            s_rtc, packet, sizeof(packet), I2C_TIMEOUT_MS));

        ESP_LOGW(TAG, "RTC clock-halt bit cleared");
    }
}

esp_err_t rtc_get_datetime(
    int *year,
    int *month,
    int *day,
    int *hour,
    int *minute,
    int *second,
   i2c_master_dev_handle_t s_rtc)
{
    uint8_t r[7];

    esp_err_t err = rtc_read_registers(r, sizeof(r), s_rtc);
    if (err != ESP_OK) {
        return err;
    }

    *second = bcd_to_int(r[0] & 0x7F);
    *minute = bcd_to_int(r[1] & 0x7F);

    // DS1307 supports both 24-hour and 12-hour modes.
    if ((r[2] & 0x40) != 0) {
        int h = bcd_to_int(r[2] & 0x1F);
        bool pm = (r[2] & 0x20) != 0;

        if (pm && h != 12) {
            h += 12;
        } else if (!pm && h == 12) {
            h = 0;
        }

        *hour = h;
    } else {
        *hour = bcd_to_int(r[2] & 0x3F);
    }

    *day = bcd_to_int(r[4] & 0x3F);
    *month = bcd_to_int(r[5] & 0x1F);
    *year = 2000 + bcd_to_int(r[6]);

    return ESP_OK;
}