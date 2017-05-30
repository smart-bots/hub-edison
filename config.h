#ifndef CONFIG_H

#define CONFIG_H

#include <Arduino.h>
#include <fstream>
#include <string>
#include <ArduinoJson.h>

#undef ARDUINOJSON_ENABLE_STD_STRING
#define ARDUINOJSON_ENABLE_STD_STRING 1

class _Config {
public:
	std::string ssid;
	std::string password;
	std::string host;
	std::string token;
	uint16_t port = 0;

	_Config(): ssid(), password(), host(), token() {};

	bool read_from_file();
	bool read(std::string content);
	void write();
};

extern _Config Config;

#endif