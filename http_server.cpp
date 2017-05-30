#include "http_server.h"

namespace HTTP {
	std::string Server::render_index_page(){
		char buf[sizeof(index_page_template) * 2];
		sprintf(buf, index_page_template,
			    Config.ssid.c_str(),
			    Config.password.c_str(),
			    Config.token.c_str(),
			    Config.host.c_str(),
			    Config.port);

		return std::string(buf);
	}

	void Server::handle_request(){
		WiFiClient client = server.available();
		if (!client){
			return;
		}
		
		Serial.println("[HTTP::Server::handle_request] There's a client");
		HTTP::Request req(client);
		HTTP::Response resp;
		if (!req.success){
			Serial.println("[HTTP::Server::handle_request] Can't parse client data");
			resp.status_code = 400;
		} else if (req.method == REQUEST_METHOD_GET
			       && req.path == "/"){
			Serial.println("[HTTP::Server::handle_request] User is requesting GET /");
			resp.status_code = 200;
			std::string index_page = render_index_page();
			resp.headers["Content-Length"] = std::to_string(index_page.length());
			resp.body = index_page;
		} else if (req.method == REQUEST_METHOD_POST
				   && req.path == "/set"
				   && req.headers["Content-Type"] == "application/json"){
			Serial.println("[HTTP::Server::handle_request] User is requesting POST /set");
			if (Config.read(req.body)){
				Serial.println("[HTTP::Server::handle_request] Config is valid, saving");
				Config.write();
				resp.status_code = 200;
				resp.headers["Content-Type"] = "text/plain";
				resp.body = "Hub setup successfully!";
			} else {
				Serial.println("[HTTP::Server::handle_request] Config is invalid, returning 400");
				resp.status_code = 400;
			}
		} else {
			Serial.println("[HTTP::Server::handle_request] 404");
			resp.status_code = 404;
		}

		resp.headers["Connection"] = "closed";

		client.write(resp.render().c_str());

		client.stop();
	}
}