#include "config_nvs.h"

#include <stdio.h>
#include <string.h>

#include "nvs_flash.h"
#include "nvs.h"

static const char *NVS_NAMESPACE = "app_config";

esp_err_t config_nvs_init(void){
  esp_err_t init_result = nvs_flash_init();

  if (init_result == ESP_ERR_NVS_NO_FREE_PAGES || 
      init_result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    init_result = nvs_flash_init();
  }

    return init_result;
}

esp_err_t config_set_wifi(const char *ssid, const char *password) {
  nvs_handle_t nvs_handle;
  esp_err_t open_result = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);

  if (open_result != ESP_OK) return open_result;

  esp_err_t ssid_write_result = nvs_set_str(nvs_handle, "wifi_ssid", ssid);
  if (ssid_write_result != ESP_OK) {
    nvs_close(nvs_handle);
    return ssid_write_result;
  }

  esp_err_t password_write_result = nvs_set_str(nvs_handle, "wifi_password", password);
  if (password_write_result != ESP_OK) {
    nvs_close(nvs_handle);
    return password_write_result;
  }

  esp_err_t commit_result = nvs_commit(nvs_handle);
  nvs_close(nvs_handle);

  return commit_result;
}

esp_err_t config_get_wifi(char *ssid_out, size_t ssid_size, 
                          char *password_out, size_t password_size) {
  nvs_handle_t nvs_handle;
  esp_err_t open_result = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);

  if (open_result != ESP_OK) return open_result;

  esp_err_t ssid_read_result = nvs_get_str(nvs_handle, "wifi_ssid", ssid_out, &ssid_size);
  if (ssid_read_result != ESP_OK) {
    nvs_close(nvs_handle);
    return ssid_read_result;
  }

  esp_err_t password_read_result = nvs_get_str(nvs_handle, "wifi_password", password_out, &password_size);

  nvs_close(nvs_handle);
  return password_read_result;
}

esp_err_t config_set_aws_url(const char *url){
  nvs_handle_t nvs_handle;
  esp_err_t open_result = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);

  if (open_result != ESP_OK) return open_result;

  esp_err_t write_result = nvs_set_str(nvs_handle, "aws_url", url);
  if (write_result != ESP_OK) {
    nvs_close(nvs_handle);
    return write_result;
  }

  esp_err_t commit_result = nvs_commit(nvs_handle);
  nvs_close(nvs_handle);

  return commit_result;
}

esp_err_t config_get_aws_url(char *url_out, size_t url_size) {
  nvs_handle_t nvs_handle;
  esp_err_t open_result = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);

  if (open_result != ESP_OK) return open_result;

  esp_err_t read_result = nvs_get_str(nvs_handle, "aws_url", url_out, &url_size);

  nvs_close(nvs_handle);
  return read_result;
}

esp_err_t config_set_device_id(const char *id) {
  nvs_handle_t nvs_handle;
  esp_err_t open_result = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);

  if (open_result != ESP_OK) return open_result;

  esp_err_t write_result = nvs_set_str(nvs_handle, "device_id", id);
  if (write_result != ESP_OK) {
    nvs_close(nvs_handle);
    return write_result;
  }

  esp_err_t commit_result = nvs_commit(nvs_handle);
  nvs_close(nvs_handle);

  return commit_result;
}

esp_err_t config_get_device_id(char *out_id, size_t max_len) {
  nvs_handle_t nvs_handle;
  esp_err_t open_result = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);

  if (open_result != ESP_OK) return open_result;

  esp_err_t read_result = nvs_get_str(nvs_handle, "device_id", out_id, &max_len);

  nvs_close(nvs_handle);
  return read_result;
}
