#ifndef HTTP_CLIENT_H

#define HTTP_CLIENT_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <string>
#include <map>
#include <sstream>

#define TIMEOUT 5000

typedef std::map<std::string, std::string> post_data;
typedef std::map<std::string, std::string> headers_t;

class HTTPClient {
private:
	WiFiClient client;

	void parse_response();
	void parse_status_line();
	bool parse_header();
	void read_body();
public:
	bool success = true;
	int status_code = 0;

	headers_t headers;

	std::string response;

	HTTPClient(){
		response = "";
		headers.clear();
	};

	void post(std::string host,
		         short port,
		         std::string url,
		         post_data data);
};

#endif