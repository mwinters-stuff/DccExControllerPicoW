#include "WitServer.h"

void WitServer::witServiceLoop() {
    if (dccexConnectionState == CONNECTION_STATE_DISCONNECTED) {
        browseWitService();
    }

    if (dccexConnectionState == CONNECTION_STATE_ENTRY_REQUIRED) {
        enterWitServer();
    }

    if ((dccexConnectionState == CONNECTION_STATE_SELECTED) || (dccexConnectionState == CONNECTION_STATE_ENTERED)) {
        connectWitServer();
    }
}

void WitServer::browseWitService() {
    // debug_println("browseWitService()");

    keypadUseType = KEYPAD_USE_SELECT_WITHROTTLE_SERVER;

    double startTime = millis();
    double nowTime = startTime;

    const char *service = "withrottle";
    const char *proto = "tcp";

    // debug_printf("Browsing for service _%s._%s.local. on %s ... ", service, proto, selectedSsid.c_str());
    clearOledArray();
    oledText[0] = appName;
    oledText[6] = appVersion;
    oledText[1] = selectedSsid;
    oledText[2] = MSG_BROWSING_FOR_SERVICE;
    writeOledArray(false, false, true, true);

    noOfWitServices = 0;
    if ((selectedSsid.substr(0, 6) == "DCCEX_") && (selectedSsid.length() == 12)) {
        // debug_println(MSG_BYPASS_WIT_SERVER_SEARCH);
        oledText[1] = MSG_BYPASS_WIT_SERVER_SEARCH;
        writeOledArray(false, false, true, true);
        delay(500);
    } else {
        while ((noOfWitServices == 0) && ((nowTime - startTime) <= 5000)) { // try for 5 seconds
            noOfWitServices = MDNS.queryService(service, proto);
            if (noOfWitServices == 0) {
                delay(500);
                debug_print(".");
            }
            nowTime = millis();
        }
    }
    debug_println("");

    foundWitServersCount = noOfWitServices;
    if (noOfWitServices > 0) {
        for (int i = 0; ((i < noOfWitServices) && (i < maxFoundWitServers)); ++i) {
            foundWitServersNames[i] = MDNS.hostname(i);
            // foundWitServersIPs[i] = MDNS.IP(i);
            foundWitServersIPs[i] = ESPMDNS_IP_ATTRIBUTE_NAME;
            foundWitServersPorts[i] = MDNS.port(i);
            // debug_print("txt 0: key: "); debug_print(MDNS.txtKey(i, 0)); debug_print(" value: '"); debug_print(MDNS.txt(i, 0)); debug_println("'");
            // debug_print("txt 1: key: "); debug_print(MDNS.txtKey(i, 1)); debug_print(" value: '"); debug_print(MDNS.txt(i, 1)); debug_println("'");
            // debug_print("txt 2: key: "); debug_print(MDNS.txtKey(i, 2)); debug_print(" value: '"); debug_print(MDNS.txt(i, 2)); debug_println("'");
            // debug_print("txt 3: key: "); debug_print(MDNS.txtKey(i, 3)); debug_print(" value: '"); debug_println(MDNS.txt(i, 3)); debug_println("'");
            if (MDNS.hasTxt(i, "jmri")) {
                std::string node = MDNS.txt(i, "node");
                std::transform(node.begin(), node.end(), node.begin(), ::tolower);
                if (foundWitServersNames[i] == node) {
                    foundWitServersNames[i] = "JMRI  (v" + MDNS.txt(i, "jmri") + ")";
                }
            }
        }
    }
    if ((selectedSsid.substr(0, 6) == "DCCEX_") && (selectedSsid.length() == 12)) {
        foundWitServersIPs[foundWitServersCount] = "192.168.4.1";
        foundWitServersPorts[foundWitServersCount] = 2560;
        foundWitServersNames[foundWitServersCount] = MSG_GUESSED_EX_CS_WIT_SERVER;
        foundWitServersCount++;
    }

    if (foundWitServersCount == 0) {
        oledText[1] = MSG_NO_SERVICES_FOUND;
        writeOledArray(false, false, true, true);
        // debug_println(oledText[1]);
        delay(1000);
        buildWitEntry();
        dccexConnectionState = CONNECTION_STATE_ENTRY_REQUIRED;

    } else {
        // debug_print(noOfWitServices);  debug_println(MSG_SERVICES_FOUND);
        clearOledArray();
        oledText[1] = MSG_SERVICES_FOUND;

        for (int i = 0; i < foundWitServersCount; ++i) {
            // Print details for each service found
            // debug_print("  "); debug_print(i); debug_print(": '"); debug_print(foundWitServersNames[i]);
            // debug_print("' ("); debug_print(foundWitServersIPs[i]); debug_print(":"); debug_print(foundWitServersPorts[i]); debug_println(")");
            if (i < 5) { // only have room for 5
                std::string truncatedIp = ".." + foundWitServersIPs[i].substr(foundWitServersIPs[i].rfind("."));
                oledText[i] = std::to_string(i) + ": " + truncatedIp + ":" + std::to_string(foundWitServersPorts[i]) + " " + foundWitServersNames[i];
            }
        }

        if (foundWitServersCount > 0) {
            // oledText[5] = menu_select_wit_service;
            setMenuTextForOled(menu_select_wit_service);
        }
        writeOledArray(false, false);

        if ((foundWitServersCount == 1) && (autoConnectToFirstWiThrottleServer)) {
            // debug_println("WiT Selection - only 1");
            selectedWitServerIP = foundWitServersIPs[0];
            selectedWitServerPort = foundWitServersPorts[0];
            selectedWitServerName = foundWitServersNames[0];
            dccexConnectionState = CONNECTION_STATE_SELECTED;
        } else {
            // debug_println("WiT Selection required");
            dccexConnectionState = CONNECTION_STATE_SELECTION_REQUIRED;
        }
    }
}

void WitServer::selectWitServer(int selection) {
    // debug_print("selectWitServer() "); debug_println(selection);

    if ((selection >= 0) && (selection < foundWitServersCount)) {
        dccexConnectionState = CONNECTION_STATE_SELECTED;
        selectedWitServerIP = foundWitServersIPs[selection];
        selectedWitServerPort = foundWitServersPorts[selection];
        selectedWitServerName = foundWitServersNames[selection];
        keypadUseType = KEYPAD_USE_OPERATION;
    }
}

void WitServer::connectWitServer() {
    // Pass the delegate instance to dccexProtocol
    dccexProtocol.setDelegate(&myDelegate);

    debug_println("Connecting to the server...");
    clearOledArray();
    setAppnameForOled();
    oledText[1] = "        " + selectedWitServerIP + " : " + std::to_string(selectedWitServerPort);
    oledText[2] = "        " + selectedWitServerName;
    oledText[3] = MSG_CONNECTING;
    writeOledArray(false, false, true, true);

    if (!client.connect(selectedWitServerIP, selectedWitServerPort)) {
        debug_println(MSG_CONNECTION_FAILED);
        oledText[3] = MSG_CONNECTION_FAILED;
        writeOledArray(false, false, true, true);
        delay(5000);

        dccexConnectionState = CONNECTION_STATE_DISCONNECTED;
        ssidConnectionState = CONNECTION_STATE_DISCONNECTED;
        ssidSelectionSource = SSID_CONNECTION_SOURCE_LIST;
        witServerIpAndPortChanged = true;

    } else {
        debug_print("Connected to server: ");
        debug_println(selectedWitServerIP);
        debug_println(selectedWitServerPort);

        // Pass the communication to WiThrottle
        #if DCCEXPROTOCOL_DEBUG == 0
        dccexProtocol.setLogStream(&Serial);
        #endif
        dccexProtocol.connect(&client);
        debug_println("WiThrottle connected");

        dccexConnectionState = CONNECTION_STATE_CONNECTED;
        setLastServerResponseTime(true);

        oledText[3] = MSG_CONNECTED;
        if (!hashShowsFunctionsInsteadOfKeyDefs) {
            // oledText[5] = menu_menu;
            setMenuTextForOled(menu_menu);
        } else {
            // oledText[5] = menu_menu_hash_is_functions;
            setMenuTextForOled(menu_menu_hash_is_functions);
        }
        writeOledArray(false, false, true, true);
        writeOledBattery();
        u8g2.sendBuffer();

        keypadUseType = KEYPAD_USE_OPERATION;
    }
}

void WitServer::enterWitServer() {
    keypadUseType = KEYPAD_USE_ENTER_WITHROTTLE_SERVER;
    if (witServerIpAndPortChanged) { // don't refresh the screen if nothing has changed
        debug_println("enterWitServer()");
        clearOledArray();
        setAppnameForOled();
        oledText[1] = MSG_NO_SERVICES_FOUND_ENTRY_REQUIRED;
        oledText[3] = witServerIpAndPortConstructed;
        // oledText[5] = menu_select_wit_entry;
        setMenuTextForOled(menu_select_wit_entry);
        writeOledArray(false, false, true, true);
        witServerIpAndPortChanged = false;
    }
}

void WitServer::disconnectWitServer() {
    debug_println("disconnectWitServer()");
    for (int i = 0; i < maxThrottles; i++) {
        releaseAllLocos(i);
    }
    dccexProtocol.disconnect();
    debug_println("Disconnected from wiThrottle server\n");
    clearOledArray();
    oledText[0] = MSG_DISCONNECTED;
    writeOledArray(false, false, true, true);
    dccexConnectionState = CONNECTION_STATE_DISCONNECTED;
    witServerIpAndPortChanged = true;
}

void WitServer::witEntryAddChar(char key) {
    if (witServerIpAndPortEntered.length() < 17) {
        witServerIpAndPortEntered += key;
        // debug_print("wit entered: ");
        // debug_println(witServerIpAndPortEntered);
        buildWitEntry();
        witServerIpAndPortChanged = true;
    }
}

void WitServer::witEntryDeleteChar(char key) {
    if (witServerIpAndPortEntered.length() > 0) {
        witServerIpAndPortEntered = witServerIpAndPortEntered.substr(0, witServerIpAndPortEntered.length() - 1);
        // debug_print("wit deleted: ");
        // debug_println(witServerIpAndPortEntered);
        buildWitEntry();
        witServerIpAndPortChanged = true;
    }
}

void WitServer::buildWitEntry() {
    debug_println("buildWitEntry()");
    witServerIpAndPortConstructed = "";
    for (size_t i = 0; i < witServerIpAndPortEntered.length(); i++) {
        if ((i == 3) || (i == 6) || (i == 9)) {
            witServerIpAndPortConstructed += ".";
        } else if (i == 12) {
            witServerIpAndPortConstructed += ":";
        }
        witServerIpAndPortConstructed += witServerIpAndPortEntered.substr(i, 1);
    }
    debug_print("wit Constructed: ");
    debug_println(witServerIpAndPortConstructed);
    if (witServerIpAndPortEntered.length() < 17) {
        witServerIpAndPortConstructed += witServerIpAndPortEntryMask.substr(witServerIpAndPortConstructed.length());
    }
    debug_print("wit Constructed: ");
    debug_println(witServerIpAndPortConstructed);

    if (witServerIpAndPortEntered.length() == 17) {
        selectedWitServerIP = witServerIpAndPortConstructed.substr(0, 15);
        selectedWitServerPort = std::stoi(witServerIpAndPortConstructed.substr(16));
    }
}