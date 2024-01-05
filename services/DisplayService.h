#include "epd_driver.h"
#define CLEAR(s) memset(&(s), 0, sizeof(s))

class DisplayService {
    public: 
        void setMemBufferDisplay() {
            
            framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
            
            if (!framebuffer) {
                Serial.println("alloc memory failed !!!");
                while (1);
            }

            memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
        }
};