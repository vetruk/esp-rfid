#ifndef WEBSOCKET_H

#define WEBSOCKET_H

#include "webserver.h"

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void setupWebSocket();

#endif