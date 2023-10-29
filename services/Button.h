#define BUTTON_2  (34)
#define BUTTON_3  (35)
#define BUTTON_1  (39)

#define BATT_PIN  (36)

#define SD_MISO   (12)
#define SD_MOSI   (13)
#define SD_SCLK   (14)
#define SD_CS     (15)

#define TOUCH_SCL (14)
#define TOUCH_SDA (15)
#define TOUCH_INT (13)

#define GPIO_MISO (12)
#define GPIO_MOSI (13)
#define GPIO_SCLK (14)
#define GPIO_CS   (15)

typedef void (*ButtonCallback) (uint8_t);

class Button {
    public: 
        Button(uint8_t _PIN) {
            PIN = _PIN;
            pinMode(_PIN, INPUT_PULLDOWN);
        }

        void setOnClick(ButtonCallback _call) {
            call = _call;
        }

        void requestLoop() {
            prev_state = state;
            state = digitalRead(PIN);

            if (prev_state == HIGH && state == LOW) {
                click_count++;
                call(PIN);
            }
        }
    private: 
        uint8_t PIN;
        int prev_state;
        int state = HIGH;
        uint8_t click_count = 0;
        ButtonCallback call;
};