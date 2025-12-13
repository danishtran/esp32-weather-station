#include "wifi.h"
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *WIFI_TAG = "WIFI";
static bool wifi_connected = false;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data) {
  if (event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_id == WIFI_EVENT_STA_DISCONNECTED){
    wifi_connected = false;
    esp_wifi_connect();
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    wifi_connected = true;
  }
}

esp_err_t wifi_init_and_connect(const char *ssid, const char *password) {
  esp_err_t status = nvs_flash_init();

  if (status != ESP_OK) return status;

  esp_netif_init();
  esp_event_loop_create_default();
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t wifi_init = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_init);

  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

  wifi_config_t cfg = { 0 };
  strcpy((char*) cfg.sta.ssid, ssid);
  strcpy((char*) cfg.sta.password, password);

  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(WIFI_IF_STA, &cfg);
  esp_wifi_start();

  // Wait loop
  int retries = 0;
  while(!wifi_connected && retries < 20) {
    vTaskDelay(pdMS_TO_TICKS(500));
    retries++;
  }

  if (!wifi_connected) {
    ESP_LOGE(WIFI_TAG, "WiFi connection failed");
    return ESP_FAIL;
  }

  ESP_LOGI(WIFI_TAG, "WiFi connected!");
  return ESP_OK;
}
