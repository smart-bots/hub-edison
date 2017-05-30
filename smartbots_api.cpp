#include "smartbots_api.h"

void SmartBotsAPI::setup(std::string &host,
                         short port,
                         std::string &token){
    
    this->host = host;
    this->port = port;
    this->token = token;
}

bool SmartBotsAPI::wifi_connect(std::string &ssid,
                                std::string &password){
    Serial.print("[SmartBotsAPI::wifi_connect] SSID: ");
    Serial.println(ssid.c_str());
    Serial.print("[SmartBotsAPI::wifi_connect] Password: ");
    Serial.println(password.c_str());

    int8_t remaining = RETRIES_NUM;
    while (remaining >= 0){
        if (WiFi.begin((char*)ssid.c_str(), (char*)password.c_str()) == WL_CONNECTED){
            return true;
        }
        Serial.print("[SmartBotsAPI::wifi_connect] Connect to wifi failed, ");
        Serial.print(remaining);
        Serial.println(" retries left");
        delay(3000);
        remaining--;
    }
    return false;
}

bool SmartBotsAPI::setup(std::string &ssid,
                         std::string &password){
    WiFi.disconnect();
    WiFi.setDNS(GOOGLE_DNS_1, GOOGLE_DNS_2);
    if (this->wifi_connect(ssid, password)){
        connected = true;
    } else {
        connected = false;
    }
    return connected;
}

bool SmartBotsAPI::connect_to_host(WiFiClient &client){
    client.stop();

    int8_t remaining = RETRIES_NUM;

    while (remaining >= 0){
        if (client.connect(host.c_str(), port)){
            return true;
        }
        Serial.print("[SmartBotsAPI::connect_to_host] Connect to host failed, ");
        Serial.print(remaining);
        Serial.println(" retries left");
        delay(500);
        remaining--;
    }
    return false;
}

bool SmartBotsAPI::send_cmd(receive_msg recv){
    DEBUG("[SmartBotsAPI::send_cmd] Called");

    if (!connected){
        return false;
    }

    WiFiClient client;
    if (!connect_to_host(client)){
        return false;
    }
    
    HTTP::Type::post_data data;
    data["hubToken"] = token;
    data["botToken"] = recv.token;
    data["status"] = std::to_string(recv.state);
    if (recv.type == MSG_TYPE_HARD_CONTROL){
        data["hard"] = "1";
    } else {
        data["hard"] = "0";
    }

    HTTP::Response req = HTTP::Shortcut::POST(client,
                                              REQUEST_METHOD_POST,
                                              host,
                                              "/api/up",
                                              data);
    client.stop();

    if (!req.success || req.status_code != 200){
        return false;
    } else {
        return true;
    }
}

bool SmartBotsAPI::get_pending_cmds(cmd_queue &cmds){
    DEBUG("[SmartBotsAPI::get_pending_cmds] in");

    if (!connected){
        return false;
    }

    WiFiClient client;
    if (!connect_to_host(client)){
        return false;
    }

    cmds.clear();

    HTTP::Type::post_data data;
    data["hubToken"] = token;

    HTTP::Response req = HTTP::Shortcut::POST(client,
                                              REQUEST_METHOD_POST,
                                              host,
                                              "/api/down",
                                              data);

    client.stop();

    if (req.status_code != 200){
        Serial.print("[SmartBotsAPI::get_pending_cmds] fail, status code is ");
        Serial.println(req.status_code);
        return false;
    } else {
        DynamicJsonBuffer jsonBuf;
        JsonObject &data = jsonBuf.parseObject(req.body.c_str());

        int i;
        send_msg msg;
        char key[10];
        for (i = 0; i < data["c"]; i++){
            sprintf(key, "%d", i);

            msg.type = MSG_TYPE_SOFT_CONTROL;
            strncpy(msg.token, data[key]["token"], 10);
            msg.token[10] = '\0';
            msg.state = data[key]["state"];

            Serial.println("[SmartBotsAPI::get_pending_cmds] Pushing cmd");
            cmds.push_back(msg);
        }

        return true;
    }
}