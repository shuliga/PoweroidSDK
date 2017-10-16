#ifndef BT_H
#define BT_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <context.h>
#include "commons.h"

#define CONNECTION_CHECK 60000L

#define BT_VER_05 "+VERSION:hc01.comV2.1"
#define BT_VER_06 "hc01.comV2.0"
#define PASSWD "1234"


#define HISPEED 115200

class Bt {
public:
  const char *name;
  bool firstRun = true;
  bool active = false;
  Bt(const char *id);

  void begin();

  void getResult();
  bool getPassive();
  bool isConnected();

private:
    bool connected = false;
    bool passive = false;

    String execBtAtCommand(const __FlashStringHelper *cmd, const char *cmd2, unsigned long timeout);
    String execBtAtCommand(const __FlashStringHelper *cmd);

    void applyBt05();

    void execReset();
};

#endif // BT_H
