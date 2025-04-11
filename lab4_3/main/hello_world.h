#include <stdio.h>
#include <esp_log.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO 8            // GPIO for SCL
#define I2C_MASTER_SDA_IO 10           // GPIO for SDA
#define I2C_MASTER_NUM I2C_NUM_0       // I2C port number
#define I2C_MASTER_FREQ_HZ 100000      // Frequency of I2C
#define I2C_TIMEOUT_MS 1000            // I2C timeout in ms
#define I2C_MASTER_TX_BUF_DISABLE   0         // I2C master doesn't need TX buffer
#define I2C_MASTER_RX_BUF_DISABLE   0         // I2C master doesn't need RX buffer
#define I2C_TIMEOUT_MS              1000      // I2C timeout in milliseconds
#define ICM42670_ADDR               0x68      // Default I2C address for ICM-42670-P
#define ICM_ADDR 0x68					      // 
#define ACCEL_DATA_X1 0x0B
#define ACCEL_DATA_X0 0x0C
#define ACCEL_DATA_Y1 0x0D
#define ACCEL_DATA_Y0 0x0E
#define ACCEL_DATA_Z1 0x0F
#define ACCEL_DATA_Z0 0x10
#define GYRO_DATA_X1 0x11
#define GYRO_DATA_X0 0x12
#define GYRO_DATA_Y1 0x13
#define GYRO_DATA_Y0 0x14
#define PWR_MGMTO 0x1F
#define PWR_D_SETTING 0x0C
#define GYRO_CONFIG 0x20
#define GYRO_D_SETTING 0x6C

#define A_BIT_LEFT 1
#define A_LOT_LEFT 2
#define A_BIT_RIGHT 3
#define A_LOT_RIGHT 4
#define UP 5
#define A_LOT_UP 6
#define DOWN 7
#define A_LOT_DOWN 8


 // Initialize I2C master
    void i2c_master_init() {
        i2c_config_t conf;
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = I2C_MASTER_SDA_IO;
        conf.scl_io_num = I2C_MASTER_SCL_IO;
        conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

        conf.clk_flags = 0;
        i2c_param_config(I2C_MASTER_NUM, &conf);
        i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    }

uint8_t read_register(uint8_t reg) {
    uint8_t data; 
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd); 
    i2c_master_write_byte(cmd, (ICM_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000)); 
    i2c_cmd_link_delete(cmd);
    return data;
}

esp_err_t write_register(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ICM_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C_WRITE", "Failed to write to register 0x%02X", reg);
    }
    return ret;
}


void configure_sensor() {
    // Power and gyroscope configuration
    if (write_register(PWR_MGMTO, PWR_D_SETTING) != ESP_OK ||
        write_register(GYRO_CONFIG, GYRO_D_SETTING) != ESP_OK) {
        ESP_LOGE("CONFIG_ERROR", "Failed to configure sensor.");
    } else {
        ESP_LOGI("CONFIG_SUCCESS", "Sensor configured successfully.");
    }
}

int16_t read_gyro_data(uint8_t high_reg, uint8_t low_reg) {
    return (int16_t)((read_register(high_reg) << 8) | read_register(low_reg));
}

int read_inclination_level() {
    int16_t xGyro = read_gyro_data(GYRO_DATA_X1, GYRO_DATA_X0);
    int16_t yGyro = read_gyro_data(GYRO_DATA_Y1, GYRO_DATA_Y0);

    // Add dead zone to ignore slight variations
    int dead_zone_threshold = 1000;
    if (abs(xGyro) < dead_zone_threshold && abs(yGyro) < dead_zone_threshold) {
        return 0;  // No significant movement
    }

    if (xGyro > 2500 && yGyro < -2500) {
        return A_BIT_LEFT;
    } else if (xGyro > 2500 && yGyro > 2500) {
        return A_BIT_RIGHT;
    } else if (xGyro < -2500 && yGyro < -2500) {
        return A_LOT_LEFT;
    } else if (xGyro < -2500 && yGyro > 2500) {
        return A_LOT_RIGHT;
    } else if (xGyro > 5000) {
        return A_LOT_UP;
    } else if (xGyro > 2500) {
        return UP;
    } else if (xGyro < -5000) {
        return A_LOT_DOWN;
    } else if (xGyro < -2500) {
        return DOWN;
    } else if (yGyro < -2500) {
        return A_BIT_LEFT;
    } else if (yGyro > 2500) {
        return A_BIT_RIGHT;
    }

    return 0;
}

void detect_tilt_direction() {
    int16_t xGyro = read_gyro_data(GYRO_DATA_X1, GYRO_DATA_X0);
    int16_t yGyro = read_gyro_data(GYRO_DATA_Y1, GYRO_DATA_Y0);

    if (xGyro > 2500 && yGyro < -2500) {
        ESP_LOGI("Gyroscope", "UP LEFT");
    } else if (xGyro > 2500 && yGyro > 2500) {
        ESP_LOGI("Gyroscope", "UP RIGHT");
    } else if (xGyro < -2500 && yGyro < -2500) {
        ESP_LOGI("Gyroscope", "DOWN LEFT");
    } else if (xGyro < -2500 && yGyro > 2500) {
        ESP_LOGI("Gyroscope", "DOWN RIGHT");
    } else if (xGyro < -2500) {
        ESP_LOGI("Gyroscope", "DOWN");
    } else if (xGyro > 2500) {
        ESP_LOGI("Gyroscope", "UP");     
    } else if (yGyro < -2500) {
        ESP_LOGI("Gyroscope", "LEFT");
    } else if (yGyro > 2500) {
        ESP_LOGI("Gyroscope", "RIGHT");
    }
}

