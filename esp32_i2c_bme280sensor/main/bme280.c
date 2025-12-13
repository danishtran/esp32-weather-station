#include "bme280.h"
#include <stdio.h>
#include "driver/i2c_master.h"

#define BME280_ADDR        0x76
#define REG_ID             0xD0
#define REG_CALIB_START    0x88
#define REG_TEMP_MSB       0xFA
#define REG_CTRL_HUM       0xF2
#define REG_CTRL_MEAS      0xF4
#define REG_CONFIG         0xF5

static i2c_master_dev_handle_t bme280_device;

static uint16_t calibration_temperature_1;
static int16_t  calibration_temperature_2;
static int16_t  calibration_temperature_3;

static int32_t temperature_fine_value;

static esp_err_t bme280_read_register(uint8_t reg, uint8_t *data, size_t len) {
  esp_err_t status = i2c_master_transmit(bme280_device, &reg, 1, -1);
  if (status != ESP_OK) return status;
  return i2c_master_receive(bme280_device, data, len, -1);
}

static esp_err_t bme280_write_register(uint8_t reg, uint8_t value) {
  uint8_t buffer[2] = { reg, value };
  return i2c_master_transmit(bme280_device, buffer, sizeof(buffer), -1);
}

void bme280_init(i2c_master_bus_handle_t bus) {
  i2c_device_config_t config = {
    .device_address = BME280_ADDR,
    .scl_speed_hz = 100000,
    .dev_addr_length = I2C_ADDR_BIT_LEN_7
  };

  i2c_master_bus_add_device(bus, &config, &bme280_device);

  uint8_t buffer[6];
  bme280_read_register(REG_CALIB_START, buffer, 6);

  calibration_temperature_1 = (buffer[1] << 8) | buffer[0];
  calibration_temperature_2 = (buffer[3] << 8) | buffer[2];
  calibration_temperature_3 = (buffer[5] << 8) | buffer[4];

  bme280_write_register(REG_CTRL_HUM, 0x01);
  bme280_write_register(REG_CTRL_MEAS,
                        (1 << 5) |
                        (1 << 2) |
                        0x03);
}

static double convert_raw_temperature_to_celsius(int raw) {

  double var1 = (((double)raw) / 16384.0 - ((double)calibration_temperature_1) / 1024.0)
                 * ((double)calibration_temperature_2);

  double var2 = ((((double)raw) / 131072.0 - ((double)calibration_temperature_1) / 8192.0) *
                 (((double)raw) / 131072.0 - ((double)calibration_temperature_1) / 8192.0))
                 * ((double)calibration_temperature_3);

  temperature_fine_value = (int32_t)(var1 + var2);

  return (var1 + var2) / 5120.0;
}

double bme280_read_temperature_celsius(void) {
  uint8_t data[3];
  bme280_read_register(REG_TEMP_MSB, data, 3);

  int raw = (data[0] << 12) |
            (data[1] << 4)  |
            (data[2] >> 4);

  return convert_raw_temperature_to_celsius(raw);
}
