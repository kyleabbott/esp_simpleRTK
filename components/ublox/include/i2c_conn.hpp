#pragma once

#include <driver/i2c.h>

#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_READ_BUFFER_SIZE        1024
#define I2C_BUS_TIMEOUT             0xFFFFF
#define I2C_MASTER_ENABLE_PULLUPS   0x1

#define UBLOX_ZED_F9P_ADDR              0x42        /*!< Slave address */
#define UBLOX_ZED_F9P_SIZE_REG_ADDR     0xFD        /*!< Register address for size of bytes available (2-bytes big endian) */
#define UBLOX_READ_DELAY_MS             500         /*!< Wait between reads. Should increase frequency of reads for higher-freq data such as AHRS */

namespace ublox {

class i2c_conn {
public:
    i2c_conn( int scl = CONFIG_I2C_MASTER_SCL, // set in Kconfig
                int sda = CONFIG_I2C_MASTER_SDA, // set in Kconfig
                uint32_t i2c_bus_freq = I2C_MASTER_FREQ_HZ,
                i2c_port_t i2c_num = I2C_MASTER_NUM,            
                int i2c_bus_timeout = I2C_BUS_TIMEOUT,
                uint8_t device_addr = UBLOX_ZED_F9P_ADDR,
                bool enable_pullups = I2C_MASTER_ENABLE_PULLUPS
    );

    void zed_f9p_i2c_do_read();
    esp_err_t send(uint8_t* buf, size_t len);

private:
    size_t zed_f9p_i2c_bytes_available();
    private:
    uint8_t m_addr;
    i2c_port_t m_i2c_port;
};

}// ns ublox
