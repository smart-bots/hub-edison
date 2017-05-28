#include "http.h"

namespace HTTP {
	namespace Common {
		bool parse_status_line(WiFiClient client, int *status_code){
			String status = client.readStringUntil('\n');
			// Serial.print("[HTTPClient::parse_status_line] status line: ");
			// Serial.println(status);
			if (sscanf(status.c_str(),
				       "HTTP/%*s %d %*s\r",
				       status_code) != 1){
				Serial.print("[HTTP::Common::parse_status_line] can't parse status line, line is ");
				Serial.println(status);
				return false;
			} else {
				// Serial.print("[HTTPClient::parse_status_line] status code is ");
				// Serial.println(status_code);
				return true;
			}
		}

		bool parse_headers(WiFiClient client, HTTP::Type::headers_t &headers){
			while (true){
				String header = client.readStringUntil('\n');

				//Serial.print("[HTTPClient::parse_header] header: ");
				//Serial.println(header);

				if (header == "\r"){
					return true;
				}

				char header_key[10240], header_content[10240];
				if (sscanf(header.c_str(),
					       "%10239[^:\r\n]: %10239[^\r\n]\r",
					       header_key,
					       header_content) != 2){
					Serial.print("[HTTP::Common::parse_header] can't parse header, header is ");
					Serial.println(header);
					return false;
				} else {
					// Serial.print("[HTTPClient::parse_header] key is \"");
					// Serial.print(header_key);
					// Serial.print("\", content is \"");
					// Serial.print(header_content);
					// Serial.println("\"");
					headers[std::string(header_key)] = header_content;
				}
			}
		}

		bool read_body(WiFiClient client, long length, std::string &dest){
			if (length < 0){
				Serial.println("[HTTP::Common::read_body] length is null, using fallback");
				// if the server doesn't supply Content-Length
				while (client.available()){
					dest += client.read();
				}
			} else {		
				// Serial.print("[HTTPClient::read_body] Content-Length is ");
				// Serial.println(length);

				char tmp_response[length + 1];
				client.readBytes(tmp_response, length);
				dest.assign(tmp_response, length);

				Serial.print("[HTTPClient::read_body] body is ");
				Serial.println(dest.c_str());
			}
			return true;
		}
	}
}