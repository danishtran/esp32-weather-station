#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/i2c_master.h"
#include "esp_http_server.h"

#include "wifi.h"
#include "https_client.h"
#include "bme280.h"
#include "conversions.h"
#include "config_nvs.h"
#include "lcd1602.h"

#define SDA_PIN 21
#define SCL_PIN 22

// 10 minutes in ms
#define SEND_INTERVAL_MS  (10 * 60 * 1000)

// 5 seconds for testing
// #define SEND_INTERVAL_MS  (5000)

static QueueHandle_t temperature_queue;
static char device_id_global[32];

static double latest_temperature_c = 0.0;
static double latest_temperature_f = 0.0;

void sensor_task(void *pvParameters) {
  (void) pvParameters;

  while (1) {
    double temperature_c = bme280_read_temperature_celsius();
    double temperature_f = celsius_to_fahrenheit(temperature_c);

    xQueueSend(temperature_queue, &temperature_c, portMAX_DELAY);

    printf("[SensorTask] Queued temperature %.2f°C\n", temperature_c);

    char line1[17];
    char line2[17];

    snprintf(line1, sizeof(line1), "%.1fC | %.1fF", temperature_c, temperature_f);
    snprintf(line2, sizeof(line2), "%.16s", device_id_global);

    lcd1602_print(line1, line2);

    vTaskDelay(pdMS_TO_TICKS(SEND_INTERVAL_MS));
  }
}

void network_task(void *pvParameters) {
  const char *device_id = (const char *)pvParameters;
  double temperature_c;

  while (1) {
    if (xQueueReceive(temperature_queue, &temperature_c, portMAX_DELAY)) {
      latest_temperature_c = temperature_c;
      latest_temperature_f = celsius_to_fahrenheit(temperature_c);

      printf("[NetworkTask] Sending temp: %.2f°C / %.2f°F (device: %s)\n",
             temperature_c, latest_temperature_f,
             device_id ? device_id : "unknown-device");

      https_send_temperature(temperature_c,
                             latest_temperature_f,
                             device_id);
    }
  }
}

static esp_err_t root_get_handler(httpd_req_t *req) {
  char response[512];

  snprintf(response, sizeof(response),
    "<html>"
    "<head><title>ESP32 Sensor</title></head>"
    "<body>"
    "<h1>Device ID: %s</h1>"
    "<h2>Temperature</h2>"
    "<p><strong>%.2f C</strong></p>"
    "<p><strong>%.2f F</strong></p>"
    "</body>"
    "</html>",
    device_id_global,
    latest_temperature_c,
    latest_temperature_f
  );

  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, response, strlen(response));
  return ESP_OK;
}

static httpd_handle_t start_webserver(void) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_handle_t server = NULL;

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_uri_t root_uri = {
      .uri      = "/",
      .method   = HTTP_GET,
      .handler  = root_get_handler,
      .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &root_uri);
    printf("Web server started on port %d\n", config.server_port);
  }

  return server;
}

void webserver_task(void *pvParameters) {
  printf("[WebServerTask] Starting HTTP server...\n");
  start_webserver();

  while (1) {
    vTaskDelay(portMAX_DELAY);
  }
}

void app_main(void) {
  if (config_nvs_init() != ESP_OK) {
    printf("Error: NVS init failed\n");
    return;
  }

  // Load WiFi from NVS
  char wifi_ssid[64];
  char wifi_pass[64];

  if (config_get_wifi(wifi_ssid, sizeof(wifi_ssid),
                      wifi_pass, sizeof(wifi_pass)) != ESP_OK) {
    printf("WiFi credentials not in NVS\n");
    return;
  }

  // Load AWS URL from NVS
  char aws_url[128];

  if (config_get_aws_url(aws_url, sizeof(aws_url)) != ESP_OK) {
    printf("AWS URL not in NVS\n");
    return;
  }

  // Load device ID from NVS
  if (config_get_device_id(device_id_global, sizeof(device_id_global)) != ESP_OK) {
    printf("Device ID not in NVS\n");
    return;
  }

  printf("Using device ID: %s\n", device_id_global);

  // Connect to WiFi
  if (wifi_init_and_connect(wifi_ssid, wifi_pass) != ESP_OK) {
    printf("WiFi connection FAILED.\n");
    return;
  }

  // HTTPS endpoint
  https_set_endpoint(aws_url);

  // I2C bus for BME280 + LCD
  i2c_master_bus_handle_t bus;

  i2c_master_bus_config_t bus_config = {
    .i2c_port = 0,
    .sda_io_num = SDA_PIN,
    .scl_io_num = SCL_PIN,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
  };

  if (i2c_new_master_bus(&bus_config, &bus) != ESP_OK) {
    printf("Failed to create I2C bus\n");
    return;
  }

  bme280_init(bus);
  lcd1602_init(bus);

  temperature_queue = xQueueCreate(5, sizeof(double));
  if (temperature_queue == NULL) {
    printf("Failed to create temperature queue\n");
    return;
  }

  xTaskCreatePinnedToCore(sensor_task, "SensorTask", 4096, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(network_task, "NetworkTask", 4096, device_id_global, 5, NULL, 1);

  xTaskCreatePinnedToCore(webserver_task, "WebServerTask", 4096, NULL, 5, NULL, 0);

  printf("RTOS tasks started.\n");
}
