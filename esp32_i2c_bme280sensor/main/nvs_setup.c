#include <stdio.h>
#include "nvs_flash.h"
#include "config_nvs.h"

void app_main(void) {
  if (config_nvs_init() != ESP_OK) {
    printf("Failed to init NVS\n");
    return;
  }

  const char *ssid = "your-wifi-name";
  const char *pass = "your-wifi-password";
  const char *aws_url = "https://<your-gateway>.amazonaws.com/sensor-data";
  const char *device_id = "<your-choice>-room";

  printf("Writing WiFi and AWS credentials to NVS...\n");

  if (config_set_wifi(ssid, pass) == ESP_OK) {
    printf("Saved WiFi SSID + password\n");
  } else {
    printf("Failed to save WiFi\n");
  }

  if (config_set_aws_url(aws_url) == ESP_OK) {
    printf("Saved AWS URL\n");
  } else {
    printf("Failed to save AWS URL\n");
  }

  if (config_set_device_id(device_id) == ESP_OK) {
    printf("Saved device ID\n");
  } else {
    printf("Failed to save device ID\n");
  }

  printf("DONE. Re-flash the main firmware now.\n");
}
