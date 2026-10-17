#include <stdint.h>

#include "bme280.h"
#include "config.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BME280_CHIP_ID       0x60
#define BME280_REG_ID        0xD0
#define BME280_REG_RESET     0xE0
#define BME280_REG_CTRL_HUM  0xF2
#define BME280_REG_CONFIG    0xF5
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CALIB_TP  0x88
#define BME280_REG_CALIB_H1  0xA1
#define BME280_REG_CALIB_H2  0xE1
#define BME280_REG_DATA      0xF7

typedef struct {
    uint16_t t1;
    int16_t t2;
    int16_t t3;
    uint16_t p1;
    int16_t p2;
    int16_t p3;
    int16_t p4;
    int16_t p5;
    int16_t p6;
    int16_t p7;
    int16_t p8;
    int16_t p9;
    uint8_t h1;
    int16_t h2;
    uint8_t h3;
    int16_t h4;
    int16_t h5;
    int8_t h6;
} bme280_calibration_t;

static i2c_master_dev_handle_t s_device;
static bme280_calibration_t s_calibration;
static double s_t_fine;

static uint16_t read_u16_le(const uint8_t *data)
{
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static int16_t read_s16_le(const uint8_t *data)
{
    return (int16_t)read_u16_le(data);
}

static esp_err_t read_registers(uint8_t reg, uint8_t *data, size_t length)
{
    return i2c_master_transmit_receive(
        s_device, &reg, 1, data, length, I2C_TIMEOUT_MS);
}

static esp_err_t write_register(uint8_t reg, uint8_t value)
{
    const uint8_t packet[] = {reg, value};
    return i2c_master_transmit(
        s_device, packet, sizeof(packet), I2C_TIMEOUT_MS);
}

static esp_err_t read_calibration(void)
{
    uint8_t tp[26];
    uint8_t h1;
    uint8_t humidity[7];

    esp_err_t err = read_registers(BME280_REG_CALIB_TP, tp, sizeof(tp));
    if (err != ESP_OK) {
        return err;
    }
    err = read_registers(BME280_REG_CALIB_H1, &h1, sizeof(h1));
    if (err != ESP_OK) {
        return err;
    }
    err = read_registers(BME280_REG_CALIB_H2, humidity, sizeof(humidity));
    if (err != ESP_OK) {
        return err;
    }

    s_calibration.t1 = read_u16_le(&tp[0]);
    s_calibration.t2 = read_s16_le(&tp[2]);
    s_calibration.t3 = read_s16_le(&tp[4]);
    s_calibration.p1 = read_u16_le(&tp[6]);
    s_calibration.p2 = read_s16_le(&tp[8]);
    s_calibration.p3 = read_s16_le(&tp[10]);
    s_calibration.p4 = read_s16_le(&tp[12]);
    s_calibration.p5 = read_s16_le(&tp[14]);
    s_calibration.p6 = read_s16_le(&tp[16]);
    s_calibration.p7 = read_s16_le(&tp[18]);
    s_calibration.p8 = read_s16_le(&tp[20]);
    s_calibration.p9 = read_s16_le(&tp[22]);
    s_calibration.h1 = h1;
    s_calibration.h2 = read_s16_le(&humidity[0]);
    s_calibration.h3 = humidity[2];

    uint16_t h4 = ((uint16_t)humidity[3] << 4) | (humidity[4] & 0x0F);
    uint16_t h5 = ((uint16_t)humidity[5] << 4) | (humidity[4] >> 4);
    s_calibration.h4 = (h4 & 0x0800) ? (int16_t)((int32_t)h4 - 4096) : (int16_t)h4;
    s_calibration.h5 = (h5 & 0x0800) ? (int16_t)((int32_t)h5 - 4096) : (int16_t)h5;
    s_calibration.h6 = (int8_t)humidity[6];

    return s_calibration.p1 == 0 ? ESP_ERR_INVALID_RESPONSE : ESP_OK;
}

esp_err_t bme280_init(i2c_master_bus_handle_t bus, uint8_t address)
{
    if (bus == NULL || (address != ADDR_BME280 && address != ADDR_BME280_ALT)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_device != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    const i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = I2C_HZ,
    };
    esp_err_t err = i2c_master_bus_add_device(bus, &cfg, &s_device);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t chip_id;
    err = read_registers(BME280_REG_ID, &chip_id, sizeof(chip_id));
    if (err == ESP_OK && chip_id != BME280_CHIP_ID) {
        err = ESP_ERR_NOT_SUPPORTED;
    }
    if (err == ESP_OK) {
        err = write_register(BME280_REG_RESET, 0xB6);
    }
    if (err == ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(10));
        err = read_calibration();
    }
    if (err == ESP_OK) {
        err = write_register(BME280_REG_CTRL_HUM, 0x01);
    }
    if (err == ESP_OK) {
        err = write_register(BME280_REG_CONFIG, 0xA0);
    }
    if (err == ESP_OK) {
        err = write_register(BME280_REG_CTRL_MEAS, 0x27);
    }
    if (err != ESP_OK) {
        i2c_master_bus_rm_device(s_device);
        s_device = NULL;
    } else {
        ESP_LOGD(TAG, "Initialized at address 0x%02X", address);
    }
    return err;
}

esp_err_t bme280_read(float *temperature_c, float *pressure_pa,
                      float *humidity_percent)
{
    if (s_device == NULL || temperature_c == NULL ||
        pressure_pa == NULL || humidity_percent == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data[8];
    esp_err_t err = read_registers(BME280_REG_DATA, data, sizeof(data));
    if (err != ESP_OK) {
        return err;
    }

    int32_t adc_p = ((int32_t)data[0] << 12) |
                    ((int32_t)data[1] << 4) | (data[2] >> 4);
    int32_t adc_t = ((int32_t)data[3] << 12) |
                    ((int32_t)data[4] << 4) | (data[5] >> 4);
    int32_t adc_h = ((int32_t)data[6] << 8) | data[7];

    double var1 = ((double)adc_t / 16384.0 -
                   (double)s_calibration.t1 / 1024.0) * s_calibration.t2;
    double var2 = ((double)adc_t / 131072.0 -
                   (double)s_calibration.t1 / 8192.0);
    var2 = var2 * var2 * s_calibration.t3;
    s_t_fine = var1 + var2;
    *temperature_c = (float)(s_t_fine / 5120.0);

    var1 = s_t_fine / 2.0 - 64000.0;
    var2 = var1 * var1 * s_calibration.p6 / 32768.0;
    var2 += var1 * s_calibration.p5 * 2.0;
    var2 = var2 / 4.0 + s_calibration.p4 * 65536.0;
    var1 = (s_calibration.p3 * var1 * var1 / 524288.0 +
            s_calibration.p2 * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * s_calibration.p1;
    if (var1 == 0.0) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    double pressure = 1048576.0 - adc_p;
    pressure = (pressure - var2 / 4096.0) * 6250.0 / var1;
    var1 = s_calibration.p9 * pressure * pressure / 2147483648.0;
    var2 = pressure * s_calibration.p8 / 32768.0;
    pressure += (var1 + var2 + s_calibration.p7) / 16.0;
    *pressure_pa = (float)pressure;

    double humidity = s_t_fine - 76800.0;
    humidity = (adc_h -
                (s_calibration.h4 * 64.0 +
                 s_calibration.h5 / 16384.0 * humidity)) *
               (s_calibration.h2 / 65536.0 *
                (1.0 + s_calibration.h6 / 67108864.0 * humidity *
                 (1.0 + s_calibration.h3 / 67108864.0 * humidity)));
    humidity *= 1.0 - s_calibration.h1 * humidity / 524288.0;
    if (humidity < 0.0) {
        humidity = 0.0;
    } else if (humidity > 100.0) {
        humidity = 100.0;
    }
    *humidity_percent = (float)humidity;

    return ESP_OK;
}
