#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

const char* pathConfig = "/config_v4.json";

int _x = 10;
int _y = 50;

const String resources[] PROGMEM = {
    "/resources/bkn_-ra_d.bin",
    "/resources/bkn_-ra_dL.bin",
    "/resources/bkn_-ra_n.bin",
    "/resources/bkn_-ra_nL.bin",
    "/resources/bkn_-sn_d.bin",
    "/resources/bkn_-sn_dL.bin",
    "/resources/bkn_-sn_n.bin",
    "/resources/bkn_-sn_nL.bin",
    "/resources/bkn_+ra_d.bin",
    "/resources/bkn_+ra_dL.bin",
    "/resources/bkn_+ra_n.bin",
    "/resources/bkn_+ra_nL.bin",
    "/resources/bkn_d.bin",
    "/resources/bkn_dL.bin",
    "/resources/bkn_n.bin",
    "/resources/bkn_nL.bin",
    "/resources/bkn_ra_d.bin",
    "/resources/bkn_ra_dL.bin",
    "/resources/bkn_ra_n.bin",
    "/resources/bkn_ra_nL.bin",
    "/resources/bkn_sn_d.bin",
    "/resources/bkn_sn_dL.bin",
    "/resources/bkn_sn_n.bin",
    "/resources/bkn_sn_nL.bin",
    "/resources/bl.bin",
    "/resources/blL.bin",
    "/resources/blob.bin",
    "/resources/fg_d.bin",
    "/resources/fg_dL.bin",
    "/resources/moon_new.bin",
    "/resources/ovc_-ra.bin",
    "/resources/ovc_-raL.bin",
    "/resources/ovc_-sn.bin",
    "/resources/ovc_-snL.bin",
    "/resources/ovc_+ra.bin",
    "/resources/ovc_+raL.bin",
    "/resources/ovc_+sn.bin",
    "/resources/ovc_+snL.bin",
    "/resources/ovc_ra_sn.bin",
    "/resources/ovc_ra_snL.bin",
    "/resources/ovc_ra.bin",
    "/resources/ovc_raL.bin",
    "/resources/ovc_sn.bin",
    "/resources/ovc_snL.bin",
    "/resources/ovc_ts_ra.bin",
    "/resources/ovc_ts_raL.bin",
    "/resources/ovc_ts.bin",
    "/resources/ovc_tsL.bin",
    "/resources/ovc.bin",
    "/resources/ovcL.bin",
    "/resources/skc_d.bin",
    "/resources/skc_dL.bin",
    "/resources/skc_n.bin",
    "/resources/skc_nL.bin",
    "/resources/sunrise.bin",
    "/resources/sunset.bin",
    "/resources/url_img.bin",
    "/resources/wifi_img.bin"
};

const int SIZE_LIST_WIFI    = 3;
const int SIZE_LIST_REGIONS = 5;

struct WifiData {
    String ssid;
    String pass; 
};

struct Config {
    WifiData wifiList[SIZE_LIST_WIFI];
    String yandexApiKey;
    linkRemoteRegins _linkRemoteRegins;
    region _region[SIZE_LIST_REGIONS];
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

                config.yandexApiKey = "";

                config._linkRemoteRegins.host = "";
                config._linkRemoteRegins.port = 80;
                config._linkRemoteRegins.path = "";

                for (int i = 0; i < SIZE_LIST_REGIONS; i++) {
                    if (i == 0) {
                        //default region (Moscow)
                        config._region[i].isActive = true;
                        config._region[i].name = "Москва";
                        config._region[i].lat = 55.755376;
                        config._region[i].lon = 37.619595;
                        continue;
                    }

                    config._region[i].isActive = false;
                    config._region[i].name = "Резервный регион";
                    config._region[i].lat = 0.0;
                    config._region[i].lon = 0.0;

                }

                saveConfig();
            }

        }

        void downloadResources() {
            for (String path : resources) {
                Serial.println("Downloading file...: " + path);
                downloadFile(path);
                Serial.println("Downloaded file: " + path);
            }
        }

        void downloadFile(String pathName) {
            File f = SPIFFS.open(pathName, FILE_WRITE);
            HTTPClient _http;
            String _host = config._linkRemoteRegins.host;
            WiFiClient _client;

            _client.stop();


            // if (!f) {
            _http.begin(_client, _host, config._linkRemoteRegins.port, pathName, true);
            int _httpCode = _http.GET();


            if (_httpCode > 0) {
                if (_httpCode == HTTP_CODE_OK) {
                    
                    int _size = _client.available();

                    uint8_t *_data;
                    _data = (uint8_t *)ps_calloc(sizeof(uint8_t), _size);
                    _client.readBytes(_data, _size);
                    f.write(_data, _size);
                    f.close();
                    free(_data);
                    
                    _x += 5;
                    writeln((GFXfont *) &osans8b, "+", &_x, &_y, NULL);

                }
            } else {
                Serial.printf("[HTTP] GET... failed, error: %s\n", _http.errorToString(_httpCode).c_str());
            }
                
            // }
            _http.end();
        }

        WifiData getWifiOfList(int index) {
            return config.wifiList[index];
        }
        
        region getRegionOfList(int index) {
            return config._region[index];
        }

        int lengthWifiList() {
            return SIZE_LIST_WIFI;
        }

        int lengthRegionList() {
            return SIZE_LIST_REGIONS;
        }

        bool setRegionOnList(int listIndex, String name, float lat, float lon, bool isActive) {
            if (listIndex > SIZE_LIST_WIFI) {
                return false;
            }

            config._region[listIndex].name       = name;
            config._region[listIndex].lat        = lat;
            config._region[listIndex].lon        = lon;
            config._region[listIndex].isActive   = isActive;

            return true;
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

        String getYandexApiKey() {
            return config.yandexApiKey;
        }

        void setYandexApiKey(String apiKey) {
            config.yandexApiKey = apiKey;
            
            saveConfig();
        }

        void setRemoteUrlListRegions(String host, int port, String path) {
            config._linkRemoteRegins.host = host;
            config._linkRemoteRegins.port = port;
            config._linkRemoteRegins.path = path;

            saveConfig();
        }

        linkRemoteRegins getUrlRemoteRegions() {
            return config._linkRemoteRegins;
        }

        void readConfig() {
            Serial.println(F("Start read..."));

            if (!SPIFFS.exists(pathConfig)) {
                Serial.println(F("Not found file!"));
                return;
            }

            File file = SPIFFS.open(pathConfig, FILE_READ);

            if (!file) {
                Serial.println(F("Not found file!"));
                return;
            }

            String json = "";

            Serial.println(F("Read content file: "));

            json = file.readString();

            file.close();

            Serial.println("Result: " + json);

            if (json == "" || json == NULL) {
                Serial.println("Emtpy content!");
                return;
            }

            deserializeJson(doc, json);

            int index = 0;

            for (JsonObject item : doc["wifiList"].as<JsonArray>()) {
                config.wifiList[index].ssid = String((const char*) item["ssid"]);
                config.wifiList[index].pass = String((const char*) item["pass"]);
                index++;
            }

            index = 0;

            for (JsonObject item : doc["regionList"].as<JsonArray>()) {
                config._region[index].name = String((const char*) item["name"]);
                config._region[index].lat = (float) item["lat"];
                config._region[index].lon = (float) item["lon"];
                config._region[index].isActive = (bool) item["isActive"];
                index++;
            }

            config.yandexApiKey         = String((const char*) doc["yandexApiKey"]);
            
            config._linkRemoteRegins.host = String((const char*) doc["linkRemoteRegins"]["host"]);
            config._linkRemoteRegins.port = (int) doc["linkRemoteRegins"]["port"];
            config._linkRemoteRegins.path = String((const char*) doc["linkRemoteRegins"]["path"]);

            isInit = doc["isInit"];

            config.yandexApiKey = String((const char*) doc["yandexApiKey"]);

            doc.clear();
        }

        void saveConfig() {

            Serial.println(F("Start write..."));

            for (int i = 0; i < SIZE_LIST_WIFI; i++) {
                doc["wifiList"][i]["ssid"] = config.wifiList[i].ssid;
                doc["wifiList"][i]["pass"] = config.wifiList[i].pass;

                Serial.println(F("Set wifi..."));
            }

            for (int i = 0; i < SIZE_LIST_REGIONS; i++) {
                doc["regionList"][i]["name"]     = config._region[i].name;
                doc["regionList"][i]["lat"]      = config._region[i].lat;
                doc["regionList"][i]["lon"]      = config._region[i].lon;
                doc["regionList"][i]["isActive"] = config._region[i].isActive;

                Serial.println(F("Set region..."));
            }

            doc["isInit"]                   = isInit;
            doc["yandexApiKey"]             = config.yandexApiKey;

            doc["linkRemoteRegins"]["host"] = config._linkRemoteRegins.host;
            doc["linkRemoteRegins"]["port"] = config._linkRemoteRegins.port;
            doc["linkRemoteRegins"]["path"] = config._linkRemoteRegins.path;

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