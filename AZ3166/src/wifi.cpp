// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

#include "../inc/globals.h"
#include "AZ3166WiFi.h"
#include "../inc/config.h"

bool initApWiFi() {
    char ap_name[STRING_BUFFER_32] = {0};
    byte mac[6] = {0};
    WiFi.macAddress(mac);
    unsigned length = snprintf(ap_name, STRING_BUFFER_32 - 1, "AZ3166_%c%c%c%c%c%c",
            mac[0] % 26 + 65, mac[1]% 26 + 65, mac[2]% 26 + 65, mac[3]% 26 + 65,
            mac[4]% 26 + 65, mac[5]% 26 + 65);
    ap_name[length] = char(0);

    char macAddress[STRING_BUFFER_32] = {0};
    length = snprintf(macAddress, STRING_BUFFER_32 - 1, "mac:%02X%02X%02X%02X%02X%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    macAddress[length] = char(0);

    int ret = WiFi.beginAP(ap_name, "");

    Screen.print(0, "WiFi name:");
    Screen.print(1, ap_name);
    Screen.print(2, macAddress);
    Screen.print(3, "go-> 192.168.0.1");

    if ( ret != WL_CONNECTED) {
      Screen.print(0, "AP Failed:");
      Screen.print(2, "Reboot device");
      Screen.print(3, "and try again");
      Serial.println("AP creation failed");
      return false;
    }
    Serial.println("AP started");
    return true;
}

bool initWiFi() {
    bool connected = false;
    Screen.print("WiFi \r\n \r\nConnecting...\r\n             \r\n");

    if(WiFi.begin() == WL_CONNECTED) {
        Serial.println("WiFi WL_CONNECTED");
        digitalWrite(LED_WIFI, 1);
        connected = true;
    } else {
        Screen.print("WiFi\r\nNot Connected\r\nEnter AP Mode?\r\n");
    }

    return connected;
}

void shutdownWiFi() {
    WiFi.disconnect();
}

void shutdownApWiFi() {
    WiFi.disconnectAP();
}

String * getWifiNetworks(int &count) {
    String foundNetworks = "";  // keep track of network SSID so as to remove duplicates from mesh and repeater networks
    int numSsid = WiFi.scanNetworks();
    count = 0;

    if (numSsid == -1) {
      return NULL;
    } else {
        String *networks = new String[numSsid + 1]; // +1 if all unique
        if (networks == NULL /* unlikely */) {
            return NULL;
        }

        if (numSsid > 0) {
            networks[0] = WiFi.SSID(0);
            count = 1;
        }

        // TODO: can we make this better?
        for (int thisNet = 1; thisNet < numSsid; thisNet++) {
            const char* ssidName = WiFi.SSID(thisNet);
            if (strlen(ssidName) == 0) continue;

            networks[count] = ssidName; // prefill
            int lookupPosition = 0;
            for(; lookupPosition < count; lookupPosition++) {
                if (networks[count] == networks[lookupPosition]) break;
            }
            if (lookupPosition < count) { // ditch if found
                continue;
            }
            count++; // save if not found
        }

        return networks;
    }
}

void displayNetworkInfo() {
    char buff[STRING_BUFFER_128] = {0};
    IPAddress ip = WiFi.localIP();
    byte mac[6] = {0};

    WiFi.macAddress(mac);
    unsigned length = snprintf(buff, STRING_BUFFER_128,
        "WiFi:\r\n%s\r\n%s\r\nmac:%02X%02X%02X%02X%02X%02X",
        WiFi.SSID(), ip.get_address(),
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    buff[length] = char(0);
    Screen.print(0, buff);
}
