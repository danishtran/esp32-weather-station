#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "bme280.h"

#define SDA_PIN 21
#define SCL_PIN 22

void app_main(void) {

  i2c_master_bus_handle_t bus;

  i2c_master_bus_config_t bus_config = {
    .i2c_port = 0,
    .sda_io_num = SDA_PIN,
    .scl_io_num = SCL_PIN,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true
  };

  if (i2c_new_master_bus(&bus_config, &bus) != ESP_OK) {
    printf("I2C bus init failed\n");
    return;
  }

  bme280_init(bus);

  printf("BME280 Test Starting...\n");

  while (1) {

    double temp_c = bme280_read_temperature_celsius();

    printf("Temperature: %.2f C\n", temp_c);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
