#ifndef BOARD_HAS_PSRAM
    #error "Please enable PSRAM !!!"
#endif

// load libs
#include <EEPROM.h>
#include "epd_driver.h"



// load resources
#include "fonts/osans6b.h"
#include "fonts/osans8b.h"
#include "fonts/osans10b.h"
#include "fonts/osans18b.h"
#include "fonts/osans24b.h"
#include "fonts/osans26b.h"
#include "fonts/osans32b.h"
#include "fonts/osans48b.h"
#include "icons/icon_wifi_small.h"
#include "fonts/osans12b.h"
#include "fonts/osans16b.h"

// load services
#include "services/ConfigService.h"
ConfigService configService = ConfigService();
#include "services/WifiService.h"
WifiService wifiService = WifiService();
#include "services/Button.h"

// // load layouts
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
    if (typePage == START_APP) {
        btn1.setOnClick(startApp_click1);
        btn2.setOnClick(startApp_click2);
        btn3.setOnClick(startApp_click3);
    }
}

// =============================START APP ===============================>>

void startApp_click1(uint8_t _PIN) {
    Serial.println("click: 1");
    startApp.click1();
}

void startApp_click2(uint8_t _PIN) {
    Serial.println("click: 2");
    startApp.click2();
}

void startApp_click3(uint8_t _PIN) {
    Serial.println("click: 3");
    startApp.click3();
}

// =============================START APP ===============================>>

void loop()
{
    delay(120);
    startApp.update();
    btn1.requestLoop();
    btn2.requestLoop();
    btn3.requestLoop();
}

void initDisplay() {
    epd_init();
    epd_poweroff_all();
}