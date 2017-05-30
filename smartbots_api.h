#ifndef SMARBOTS_API_H

#define SMARBOTS_API_H

#include <map>
#include <sstream>
#include <vector>
#include <mutex>

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#include "radio.h"
#include "http.h"

#define RETRIES_NUM 5
#define TIMEOUT 5000

#define GOOGLE_DNS_1 IPAddress(8, 8, 8, 8)
#define GOOGLE_DNS_2 IPAddress(8, 8, 4, 4)

#define DEBUG(x) Serial.println(x)

typedef std::vector<send_msg> cmd_queue;

typedef struct {
    WiFiClient client;
    bool in_use = false;
    bool fail = false;
} client_s;

class SmartBotsAPI {
private:
    bool connected = false;

    std::string host, token;
    short port;
    std::vector<client_s> clients;
    WiFiClient client;

    bool wifi_connect(std::string &ssid,
                      std::string &password);
    client_s& get_available_client();
    bool connect_to_host(WiFiClient &client);
public:
    void setup(std::string &host,
               short port,
               std::string &token);
    bool setup(std::string &ssid, std::string &password);
    bool send_cmd(receive_msg msg);
    bool get_pending_cmds(cmd_queue &cmds);
};

#endif