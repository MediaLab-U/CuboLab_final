// sleep_lab.h

#ifndef SENDERLAB_H
#define SENDERLAB_H

#include <Arduino.h>
#include "configuration.h"
#include "hmi.h"

extern HTTPClient http;

void sendData();
void sendNewData();
void manejarTrabajoRealizado();


#endif // SENDERLAB_H