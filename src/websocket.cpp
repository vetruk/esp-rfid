#include <ArduinoJson.h>

#include "websocket.h"



void sendUserList(int page, AsyncWebSocketClient * client) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["command"] = "userlist";
  root["page"] = page;
  JsonArray& users = root.createNestedArray("list");
  Dir dir = SPIFFS.openDir("/P/");
  int first = (page - 1) * 15;
  int last = page * 15;
  int i = 0;
  while (dir.next()) {
    if (i >= first && i < last) {
      JsonObject& item = users.createNestedObject();
      String uid = dir.fileName();
      uid.remove(0, 3);
      item["uid"] = uid;
      File f = SPIFFS.open(dir.fileName(), "r");
      size_t size = f.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      // We don't use String here because ArduinoJson library requires the input
      // buffer to be mutable. If you don't use ArduinoJson, you may as well
      // use configFile.readString instead.
      f.readBytes(buf.get(), size);
      DynamicJsonBuffer jsonBuffer2;
      JsonObject& json = jsonBuffer2.parseObject(buf.get());
      if (json.success()) {
        String username = json["user"];
        int AccType = json["acctype"];
        unsigned long validuntil = json["validuntil"];
        item["username"] = username;
        item["acctype"] = AccType;
        item["validuntil"] = validuntil;
      }
    }
    i++;
  }
  float pages = i / 15.0;
  root["haspages"] = ceil(pages);
  size_t len = root.measureLength();
  AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
  if (buffer) {
    root.printTo((char *)buffer->get(), len + 1);
    if (client) {
      client->text(buffer);
      client->text("{\"command\":\"result\",\"resultof\":\"userlist\",\"result\": true}");
    } else {
      ws.textAll("{\"command\":\"result\",\"resultof\":\"userlist\",\"result\": false}");
    }
  }
}

#ifdef ESP8266
extern "C" {
#include "user_interface.h"  // Used to get Wifi status information
}
#endif

// Send system status on a WS request
void sendStatus() {
  struct ip_info info;
  FSInfo fsinfo;
  if (!SPIFFS.info(fsinfo)) {
    Serial.print(F("[ WARN ] Error getting info on SPIFFS"));
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["command"] = "status";

  root["heap"] = ESP.getFreeHeap();
  root["chipid"] = String(ESP.getChipId(), HEX);
  root["cpu"] = ESP.getCpuFreqMHz();
  root["availsize"] = ESP.getFreeSketchSpace();
  root["availspiffs"] = fsinfo.totalBytes - fsinfo.usedBytes;
  root["spiffssize"] = fsinfo.totalBytes;
  root["uptime"] = NTP.getUptimeString();

  if (inAPMode) {
    wifi_get_ip_info(SOFTAP_IF, &info);
    struct softap_config conf;
    wifi_softap_get_config(&conf);
    root["ssid"] = String(reinterpret_cast<char*>(conf.ssid));
    root["dns"] = printIP(WiFi.softAPIP());
    root["mac"] = WiFi.softAPmacAddress();
  }
  else {
    wifi_get_ip_info(STATION_IF, &info);
    struct station_config conf;
    wifi_station_get_config(&conf);
    root["ssid"] = String(reinterpret_cast<char*>(conf.ssid));
    root["dns"] = printIP(WiFi.dnsIP());
    root["mac"] = WiFi.macAddress();
  }

  IPAddress ipaddr = IPAddress(info.ip.addr);
  IPAddress gwaddr = IPAddress(info.gw.addr);
  IPAddress nmaddr = IPAddress(info.netmask.addr);
  root["ip"] = printIP(ipaddr);
  root["gateway"] = printIP(gwaddr);
  root["netmask"] = printIP(nmaddr);

  size_t len = root.measureLength();
  AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
  if (buffer) {
    root.printTo((char *)buffer->get(), len + 1);
    ws.textAll(buffer);
  }
}

// Send Scanned SSIDs to websocket clients as JSON object
void printScanResult(int networksFound) {
  // sort by RSSI
  int n = networksFound;
  int indices[n];
  int skip[n];
  int loops = 0;
  for (int i = 0; i < networksFound; i++) {
    indices[i] = i;
  }
  for (int i = 0; i < networksFound; i++) {
    for (int j = i + 1; j < networksFound; j++) {
      if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
        loops++;
        //int temp = indices[j];
        //indices[j] = indices[i];
        //indices[i] = temp;
        std::swap(indices[i], indices[j]);
        std::swap(skip[i], skip[j]);
      }
    }
  }

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["command"] = "ssidlist";
  JsonArray& scan = root.createNestedArray("list");
  for (int i = 0; i < 5; ++i) {
    JsonObject& item = scan.createNestedObject();
    // Print SSID for each network found
    item["ssid"] = WiFi.SSID(indices[i]);
    item["bssid"] = WiFi.BSSIDstr(indices[i]);
    item["rssi"] = WiFi.RSSI(indices[i]);
    item["channel"] = WiFi.channel(indices[i]);
    item["enctype"] = WiFi.encryptionType(indices[i]);
    item["hidden"] = WiFi.isHidden(indices[i]) ? true : false;
  }
  size_t len = root.measureLength();
  AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
  if (buffer) {
    root.printTo((char *)buffer->get(), len + 1);
    ws.textAll(buffer);
  }
  WiFi.scanDelete();
}

void sendTime() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["command"] = "gettime";
  root["epoch"] = now();
  root["timezone"] = timeZone;
  size_t len = root.measureLength();
  AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
  if (buffer) {
    root.printTo((char *)buffer->get(), len + 1);
    ws.textAll(buffer);
  }
}

// Handles WebSocket Events
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_ERROR) {
    Serial.printf("[ WARN ] WebSocket[%s][%u] error(%u): %s\r\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  }
  else if (type == WS_EVT_DATA) {

    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";

    if (info->final && info->index == 0 && info->len == len) {
      //the whole message is in a single frame and we got all of it's data
      for (size_t i = 0; i < info->len; i++) {
        msg += (char) data[i];
      }

      // We should always get a JSON object (stringfied) from browser, so parse it
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(msg);
      if (!root.success()) {
        Serial.println(F("[ WARN ] Couldn't parse WebSocket message"));
        return;
      }

      // Web Browser sends some commands, check which command is given
      const char * command = root["command"];

      // Check whatever the command is and act accordingly
      if (strcmp(command, "remove")  == 0) {
        const char* uid = root["uid"];
        String filename = "/P/";
        filename += uid;
        SPIFFS.remove(filename);
      }
      else if (strcmp(command, "configfile")  == 0) {
        File f = SPIFFS.open("/auth/config.json", "w+");
        if (f) {
          root.prettyPrintTo(f);
          //f.print(msg);
          f.close();
          ESP.reset();
        }
      }
      else if (strcmp(command, "userlist")  == 0) {
        int page = root["page"];
        sendUserList(page, client);
      }
      else if (strcmp(command, "status")  == 0) {
        sendStatus();
      }
      else if (strcmp(command, "userfile")  == 0) {
        const char* uid = root["uid"];
        String filename = "/P/";
        filename += uid;
        File f = SPIFFS.open(filename, "w+");
        // Check if we created the file
        if (f) {
          f.print(msg);
        }
        f.close();
        ws.textAll("{\"command\":\"result\",\"resultof\":\"userfile\",\"result\": true}");
      }
      else if (strcmp(command, "testrelay")  == 0) {
        activateRelay = true;
        previousMillis = millis();
      }
      else if (strcmp(command, "latestlog")  == 0) {
        File logFile = SPIFFS.open("/auth/latestlog.json", "r");
        if (logFile) {
          size_t len = logFile.size();
          AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
          if (buffer) {
            logFile.readBytes((char *)buffer->get(), len + 1);
            ws.textAll(buffer);
          }
          logFile.close();
        }
        else {
          ws.textAll("{\"type\":\"result\",\"resultof\":\"latestlog\",\"result\": false}");
        }
      }
      else if (strcmp(command, "scan")  == 0) {
        WiFi.scanNetworksAsync(printScanResult, true);
      }
      else if (strcmp(command, "gettime")  == 0) {
        sendTime();
      }
      else if (strcmp(command, "settime")  == 0) {
        unsigned long t = root["epoch"];
        setTime(t);
        sendTime();
      }
      else if (strcmp(command, "getconf")  == 0) {
        File configFile = SPIFFS.open("/auth/config.json", "r");
        if (configFile) {
          size_t len = configFile.size();
          AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
          if (buffer) {
            configFile.readBytes((char *)buffer->get(), len + 1);
            ws.textAll(buffer);
          }
          configFile.close();
        }
      }
    }
  }
}

void setupWebSocket() {
  // Start WebSocket Plug-in and handle incoming message on "onWsEvent" function
  server.addHandler(&ws);
  ws.onEvent(onWsEvent);
}