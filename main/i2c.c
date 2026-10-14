#include "i2c.h"
#include "esp_log.h"


static i2c_master_bus_handle_t s_bus = NULL;

void i2c_init(void)
{
    const i2c_master_bus_config_t cfg = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA_GPIO,
        .scl_io_num = I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &s_bus));

    ESP_LOGI(TAG, "I2C initialized: SDA=%d, SCL=%d, %d Hz",
             (int)I2C_SDA_GPIO,
             (int)I2C_SCL_GPIO,
             I2C_HZ);
}

bool device_found(uint8_t addr)
{
    esp_err_t err = i2c_master_probe(s_bus, addr, I2C_TIMEOUT_MS);
    return err == ESP_OK;
}


i2c_master_bus_handle_t i2c_bus_get_handle(void)
{
    return s_bus;
}