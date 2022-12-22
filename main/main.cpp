#include <stdio.h>

#include <ublox.hpp>

using namespace ublox;

static const char *TAG = "esp-simple-RTK-main";

extern "C" {

    void app_main(void){
        // configure i2c stream
        // create i2c stream task
        // create ublox parsing task
        // create serial task
        // create network task
        ublox_gps gps;//i2c_conn connection;
        gps.disable_nmea();
        gps.start();
        //connection.zed_f9p_i2c_do_read();
    }

}