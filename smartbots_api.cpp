#include "smartbots_api.h"

SmartBotsAPI::SmartBotsAPI(std::string host,
             short port,
             std::string token){
    
    this->host = host;
    this->port = port;
    this->token = token;
}

bool SmartBotsAPI::wifi_connect(std::string ssid,
                                std::string password){
    Serial.print("[SmartBotsAPI::wifi_connect] ");
    Serial.println(ssid.c_str());
    Serial.print("[SmartBotsAPI::wifi_connect] ");
    Serial.println(password.c_str());

    int8_t remaining = RETRIES_NUM;
    while (remaining >= 0){
        if (WiFi.begin((char*)ssid.c_str(), (char*)password.c_str()) == WL_CONNECTED){
            connected = true;   
            return true;
        }
        Serial.print("[SmartBotsAPI::ssid_connect] failure, ");
        Serial.print(remaining);
        Serial.println(" retries left");
        delay(3000);
        remaining--;
    }
    connected = false;
    return false;
}

bool SmartBotsAPI::setup(std::string ssid,
                         std::string password){
    WiFi.disconnect();
    WiFi.setDNS(GOOGLE_DNS_1, GOOGLE_DNS_2);
    return this->wifi_connect(ssid, password);  
}

bool SmartBotsAPI::send_cmd(receive_msg recv){
    DEBUG("[SmartBotsAPI::send_cmd] in");

    if (!connected){
        return false;
    }

    post_data data;
    data["hubToken"] = token;
    data["botToken"] = recv.token;
    data["status"] = std::to_string(recv.state);
    if (recv.type == MSG_TYPE_HARD_CONTROL){
        data["hard"] = "1";
    } else {
        data["hard"] = "0";
    }

    HTTPClient req;
    req.post(host, port, "/api/up", data);
    if (!req.success || req.status_code != 200){
        return false;
    } else {
        return true;
    }
}

bool SmartBotsAPI::get_pending_cmd(cmd_vector &cmds){
    DEBUG("[SmartBotsAPI::get_pending_cmd] in");

    if (!connected){
        return false;
    }

    cmds.clear();

    post_data data;
    data["hubToken"] = token;

    HTTPClient req;
    req.post(host, port, "/api/down", data);
    if (!req.success || req.status_code != 200){
        return false;
    } else {
        DynamicJsonBuffer jsonBuf;
        JsonObject &data = jsonBuf.parseObject(req.response.c_str());

        int i;
        send_msg msg;
        char key[10];
        for (i = 0; i < data["c"]; i++){
            sprintf(key, "%d", i);

            msg.type = MSG_TYPE_SOFT_CONTROL;
            strncpy(msg.token, data[key]["token"], 10);
            msg.token[10] = '\0';
            msg.state = data[key]["state"];

            Serial.println("[SmartBotsAPI::get_pending_cmd] Pushing cmd");
            cmds.push_back(msg);
        }

        return true;
    }
}