#include "https_client.h"
#include <stdio.h>
#include <string.h>
#include "esp_crt_bundle.h"
#include "esp_http_client.h"

static char aws_endpoint_url[128] = {0};

void https_set_endpoint(const char *url) {
  strncpy(aws_endpoint_url, url, sizeof(aws_endpoint_url) - 1);
  aws_endpoint_url[sizeof(aws_endpoint_url) - 1] = '\0';
}

void https_send_temperature(double celsius, double fahrenheit, const char *device_id) {
  if (aws_endpoint_url[0] == '\0') {
    printf("AWS endpoint URL not set!\n");
    return;
  }

  char json_body[256];

  snprintf(json_body, sizeof(json_body),
    "{\"temperature_c\": %.2f, \"temperature_f\": %.2f, \"device_id\": \"%s\"}",
    celsius, fahrenheit, device_id ? device_id : "unknown-device");

  esp_http_client_config_t config = {
    .url = aws_endpoint_url,
    .method = HTTP_METHOD_POST,
    .crt_bundle_attach = esp_crt_bundle_attach,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);

  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_post_field(client, json_body, strlen(json_body));

  esp_err_t status = esp_http_client_perform(client);

  if (status == ESP_OK) {
    printf("AWS POST OK (status %d)\n",
           esp_http_client_get_status_code(client));
  } else {
    printf("AWS POST FAILED (esp_err_t: 0x%x)\n", status);
  }

  esp_http_client_cleanup(client);
}
