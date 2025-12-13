#pragma once

#include <stddef.h>
#include "esp_err.h"


esp_err_t config_nvs_init(void);

esp_err_t config_set_wifi(const char *ssid, const char *password);
esp_err_t config_get_wifi(char *ssid_out, size_t ssid_size, char *password_out, size_t password_size);

esp_err_t config_set_aws_url(const char *url);
esp_err_t config_get_aws_url(char *url_out, size_t url_size);

esp_err_t config_set_device_id(const char *id);
esp_err_t config_get_device_id(char *out_id, size_t max_len);
