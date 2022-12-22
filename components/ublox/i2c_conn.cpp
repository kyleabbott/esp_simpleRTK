#include <stdio.h>
#include <algorithm>
#include <esp_log.h>
#include "i2c_conn.hpp"

static const char *TAG = "ublox-i2c-conn";

namespace ublox {

__attribute__((unused)) static void dump_hex(const void* data, size_t size){
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            printf(" ");
            if ((i+1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

esp_err_t i2c_conn::send(uint8_t* buf, size_t len){
    esp_err_t res = i2c_master_write_to_device(m_i2c_port, m_addr, buf, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    
    if(res == ESP_OK){
        ESP_LOGI(TAG, "Successfully sent bytes");
    }else{
        ESP_LOGW(TAG, "Unable to write config");
    }

    return res;
}

i2c_conn::i2c_conn(int scl, int sda, uint32_t i2c_bus_freq, i2c_port_t i2c_num, int i2c_bus_timeout, uint8_t device_addr, bool enable_pullups)//, uint32_t read_delay_ms, size_t read_buffer_size
    : m_addr(device_addr), m_i2c_port(i2c_num)
    {
        // TODO: check bus freq is either 400k or 100k or whatever
        assert(m_i2c_port >= 0 && m_i2c_port < I2C_NUM_MAX);

        i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = sda,
            .scl_io_num = scl,
            .sda_pullup_en = enable_pullups,
            .scl_pullup_en = enable_pullups,
            .master = {.clk_speed = i2c_bus_freq},
            .clk_flags = 0
        };

        ESP_ERROR_CHECK(i2c_param_config(m_i2c_port, &conf));
        ESP_ERROR_CHECK(i2c_set_timeout(m_i2c_port, i2c_bus_timeout));
        ESP_ERROR_CHECK(i2c_driver_install(m_i2c_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    }

size_t i2c_conn::zed_f9p_i2c_bytes_available(){
    uint8_t data[2];
    const uint8_t reg_addr = UBLOX_ZED_F9P_SIZE_REG_ADDR;
    esp_err_t ret = i2c_master_write_read_device(m_i2c_port, m_addr, &reg_addr, 1, data, sizeof(data), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    uint16_t bytes_available = 0;
    if(ret == ESP_OK){
        bytes_available = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
    }else if(ret == ESP_FAIL){
        ESP_LOGW(TAG, "Sending command error, slave hasn't ACK the transfer");
    }else if(ret == ESP_ERR_INVALID_ARG){
        ESP_LOGW(TAG, "Parameter error");
    }else if(ret == ESP_ERR_TIMEOUT){
        ESP_LOGW(TAG, "Operation timeout because the bus is busy");
    }else if(ret == ESP_ERR_INVALID_STATE){
        ESP_LOGW(TAG, "driver not installed, or not in master mode");
    }else{
        ESP_LOGW(TAG, "Some other I2C error");
    }
    return static_cast<size_t>(bytes_available);    
}

void i2c_conn::zed_f9p_i2c_do_read(){
    const size_t BUF_SIZE = 1024;
    uint8_t* buf = (uint8_t*)heap_caps_malloc(BUF_SIZE * sizeof(uint8_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    size_t bytes_available = 0;
    while(true){
        if(!bytes_available)
            bytes_available = zed_f9p_i2c_bytes_available();
        //printf("%u bytes available\n", bytes_available);
        if(bytes_available){
            const size_t bytes_to_read = std::min<size_t>(BUF_SIZE, bytes_available);
            //printf("reading %u bytes\n", bytes_to_read);
            esp_err_t read_ret = i2c_master_read_from_device(m_i2c_port, m_addr, buf, bytes_to_read, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
            if(read_ret == ESP_OK){
                //dump_hex(buf, bytes_to_read);
                // TODO: push bytes onto stream buffer here
                bytes_available = bytes_available - bytes_to_read;
            }else{
                bytes_available = 0;
                if(read_ret == ESP_ERR_TIMEOUT)
                    ESP_LOGW(TAG, "I2C Bus is busy during read");           
            }
        }
        if(!bytes_available)
            vTaskDelay(UBLOX_READ_DELAY_MS / portTICK_PERIOD_MS);
    }
}

}// ns ublox
