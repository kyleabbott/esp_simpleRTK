#include <stdio.h>
#include <esp_log.h>
#include "ublox.hpp"

static const char *TAG = "ublox-ublox";

namespace ublox {

ublox_gps::ublox_gps()
: m_conn()
{}

void ublox_gps::start(){
    m_conn.zed_f9p_i2c_do_read();
}

void ublox_gps::calculate_checksum(  const uint8_t msg_cls,
                                    const uint8_t msg_id,
                                    const uint16_t len,
                                    const uint8_t* payload,
                                    uint8_t &ck_a,
                                    uint8_t &ck_b           ){
    ck_a = ck_b = 0;

    // Add in class
    ck_a += msg_cls;
    ck_b += ck_a;

    // Id
    ck_a += msg_id;
    ck_b += ck_a;

    // Length
    ck_a += len & 0xFF;
    ck_b += ck_a;
    ck_a += (len >> 8) & 0xFF;
    ck_b += ck_a;

    // Payload
    for (int i = 0; i < len; i++)
    {
        ck_a += payload[i];
        ck_b += ck_a;
    }
}

void ublox_gps::disable_nmea(){
    //ublox_configure(CFG_VALSET_t::VERSION_0, CFG_VALSET_t::RAM, CFG_VALSET_t::CFG_I2COUTPROT_NMEA, 0, CFG_VALSET_t::TYPE_L);
    disable(CFG_VALSET_t::CFG_I2COUTPROT_NMEA);
}

void ublox_gps::enable(uint32_t cfg_key){
    ublox_configure(CFG_VALSET_t::VERSION_0, CFG_VALSET_t::RAM, cfg_key, 1, CFG_VALSET_t::TYPE_L);
}

void ublox_gps::disable(uint32_t cfg_key){
    ublox_configure(CFG_VALSET_t::VERSION_0, CFG_VALSET_t::RAM, cfg_key, 0, CFG_VALSET_t::TYPE_L);
}

void ublox_gps::ublox_configure(uint8_t version, uint8_t layer, uint32_t cfg_key, uint32_t cfg_data, size_t data_size){
    CFG_VALSET_t msg = {};
    msg.version  = version;
    msg.layer    = layer;
    msg.cfg_key  = cfg_key;
    msg.cfg_data.word = cfg_data;
    size_t msg_size = (sizeof(CFG_VALSET_t) - 4) + data_size;

    UBX_msg_t cfg_msg = {.cfg_msg = msg};

    uint8_t cls = 0x06;
    uint8_t id = 0x8a;

    uint8_t some_bytes[17] = {0xb5, 0x62, 0x06, 0x8a, 0x09, 0x00,   0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x72, 0x10,   0x00,   0x1e, 0xb1};
    //uint8_t some_bytes[17] = {0xb5, 0x62, 0x06, 0x8a, 0x09, 0x00,   0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x72, 0x10,   0x01,   0x1f, 0xb2};
    
    //dump_hex(some_bytes, 17);
    
    uint8_t ck_a = 0, ck_b = 0;
    calculate_checksum(cls, id, msg_size, cfg_msg.buffer, ck_a, ck_b);
    printf("\nck_a=%d, ck_b=%d", ck_a, ck_b);

//    esp_err_t res = i2c_master_write_to_device(m_i2c_port, m_addr, some_bytes, 17, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
//    i2c_master_write_to_device(m_i2c_port, m_addr, reinterpret_cast<uint8_t*>(&msg_size), 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
//    esp_err_t res = i2c_master_write_to_device(m_i2c_port, m_addr, cfg_msg.buffer, msg_size, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
//    i2c_master_write_to_device(m_i2c_port, m_addr, &ck_a, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
//    i2c_master_write_to_device(m_i2c_port, m_addr, &ck_b, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    esp_err_t res = m_conn.send(some_bytes, 17);
    if(res == ESP_OK){
        ESP_LOGI(TAG, "Successfully wrote config");
    }else{
        ESP_LOGW(TAG, "Unable to write config");
    }
}

}// ns ublox
