// WiFiManager.h
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <string>

class WiFiManager {
public:
    
      static WiFiManager &getInstance() {
    static WiFiManager instance;
    return instance;
  }

    void browseSsids();
    void connectSsid(const std::string& ssid, const std::string& password);
    void showListOfSsids();
    void enterSsidPassword();
    void selectSsid(int selection);
    void requestHeartbeat();

    void ssidPasswordAddChar(char key);
    void ssidPasswordDeleteChar(char key);
    void getSsidPasswordAndWitIpForFound();
    void selectSsidFromFound(int selection);
    void enterWitServer();
    void disconnectWitServer();

private:
    // String selectedSsid;
    // String selectedSsidPassword;
    // int ssidConnectionState;
    // String foundSsids[10]; // Adjust size as needed
    // int foundSsidsCount;

    

      WiFiManager() = default;
  ~WiFiManager() = default;
  WiFiManager(const WiFiManager &) = delete;
  WiFiManager &operator=(const WiFiManager &) = delete;
};

#endif // WIFIMANAGER_H