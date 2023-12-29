#include <WiFi.h>

class WifiService {
    public: 

        void init(ConfigService _configService) {
            configService = _configService;
            currentWifi   = 0;
            wifiDataList  = new WifiData[configService.lengthWifiList()];

            for (int i = 0; i < configService.lengthWifiList(); i++) {
                wifiDataList[i] = configService.getWifiOfList(i);
            }
        }

        bool setWifi(char* ssid, char* pass, int number = 0) {
            return configService.setWifiOnList(number, ssid, pass);
        }

        WifiData getWifiData(int number = 0) {
            return wifiDataList[number];
        }

        WifiData getCurrentWifi() {
            return currentWifiData;
        }

        void setWifiPoint(char* ssid, char* pass) {
            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid, pass);
            IPAddress Ip(192, 168, 1, 1);
            IPAddress NMask(255, 255, 255, 0);
            WiFi.softAPConfig(Ip, Ip, NMask);
        }

        bool nextConnect() {
            if (currentWifi >= configService.lengthWifiList()) {
                return false;
            }

            WifiData data = (WifiData) wifiDataList[currentWifi];

            // Serial.println("Connection to Wifi: ");
            // Serial.println("ssid: " + String(data.ssid) + " | pass: " + String(data.pass));

            currentWifi++;

            if (String(data.ssid) == "" || String(data.pass) == "") {
                // Serial.println("error to Wifi: ");
                return false;
            }
            // Serial.println("wifi begin... ");
            WiFi.begin(String(data.ssid), String(data.pass));
            currentWifiData = data;
            return true;
        }

        bool statusConnect() {
            return WiFi.status() == WL_CONNECTED;
        }
    
    private:
        int currentWifi = 0;
        WifiData currentWifiData;
        ConfigService configService;
        WifiData* wifiDataList;
};