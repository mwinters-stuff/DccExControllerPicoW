#ifndef CONFIG_NETWORK_H
#define CONFIG_NETWORK_H

#include <string>
// enter the SSIDs and passwords of as many networks you wish to try to connect to.
const int maxSsids = 1;

// There must be the same number of entities (specified above) in each of the following arrays

const std::string ssids[maxSsids] = {"WINTERS"};
const std::string passwords[maxSsids] = {"tesskatt"};

// If defined this must be of the form 999999999999:99999  e.g. "19216800400102560" equals 192.168.004.001:2560
// Each block of the ip address MUST be 3 digit. Use leading zeros if needed to insure 3 digits.
// The port MUST be 5 digit. Use leading zeros if needed to insure 5 digits.
// #define DEFAULT_IP_AND_PORT      "19216800400102560"

// ********************************************************************************************

// The period that the controller waits for a connection in milliseconds. Default is 10 seconds (10000ms)
// If you are having problems connection to you network, try uncommenting the line increasing this
// #define SSID_CONNECTION_TIMEOUT 10000

// ********************************************************************************************

// Autoconnect to first SSID in the list above (default, if not specified is false)
#define AUTO_CONNECT_TO_FIRST_DEFINED_SERVER true

// Autoconnect to first found server (default, if not specified is true)
#define AUTO_CONNECT_TO_FIRST_WITHROTTLE_SERVER true

// ********************************************************************************************
// uncomment and increase the #define line below if you need to override the default buffer size 
// (default, if not specified is 500)
//#define MAX_BUFFER_SIZE 1000

// *******************************************************************************************************************
// Roster selection
// 
// uncomment this line if you wish the DccEXcontroller to search the roster when to enter a dcc address
// and use the roster entry if it finds it.
// #define SEARCH_ROSTER_ON_ENTRY_OF_DCC_ADDRESS true

// *******************************************************************************************************************
#endif