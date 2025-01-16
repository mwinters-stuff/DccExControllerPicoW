#ifndef WITSERVER_H
#define WITSERVER_H

#include <vector>
#include <string>
#include <DCCEXProtocol.h>

#include "Base.h"

class WitServer : public Base {
public:
  
  void setup() override;
  void loop() override;

    void browseWitService();
    void selectWitServer(int selection);
    void connectWitServer();
    void enterWitServer();
    void disconnectWitServer();
    void witEntryAddChar(char key);
    void witEntryDeleteChar(char key);
    void buildWitEntry();
    void setLastServerResponseTime(bool force);
    void checkForShutdownOnNoResponse();
private:
  std::string witServerIpAndPortEntryMask = "###.###.###.###:#####";


};

#endif // WITSERVER_H