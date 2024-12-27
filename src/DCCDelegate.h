#ifndef _DCC_DELEGATE_H
#define _DCC_DELEGATE_H

#include <DCCEXProtocol.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <string>
#include <stdlib.h>

#define debug_print printf
#define debug_println(x) printf("%s\n", x)



// dccexProtocol Delegate class
class MyDelegate : public DCCExController::DCCEXProtocolDelegate {
  
  public:
    std::string broadcastMessageText;
    uint32_t broadcastMessageTime;
    uint32_t heartBeatPeriod = 0;
    DCCExController::TrackPower trackPower = DCCExController::PowerUnknown;
    

    void heartbeatConfig(int seconds) { 
      debug_print("Received heartbeat. From: "); debug_print(std::to_string(heartBeatPeriod).c_str()); 
      debug_print(" To: "); debug_println(seconds); 
      heartBeatPeriod = seconds;
    }
    void receivedServerVersion(int major, int minor, int patch) {
      debug_print("\n\nReceived version: ");
      debug_print(std::to_string(major).c_str());
      debug_print(".");
      debug_print(std::to_string(minor).c_str());
      debug_print(".");
      debug_println(patch);
    }
    void receivedMessage(char* message) {
      debug_print("Broadcast Message: ");
      debug_println(message);
      broadcastMessageText = std::string(message);
      broadcastMessageTime = millis();
      // refreshOled();
    }
    virtual void receivedLocoUpdate(DCCExController::Loco* loco) {
      debugLocoSpeed("receivedLocoUpdate: ", loco); 
      // _processLocoUpdate(loco);
    }
    void receivedTrackPower(DCCExController::TrackPower state) { 
      debug_print("Received TrackPower: "); debug_println(std::to_string(state).c_str());
      if (trackPower != state) {
        trackPower = state;
        // displayUpdateFromWit(-1); // dummy multithrottlehumm
        // refreshOled();
      }
    }
    void receivedIndividualTrackPower(DCCExController::TrackPower state, int track) { 
      debug_print("Received Indivdual TrackPower: "); 
      debug_print(std::to_string(state).c_str());
      debug_print(" : ");
      debug_println(std::to_string(track).c_str());
    }
    void receivedRosterList() {
      debug_println("Received Roster List");
      // _loadRoster();
    }
    void receivedTurnoutList() {
      printf("Received Turnouts/Points list\n");
      // _loadTurnoutList();
    }    
    void receivedRouteList() {
      printf("Received Routes List\n");
      // _loadRouteList();
    }

    void debugLocoSpeed(std::string txt, DCCExController::Loco* loco) {
      debugLocoSpeed(txt, loco->getAddress(), loco->getSource(), loco->getSpeed(), loco->getDirection()); 
    }

    void debugLocoSpeed(std::string txt, int locoId, DCCExController::LocoSource source, int speed, DCCExController::Direction dir) {
      debug_print(txt.c_str());
      debug_print(" loco: "); debug_print(std::to_string(locoId).c_str()); 
      debug_print(" source: "); debug_print( (source == 0) ? "Roster" : "Entered"); 
      debug_print(" speed: "); debug_print(std::to_string(speed).c_str()); 
      debug_print(" dir: "); debug_print( (dir == DCCExController::Forward) ? "Forward" : "Reverse" ); 
      debug_println("");
    }

    uint32_t millis() {
      return to_ms_since_boot(get_absolute_time());
    }
};
#endif