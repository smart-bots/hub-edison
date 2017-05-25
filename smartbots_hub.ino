#include "radio.h"

char token[] = "abcabcabc0";

bool has_data; byte type; short state; float data;
void handler(byte type, char token[], short state, float data){
    // has_data = true;
    // type = type;
    // state = state;
    // data = data;
}
void setup(){
    Serial.begin(115200);
    Radio.setup(true);
    //Radio.register_receive_handler(handler);
}

void loop(){
    Radio.send(MSG_TYPE_SOFT_CONTROL, token, STATE_ON);
    delay(3000);
    // if (has_data){
    //     has_data = false;
    //     Serial.println(type);
    //     Serial.println(state);
    //     Serial.println(data);
    // }
    Radio.send(MSG_TYPE_SOFT_CONTROL, token, STATE_OFF);
    delay(3000);
}