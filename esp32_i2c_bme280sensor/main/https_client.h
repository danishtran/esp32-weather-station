#pragma once

void https_set_endpoint(const char *url);
void https_send_temperature(double celsius, double fahrenheit, const char *device_id);