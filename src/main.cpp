/*
   Copyright (c) 2017 Omer Siar Baysal
   Copyright (c) 2018 ESP-RFID Community

   Released to Public Domain

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

 */

#include <Arduino.h>
#include <NtpClientLib.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

#include "webserver.h"

bool activateRelay = false;
unsigned long previousMillis = 0;

// Set things up
void setup() {
        Serial.begin(115200);
        Serial.println();
        Serial.println(F("[ INFO ] ESP RFID v1.0"));

        // Start SPIFFS filesystem
        // SPIFFS.begin();

        /* Remove Users Helper
           Dir dir = SPIFFS.openDir("/P/");
           while (dir.next()){
           SPIFFS.remove(dir.fileName());
           }
         */

        // Try to load configuration file so we can connect to an Wi-Fi Access Point
        // Do not worry if no config file is present, we fall back to Access Point mode and device can be easily configured
		/*
        if (!loadConfiguration()) {
                fallbacktoAPMode();
        }
		*/
        setupWebServer();
}

// Main Loop
void loop() {
	    /*
        unsigned long currentMillis = millis();
        unsigned long deltaTime = currentMillis - previousLoopMillis;
        unsigned long uptime = NTP.getUptime();
        previousLoopMillis = currentMillis;

        if (autoRestartIntervalSeconds > 0 && uptime > autoRestartIntervalSeconds * 1000) {
                Serial.println(F("[ WARN ] Auto restarting..."));
                shouldReboot = true;
        }

        // check for a new update and restart
        if (shouldReboot) {
                Serial.println(F("[ UPDT ] Rebooting..."));
                delay(100);
                ESP.restart();
        }
        if (currentMillis - previousMillis >= activateTime && activateRelay) {
                activateRelay = false;
                digitalWrite(relayPin, relayType);
        }
        if (activateRelay) {
                digitalWrite(relayPin, !relayType);
        }
        if (isWifiConnected) {
                wiFiUptimeMillis += deltaTime;
        }
        if (wifiTimeout > 0 && wiFiUptimeMillis > (wifiTimeout * 1000) && isWifiConnected == true) {
                doDisableWifi = true;
        }
        if (doDisableWifi == true) {
                doDisableWifi = false;
                wiFiUptimeMillis = 0;
                disableWifi();
        }
        else if (doEnableWifi == true) {
                doEnableWifi = false;
                if (!isWifiConnected) {
                        wiFiUptimeMillis = 0;
                        enableWifi();
                }
        }
        // Another loop for RFID Events, since we are using polling method instead of Interrupt we need to check RFID hardware for events
        if (currentMillis >= cooldown) {
                rfidloop();
        }
		*/

}
