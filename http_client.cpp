#include "http_client.h"

void HTTPClient::post(std::string host,
				      short port,
	                  std::string url,
	                  post_data data){
    std::ostringstream data_stream;
    for (auto pair = data.begin(); pair != data.end(); ++pair){
        data_stream << (*pair).first << "=" << (*pair).second << "&";
    }

    std::string data_str = data_stream.str();

    std::ostringstream request_data;
    request_data << "POST " << url << " HTTP/1.1\r\n";
    request_data << "Host: " << host << "\r\n";
    request_data << "Content-Length: " << data_str.length() << "\r\n";
    request_data << "Content-Type: application/x-www-form-urlencoded\r\n";
    request_data << "Connection: close\r\n";
    request_data << "\r\n";
    request_data << data_str;

    Serial.println("[HTTPClient::post] post data: ");
    Serial.println(request_data.str().c_str());

    if (client.connect(host.c_str(), port)){
    	client.print(request_data.str().c_str());
    	parse_response();
    } else {
    	success = false;
    }
}

void HTTPClient::parse_response(){
	client.setTimeout(TIMEOUT);

	parse_status_line();
	if (!success){
		return;
	}
	while (true){
		if (!parse_header()){
			if (!success){
				return;
			}
			// no more headers to be parsed
			read_body();
			// no more to parsed, we're done here
			return;
		}
		if (!success){
			return;
		}
	}
}

void HTTPClient::parse_status_line(){
	String status = client.readStringUntil('\n');
	Serial.print("[HTTPClient::parse_status_line] status line: ");
	Serial.println(status);
	if (sscanf(status.c_str(),
		       "HTTP/%*s %d %*s\r",
		       &status_code) != 1){
		Serial.println("[HTTPClient::parse_status_line] can't parse status line ");
		success = false;
	} else {
		Serial.print("[HTTPClient::parse_status_line] status code is ");
		Serial.println(status_code);
	}
}

bool HTTPClient::parse_header(){
	String header = client.readStringUntil('\n');

	Serial.print("[HTTPClient::parse_header] header: ");
	Serial.println(header);

	if (header == "\r"){
		return false;
	}

	char header_key[10240], header_content[10240];
	if (sscanf(header.c_str(),
		       "%10239[^:\r\n]: %10239[^\r\n]\r",
		       header_key,
		       header_content) != 2){
		Serial.println("[HTTPClient::parse_header] can't parse header");
		success = false;
		return false;
	} else {
		Serial.print("[HTTPClient::parse_header] key is \"");
		Serial.print(header_key);
		Serial.print("\", content is \"");
		Serial.print(header_content);
		Serial.println("\"");
		headers[std::string(header_key)] = header_content;
	}
	return true;
}

void HTTPClient::read_body(){
	auto iter = headers.find("Content-Length");
	if (iter == headers.end()){
		Serial.println("[HTTPClient::read_body] Content-Length not found");
		// if the server doesn't supply Content-Length
		while (client.available()){
			response += client.read();
		}
	} else {
		long length = std::stol(headers["Content-Length"]);
		Serial.print("[HTTPClient::read_body] Content-Length is ");
		Serial.println(length);

		char tmp_response[length + 1];
		client.readBytes(tmp_response, length);
		response.assign(tmp_response, length);

		Serial.print("[HTTPClient::read_body] body is ");
		Serial.println(response.c_str());
	};
}