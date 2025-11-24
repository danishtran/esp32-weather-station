#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "BME_TUTORIAL";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "App started");
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
