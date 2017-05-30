#ifndef RADIO_H

#define RADIO_H

#include <Arduino.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <RF24_config.h>

#define NRF_IRQ_PIN 2
#define NRF_CE_PIN 7
#define NRF_CS_PIN 8

#define NRF_READ_ADDR 0xF0F0F0F0D2LL
#define NRF_WRITE_ADDR 0xF0F0F0F0E1LL

#define NRF_ACK true

#define NRF_PA_LEVEL RF24_PA_MIN
#define NRF_DATA_RATE RF24_1MBPS

#define STATE_ON 1
#define STATE_OFF 0

#define MSG_TYPE_SOFT_CONTROL 1
#define MSG_TYPE_CHECK_ONLINE 2

#define MSG_TYPE_ACK_SOFT_CONTROL 1
#define MSG_TYPE_ACK_CHECK_ONLINE 2
#define MSG_TYPE_HARD_CONTROL 3
#define MSG_TYPE_SEND_DATA 4

typedef struct {
    uint8_t type;
    char token[11];
    short state;
} send_msg;

typedef struct {
    byte type;
    char token[11];
    short state;
    float data;
} receive_msg;

class _Radio {
private:
    void (*handler)(receive_msg) = NULL;
    
    bool check_valid_params();
public:
    RF24 radio;

    _Radio(): radio(NRF_CE_PIN, NRF_CS_PIN) {};
    bool setup(bool is_hub);
    void send(send_msg msg);
    void send(byte type, char token[], short state);
    void register_receive_handler(void (*handler)(receive_msg));
    void on_received();
};

extern _Radio Radio;

#endif

// STATUS       = 0x0e RX_DR=0 TX_DS=0 MAX_RT=0 RX_P_NO=7 TX_FULL=0
// RX_ADDR_P0-1     = 0xf0f0f0f0e1 0xf0f0f0f0d2
// RX_ADDR_P2-5     = 0xc3 0xc4 0xc5 0xc6
// TX_ADDR      = 0xf0f0f0f0e1
// RX_PW_P0-6   = 0x20 0x20 0x00 0x00 0x00 0x00
// EN_AA        = 0x3f
// EN_RXADDR    = 0x02
// RF_CH        = 0x4c
// RF_SETUP     = 0x01
// CONFIG       = 0x3f
// DYNPD/FEATURE    = 0x3f 0x06
// Data Rate    = 1MBPS
// Model        = nRF24L01+
// CRC Length   = 16 bits
// PA Power     = PA_MIN