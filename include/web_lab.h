// ads.h

#ifndef WEBLAB_H
#define WEBLAB_H

#include <Arduino.h>
#include "configuration.h"

extern WebServer server;

void handleRoot();
void handleSave();
void handleSelect();
void handleImage();
void handleGif();


#endif // WEBLAB_H