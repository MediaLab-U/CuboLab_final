// ads.h

#ifndef WEBLAB_H
#define WEBLAB_H

#include "configuration.h"

extern WebServer server;
extern boolean wifiUpdate;

void handleRoot();
void handleSave();
void handleTest();
void handleExit();
void handleImage();
void handleGif();


#endif // WEBLAB_H