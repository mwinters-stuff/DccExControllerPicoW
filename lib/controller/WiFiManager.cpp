#include "WiFiManager.h"



void WiFiManager::browseSsids() {
    // int n = WiFi.scanNetworks();
    // for (int i = 0; i < n; ++i) {
    //     Serial.println(WiFi.SSID(i));
    // }
}

void WiFiManager::selectSsid(int selection) {
    // if (selection >= 0 && selection < WiFi.scanNetworks()) {
    //     selectedSsid = WiFi.SSID(selection);
    // }
}

void WiFiManager::connectSsid(const std::string& ssid, const std::string& password) {
    // if (selectedSsid.length() > 0) {
    //     WiFi.begin(selectedSsid.c_str(), selectedSsidPassword.c_str());
    //     while (WiFi.status() != WL_CONNECTED) {
    //         delay(1000);
    //         Serial.println("Connecting to WiFi...");
    //     }
    //     Serial.println("Connected to WiFi");
    // }
}

void WiFiManager::enterSsidPassword() {
    // Serial.println("Enter SSID password:");
    // while (true) {
    //     if (Serial.available() > 0) {
    //         char c = Serial.read();
    //         if (c == '\n') break;
    //         selectedSsidPassword += c;
    //     }
    // }
}

void WiFiManager::showListOfSsids() {
    // int n = WiFi.scanNetworks();
    // for (int i = 0; i < n; ++i) {
    //     Serial.println(WiFi.SSID(i));
    // }
}

void WiFiManager::ssidPasswordAddChar(char key) {
    // selectedSsidPassword += key;
}

void WiFiManager::ssidPasswordDeleteChar(char key) {
    // if (!selectedSsidPassword.isEmpty()) {
    //     selectedSsidPassword.remove(selectedSsidPassword.length() - 1);
    // }
}

void WiFiManager::getSsidPasswordAndWitIpForFound() {
    // Assuming the SSID contains the password and IP in a specific format
    // if (selectedSsid.startsWith("DCCEX_") && selectedSsid.length() == 12) {
    //     selectedSsidPassword = selectedSsid.substring(6, 12);
    //     selectedWitServerIP.fromString(selectedSsid.substring(6, 12));
    // }
}

void WiFiManager::selectSsidFromFound(int selection) {
    // if (selection >= 0 && selection < foundSsidsCount) {
    //     selectedSsid = foundSsids[selection];
    //     selectedSsidPassword = ""; // Reset password
    // }
}

void WiFiManager::enterWitServer() {
    // Serial.println("Enter WiThrottle server IP and port:");
    // while (true) {
    //     if (Serial.available() > 0) {
    //         char c = Serial.read();
    //         if (c == '\n') break;
    //         witServerIpAndPortEntered += c;
    //     }
    // }
}

void WiFiManager::disconnectWitServer() {
    // client.stop();
    // Serial.println("Disconnected from WiThrottle server");
}

void WiFiManager::requestHeartbeat() {
    // if (client.connected()) {
    //     client.print("Heartbeat");
    //     lastHeartBeatSentTime = millis();
    // }
}