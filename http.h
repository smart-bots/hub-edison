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
        bool parse_status_line(WiFiClient &client, uint16_t *status_code);
        bool parse_request_line(WiFiClient &client, uint8_t *method, std::string &path);
        bool parse_headers(WiFiClient &client, HTTP::Type::headers_t &headers);
        bool read_body(WiFiClient &client, long length, std::string &dest);
        long determine_body_length(HTTP::Type::headers_t &headers);
    }

    #define REQUEST_METHOD_GET 1
    #define REQUEST_METHOD_POST 2

    class Request {
    public:
        uint8_t method = REQUEST_METHOD_GET;
        std::string path;
        HTTP::Type::headers_t headers;
        std::string body;

        bool success = false;

        Request(): path(), headers(), body() {};
        Request(WiFiClient client){
            success = parse(client);
        }

        bool parse(WiFiClient &client);
        std::string render();
    };

    class Response {
    public:
        uint16_t status_code;
        HTTP::Type::headers_t headers;
        std::string body;

        bool success = false;

        Response(): headers(), body() {};
        Response(WiFiClient client){
            success = parse(client);
        }

        bool parse(WiFiClient &client);     
        std::string render();
    };

    namespace Shortcut {
        HTTP::Response POST(WiFiClient &client,
                            uint8_t method,
                            std::string &host,
                            const char url[],
                            HTTP::Type::post_data &data);
    }
}

#endif