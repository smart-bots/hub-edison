#include "wifi_hotspot.h"

void hostapd_cfg_change_value(std::string name, std::string value){
	std::ifstream cfg_file(HOSTAPD_CONFIG_PATH);
	std::ofstream tmp_cfg_file(HOSTAPD_TMP_CONFIG_FILE);

	std::string line;
	while (std::getline(cfg_file, line)){
		if (line.find(name + "=") == 0){
			Serial.println("[hostapd_cfg_change_value] found config");
			Serial.print("[hostapd_cfg_change_value] original line was: ");
			Serial.println(line.c_str());
			tmp_cfg_file << name << "=" << value << std::endl;
		} else {
			tmp_cfg_file << line << std::endl;
		}
	}

	cfg_file.close();
	tmp_cfg_file.close();

	system("mv " HOSTAPD_TMP_CONFIG_FILE " " HOSTAPD_CONFIG_PATH);
}

void start_hotspot(std::string ssid, std::string password){
	hostapd_cfg_change_value("ssid", ssid);
	hostapd_cfg_change_value("wpa_passphrase", password);

	//system("systemctl stop wpa_supplicant");
	//system("systemctl restart hostapd");
}