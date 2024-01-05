#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "WeatherViewApp.h"

#define WEATHER_VIEW_APP (2)

class StartAppLayout {
    public:

        void show(WifiService _wifiService) {
            wifiService          = _wifiService;
            showSelectBtns();
        }
        
        void update() {
            if (!isActive) {
                __weatherViewAppLayout.update();
            }
        }

        void click1() {
            if (isActive) {
                isActive = false;
                showAdminWeb();
            } else {
                __weatherViewAppLayout.click1();
            }
        }

        void click2() {
            if (isActive) {
                isActive = false;
                showSearchWifi();
                webServerService.init();
            } else {
                __weatherViewAppLayout.click2();
            }
        }

        void click3() {
            __weatherViewAppLayout.click3();
        }

        void dismissOnClick() {
            isActive = false;
        }
    
    private: 
        char buf[128];

        uint8_t iconWifi;
        Rect_t area_zone_text_center;
        Rect_t area_zone_text_botton_center;
        Rect_t area_zone_text_botton_center2;

        bool isActive = true;

        WifiService wifiService;
        WeatherViewAppLayout __weatherViewAppLayout;

        void runActualityRegionList() {
            epd_poweron();

            if (!requestRemoteListRegions()) {
                setTextBottomCenter("Не удалось получить список регионов", "Используем старый список!");
            } else {
                setTextBottomCenter("Регионы актуализированы!", "Подгрузка ресурсов...");
                configService.downloadResources();
            }

            epd_clear();
            epd_poweroff();

            // open weather view app
            __weatherViewAppLayout.show();
        }

        bool requestRemoteListRegions() {
            remoteServer _remoteServer = configService.getRemoteServer();
            HTTPClient _http;
            String _host = _remoteServer.host;
            WiFiClient _client;
            _client.stop();

            _http.begin(_client, _host, _remoteServer.port, REMOTE_PATH_REGIONS, true);

            int _httpCode = _http.GET();

            if (_httpCode == HTTP_CODE_OK)
            {
                int _size = _client.available();
                char _data[_size];
                
                bool isResult = false;

                _http.getStream().readBytes(_data, _size);
                
                isResult = saveFreshRegions(_data, _size);

                _client.stop();
                _http.end();

                return isResult;
            }

            return false;
        }

        bool saveFreshRegions(char *jsonStr, int size)
        {
            DynamicJsonDocument jsonDoc(1440);                              // allocate the JsonDocument
            DeserializationError error = deserializeJson(jsonDoc, jsonStr); // Deserialize the JSON document
            if (error)
            { // Test if parsing succeeds.
                Serial.println("deserializeJson() failed: " + String(error.c_str()));
                return false;
            }
            // convert it to a JsonObject
            JsonObject jo = jsonDoc.as<JsonObject>();

            int index = 0;

            for (JsonObject item : jo["regions"].as<JsonArray>()) {
                Serial.println(String((const char*) item["name"]));
                configService.setRegionOnList(index, item["name"], (float) item["lat"], (float) item["lon"], (int) item["timezone"], (bool) item["isActive"]);
                index++;
            }

            configService.saveConfig();

            jsonDoc.clear();

            return true;
        }

        void setTextBottomCenter(char* text, char* text2 = nullptr) {
            if (&area_zone_text_botton_center != nullptr) {
                epd_clear_area(area_zone_text_botton_center);
            }
            if (&area_zone_text_botton_center2 != nullptr) {
                epd_clear_area(area_zone_text_botton_center2);
                area_zone_text_botton_center2 = {};
            }

            int x = (960 / 2) - (String(text).length() * 4);
            int y = 540 - 35;

            area_zone_text_botton_center = {
                .x = x - 14,
                .y = y - 34,
                .width = (int32_t) (x + ((String(text).length() * 7))),
                .height = 40,
            };

            writeln((GFXfont *)&osans12b, text, &x, &y, NULL);

            if (text2 != nullptr) {
                int y2 = 540 - 6;
                int x2 = (960 / 2) - (String(text2).length() * 4);

                area_zone_text_botton_center2 = {
                    .x = x2 - 14,
                    .y = y2 - 30,
                    .width = (int32_t) (x2 + ((String(text2).length() * 7))),
                    .height = 40,
                };

                writeln((GFXfont *)&osans12b, text2, &x2, &y2, NULL);
                
            }

        }

        void showAdminWeb() {
            // displayService.setMemBufferDisplay();
            free(framebuffer);
            displayService.setMemBufferDisplay();

            epd_poweron();
            epd_clear();

            showQrCodeLoginAdminWifiIcon();
            showQrCodeLoginAdminWebpage();
            
            const char *overview[] = {
                "[ WIFI ] Наведие камерой на QR код или подключитесь к WIFI сети \nSSID: lilygo-admin | Пароль: 1qwe2rty3 \n\n"\
                "[ Admin Panel ] Наведие камерой на QR код или перейдите \nпо адресу: http://192.168.1.1"
            };

            cursor_x = 30;
            cursor_y = 30;

            write_string((GFXfont *)&osans12b, overview[0], &cursor_x, &cursor_y, framebuffer);            

            cursor_x = 200;
            cursor_y = EPD_HEIGHT - 35;

            write_string((GFXfont *)&osans18b, "WIFI", &cursor_x, &cursor_y, framebuffer);

            cursor_y = EPD_HEIGHT - 35;
            cursor_x = EPD_WIDTH - 100 - 300 + 30;

            write_string((GFXfont *)&osans18b, "Admin Panel", &cursor_x, &cursor_y, framebuffer);

            epd_draw_grayscale_image(epd_full_screen(), framebuffer);
            
            epd_poweroff();
        }

        void showSelectBtns() {
            // displayService.setMemBufferDisplay();
            free(framebuffer);
            displayService.setMemBufferDisplay();

            epd_poweron();
            epd_clear();

            const char *overview[] = {
                "[1] Режим поиска сети WIFI и отображение погоды\n"\
                "[2] Режим админ панели, будет сгенерирован вход \n     в админ панель"
            };

            int top = -15;

            cursor_x = 440;
            cursor_y = 50 + top;

            writeln((GFXfont *)&osans12b, "1", &cursor_x, &cursor_y, framebuffer);

            cursor_x += 50;

            writeln((GFXfont *)&osans12b, "2", &cursor_x, &cursor_y, framebuffer);

            cursor_x = 430;
            cursor_y = 25 + top;
            
            epd_draw_rect(cursor_x, cursor_y, 30, 30, 0, framebuffer);

            cursor_x += 67;
            
            epd_draw_rect(cursor_x, cursor_y, 30, 30, 0, framebuffer);

            // выводим информацию...

            cursor_x = 50;
            cursor_y = 150;

            write_string((GFXfont *)&osans16b, overview[0], &cursor_x, &cursor_y, framebuffer);

            epd_draw_grayscale_image(epd_full_screen(), framebuffer);

            epd_poweroff();

        }

        void showSearchWifi() {
            epd_poweron();
            epd_clear();

            setTextBottomCenter("Ищем подключение к сети...");

            showWifiIcon();

            if (connectWifi()) {
                setTextBottomCenter("Соединение установлено!", "Актуализируем список регионов...");
                epd_poweroff();

                runActualityRegionList();
                return;
            }

            setTextBottomCenter("Не удалось подключиться к сети", "Устройство перезагружается...");
            epd_poweroff();

            delay(3000);
            ESP.restart();
        }
        
        void setTextCenter(const char* text) {
            if (&area_zone_text_center != nullptr) {
                epd_clear_area(area_zone_text_center);
            }

            int y = 150 + IconWifiSmall_height + 35;
            int x = (960 / 2) - (String(text).length() * 11);

            area_zone_text_center = {
                .x = x - 14,
                .y = y - 34,
                .width = (int32_t) (x + ((String(text).length()) - 11)),
                .height = 40,
            };

            writeln((GFXfont *)&osans16b, text, &x, &y, NULL);
        }

        void showWifiIcon() {
            Rect_t area = {
                .x = 480 - (IconWifiSmall_width / 2),
                .y = 150,
                .width = IconWifiSmall_width,
                .height = IconWifiSmall_height,
            };

            epd_draw_image(area, (uint8_t *)IconWifiSmall_data, BLACK_ON_WHITE);
        }

        void showQrCodeLoginAdminWifiIcon() {
            Rect_t area = {
                .x = 100,
                .y = 200,
                .width = QrCodeLoginAdminWifi_width,
                .height = QrCodeLoginAdminWifi_height,
            };

            this->wifiService.setWifiPoint("lilygo-admin", "1qwe2rty3");
            
            webServerService.init();

            epd_draw_image(area, (uint8_t *)QrCodeLoginAdminWifi_data, BLACK_ON_WHITE);
        }

        void showQrCodeLoginAdminWebpage() {
            Rect_t area = {
                .x = EPD_WIDTH - 300 - 100,
                .y = 200,
                .width = QrCodeLoginWebpage_width,
                .height = QrCodeLoginWebpage_height,
            };

            epd_draw_image(area, (uint8_t *)QrCodeLoginWebpage_data, BLACK_ON_WHITE);
        }

        String stringS = "[";

        void hideProcessBar() {
            int cursor_x = 15;
            int cursor_y = 35;

            Rect_t area_zone = {
                .x = cursor_x,
                .y = cursor_y - 28,
                .width = 940,
                .height = 32,
            };

            epd_clear_area(area_zone);
            stringS = "[";
        }

        void showProcessBar() {
            hideProcessBar();

            int cursor_x = 20;
            int cursor_y = 35;

            const char *string2 = "[                                                                                                                                ]\n";
            
            writeln((GFXfont *)&osans12b, string2 , &cursor_x, &cursor_y, NULL);
        }


        bool setProcessBar(int i = 0) {

            int cursor_x = 20;
            int cursor_y = 35;

            if (i == 0) {
                stringS += "==============";
            } else {
                stringS += "==========";
            } 

            writeln((GFXfont *)&osans12b, stringS.c_str() , &cursor_x, &cursor_y, NULL);

            return i == 5;
        }

        bool connectWifi() {
            showProcessBar();

            if (!wifiService.nextConnect()) {
                return false;
            }

            WifiData wifi = wifiService.getCurrentWifi();
            
            Serial.println("search ... : " + wifi.ssid);

            setTextCenter(wifi.ssid.c_str());

            int step = 0;

            while(!setProcessBar(step)) {
                step++;
                
                if (wifiService.statusConnect()) {
                    hideProcessBar();
                    return true;
                }

                delay(3000);
            }

            hideProcessBar();

            return connectWifi();
        }
};