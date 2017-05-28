#ifndef HTTP_H

#define HTTP_H

#include <WiFi.h>
#include <WiFiClient.h>
#include <string>
#include <map>
#include <sstream>

namespace HTTP {
	const int TIMEOUT = 2000;

	namespace Type {
		typedef std::map<std::string, std::string> post_data;
		typedef std::map<std::string, std::string> headers_t;
	}

	namespace Common {
		bool parse_status_line(WiFiClient client, int *status_code);
		bool parse_headers(WiFiClient client, HTTP::Type::headers_t &headers);
		bool read_body(WiFiClient client, long length, std::string &dest);
	}
}

#endif