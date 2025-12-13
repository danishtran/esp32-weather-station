#pragma once
#include "driver/i2c_master.h"

void bme280_init(i2c_master_bus_handle_t bus);
double bme280_read_temperature_celsius(void);
