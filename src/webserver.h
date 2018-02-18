#ifndef WEBSERVER_H

#define WEBSERVER_H

#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern AsyncWebSocket ws;
void setupWebServer();

#endif