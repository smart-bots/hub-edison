#include "radio.h"
_Radio Radio;

void interrupt_handler(){
	Radio.on_received();
}

bool _Radio::setup(bool is_hub){
	attachInterrupt(NRF_IRQ_PIN, interrupt_handler, FALLING);

    pinMode(NRF_CE_PIN, OUTPUT);
    pinMode(NRF_CS_PIN, OUTPUT);
    // digitalWrite(NRF_CE_PIN, HIGH);
    // digitalWrite(NRF_CS_PIN, HIGH);

    do {
        Serial.println("[Radio::setup] Initializing RF");
        radio.begin();
        radio.setPALevel(NRF_PA_LEVEL);
        radio.setDataRate(NRF_DATA_RATE);
        radio.enableAckPayload();
        radio.setAutoAck(true);
        radio.enableDynamicPayloads();
        radio.setRetries(15,15);
        radio.maskIRQ(true, true, false); // mask tx_ok and tx_failure IRQ
        radio.setCRCLength(RF24_CRC_16);
        if (is_hub){
        	radio.openReadingPipe(1, NRF_READ_ADDR);
        	radio.openWritingPipe(NRF_WRITE_ADDR);	
        } else {
        	radio.openReadingPipe(1, NRF_WRITE_ADDR);
        	radio.openWritingPipe(NRF_READ_ADDR);	
        }
        radio.startListening();

        radio.printDetails();
    } while (!check_valid_params());

    return true;
}

bool _Radio::check_valid_params(){
    return (radio.getCRCLength() == RF24_CRC_16)
        && (radio.isPVariant())
        && (radio.getPALevel() == NRF_PA_LEVEL)
        && (radio.getDataRate() == NRF_DATA_RATE)
        && (!radio.failureDetected);
}

void _Radio::send(send_msg msg){
    Serial.print("[Radio::send] sending the following: ");
    Serial.print(msg.type);
    Serial.print(" ");
    Serial.print(msg.token);
    Serial.print(" ");
    Serial.println(msg.state);

    radio.stopListening();
    radio.write(&msg, sizeof(msg));
    radio.startListening();
    Serial.println("[Radio::send] out");
}

void _Radio::send(byte type, char token[], short state){
    send_msg req;

    req.type = type;
    strncpy(req.token, token, 10);
    req.token[10] = '\0';
    req.state = state;

    send(req);
}

void _Radio::register_receive_handler(void (*handler)(receive_msg)){
    this->handler = handler;
}

void _Radio::on_received(){
    Serial.println("[_Radio::on_received] called");

    bool tx_ok, tx_fail, rx_avail;
    radio.whatHappened(tx_ok, tx_fail, rx_avail);
    if (rx_avail || (radio.available() > 0)){
        Serial.println("[_Radio::on_received] There's data to read");
        if (handler != NULL){
            receive_msg msg;
            radio.read(&msg, sizeof(msg));
            Serial.println("[_Radio::on_received] Calling handler");
            handler(msg);
            Serial.println("[_Radio::on_received] out");
        }
    }        
}