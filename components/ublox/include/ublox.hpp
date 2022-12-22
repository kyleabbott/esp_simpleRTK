#pragma once

#include "i2c_conn.hpp"
#include "ublox_iface.hpp"

namespace ublox {

class ublox_gps {
public:
    ublox_gps();

    void start();

    void disable_nmea();
    void enable(uint32_t cfg_key);
    void disable(uint32_t cfg_key);

private:
    void ublox_configure(uint8_t version, uint8_t layer, uint32_t cfg_key, uint32_t cfg_data, size_t data_size);
    void calculate_checksum(const uint8_t msg_cls, const uint8_t msg_id, const uint16_t len, const uint8_t* payload, uint8_t &ck_a, uint8_t &ck_b);

private:
    i2c_conn m_conn;
};

}// ns ublox
