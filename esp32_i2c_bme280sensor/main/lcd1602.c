// lcd1602.c

#include "lcd1602.h"

#include <stdbool.h>
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LCD_I2C_ADDRESS 0x27
#define LCD_CHARS_PER_LINE 16

#define LCD_CMD_CLEAR_DISPLAY    0x01
#define LCD_CMD_RETURN_HOME      0x02
#define LCD_CMD_ENTRY_MODE_SET   0x04
#define LCD_CMD_DISPLAY_CONTROL  0x08
#define LCD_CMD_FUNCTION_SET     0x20
#define LCD_CMD_SET_DDRAM_ADDR   0x80

#define LCD_ENTRY_LEFT           0x02
#define LCD_ENTRY_SHIFT_DECR     0x00

#define LCD_DISPLAY_ON           0x04
#define LCD_CURSOR_OFF           0x00
#define LCD_BLINK_OFF            0x00

#define LCD_4BIT_MODE            0x00
#define LCD_2_LINE               0x08
#define LCD_5x8_DOTS             0x00

#define LCD_PIN_RS   (1u << 0)
#define LCD_PIN_RW   (1u << 1)
#define LCD_PIN_EN   (1u << 2)
#define LCD_PIN_BL   (1u << 3)
#define LCD_PIN_D4   (1u << 4)
#define LCD_PIN_D5   (1u << 5)
#define LCD_PIN_D6   (1u << 6)
#define LCD_PIN_D7   (1u << 7)

static i2c_master_dev_handle_t lcd_device = NULL;

static esp_err_t lcd_i2c_write(uint8_t value) {
    return i2c_master_transmit(lcd_device, &value, 1, -1);
}

static void lcd_pulse(uint8_t data) {
    lcd_i2c_write(data | LCD_PIN_EN);
    vTaskDelay(pdMS_TO_TICKS(1));

    lcd_i2c_write(data & ~LCD_PIN_EN);
    vTaskDelay(pdMS_TO_TICKS(1));
}

static uint8_t pack_nibble(uint8_t nibble, uint8_t flags) {
    uint8_t out = 0;

    if (nibble & 0x01) out |= LCD_PIN_D4;
    if (nibble & 0x02) out |= LCD_PIN_D5;
    if (nibble & 0x04) out |= LCD_PIN_D6;
    if (nibble & 0x08) out |= LCD_PIN_D7;

    out |= LCD_PIN_BL;
    out |= flags;

    return out;
}

static void lcd_write4(uint8_t nibble, uint8_t flags) {
    uint8_t out = pack_nibble(nibble & 0x0F, flags);
    lcd_pulse(out);
}

static void lcd_write8(uint8_t value, uint8_t flags) {
    lcd_write4(value >> 4, flags);
    lcd_write4(value & 0x0F, flags);
}

static void lcd_command(uint8_t cmd) {
    lcd_write8(cmd, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
}

static void lcd_data(uint8_t data) {
    lcd_write8(data, LCD_PIN_RS);
    vTaskDelay(pdMS_TO_TICKS(1));
}

esp_err_t lcd1602_init(i2c_master_bus_handle_t i2c_bus) {
    i2c_device_config_t cfg = {
        .device_address = LCD_I2C_ADDRESS,
        .scl_speed_hz = 100000,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    };

    esp_err_t err = i2c_master_bus_add_device(i2c_bus, &cfg, &lcd_device);
    if (err != ESP_OK) return err;

    vTaskDelay(pdMS_TO_TICKS(50));

    lcd_write4(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));

    lcd_write4(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));

    lcd_write4(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(1));

    lcd_write4(0x02, 0);
    vTaskDelay(pdMS_TO_TICKS(1));

    lcd_command(LCD_CMD_FUNCTION_SET |
                LCD_4BIT_MODE | LCD_2_LINE | LCD_5x8_DOTS);

    lcd_command(LCD_CMD_DISPLAY_CONTROL |
                LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);

    lcd_command(LCD_CMD_CLEAR_DISPLAY);

    lcd_command(LCD_CMD_ENTRY_MODE_SET |
                LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECR);

    return ESP_OK;
}

esp_err_t lcd1602_print(const char *line1, const char *line2) {
    lcd_command(LCD_CMD_CLEAR_DISPLAY);
    vTaskDelay(pdMS_TO_TICKS(2));

    // First line
    lcd_command(LCD_CMD_SET_DDRAM_ADDR | 0x00);
    if (line1) {
        for (int i = 0; i < LCD_CHARS_PER_LINE && line1[i] != '\0'; ++i) {
            lcd_data((uint8_t)line1[i]);
        }
    }

    // Second line
    lcd_command(LCD_CMD_SET_DDRAM_ADDR | 0x40);
    if (line2) {
        for (int i = 0; i < LCD_CHARS_PER_LINE && line2[i] != '\0'; ++i) {
            lcd_data((uint8_t)line2[i]);
        }
    }

    return ESP_OK;
}
