#include "config.h"

_Config Config;

void _Config::write(){
	DynamicJsonBuffer json_buf;
	JsonObject &cfg = json_buf.createObject();

	cfg["ssid"] = ssid.c_str();
	cfg["password"] = password.c_str();
	cfg["host"] = host.c_str();
	cfg["token"] = token.c_str();
	cfg["port"] = port;

	std::ofstream cfg_file("/home/root/config.json");
	String result;
	cfg.printTo(result);
	cfg_file << result.c_str();
	cfg_file.close();
}

bool _Config::read_from_file(){
	std::ifstream cfg_file("/home/root/config.json");
	std::string cfg_content;
	std::getline(cfg_file, cfg_content);

	cfg_file.close();

	return read(cfg_content);
}

bool _Config::read(std::string content){
	DynamicJsonBuffer json_buf;
	JsonObject &cfg = json_buf.parseObject(content.c_str());

	if (!cfg.success()){
		Serial.println("[_Config::read] oops, can't read config file");
		return false;
	}

	char buf[10240];

	strcpy(buf, cfg["ssid"]);
	ssid.assign(buf);

	strcpy(buf, cfg["password"]);
	password.assign(buf);

	strcpy(buf, cfg["host"]);
	host.assign(buf);

	strcpy(buf, cfg["token"]);
	token.assign(buf);

	port = cfg["port"];

	Serial.println("[Config::read] read config: ");
	Serial.print("[Config::read] SSID: ");
	Serial.println(ssid.c_str());
	Serial.print("[Config::read] Password: ");
	Serial.println(password.c_str());
	Serial.print("[Config::read] Host: ");
	Serial.println(host.c_str());
	Serial.print("[Config::read] Token: ");
	Serial.println(token.c_str());
	Serial.print("[Config::read] Port: ");
	Serial.println(port);

	return true;
}
