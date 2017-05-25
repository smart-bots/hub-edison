#include <printf.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <RF24_config.h>

#define CE_PIN 7
#define CS_PIN 8

RF24 radio(CE_PIN, CS_PIN);

typedef struct {
  byte type;
  char token[11];
  short state;
} request;

void setup(){
  pinMode(CE_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  Serial.begin(9600);
    radio.begin();
    radio.setAutoAck(true);
    radio.enableDynamicPayloads();
    radio.setRetries(15,15);
    radio.openReadingPipe(1, 0xF0F0F0F0D2LL);
    radio.openWritingPipe(0xF0F0F0F0E1LL);
    radio.startListening();
}

void loop(){
	request req;
  req.type = 1;
  strcpy(req.token, "abcabcabc0");
  Serial.println(req.token);
  req.state = 1;

  radio.stopListening();
  if (radio.write(&req, sizeof(req))){
    Serial.println("transmission ok (on)");
  } else {
    Serial.println("transmission failed (on)");
  }
  radio.startListening();
  
  delay(5000);

  req.state = 0;

  radio.stopListening();
  if (radio.write(&req, sizeof(req))){
    Serial.println("transmission ok (off)");
  } else {
    Serial.println("transmission failed (off)");
  }
  radio.startListening();

  delay(5000);
}

