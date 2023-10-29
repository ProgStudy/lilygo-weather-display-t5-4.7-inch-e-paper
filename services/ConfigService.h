#include <EEPROM.h>
#include <Preferences.h>
#include <ArduinoJson.h>

const int SIZE_LIST_WIFI = 3;

Preferences prefs;

struct WifiData {
    String ssid;
    String pass; 
};

struct Config {
    WifiData wifiList[SIZE_LIST_WIFI];
};

DynamicJsonDocument doc(1024);

class ConfigService {
    public: 
        void init() {
            prefs.begin("config", false);

            if(prefs.isKey("init")) {
                // init
                Serial.println("init config...");

                for (int i = 0; i < SIZE_LIST_WIFI; i++) {
                    doc["wifiList"][i]["ssid"] = "";
                    doc["wifiList"][i]["pass"] = "";
                }

                String json = "";

                serializeJson(doc, json);

                prefs.putString("config", json);
                prefs.putInt("init", 1);
                prefs.end();
                
            } else {
                // read
                Serial.println("read config...");

                deserializeJson(doc, prefs.getString("config"));

                
                int index = 0;

                for (JsonObject item : doc["wifiList"].as<JsonArray>()) {
                    config.wifiList[index].ssid = String((const char*) item["ssid"]);
                    config.wifiList[index].pass = String((const char*) item["pass"]);
                    index++;
                }
                
            }

            prefs.end();

        }

        WifiData getWifiOfList(int index) {
            return config.wifiList[index];
        }

        int lengthWifiList() {
            return SIZE_LIST_WIFI;
        }

        bool setWifiOnList(int listIndex, char* ssid, char* pass) {
            if (listIndex > SIZE_LIST_WIFI) {
                return false;
            }

            config.wifiList[listIndex].ssid = ssid;
            config.wifiList[listIndex].pass = pass;

            saveConfig();

            return true;
        }

        void saveConfig() {
            prefs.begin("config", false);

            for (int i = 0; i < SIZE_LIST_WIFI; i++) {
                doc["wifiList"][i]["ssid"] = config.wifiList[i].ssid;
                doc["wifiList"][i]["pass"] = config.wifiList[i].pass;
            }

            String json = "";

            serializeJson(doc, json);

            prefs.putString("config", json);
            prefs.end();
        }
    private: 

        Config config = Config();

};