#include "radio.h"
#include "smartbots_api.h"
#include "http_server.h"
#include "wifi_hotspot.h"
#include "config.h"
#include <printf.h>
#include <string>
#include <future>
#include <vector>
#include <mutex>
#include <utility>

SmartBotsAPI api;

std::vector<
	std::future<
		std::pair<receive_msg, bool>>> async_queue;

void handler(receive_msg msg){
	async_queue.push_back(std::async(std::launch::async, [](receive_msg msg){
		return std::make_pair(msg, api.send_cmd(msg));
	}, msg));
}

HTTP::Server server(80);

void setup(){
	printf_begin();
	Serial.begin(115200);
	
	if (!Radio.setup(true)){
		Serial.println("[setup] RF init failed");
	} else {
		Serial.println("[setup] RF init ok");
	}
	Radio.register_receive_handler(handler);

	api.setup(Config.host, Config.port, Config.token);

	Config.read_from_file();

	start_hotspot("tuankiet65", "tuankiet65");
	server.setup();

	// Serial.println("[setup] Connecting to wifi");
	// if (!api.setup(wifi_ssid, wifi_password)){
	// 	Serial.println("[setup] Connect to wifi failed");
	// } else {
	// 	Serial.println("[setup] Connect to wifi ok");
	// }
}

void loop(){
	// cmd_queue cmds;

	// long time = millis();
	// api.get_pending_cmds(cmds);
	// Serial.print("[loop] Request time is ");
	// Serial.print(millis() - time);
	// Serial.println("ms");

	// for (auto cmd = cmds.begin(); cmd != cmds.end(); cmd++){
	// 	Radio.send(*cmd);
	// }

//	delay(1000);
	server.handle_request();

	// async_queue_lock.lock();
	// for (auto async_future = async_queue.begin(); async_future != async_queue.end(); ){
	// 	if (*async_future.wait_for(0) == std::future_status::ready){
	// 		std::pair<receive_msg, bool>> result = *async_future.get();
	// 		if (result.second){
	// 			// success
	// 			async_future = async_queue.remove(async_future);
	// 		} else {
	// 			// fail				
	// 			fail_flag = true;
	// 			async_result.push_back(std::async([](receive_msg msg){
	// 				return {msg, api.send_cmd(msg)};
	// 			}, msg));
	// 		}
	// 	} else {

	// 	}
	// }
	// async_queue_lock.unlock();

	// if (fail_flag){
	// 	if (!api.setup(wifi_ssid, wifi_password)){
	// 		Serial.println("[setup] Connect to wifi failed");
	// 	} else {
	// 		Serial.println("[setup] Connect to wifi ok");
	// 		fail_flag = false;
	// 	}	
	// }
}