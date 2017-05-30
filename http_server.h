#ifndef WIFI_SERVER_H

#define WIFI_SERVER_H

#include "http.h"
#include "config.h"
#include <WiFiServer.h>

const char index_page_template[] = "<!DOCTYPE html><meta charset='utf-8'><meta content='ie=edge' http-equiv='x-ua-compatible'><meta content='width=device-width, initial-scale=1.0' name='viewport'><title>Setting up your hub</title><style>*{-moz-box-sizing: border-box; box-sizing: border-box}body{font: 16px Helvetica; background: #36404a}section{width: 275px; background: #ecf0f1; padding: 0 30px 30px 30px; margin: 60px auto; text-align: center; border-radius: 5px}span{display: block; position: relative; margin: 0 auto; top: -40px; height: 80px; width: 80px; font-size: 70px; border-radius: 50%%; background-color: #5fbeaa; color: #fff; box-shadow: 1px 1px 2px rgba(0, 0, 0, .3)}h1{font-size: 24px; font-weight: 100; margin-bottom: 30px; margin-top: -10px}input{width: 100%%; background: #bdc3c7; border: none; height: 30px; margin-bottom: 15px; border-radius: 5px; text-align: center; font-size: 14px; color: #7f8c8d}input:focus{outline: 0}button{width: 100%%; height: 30px; border: none; background: #5fbeaa; color: #ecf0f1; font-weight: 100; margin-bottom: 15px; border-radius: 5px; transition: all ease-in-out .2s; border-bottom: 3px solid #56a695}button:focus{outline: 0}button:hover{background: #56a695}</style><body> <script>function genv(name){return document.getElementsByName(name)[0].value;}function setup(){var xhr=new XMLHttpRequest(); xhr.onreadystatechange=function(){if (xhttp.readyState==4 && xhttp.status==200){alert(xhttp.responseText);}}; data={'ssid': genv('ssid'), 'password': genv('password'), 'token': genv('token'), 'host': genv('host'), 'port': genv('port')}; xhr.open('POST', '/set'); xhr.setRequestHeader('Content-Type', 'application/json'); xhr.send(JSON.stringify(data)); return false;}</script> <section> <span>+</span> <h1>Hub Setup</h1> <form id='hubSetup'> <input name='ssid' placeholder='SSID' value='%s'> <input name='password' placeholder='Password' type='password' value='%s'> <input name='token' placeholder='Token' value='%s'> <input name='host' placeholder='Host' value='%s'> <input name='port' placeholder='Port' value='%u'> <button type='submit' onclick='return setup()'>Setup</button> </form> </section></body></html>";

namespace HTTP {
    class Server {
    private:
        WiFiServer server;
        bool started = false;

        std::string render_index_page();
    public:
        Server(uint16_t port): server(port) {};

        void setup(){
        	server.begin();
        }

        void handle_request();
    };
}

#endif