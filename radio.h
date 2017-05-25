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
    
    #define STATE_ON 1
    #define STATE_OFF 0
    
    #define MSG_TYPE_SOFT_CONTROL 1
    #define MSG_TYPE_CHECK_ONLINE 2

    #define MSG_TYPE_ACK_SOFT_CONTROL 1
    #define MSG_TYPE_ACK_CHECK_ONLINE 2
    #define MSG_TYPE_HARD_CONTROL 3
    #define MSG_TYPE_SEND_DATA 4

    typedef struct {
        byte type;
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
        RF24 radio;
        void (*handler)(byte, char[], short, float) = NULL;

    public:
        _Radio(): radio(NRF_CE_PIN, NRF_CS_PIN) {};
        void setup(bool is_hub);
        void send(byte type, char token[], short state);
        void register_receive_handler(void (*handler)(byte, char[], short, float));
        void on_received();
    };

    extern _Radio Radio;

#endif