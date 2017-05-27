#include "radio.h"
#include "smartbots_api.h"
#include <string>

std::string wifi_ssid="Floor 5";
std::string wifi_password="123haiphong";
std::string host="smart-bots.xyz";
short port=80;

SmartBotsAPI api(host, port, "Tt7yfpljvOxw28BOpYT6lnE41RcGDqGIDC6QluL3Y1vE8SEv4U");

void setup(){
	Radio.setup(true);
	Serial.begin(2000000);
	Serial.println("[setup] connecting");
	if (!api.setup(wifi_ssid, wifi_password)){
		Serial.println("[setup] connect to wifi failed");
	} else {
		Serial.println("[setup] connect to wifi ok");
	}
}

void loop(){
	cmd_vector cmds;
	api.get_pending_cmd(cmds);
	
	for (auto cmd = cmds.begin(); cmd != cmds.end(); ++cmd){
		Serial.println("[loop] iterating through cmd");
		Radio.send(*cmd);
	}

	delay(1000);
}