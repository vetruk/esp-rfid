#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "webserver.h"

// Include the header files that we created with gulp script
#include "glyphicons-halflings-regular.woff.gz.h"
#include "index.html.gz.h"
#include "required.css.gz.h"
#include "required.js.gz.h"

// Create AsyncWebServer instance on port "80"
AsyncWebServer server(80);
// Create WebSocket instance on URL "/ws"
AsyncWebSocket ws("/ws");

/* ------------------ BASIC SYSTEM Functions ------------------- */
void setupWebServer() {
  // Serve all files in root folder
  // server.serveStatic("/", SPIFFS, "/");
  // Handle what happens when requested web file couldn't be found
  server.onNotFound([](AsyncWebServerRequest * request) {
    AsyncWebServerResponse * response = request->beginResponse(404, "text/plain", "Not found");
    request->send(response);
  });
   /*
  // Simple Firmware Update Handler
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    shouldReboot = !Update.hasError();
    AsyncWebServerResponse * response = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : "FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t * data, size_t len, bool final) {
    if (!index) {
      Serial.printf("[ UPDT ] Firmware update started: %s\n", filename.c_str());
      Update.runAsync(true);
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
        Update.printError(Serial);
      }
    }
    if (!Update.hasError()) {
      if (Update.write(data, len) != len) {
        Update.printError(Serial);
      }
    }
    if (final) {
      if (Update.end(true)) {
        Serial.printf("[ UPDT ] Firmware update finished: %uB\n", index + len);
      } else {
        Update.printError(Serial);
      }
    }
  });
  */
  
  // Bootstrap Fonts hardcode workaround
  // Inspect impact on memory, firmware size.

  server.on("/fonts/glyphicons-halflings-regular.woff", HTTP_GET, [](AsyncWebServerRequest * request) {
    // Dump the byte array in PROGMEM with a 200 HTTP code (OK)
    AsyncWebServerResponse * response = request->beginResponse_P(200, "font/woff", glyphicons_halflings_regular_woff_gz, glyphicons_halflings_regular_woff_gz_len);
    // Tell the browswer the contemnt is Gzipped
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/css/required.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    // Dump the byte array in PROGMEM with a 200 HTTP code (OK)
    AsyncWebServerResponse * response = request->beginResponse_P(200, "text/css", required_css_gz, required_css_gz_len);
    // Tell the browswer the contemnt is Gzipped
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/js/required.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    // Dump the byte array in PROGMEM with a 200 HTTP code (OK)
    AsyncWebServerResponse * response = request->beginResponse_P(200, "text/javascript", required_js_gz, required_js_gz_len);
    // Tell the browswer the contemnt is Gzipped
    response->addHeader("Content-Encoding", "gzip");
    request-> send(response);
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    // Dump the byte array in PROGMEM with a 200 HTTP code (OK)
    AsyncWebServerResponse * response = request->beginResponse_P(200, "text/html", index_html_gz, index_html_gz_len);
    // Tell the browswer the contemnt is Gzipped
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.rewrite("/", "/index.html");

  // Start Web Server
  server.begin();
}