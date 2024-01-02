#include <WebServer.h>
#include <ArduinoJson.h>

static WebServer *_server;
DynamicJsonDocument _doc(1024);


class WebServerService {
    public:
        void init() {
            _server = new WebServer(80);

            registerListenerRequest();

            _server->begin();
            
            isRun = true;
        }

        void loop() {
            if (isRun) {
                _server->handleClient();
            }
        }

        void static handle_Reboot() {
            _server->send(200, F("application/json"), F("{}"));
            ESP.restart();
        }
        
        void static handle_OnConnect() {
            String html = HTML_CONTENT_HOME;
            _server->send(200, "text/html", html); 
        }

        void static handle_SaveWifi() {
            if (!configService.setWifiOnList(_server->arg("index").toInt(),  (char *) _server->arg("ssid").c_str(),  (char *) _server->arg("password").c_str())) {
                return _server->send(500, F("application/json"), F("{}"));
            }

            _server->send(200, F("application/json"), F("{}"));
        }

        void static handle_SaveYandexAPIKey() {
            configService.setYandexApiKey(_server->arg("apiKey"));
            _server->send(200, F("application/json"), F("{}"));
        }   

        void static handle_GetYandexAPIKey() {
            JsonObject obj = _doc.to<JsonObject>();
            obj["apiKey"] = configService.getYandexApiKey();

            String result = "";
            serializeJson(_doc, result);

            _server->send(200, F("application/json"), result);

            _doc.clear();
        }   

        void static handle_LoadRemoteUrlListRegion() {
            JsonObject obj = _doc.to<JsonObject>();
            linkRemoteRegins _linkRemoteRegions = configService.getUrlRemoteRegions();

            obj["host"] = _linkRemoteRegions.host;
            obj["port"] = _linkRemoteRegions.port;
            obj["path"] = _linkRemoteRegions.path;

            String result = "";
            serializeJson(_doc, result);

            _server->send(200, F("application/json"), result);

            _doc.clear();
        }

        void static handle_SaveRemoteUrlListRegion() {
            configService.setRemoteUrlListRegions(_server->arg("host"), _server->arg("port").toInt(), _server->arg("path"));
            _server->send(200, F("application/json"), F("{}"));
        }

        void static handle_GetListWifi() {

            JsonArray array = _doc.to<JsonArray>();

            for (int i = 0; i < configService.lengthWifiList(); i++) {
                WifiData data = configService.getWifiOfList(i);
                JsonObject nested = array.createNestedObject();
                nested["ssid"] = data.ssid;
                nested["pass"] = data.pass;
            }

            String result = "";
            serializeJson(array, result);

            _server->send(200, F("application/json"), result);
            _doc.clear();
        }

        void static handle_NotFound() {
            _server->send(404, F("text/plain"), "not found page");
        }

    private:
        void registerListenerRequest() {
            _server->on(F("/"), WebServerService::handle_OnConnect);
            
            _server->on(F("/reboot"), WebServerService::handle_Reboot);

            _server->on(F("/wifi/list"), WebServerService::handle_GetListWifi);
            _server->on(F("/wifi/save"), WebServerService::handle_SaveWifi);

            _server->on(F("/yandexApiKey/load"), WebServerService::handle_GetYandexAPIKey);
            _server->on(F("/yandexApiKey/save"), WebServerService::handle_SaveYandexAPIKey);

            _server->on(F("/remote-regions/load"), WebServerService::handle_LoadRemoteUrlListRegion);
            _server->on(F("/remote-regions/save"), WebServerService::handle_SaveRemoteUrlListRegion);

            _server->onNotFound(WebServerService::handle_NotFound);
        }

        bool isRun = false;
};     