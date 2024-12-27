// WiFiManager.h
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
    WiFiManager();
    void browseSsids();
    void connectSsid(const String& ssid, const String& password);
    void showListOfSsids();
    void enterSsidPassword();
    void selectSsid(int selection);
    void requestHeartbeat();

private:
    String selectedSsid;
    String selectedSsidPassword;
    int ssidConnectionState;
    String foundSsids[10]; // Adjust size as needed
    int foundSsidsCount;
};

#endif // WIFIMANAGER_H