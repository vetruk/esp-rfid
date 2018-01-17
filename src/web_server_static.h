#ifndef _ESP_RFID_WEB_SERVER_STATIC_H
#define _ESP_RFID_WEB_SERVER_STATIC_H

#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char _CONTENT_TYPE_HTML[] PROGMEM = "text/html";
const char _CONTENT_TYPE_TEXT[] PROGMEM = "text/text";
const char _CONTENT_TYPE_CSS[] PROGMEM = "text/css";
const char _CONTENT_TYPE_JSON[] PROGMEM = "application/json";
const char _CONTENT_TYPE_JS[] PROGMEM = "application/javascript";
const char _CONTENT_TYPE_JPEG[] PROGMEM = "image/jpeg";
const char _CONTENT_TYPE_PNG[] PROGMEM = "image/png";

struct StaticFile
{
  const char *filename;
  const char *data;
  size_t length;
  const char *type;
};

class StaticFileWebHandler: public AsyncWebHandler
{
  private:
    bool _getFile(AsyncWebServerRequest *request, StaticFile **file = NULL);
  protected:
  public:
    StaticFileWebHandler();
    virtual bool canHandle(AsyncWebServerRequest *request) override final;
    virtual void handleRequest(AsyncWebServerRequest *request) override final;
};

class StaticFileResponse: public AsyncWebServerResponse
{
  private:
    String _header;
    StaticFile *_content;

    const char *ptr;
    size_t length;

    size_t writeData(AsyncWebServerRequest *request);
    size_t write(AsyncWebServerRequest *request);

  public:
    StaticFileResponse(int code, StaticFile *file);
    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);
    bool _sourceValid() const { return true; }

};

#endif // _ESP_RFID_WEB_SERVER_STATIC_H
