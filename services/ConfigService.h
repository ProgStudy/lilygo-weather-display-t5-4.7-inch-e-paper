#include <SPIFFS.h>
#include <ArduinoJson.h>

const char* pathConfig = "/config.json";

const int SIZE_LIST_WIFI = 3;

struct WifiData {
    String ssid;
    String pass; 
};

struct Config {
    WifiData wifiList[SIZE_LIST_WIFI];
};

int isInit = 0;

DynamicJsonDocument doc(1024);

class ConfigService {
    public: 
        void init() {
            if(!SPIFFS.begin(true)){
                Serial.println("Error while mounting SPIFFS");
                return;
            }

            readConfig();

            if(!isInit) {
                // init
                Serial.println("init config...");

                for (int i = 0; i < SIZE_LIST_WIFI; i++) {
                    config.wifiList[i].ssid = "";
                    config.wifiList[i].pass = "";
                }
                
                isInit = 1;

                saveConfig();
            }

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

        void readConfig() {
            Serial.println(F("Start read..."));

            File file = SPIFFS.open(pathConfig, FILE_READ);

            if (!file) {
                Serial.println(F("Not found file!"));
                return;
            }

            String json = "";

            Serial.println(F("Read content file: "));
            // while(file.available()) {
            json = file.readString();
            //     Serial.print(json);
            // }

            file.close();

            Serial.println("Result: " + json);

            deserializeJson(doc, json);

            int index = 0;

            for (JsonObject item : doc["wifiList"].as<JsonArray>()) {
                config.wifiList[index].ssid = String((const char*) item["ssid"]);
                config.wifiList[index].pass = String((const char*) item["pass"]);
                index++;
            }

            isInit = doc["isInit"];
            doc.clear();
        }

        void saveConfig() {

            Serial.println(F("Start write..."));

            for (int i = 0; i < SIZE_LIST_WIFI; i++) {
                doc["wifiList"][i]["ssid"] = config.wifiList[i].ssid;
                doc["wifiList"][i]["pass"] = config.wifiList[i].pass;

                Serial.println(F("Set wifi..."));
            }

            doc["isInit"] = isInit;
            Serial.println(F("Set init..."));

            String json = "";
            serializeJson(doc, json);

            Serial.println("Result config: " + json);

            File file = SPIFFS.open(pathConfig, FILE_WRITE);

            Serial.println("Open file: " + String(pathConfig));

            if (!file) {
                Serial.println(F("Not found file!"));
            }

            
            if (!file.print(json)) {
                Serial.println(F("Failed write!"));
            } else {
                Serial.println(F("Saving write!"));
            }

            file.close();
            doc.clear();
        }
    private: 

        Config config = Config();

};