#ifndef WITSERVER_H
#define WITSERVER_H

#include <vector>
#include <string>

class WitServer {
public:
  static WitServer &getInstance() {
    static WitServer instance;
    return instance;
  }
  
    void witServiceLoop();
    void browseWitService();
    void selectWitServer(int selection);
    void connectWitServer();
    void enterWitServer();
    void disconnectWitServer();
    void witEntryAddChar(char key);
    void witEntryDeleteChar(char key);
    void buildWitEntry();



private:

      WitServer() = default;
  ~WitServer() = default;
  WitServer(const WitServer &) = delete;
  WitServer &operator=(const WitServer &) = delete;
};

#endif // WITSERVER_H