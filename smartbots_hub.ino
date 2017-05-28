#include "radio.h"
#include "smartbots_api.h"
#include <printf.h>
#include <string>
#include <future>
#include <vector>
#include <mutex>
#include <utility>

std::string wifi_ssid="Floor 5";
std::string wifi_password="123haiphong";
std::string host="smart-bots.xyz";
short port=80;

SmartBotsAPI api(host, port, "Tt7yfpljvOxw28BOpYT6lnE41RcGDqGIDC6QluL3Y1vE8SEv4U");

std::vector<
	std::future<
		bool>> async_queue;

void handler(receive_msg msg){
	async_queue.push_back(std::async(std::launch::async, [](receive_msg msg){
		return api.send_cmd(msg);
	}, msg));
}

void setup(){
	printf_begin();
	Radio.setup(true);
	Serial.begin(2000000);
	Radio.register_receive_handler(handler);
	Serial.println("[setup] Connecting to wifi");
	if (!api.setup(wifi_ssid, wifi_password)){
		Serial.println("[setup] Connect to wifi failed");
	} else {
		Serial.println("[setup] Connect to wifi ok");
	}
}

void loop(){
	cmd_vector cmds;
	api.get_pending_cmds(cmds);

	for (auto cmd = cmds.begin(); cmd != cmds.end(); cmd++){
		Radio.send(*cmd);
	}

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