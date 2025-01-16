#include "WitServer.h"
#include "ExternalSingletons.h"
#include "GlobalValues.h"
#include "OledDisplay.h"
#include "FunctionStates.h"
#include "Throttles.h"
#include "static.h"

#include <lwip/tcp.h>
#include <lwip/ip_addr.h>
#include <lwip/dns.h>
#include <lwip/netif.h>
#include <lwip/timeouts.h>
#include "wifi_connection.h"
#include "print_stream.h"


extern bool hashShowsFunctionsInsteadOfKeyDefs;

PrintStream printStream;

void displayUpdateFromWit(int multiThrottleIndex) {
  debug_print("displayUpdateFromWit(): keyapdeUseType "); debug_print(std::to_string(globalValues->keypadUseType).c_str()); 
  // debug_print(" menuIsShowing "); debug_print(menuIsShowing);
  // debug_print(" multiThrottleIndex "); debug_print(multiThrottleIndex);
  // debug_println("");
  if ( (globalValues->keypadUseType==KEYPAD_USE_OPERATION) && (!oledDisplay->menuIsShowing) 
  && (multiThrottleIndex==throttles->currentThrottleIndex) ) {
    oledDisplay->writeOledSpeed();
  }
}

// dccexProtocol Delegate class
class MyDelegate : public DCCExController::DCCEXProtocolDelegate {
  
  public:
    void heartbeatConfig(int seconds) { 
      debug_print("Received heartbeat. From: "); debug_print(std::to_string(globalValues->heartBeatPeriod).c_str()); 
      debug_print(" To: "); debug_println(std::to_string(seconds).c_str()); 
      globalValues->heartBeatPeriod = seconds;
    }
    void receivedServerVersion(int major, int minor, int patch) {
      debug_print("\n\nReceived version: ");
      debug_print(std::to_string(major).c_str());
      debug_print(".");
      debug_print(std::to_string(minor).c_str());
      debug_print(".");
      debug_println(std::to_string(patch).c_str());
    }
    void receivedMessage(char* message) {
      debug_print("Broadcast Message: ");
      debug_println(message);
      oledDisplay->broadcastMessageText = std::string(message);
      oledDisplay->broadcastMessageTime = millis();
      refreshOled();
    }
    virtual void receivedLocoUpdate(DCCExController::Loco* loco) {
      throttles->debugLocoSpeed("receivedLocoUpdate: ", loco); 
      dccExController->_processLocoUpdate(loco);
    }
    void receivedTrackPower(DCCExController::TrackPower state) { 
      debug_print("Received TrackPower: "); debug_println(state == DCCExController::PowerOn ? "On" : "Off");
      if (globalValues->trackPower != state) {
        globalValues->trackPower = state;
        // displayUpdateFromWit(-1); // dummy multithrottlehumm
        refreshOled();
      }
    }
    // void receivedIndividualTrackPower(TrackPower state, int track) { 
    //   debug_print("Received Indivdual TrackPower: "); 
    //   debug_print(state);
    //   debug_print(" : ");
    //   debug_println(track);
    // }
    void receivedRosterList() {
      debug_println("Received Roster List");
      dccExController->_loadRoster();
    }
    void receivedTurnoutList() {
      debug_println("Received Turnouts/Points list");
      dccExController->_loadTurnoutList();
    }    
    void receivedRouteList() {
      debug_println("Received Routes List");
      dccExController->_loadRouteList();
    }

    void refreshOled() {
     debug_print("refreshOled(): ");
     debug_println(std::to_string(oledDisplay->lastOledScreen).c_str());
        switch (oledDisplay->lastOledScreen) {
            case last_oled_screen_speed:
            oledDisplay->writeOledSpeed();
            break;
            case last_oled_screen_turnout_list:
            oledDisplay->writeOledTurnoutList(oledDisplay->lastOledStringParameter, oledDisplay->lastOledBooleanParameter);
            break;
            case last_oled_screen_route_list:
            oledDisplay->writeOledRouteList(oledDisplay->lastOledStringParameter);
            break;
            case last_oled_screen_function_list:
            oledDisplay->writeOledFunctionList(oledDisplay->lastOledStringParameter);
            break;
            case last_oled_screen_menu:
            oledDisplay->writeOledMenu(oledDisplay->lastOledStringParameter);
            break;
            case last_oled_screen_extra_submenu:
            oledDisplay->writeOledExtraSubMenu();
            break;
            case last_oled_screen_all_locos:
            oledDisplay->writeOledAllLocos(oledDisplay->lastOledBooleanParameter);
            break;
            case last_oled_screen_edit_consist:
            oledDisplay->writeOledEditConsist();
            break;
            case last_oled_screen_direct_commands:
            oledDisplay->writeOledDirectCommands();
            break;
        }
        }
};

MyDelegate myDelegate;

void WitServer::setup() {
    
}

void WitServer::loop() {
    if (globalValues->dccexConnectionState == CONNECTION_STATE_DISCONNECTED) {
        browseWitService();
    }

    if (globalValues->dccexConnectionState == CONNECTION_STATE_ENTRY_REQUIRED) {
        enterWitServer();
    }

    if ((globalValues->dccexConnectionState == CONNECTION_STATE_SELECTED) || (globalValues->dccexConnectionState == CONNECTION_STATE_ENTERED)) {
        connectWitServer();
    }
}

void WitServer::browseWitService() {
    // debug_println("browseWitService()");

    globalValues->keypadUseType = KEYPAD_USE_SELECT_WITHROTTLE_SERVER;

    double startTime = pimoroni::millis();
    double nowTime = startTime;

    const char *service = "withrottle";
    const char *proto = "tcp";

    // debug_printf("Browsing for service _%s._%s.local. on %s ... ", service, proto, selectedSsid.c_str());
    oledDisplay->clearOledArray();
    oledDisplay->oledText[0] = appName;
    oledDisplay->oledText[6] = appVersion;
    oledDisplay->oledText[1] = globalValues->selectedSsid;
    oledDisplay->oledText[2] = MSG_BROWSING_FOR_SERVICE;
    oledDisplay->writeOledArray(false, false, true, true);

    globalValues->noOfWitServices = 0;
    if ((globalValues->selectedSsid.substr(0, 6) == "DCCEX_") && (globalValues->selectedSsid.length() == 12)) {
        // debug_println(MSG_BYPASS_WIT_SERVER_SEARCH);
        oledDisplay->oledText[1] = MSG_BYPASS_WIT_SERVER_SEARCH;
        oledDisplay->writeOledArray(false, false, true, true);
        sleep_ms(500);
    } else {
        while ((globalValues->noOfWitServices == 0) && ((nowTime - startTime) <= 5000)) { // try for 5 seconds
            // noOfWitServices = MDNS.queryService(service, proto);
            // if (noOfWitServices == 0) {
            //     sleep_ms(500);
            //     debug_print(".");
            // }
            nowTime = pimoroni::millis();
        }
    }
    debug_println("");

    globalValues->foundWitServersCount = globalValues->noOfWitServices;
    if (globalValues->noOfWitServices > 0) {
        for (int i = 0; ((i < globalValues->noOfWitServices) && (i < globalValues->maxFoundWitServers)); ++i) {
            // globalValues->foundWitServersNames[i] = MDNS.hostname(i);
            // // foundWitServersIPs[i] = MDNS.IP(i);
            // globalValues->foundWitServersIPs[i] = ESPMDNS_IP_ATTRIBUTE_NAME;
            // globalValues->foundWitServersPorts[i] = MDNS.port(i);
            // // debug_print("txt 0: key: "); debug_print(MDNS.txtKey(i, 0)); debug_print(" value: '"); debug_print(MDNS.txt(i, 0)); debug_println("'");
            // // debug_print("txt 1: key: "); debug_print(MDNS.txtKey(i, 1)); debug_print(" value: '"); debug_print(MDNS.txt(i, 1)); debug_println("'");
            // // debug_print("txt 2: key: "); debug_print(MDNS.txtKey(i, 2)); debug_print(" value: '"); debug_print(MDNS.txt(i, 2)); debug_println("'");
            // // debug_print("txt 3: key: "); debug_print(MDNS.txtKey(i, 3)); debug_print(" value: '"); debug_println(MDNS.txt(i, 3)); debug_println("'");
            // if (MDNS.hasTxt(i, "jmri")) {
            //     std::string node = MDNS.txt(i, "node");
            //     std::transform(node.begin(), node.end(), node.begin(), ::tolower);
            //     if (foundWitServersNames[i] == node) {
            //         foundWitServersNames[i] = "JMRI  (v" + MDNS.txt(i, "jmri") + ")";
            //     }
            // }
        }
    }
    if ((globalValues->selectedSsid.substr(0, 6) == "DCCEX_") && (globalValues->selectedSsid.length() == 12)) {
        globalValues->foundWitServersIPs[globalValues->foundWitServersCount] = "192.168.4.1";
        globalValues->foundWitServersPorts[globalValues->foundWitServersCount] = 2560;
        globalValues->foundWitServersNames[globalValues->foundWitServersCount] = MSG_GUESSED_EX_CS_WIT_SERVER;
        globalValues->foundWitServersCount++;
    }

    if (globalValues->foundWitServersCount == 0) {
        oledDisplay->oledText[1] = MSG_NO_SERVICES_FOUND;
        oledDisplay->writeOledArray(false, false, true, true);
        // debug_println(oledText[1]);
        sleep_ms(1000);
        buildWitEntry();
        globalValues->dccexConnectionState = CONNECTION_STATE_ENTRY_REQUIRED;

    } else {
        // debug_print(noOfWitServices);  debug_println(MSG_SERVICES_FOUND);
        oledDisplay->clearOledArray();
        oledDisplay->oledText[1] = MSG_SERVICES_FOUND;

        for (int i = 0; i < globalValues->foundWitServersCount; ++i) {
            // Print details for each service found
            // debug_print("  "); debug_print(i); debug_print(": '"); debug_print(foundWitServersNames[i]);
            // debug_print("' ("); debug_print(foundWitServersIPs[i]); debug_print(":"); debug_print(foundWitServersPorts[i]); debug_println(")");
            if (i < 5) { // only have room for 5
                std::string truncatedIp = ".." + globalValues->foundWitServersIPs[i].substr(globalValues->foundWitServersIPs[i].rfind("."));
                oledDisplay->oledText[i] = std::to_string(i) + ": " + truncatedIp + ":" + std::to_string(globalValues->foundWitServersPorts[i]) + " " + globalValues->foundWitServersNames[i];
            }
        }

        if (globalValues->foundWitServersCount > 0) {
            // oledText[5] = menu_select_wit_service;
            oledDisplay->setMenuTextForOled(menu_select_wit_service);
        }
        oledDisplay->writeOledArray(false, false);

        if ((globalValues->foundWitServersCount == 1) && (globalValues->autoConnectToFirstWiThrottleServer)) {
            // debug_println("WiT Selection - only 1");
            globalValues->selectedWitServerIP = globalValues->foundWitServersIPs[0];
            globalValues->selectedWitServerPort = globalValues->foundWitServersPorts[0];
            globalValues->selectedWitServerName = globalValues->foundWitServersNames[0];
            globalValues->dccexConnectionState = CONNECTION_STATE_SELECTED;
        } else {
            // debug_println("WiT Selection required");
            globalValues->dccexConnectionState = CONNECTION_STATE_SELECTION_REQUIRED;
        }
    }
}

void WitServer::selectWitServer(int selection) {
    // debug_print("selectWitServer() "); debug_println(selection);

    if ((selection >= 0) && (selection < globalValues->foundWitServersCount)) {
        globalValues->dccexConnectionState = CONNECTION_STATE_SELECTED;
        globalValues->selectedWitServerIP = globalValues->foundWitServersIPs[selection];
        globalValues->selectedWitServerPort = globalValues->foundWitServersPorts[selection];
        globalValues->selectedWitServerName = globalValues->foundWitServersNames[selection];
        globalValues->keypadUseType = KEYPAD_USE_OPERATION;
    }
}

static err_t connect_callback(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err == ERR_OK) {
        printf("Connected to server\n");
        TCPSocketStream *stream = new TCPSocketStream(tpcb);
        // Now you can use the stream to read/write data

        debug_print("Connected to server: ");
        debug_println(globalValues->selectedWitServerIP.c_str());
        debug_println(std::to_string(globalValues->selectedWitServerPort).c_str());

        dccExProtocol->connect(stream);
        debug_println("WiThrottle connected");

        globalValues->dccexConnectionState = CONNECTION_STATE_CONNECTED;
        witServer->setLastServerResponseTime(true);

        oledDisplay->oledText[3] = MSG_CONNECTED;
        if (!hashShowsFunctionsInsteadOfKeyDefs) {
            // oledText[5] = menu_menu;
            oledDisplay->setMenuTextForOled(menu_menu);
        } else {
            // oledText[5] = menu_menu_hash_is_functions;
            oledDisplay->setMenuTextForOled(menu_menu_hash_is_functions);
        }
        oledDisplay->writeOledArray(false, false, true, true);
        oledDisplay->writeOledBattery();
        globalValues->keypadUseType = KEYPAD_USE_OPERATION;

        // dccexProtocol.connect(stream);
        // dccexProtocol.enableHeartbeat();
        // dccexProtocol.requestServerVersion();
        // dccexProtocolConnected  = true;
    } else {
        printf("Connection failed\n");
        
        debug_println(MSG_CONNECTION_FAILED);
        oledDisplay->oledText[3] = MSG_CONNECTION_FAILED;
        oledDisplay->writeOledArray(false, false, true, true);
        sleep_ms(5000);

        globalValues->dccexConnectionState = CONNECTION_STATE_DISCONNECTED;
        globalValues->ssidConnectionState = CONNECTION_STATE_DISCONNECTED;
        globalValues->ssidSelectionSource = SSID_CONNECTION_SOURCE_LIST;
        globalValues->witServerIpAndPortChanged = true;

        tcp_close(tpcb);
    }
    return err;
}

// Function to initiate a connection to the server
void connect_to_server(const char *server_ip, uint16_t port) {
    ip_addr_t server_addr;
    if (!ipaddr_aton(server_ip, &server_addr)) {
        printf("Invalid IP address\n");
        return;
    }

    struct tcp_pcb *pcb = tcp_new();
    if (pcb == nullptr) {
        printf("Failed to create PCB\n");
        return;
    }

    tcp_connect(pcb, &server_addr, port, connect_callback);
}

void WitServer::connectWitServer() {
    // Pass the delegate instance to dccexProtocol
    dccExProtocol->setLogStream(&printStream);
    dccExProtocol->setDelegate(&myDelegate);

    debug_println("Connecting to the server...");
    oledDisplay->clearOledArray();
    oledDisplay->setAppnameForOled();
    oledDisplay->oledText[1] = "        " + globalValues->selectedWitServerIP + " : " + std::to_string(globalValues->selectedWitServerPort);
    oledDisplay->oledText[2] = "        " + globalValues->selectedWitServerName;
    oledDisplay->oledText[3] = MSG_CONNECTING;
    oledDisplay->writeOledArray(false, false, true, true);

    connect_to_server(globalValues->selectedWitServerIP.c_str(), globalValues->selectedWitServerPort);
}

void WitServer::enterWitServer() {
    globalValues->keypadUseType = KEYPAD_USE_ENTER_WITHROTTLE_SERVER;
    if (globalValues->witServerIpAndPortChanged) { // don't refresh the screen if nothing has changed
        debug_println("enterWitServer()");
        oledDisplay->clearOledArray();
        oledDisplay->setAppnameForOled();
        oledDisplay->oledText[1] = MSG_NO_SERVICES_FOUND_ENTRY_REQUIRED;
        oledDisplay->oledText[3] = globalValues->witServerIpAndPortConstructed;
        // oledText[5] = menu_select_wit_entry;
        oledDisplay->setMenuTextForOled(menu_select_wit_entry);
        oledDisplay->writeOledArray(false, false, true, true);
        globalValues->witServerIpAndPortChanged = false;
    }
}

void WitServer::disconnectWitServer() {
    debug_println("disconnectWitServer()");
    for (int i = 0; i < throttles->maxThrottles; i++) {
        throttles->releaseAllLocos(i);
    }
    dccExProtocol->disconnect();
    debug_println("Disconnected from wiThrottle server\n");
    oledDisplay->clearOledArray();
    oledDisplay->oledText[0] = MSG_DISCONNECTED;
    oledDisplay->writeOledArray(false, false, true, true);
    globalValues->dccexConnectionState = CONNECTION_STATE_DISCONNECTED;
    globalValues->witServerIpAndPortChanged = true;
}

void WitServer::witEntryAddChar(char key) {
    if (globalValues->witServerIpAndPortEntered.length() < 17) {
        globalValues->witServerIpAndPortEntered += key;
        // debug_print("wit entered: ");
        // debug_println(globalValues->witServerIpAndPortEntered);
        buildWitEntry();
        globalValues->witServerIpAndPortChanged = true;
    }
}

void WitServer::witEntryDeleteChar(char key) {
    if (globalValues->witServerIpAndPortEntered.length() > 0) {
        globalValues->witServerIpAndPortEntered = globalValues->witServerIpAndPortEntered.substr(0, globalValues->witServerIpAndPortEntered.length() - 1);
        // debug_print("wit deleted: ");
        // debug_println(globalValues->witServerIpAndPortEntered);
        buildWitEntry();
        globalValues->witServerIpAndPortChanged = true;
    }
}

void WitServer::buildWitEntry() {
    debug_println("buildWitEntry()");
    globalValues->witServerIpAndPortConstructed = "";
    for (size_t i = 0; i < globalValues->witServerIpAndPortEntered.length(); i++) {
        if ((i == 3) || (i == 6) || (i == 9)) {
            globalValues->witServerIpAndPortConstructed += ".";
        } else if (i == 12) {
            globalValues->witServerIpAndPortConstructed += ":";
        }
        globalValues->witServerIpAndPortConstructed += globalValues->witServerIpAndPortEntered.substr(i, 1);
    }
    debug_print("wit Constructed: ");
    debug_println(globalValues->witServerIpAndPortConstructed.c_str());
    if (globalValues->witServerIpAndPortEntered.length() < 17) {
        globalValues->witServerIpAndPortConstructed += witServerIpAndPortEntryMask.substr(globalValues->witServerIpAndPortConstructed.length());
    }
    debug_print("wit Constructed: ");
    debug_println(globalValues->witServerIpAndPortConstructed.c_str());

    if (globalValues->witServerIpAndPortEntered.length() == 17) {
        globalValues->selectedWitServerIP = globalValues->witServerIpAndPortConstructed.substr(0, 15);
        globalValues->selectedWitServerPort = std::stoi(globalValues->witServerIpAndPortConstructed.substr(16));
    }
}

void WitServer::setLastServerResponseTime(bool force) {
  // debug_print("setLastServerResponseTime "); debug_println((force) ? "True": "False");
  // debug_print("lastServerResponseTime "); debug_print(lastServerResponseTime);
  // debug_print("  millis "); debug_println(millis() / 1000);

  globalValues->lastServerResponseTime = dccExProtocol->getLastServerResponseTime() / 1000;
  if ( (globalValues->lastServerResponseTime==0) || (force) ) {
    globalValues->lastServerResponseTime = millis() /1000;
    globalValues->lastHeartBeatSentTime = millis() /1000;
  }
  // debug_print("setLastServerResponseTime "); debug_println(lastServerResponseTime);
}

void WitServer::checkForShutdownOnNoResponse() {
  if (pimoroni::millis()-globalValues->startWaitForSelection > 240000) {  // 4 minutes
      debug_println("Waited too long for a selection. Shutting down");
    //   deepSleepStart(SLEEP_REASON_INACTIVITY);
  }
}