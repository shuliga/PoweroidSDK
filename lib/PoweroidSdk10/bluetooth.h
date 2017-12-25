#ifndef BT_H
#define BT_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <context.h>

#define CONNECTION_CHECK 60000L

#define BT_VER_05 "+VERSION:hc01.comV2.1"
#define BT_VER_06 "hc01.comV2.0"
#define PASSWD "1234"


#define HISPEED 115200

class Bt {
public:
  const char *name;
  bool firstRun = true;
  bool server = false;
  Bt(const char *id);

  void begin();

  bool getPassive();
  bool isConnectedToServer();

private:
    bool connected = false;

    String execBtAtCommand(const __FlashStringHelper *cmd, const char *cmd2, unsigned long timeout);
    String execBtAtCommand(const __FlashStringHelper *cmd);

    void applyBt05();

    void execReset();

    bool checkPeerType(const char *conn_type);
};

#endif // BT_H
