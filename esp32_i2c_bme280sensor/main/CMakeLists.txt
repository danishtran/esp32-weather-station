idf_component_register(
  SRCS 
    "main.c"
    "bme280.c"
    "conversions.c"
    "wifi.c"
    "https_client.c"
    "config_nvs.c"
    "lcd1602.c"
  INCLUDE_DIRS "."
)
