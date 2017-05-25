#include "radio.h"

_Radio Radio;

void interrupt_handler(){
	Radio.on_received();
}

void _Radio::setup(bool is_hub){
	attachInterrupt(NRF_IRQ_PIN, interrupt_handler, CHANGE);

    radio.begin();
    radio.setAutoAck(NRF_ACK);
    radio.enableDynamicPayloads();
    radio.setRetries(15,15);
    if (is_hub){
    	radio.openReadingPipe(1, NRF_READ_ADDR);
    	radio.openWritingPipe(NRF_WRITE_ADDR);	
    } else {
    	radio.openReadingPipe(1, NRF_WRITE_ADDR);
    	radio.openWritingPipe(NRF_READ_ADDR);	
    }
    radio.startListening();
}

void _Radio::send(byte type, char token[], short state){
    send_msg req;

    req.type = type;

    strcpy(req.token, token);

    req.state = state;

    radio.stopListening();
    radio.write(&req, sizeof(req));
    radio.startListening();
}

void _Radio::register_receive_handler(void (*handler)(byte, char[], short, float)){
    this->handler = handler;
}

void _Radio::on_received(){
    if (handler == NULL){
        return;
    }

    bool tx_ok, tx_fail, rx_avail;
    radio.whatHappened(tx_ok, tx_fail, rx_avail);
    if (rx_avail || radio.available()){
        receive_msg msg;
        radio.read(&msg, sizeof(msg));
        (*handler)(msg.type, msg.token, msg.state, msg.data);
    }        
}