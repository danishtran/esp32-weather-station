#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

esp_err_t lcd1602_init(i2c_master_bus_handle_t i2c_bus);

esp_err_t lcd1602_print(const char *line1, const char *line2);