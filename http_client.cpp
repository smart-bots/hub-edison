#include "http_client.h"

namespace HTTP {
	void Client::post(std::string host,
					        short port,
		                    std::string url,
		                    HTTP::Type::post_data data){
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
	    request_data << "Connection: keep-alive\r\n";
	    request_data << "\r\n";
	    request_data << data_str;

	    if (client.connect(host.c_str(), port)){
	    	client.print(request_data.str().c_str());
	    	parse_response();
	    } else {
	    	Serial.println("[HTTP::Client::post] can't post, post data is: ");
	    	Serial.println(request_data.str().c_str());
	    	this->success = false;
	    }
	    client.stop();
	}

	void Client::parse_response(){
		client.setTimeout(HTTP::TIMEOUT);

		if ((!HTTP::Common::parse_status_line(client, &status_code))
			|| (!HTTP::Common::parse_headers(client, headers))){
			success = false;
			return;
		}

		auto length_iter = headers.find("Content-Length");
		long length = -1;
		if (length_iter != headers.end()){
			length = std::stol((*length_iter).second);
		}

		HTTP::Common::read_body(client, length, body);

		success = true;
	}
}