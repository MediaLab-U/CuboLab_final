// wifi_lab.h

#ifndef WIFILAB_H
#define WIFILAB_H

#include "configuration.h"

extern String ssidAP;
extern String passwordAP;

extern String ssid;
extern String password;

extern char macStr[18];

extern String networks;
extern boolean networksAvailable;

extern WiFiMulti wifiMulti;

void getMac();

String getLastMac();

boolean connectWiFi();

boolean connectWiFi2();

void createWifi();

void scanNetworks();

void updateFirmware();

#endif // WIFILAB_H