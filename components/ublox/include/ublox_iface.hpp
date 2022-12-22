#pragma once

namespace ublox {

typedef struct {
    enum
    {
        VERSION_0 = 0,
        VERSION_1 = 1,
    };

    enum
    {
        RAM = 0b00000001,
        BBR = 0b00000010,
        FLASH = 0b00000100,
        DEFAULT = 0b01000000,
    };

    enum {
        CFG_I2COUTPROT_NMEA = 0x10720002,
    };

    enum {
        TYPE_L  = 1, // bit - stored as byte
        TYPE_U1 = 1, // unsigned 8-bit integer
        TYPE_U2 = 2, // unsinged little-endian 16-bit integer
    };

    uint8_t     version; // Message version 0x00 from u-blox F9 HPG 1.32 Interface Description
    uint8_t     layer; // bit0: RAM bit1:BBR bit2: Flash
    uint8_t     reserved[2];
    uint32_t    cfg_key;
    union {
        uint8_t bytes[4];
        uint32_t word;
    } cfg_data;
} __attribute__((packed)) CFG_VALSET_t;

typedef union {
    uint8_t buffer[sizeof(CFG_VALSET_t)]; // TODO: should be max size message, or can this be removced? constexpr maybe?
    CFG_VALSET_t cfg_msg;
} UBX_msg_t;

typedef union {
    uint8_t bytes[6];
    struct {
        uint8_t preamble1; // 0xb5
        uint8_t preamble2; // 0x62
        uint8_t msg_class;
        uint8_t msg_id;
        uint16_t payload_length; // little-endian
        // payload goes here
    } header;
} __attribute__((packed)) UBX_frame_header_t;

typedef union {
    uint8_t bytes[2];
    struct {
        uint8_t checksum_a;
        uint8_t checksum_b;
    } footer;
} __attribute__((packed)) UBX_frame_footer_t;

}// ns ublox