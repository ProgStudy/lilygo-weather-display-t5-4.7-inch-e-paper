#ifndef BOARD_HAS_PSRAM
    #error "Please enable PSRAM !!!"
#endif

// load libs
#include <EEPROM.h>
#include "epd_driver.h"

#define START_APP (1)

uint8_t *framebuffer;

int cursor_x = 0;
int cursor_y = 0;

// load resources
#include "fonts/osans6b.h"
#include "fonts/osans8b.h"
#include "fonts/osans10b.h"
#include "fonts/osans18b.h"
#include "fonts/osans24b.h"
#include "fonts/osans26b.h"
#include "fonts/osans32b.h"
#include "fonts/osans48b.h"
#include "fonts/osans12b.h"
#include "fonts/osans16b.h"
#include "web/index.h"


#include "icons/icon_wifi_small.h"
#include "icons/qr_code_login_admin_wifi.h"
#include "icons/qr_code_login_webpage.h"

// load requests data
#include "request_data/weather_yandex_data.h"
#include "request_data/remote_list_region.h"
#include "request_data/common_data.h"

// load services
#include "services/ConfigService.h"
ConfigService configService = ConfigService();
#include "services/WifiService.h"
WifiService wifiService = WifiService();
#include "services/DisplayService.h"
DisplayService displayService = DisplayService();
#include "services/WebServerService.h"
WebServerService webServerService = WebServerService();

#include "services/Button.h"

// load layouts
#include "layouts/StartApp.h"


Button btn1 = Button(BUTTON_1);
Button btn2 = Button(BUTTON_2);
Button btn3 = Button(BUTTON_3);

StartAppLayout startApp;

void setup()
{
    Serial.begin(115200);

    initDisplay();

    configService.init();

    wifiService.init(configService);

    setSwitchClickCall(START_APP);

    startApp.show(wifiService);
}

void setSwitchClickCall(uint8_t typePage) {
    btn1.setOnClick(startApp_click1);
    btn2.setOnClick(startApp_click2);
    btn3.setOnClick(startApp_click3);
}

// ============================= START APP ===============================>>

void startApp_click1(uint8_t _PIN) {
    startApp.click1();
}

void startApp_click2(uint8_t _PIN) {
    startApp.click2();
}

void startApp_click3(uint8_t _PIN) {
    startApp.click3();
}

// ============================= START APP ===============================>>

void loop()
{
    delay(120);
    startApp.update();
    btn1.requestLoop();
    btn2.requestLoop();
    btn3.requestLoop();
    webServerService.loop();
}

void initDisplay() {
    epd_init();
    displayService.setMemBufferDisplay();
}