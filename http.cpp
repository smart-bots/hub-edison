#include "http.h"

namespace HTTP {
    namespace Common {
        bool parse_status_line(WiFiClient &client, uint16_t *status_code){
            String status = client.readStringUntil('\n');
            // Serial.print("[HTTPClient::parse_status_line] status line: ");
            // Serial.println(status);
            if (sscanf(status.c_str(),
                       "HTTP/%*s %u %*s\r",
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

        bool parse_headers(WiFiClient &client, HTTP::Type::headers_t &headers){
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

        long determine_body_length(HTTP::Type::headers_t &headers){
            auto length_iter = headers.find("Content-Length");
            long length = -1;
            if (length_iter != headers.end()){
                length = std::stol((*length_iter).second);
            }
            return length;
        }

        bool read_body(WiFiClient &client, long length, std::string &dest){
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

        bool parse_request_line(WiFiClient &client, uint8_t *method, std::string &path){
            String status = client.readStringUntil('\n');
            // Serial.print("[HTTPClient::parse_status_line] status line: ");
            // Serial.println(status);

            char method_buf[5], path_buf[10240];

            if (sscanf(status.c_str(),
                       "%4s %10239[^ ] HTTP/%*s\r",
                       method_buf, path_buf) != 2){
                Serial.print("[HTTP::Common::parse_request_line] can't parse request line, line is ");
                Serial.println(status);
                return false;
            }

            path_buf[10239] = '\0'; // paranoid

            if (strcmp(method_buf, "GET") == 0){
                *method = REQUEST_METHOD_GET;
            } else if (strcmp(method_buf, "POST") == 0){
                *method = REQUEST_METHOD_POST;
            } else {
                return false;
            }
            
            path.assign(path_buf, strlen(path_buf));
            return true;
        }
    };

    namespace Shortcut {
        HTTP::Response POST(WiFiClient &client,
                            uint8_t method,
                            std::string &host,
                            const char url[],
                            HTTP::Type::post_data &data){
            std::ostringstream data_stream;
            for (auto pair = data.begin(); pair != data.end(); ++pair){
                data_stream << (*pair).first << "=" << (*pair).second << "&";
            }

            std::string data_str = data_stream.str();

            HTTP::Request req;
            req.method = method;
            req.headers["Host"] = host;
            req.path = url;
            req.body = data_str;
            req.headers["Connection"] = "Keep-Alive";
            req.headers["Content-Type"] = "application/x-www-form-urlencoded";
            req.headers["Content-Length"] = std::to_string(data_str.length());

            HTTP::Response resp;

            Serial.println("[HTTP::Shortcut::POST] Request data is ");
            Serial.println(req.render().c_str());

            //if (client.connect(host.c_str(), port)){
                client.print(req.render().c_str());
                resp.success = resp.parse(client);
            //} else {
            //    Serial.println("[HTTP::Shortcut::POST] can't connect to host");
            //}

            return resp;
        }
    }
    
    bool Request::parse(WiFiClient &client){
        client.setTimeout(HTTP::TIMEOUT);

        if (!HTTP::Common::parse_request_line(client, &method, path)
            || !HTTP::Common::parse_headers(client, headers)){
            return false;
        }

        if (method == REQUEST_METHOD_POST){
            long length = HTTP::Common::determine_body_length(headers);
            HTTP::Common::read_body(client, length, body);
        }

        return true;
    }

    std::string Request::render(){   
        std::ostringstream request_data;
        if (method == REQUEST_METHOD_POST){
            request_data << "POST ";
        } else {
            request_data << "GET ";
        }

        request_data << path << " HTTP/1.1\r\n";

        for (auto header = headers.begin(); header != headers.end(); ++header){
            request_data << (*header).first << ": " << (*header).second << "\r\n";
        }

        request_data << "\r\n";
        request_data << body;

        return request_data.str();
    }

    bool Response::parse(WiFiClient &client){
        client.setTimeout(HTTP::TIMEOUT);

        if ((!HTTP::Common::parse_status_line(client, &status_code))
            || (!HTTP::Common::parse_headers(client, headers))){
            return false;
        }

        long length = HTTP::Common::determine_body_length(headers);
        HTTP::Common::read_body(client, length, body);

        return true;
    }

    std::string Response::render(){
        std::map<uint16_t, std::string> STATUS_CODE_NAME = {
            {200, "OK"},
            {400, "Bad Request"},
            {404, "Not Found"},
            {500, "Internal Server Error"}
        };
        
        std::ostringstream request_data;
        request_data << "HTTP/1.1 " << status_code << " " << STATUS_CODE_NAME[status_code] << "\r\n";

        for (auto header = headers.begin(); header != headers.end(); ++header){
            request_data << (*header).first << ": " << (*header).second << "\r\n";
        }

        request_data << "\r\n";
        request_data << body;

        return request_data.str();
    }
}