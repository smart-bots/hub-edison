#ifndef HTTP_CLIENT_H

#define HTTP_CLIENT_H

#include "http.h"

namespace HTTP {
	class Client {
	private:
		void parse_response();
	public:
		WiFiClient client;

		bool success = true;
		int status_code = 0;

		HTTP::Type::headers_t headers;

		std::string body;

		Client(){
			body = "";
			headers.clear();
		};

		void post(std::string host,
			      short port,
			      std::string url,
			      HTTP::Type::post_data data);
	};
}

#endif