#define START_APP (1)

class StartAppLayout {
    public:

        void show(WifiService _wifiService) {
            
            wifiService = _wifiService;

            epd_clear();

            setTextBottomCenter("Ищем подключение к сети...");

            showWifiIcon();

            if (connectWifi()) {
                setTextBottomCenter("Соединение установлено!");
                return;
            }

            setTextBottomCenter("Не удалось подключиться к сети", "Устройство переключилась в режим настройки!");
        }

        void clickSwitchModeSetting() {
            //отключает режим настройки и начинает искать сеть
        }

        void clickSwitchModeWifi() {
            //останавливает поиск и переключает в режим настройки
        }
        
        void update() {
            // btn.requestLoop();
        }

        void click1() {
            Serial.println("click 1");
        }

        void click2() {
            Serial.println("click 2");
        }

        void click3() {
            Serial.println("click 3");
        }
    
    private: 
        char buf[128];

        uint8_t iconWifi;
        Rect_t area_zone_text_center;
        Rect_t area_zone_text_botton_center;
        Rect_t area_zone_text_botton_center2;

        WifiService wifiService;

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