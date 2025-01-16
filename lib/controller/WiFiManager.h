// WiFiManager.h
#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <string>
#include <memory>
#include "Base.h"

class WiFiManager: public Base {
public:
  static std::shared_ptr<WiFiManager> getInstance() {
    static std::shared_ptr<WiFiManager> instance = std::make_shared<WiFiManager>();
    return instance;
  }

  void setup() override;
  void loop() override{};

  void browseSsids();
  void connectSsid(const std::string &ssid, const std::string &password);
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

  ~WiFiManager() = default;
  WiFiManager() = default;
private:
  // String selectedSsid;
  // String selectedSsidPassword;
  // int ssidConnectionState;
  // String foundSsids[10]; // Adjust size as needed
  // int foundSsidsCount;

  WiFiManager(const WiFiManager &) = delete;
  WiFiManager &operator=(const WiFiManager &) = delete;
};

#endif // WIFIMANAGER_H