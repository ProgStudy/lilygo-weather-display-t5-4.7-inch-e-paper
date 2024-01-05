#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_adc_cal.h"

#define White 0xFF
#define LightGrey 0xBB
#define Grey 0x88
#define DarkGrey 0x44
#define Black 0x00

#define L_SIZE 250
#define S_SIZE 100

//Wind
// const String TXT_WIND_SPEED_DIRECTION = "Wind Speed/Direction";
const String TXT_N   = "N";
const String TXT_NNE = "NNE";
const String TXT_NE  = "NE";
const String TXT_ENE = "ENE";
const String TXT_E   = "E";
const String TXT_ESE = "ESE";
const String TXT_SE  = "SE";
const String TXT_SSE = "SSE";
const String TXT_S   = "S";
const String TXT_SSW = "SSW";
const String TXT_SW  = "SW";
const String TXT_WSW = "WSW";
const String TXT_W   = "W";
const String TXT_WNW = "WNW";
const String TXT_NW  = "NW";
const String TXT_NNW = "NNW";

//Day of the week
const char* weekday_D[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

//Month
const char* month_M[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

GFXfont currentFont;

const char *ntpServer = "0.europe.pool.ntp.org";

uint8_t currentHour = 0, currentMin = 0, currentSec = 0, eventCnt = 0;

weather_yandex_t weatherYandex;

enum alignment
{
  LEFT,
  RIGHT,
  CENTER
};

enum icon_size
{
  SmallIcon,
  LargeIcon
};

class WeatherViewAppLayout {
    public:
        void show() {
            
            free(framebuffer);
            displayService.setMemBufferDisplay();

            setup_time();

            if (requestWeatherByLatLon(55.755376, 37.619595)) {
                showUpdateData();
            }

            refresh();
        }


        void refresh() {
            delay(1000*60*configService.getIntervalTimeRefresh());
            
            update_local_time();
            
            if (requestWeatherByLatLon(55.755376, 37.619595)) {
                showUpdateData();
            }
            
            epd_poweroff_all();

            this->refresh();
        }

        void update() {
        }

        void click1() {
            Serial.println("<<<");
        }

        void click2() {
            Serial.println(">>>");
        }

        void click3() {}

        void dismissOnClick() {
            isActive = false;
        }

    private:
        bool isActive = true;
        bool requestWeatherByLatLon(float lat, float lon) {
            
            remoteServer rS = configService.getRemoteServer();

            HTTPClient _http;
            String _host = rS.host;
            String _uri = REMOTE_PATH_WEATHER + "?lat=" + String(lat, 6) + "&lon=" + String(lon, 6);
            WiFiClient _client;

            _http.begin(_client, _host, rS.port, _uri, true);

            int _httpCode = _http.GET();

            if (_httpCode == HTTP_CODE_OK)
            {
                int _size = _client.available();

                String _data = _http.getString();

                bool isResult = yandexJsonDecode(_data, _data.length());

                _client.stop();
                _http.end();

                return isResult;
            } else {
                Serial.println("connection failed, error[" + String(_httpCode) + "]: " + String(_http.errorToString(_httpCode).c_str()));
            }

            return false;
        }

        bool yandexJsonDecode(String jsonStr, int size)
        {
            DynamicJsonDocument jsonDoc(1440);
            DeserializationError error = deserializeJson(jsonDoc, jsonStr); // Deserialize the JSON document
            
            if (error)
            { // Test if parsing succeeds.
                Serial.println("deserializeJson() failed: " + String(error.c_str()));
                return false;
            }
            // // convert it to a JsonObject
            JsonObject jo = jsonDoc.as<JsonObject>();
            // // // read from JsonObject
            weatherYandex.fact.condition = (const char*) jo["fact"]["condition"];
            weatherYandex.fact.feels_like = (const int8_t) jo["fact"]["feels_like"];
            weatherYandex.fact.humidity = (const uint8_t) jo["fact"]["humidity"];
            weatherYandex.fact.icon = (const char*) jo["fact"]["icon"];
            weatherYandex.fact.pressure_mm = (const uint16_t) jo["fact"]["pressure_mm"];
            weatherYandex.fact.season = (const char*) jo["fact"]["season"];
            weatherYandex.fact.temp = (const int8_t) jo["fact"]["temp"];
            weatherYandex.fact.wind_dir = (const float) jo["fact"]["wind_dir"];
            weatherYandex.fact.wind_gust = (const float) jo["fact"]["wind_gust"];
            weatherYandex.fact.wind_speed = (const float) jo["fact"]["wind_speed"];

            // weatherYandex.forecast.date = String((const char*) jo["forecast"]["date"]);

            for (uint8_t i = 0; i < 2; i++)
            {
                weatherYandex.forecast.parts[i].condition = String((const char*) jo["forecasts"]["parts"][i]["condition"]);
                weatherYandex.forecast.parts[i].condition = String((const char*) jo["forecasts"]["parts"][i]["condition"]);
                weatherYandex.forecast.parts[i].icon = String((const char*) jo["forecasts"]["parts"][i]["icon"]);
                weatherYandex.forecast.parts[i].part_name = String((const char*) jo["forecasts"]["parts"][i]["part_name"]);
                weatherYandex.forecast.parts[i].wind_dir = String((const char*) jo["forecasts"]["parts"][i]["wind_dir"]);
                weatherYandex.forecast.parts[i].wind_gust = (const float) jo["forecasts"]["parts"][i]["wind_gust"];
                weatherYandex.forecast.parts[i].wind_speed = (const float) jo["forecasts"]["parts"][i]["wind_speed"];
                weatherYandex.forecast.parts[i].feels_like = (const int) jo["forecasts"]["parts"][i]["feels_like"];
                weatherYandex.forecast.parts[i].pressure_mm = (const int) jo["forecasts"]["parts"][i]["pressure_mm"];
                weatherYandex.forecast.parts[i].temp_avg = (const int) jo["forecasts"]["parts"][i]["temp_avg"];
                weatherYandex.forecast.parts[i].prec_mm = (const float) jo["forecasts"]["parts"][i]["prec_mm"];
                weatherYandex.forecast.parts[i].prec_prob = (const int) jo["forecasts"]["parts"][i]["prec_prob"];
            }
            weatherYandex.forecast.sunrise = (const char*) jo["forecasts"]["sunrise"];
            weatherYandex.forecast.sunset = (const char*) jo["forecasts"]["sunset"];
            weatherYandex.now = (const int) jo["now"];
            jsonDoc.clear();
            return true;
        }

        void showUpdateData() {
            // displayService.setMemBufferDisplay();
            epd_poweron();
            epd_clear();
            // display_info...
            setFont(osans12b);
            drawString(10, 15, "Москва", LEFT);
            drawString(400, 15, convert_unix_time(weatherYandex.now), LEFT);
            draw_battery(680, 30);

            // display_weather...
            drawLine(0, 50, EPD_WIDTH, 50, Black);
            display_fact_weather();
            display_forecast_weather();

            this->edp_update();
            // delay(5000);
            // epd_poweroff_all();
        }

        void display_fact_weather()
        {
            draw_wind_section(830, 200, weatherYandex.fact.wind_dir, weatherYandex.fact.wind_speed, weatherYandex.fact.wind_gust, 100, true);
            setFont(osans18b);
            drawString(20, 60, getSeason(weatherYandex.fact.season), LEFT);
            draw_thp_section(480, 70);
            this->draw_conditions_section(20, 50, weatherYandex.fact.icon, 0, LargeIcon);
            draw_sun_section(480, 330);
        }

        void draw_thp_section(uint16_t x, uint16_t y) // temperature, humidity, pressure section
        {
            int xOffset = 20;
            setFont(osans48b);
            drawString(x, y, String(weatherYandex.fact.temp) + " °C", CENTER);
            y += osans26b.advance_y + 4;

            setFont(osans16b);
            drawString(x, y, String(weatherYandex.fact.feels_like) + " °C", CENTER);
            y += osans8b.advance_y;

            setFont(osans8b);
            drawString(x, y, "(ощущается)", CENTER);
            y += osans12b.advance_y;

            setFont(osans24b);
            int sw = drawString(x - xOffset, y, String(weatherYandex.fact.humidity) + "%", RIGHT);

            // uint8_t *data = load_file("blob.bin");
            // if (data != NULL)
            // {
            //     Rect_t area = {
            //         .x = x - xOffset - sw - 40,
            //         .y = y,
            //         .width = 36,
            //         .height = 40};
            //     epd_draw_grayscale_image(area, (uint8_t *)data);
            //     free(data);
            // }

            int ex;
            ex = drawString(x + xOffset, y, String(weatherYandex.fact.pressure_mm), LEFT) + 5;

            setFont(osans10b);
            //drawString(x + xOffset + ex, y, "mm/Hg", LEFT);
            int exx = drawString(x + xOffset + ex, y, "mm", LEFT);
            y += osans10b.advance_y / 2;
            drawLine(x + xOffset + ex, y + 2, x + xOffset + ex + exx, y + 2, Black);
            drawString(x + xOffset + ex, y, "Hg", LEFT);
        }

        String convert_unix_time(int unix_time)
        {
            time_t tm = unix_time;
            struct tm *now_tm = localtime(&tm);
            char output[40];
            strftime(output, sizeof(output), "%H:%M %d.%m.%y", now_tm);
            return output;
        }

        bool setup_time()
        {
            configTime((3 * 3600), 0, ntpServer, "time.nist.gov");
            delay(100);
            return update_local_time();
        }

        bool update_local_time()
        {
            struct tm timeinfo;
            char time_output[30], day_output[30], update_time[30];
            while (!getLocalTime(&timeinfo, 5000))
            { // Wait for 5-sec for time to synchronise
                log_i("Failed to obtain time");
                return false;
            }
            currentHour = timeinfo.tm_hour;
            currentMin = timeinfo.tm_min;
            currentSec = timeinfo.tm_sec;
            sprintf(day_output, "%s, %02u %s %04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900);
            strftime(update_time, sizeof(update_time), "%H:%M:%S", &timeinfo); // Creates: '@ 14:05:49'   and change from 30 to 8 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
            sprintf(time_output, "%s", update_time);
            return true;
        }

        void draw_conditions_section(int x, int y, String IconName, uint8_t forecast_part, bool IconSize)
        {
            String fileName = "";
            fileName += IconName + ((IconSize == LargeIcon) ? ("L") : (""));
            fileName += ".bin";

            uint8_t *data = load_file(fileName);
            if (data != NULL)
            {
                Rect_t area = {
                    .x = x,
                    .y = y,
                    .width = ((IconSize == LargeIcon) ? (L_SIZE) : (S_SIZE)),
                    .height = ((IconSize == LargeIcon) ? (L_SIZE) : (S_SIZE))};
                epd_draw_grayscale_image(area, (uint8_t *)data);
                free(data);
            }
            else
            {
                Serial.println("No data image: " + fileName);
                if (IconSize == LargeIcon)
                setFont(osans18b);
                else
                setFont(osans10b);
                drawString(x, y, IconName, LEFT);
                getIcon(IconName);
            }

            if (IconSize == LargeIcon)
            {
                drawStringWithLB(x + L_SIZE / 2, y + L_SIZE + 5, get_description_condition(weatherYandex.fact.condition), osans8b, CENTER);
            }
            else
            {
                drawStringWithLB(x + 10, y + S_SIZE - 5, get_description_condition(weatherYandex.forecast.parts[forecast_part].condition), osans6b, LEFT);
                uint8_t prec_prob = weatherYandex.forecast.parts[forecast_part].prec_prob;
                drawString(x + S_SIZE / 2, y + S_SIZE + 30, String(weatherYandex.forecast.parts[forecast_part].prec_mm, 1) + "mm", CENTER);
                drawString(x + S_SIZE / 2, y + S_SIZE + 45, String(prec_prob) + "%", CENTER);
            }
        }

        void display_forecast_weather()
        {
            int y = 350;
            drawLine(0, y, EPD_WIDTH, y, Black);
            drawLine(EPD_WIDTH / 2, y, EPD_WIDTH / 2, EPD_HEIGHT, Black);
            int xOffSet = EPD_WIDTH / 2;
            for (uint8_t i = 0; i < 2; i++)
            {
                setFont(osans10b);
                drawString(i * xOffSet + 10, y + 5, get_partName(weatherYandex.forecast.parts[i].part_name), LEFT);
                draw_conditions_section(i * xOffSet + 10, y + 20, weatherYandex.forecast.parts[i].icon, i, SmallIcon);
                draw_wind_section((i * xOffSet) + (i + xOffSet - 90), y + 90,
                                weatherYandex.forecast.parts[i].wind_dir,
                                weatherYandex.forecast.parts[i].wind_speed,
                                weatherYandex.forecast.parts[i].wind_gust,
                                60, false);
                draw_thp_forecast_section(i * xOffSet + 210, y + 30, i);
            }
        }

        void draw_RSSI(int x, int y, int rssi)
        {
            int WIFIsignal = 0;
            int xpos = 1;
            for (int _rssi = -100; _rssi <= rssi; _rssi = _rssi + 20)
            {
                if (_rssi <= -20)
                WIFIsignal = 20; //            <-20dbm displays 5-bars
                if (_rssi <= -40)
                WIFIsignal = 16; //  -40dbm to  -21dbm displays 4-bars
                if (_rssi <= -60)
                WIFIsignal = 12; //  -60dbm to  -41dbm displays 3-bars
                if (_rssi <= -80)
                WIFIsignal = 8; //  -80dbm to  -61dbm displays 2-bars
                if (_rssi <= -100)
                WIFIsignal = 4; // -100dbm to  -81dbm displays 1-bar
                fillRect(x + xpos * 8, y - WIFIsignal, 6, WIFIsignal, Black);
                xpos++;
            }
        }

        String getSeason(String season)
        {
            if (season == "summer")
                return "Лето";
            if (season == "autumn")
                return "Осень";
            if (season == "winter")
                return "Зима";
            if (season == "spring")
                return "Весна";
            return season;
        }

        void draw_battery(int x, int y)
        {
            int vref = 1100;
            uint8_t _percentage = 100;
            esp_adc_cal_characteristics_t adc_chars;
            esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
            if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
            {
                log_i("eFuse Vref:%u mV", adc_chars.vref);
                vref = adc_chars.vref;
            }
            float _voltage = analogRead(36) / 4096.0 * 6.566 * (vref / 1000.0);
            if (_voltage > 1)
            { // Only display if there is a valid reading
                log_i("Voltage = %.2f", _voltage);
                _percentage = 2836.9625 * pow(_voltage, 4) - 43987.4889 * pow(_voltage, 3) + 255233.8134 * pow(_voltage, 2) - 656689.7123 * _voltage + 632041.7303;
                if (_voltage >= 4.20)
                _percentage = 100;
                if (_voltage <= 3.20)
                _percentage = 0; // orig 3.5
                drawRect(x + 25, y - 14, 40, 15, Black);
                fillRect(x + 65, y - 10, 4, 7, Black);
                fillRect(x + 27, y - 12, 36 * _percentage / 100.0, 11, Black);
                drawString(x + 85, y - 14, String(_percentage) + "%  " + String(_voltage, 1) + "v", LEFT);
            }
        }
        void arrow(int x, int y, int asize, float aangle, int pwidth, int plength)
        {
            float arr;
            if (aangle > 180.0)
                arr = aangle - 180.0;
            else
                arr = aangle + 180.0;

            float dx = (float)(asize - 10) * cos((arr - 90) * PI / 180) + x; // calculate X position
            float dy = (float)(asize - 10) * sin((arr - 90) * PI / 180) + y; // calculate Y position
            float x1 = 0;
            float y1 = plength;
            float x2 = pwidth / 2;
            float y2 = pwidth / 2;
            float x3 = -pwidth / 2;
            float y3 = pwidth / 2;
            float angle = arr * PI / 180 - 135;
            float xx1 = x1 * cos(angle) - y1 * sin(angle) + dx;
            float yy1 = y1 * cos(angle) + x1 * sin(angle) + dy;
            float xx2 = x2 * cos(angle) - y2 * sin(angle) + dx;
            float yy2 = y2 * cos(angle) + x2 * sin(angle) + dy;
            float xx3 = x3 * cos(angle) - y3 * sin(angle) + dx;
            float yy3 = y3 * cos(angle) + x3 * sin(angle) + dy;
            fillTriangle(xx1, yy1, xx3, yy3, xx2, yy2, Black);
        }
        void draw_wind_section(int x, int y, String dir, float speed, float gust, int Cradius, bool fact)
        {
            if (fact)
            {
                if (dir != "c")
                arrow(x, y, Cradius - 22, get_wind_angle(dir), 16, 33);
            }
            else
            {
                if (dir != "c")
                arrow(x, y, Cradius - 10, get_wind_angle(dir), 8, 20);
            }
            setFont(osans8b);
            int dxo, dyo, dxi, dyi;
            drawCircle(x, y, Cradius, Black, false);       // Draw compass circle
            drawCircle(x, y, Cradius + 1, Black, false);   // Draw compass circle
            drawCircle(x, y, Cradius + 2, Black, false);   // Draw compass circle
            drawCircle(x, y, Cradius * 0.7, Black, false); // Draw compass inner circle
            for (float a = 0; a < 360; a = a + 22.5)
            {
                dxo = Cradius * cos((a - 90) * PI / 180);
                dyo = Cradius * sin((a - 90) * PI / 180);
                if (a == 45)
                drawString(dxo + x + 15, dyo + y - 18, TXT_NE, CENTER);
                if (a == 135)
                drawString(dxo + x + 20, dyo + y - 2, TXT_SE, CENTER);
                if (a == 225)
                drawString(dxo + x - 20, dyo + y - 2, TXT_SW, CENTER);
                if (a == 315)
                drawString(dxo + x - 15, dyo + y - 18, TXT_NW, CENTER);
                dxi = dxo * 0.9;
                dyi = dyo * 0.9;
                drawLine(dxo + x, dyo + y, dxi + x, dyi + y, Black);
                dxo = dxo * 0.7;
                dyo = dyo * 0.7;
                dxi = dxo * 0.9;
                dyi = dyo * 0.9;
                drawLine(dxo + x, dyo + y, dxi + x, dyi + y, Black);
            }
            drawString(x, y - Cradius - 20, TXT_N, CENTER);
            drawString(x, y + Cradius + 10, TXT_S, CENTER);
            drawString(x - Cradius - 15, y - 5, TXT_W, CENTER);
            drawString(x + Cradius + 10, y - 5, TXT_E, CENTER);

            if (fact)
            {
                setFont(osans12b);
                String wind = dir;
                wind.toUpperCase();
                drawString(x, y - 55, wind, CENTER);
                setFont(osans24b);
                drawString(x, y - 33, String(speed, 1), CENTER);
                setFont(osans12b);
                drawString(x, y + 14, String(gust, 1), CENTER);
                setFont(osans12b);
                drawString(x, y + 40, "м/с", CENTER);
            }
            else
            {
                setFont(osans8b);
                String wind = dir;
                wind.toUpperCase();
                drawString(x, y - 35, wind, CENTER);
                setFont(osans12b);
                drawString(x, y - 17, String(speed, 1), CENTER);
                setFont(osans8b);
                drawString(x, y + 5, String(gust, 1), CENTER);
                setFont(osans8b);
                drawString(x, y + 20, "м/с", CENTER);
            }
        }

        void draw_sun_section(uint16_t x, uint16_t y)
        {
            float x1, y1;
            int16_t r = 100;
            y += 25;

            bool pen = true;
            for (uint8_t i = 0; i < 1; i++)
            {
                for (float a = 30; a <= 150;)
                {
                x1 = (r + i) * cos((a - 180.0) / 180.0 * PI) + x;
                y1 = (r + i) * sin((a - 180.0) / 180.0 * PI) + y;
                drawPixel(x1, y1, Black);
                a += 0.01;
                }
            }

            uint8_t *data;
            data = load_file("sunrise.bin");
            if (data != NULL)
            {
                Rect_t area = {.x = x - r - 10, .y = y - 50, .width = 47, .height = 35};
                epd_draw_grayscale_image(area, (uint8_t *)data);
                free(data);
            }
            data = load_file("sunset.bin");
            if (data != NULL)
            {
                Rect_t area = {.x = x + r - 35, .y = y - 50, .width = 47, .height = 40};
                epd_draw_grayscale_image(area, (uint8_t *)data);
                free(data);
            }
            setFont(osans10b);
            drawString(x - r - 20, y - 35, weatherYandex.forecast.sunrise, RIGHT);
            drawString(x + r + 20, y - 35, weatherYandex.forecast.sunset, LEFT);
        }

        void draw_thp_forecast_section(uint16_t x, uint16_t y, uint8_t part)
        {
            setFont(osans24b);
            drawString(x, y, String(weatherYandex.forecast.parts[part].temp_avg) + " °C", CENTER);

            setFont(osans18b);
            drawString(x, y + 45, String(weatherYandex.forecast.parts[part].feels_like) + " °C", CENTER);

            setFont(osans6b);
            drawString(x, y + 73, "(ощущается)", CENTER);

            setFont(osans10b);
            drawString(x, y + 95, String(weatherYandex.forecast.parts[part].pressure_mm), CENTER);

            setFont(osans6b);
            drawString(x, y + 110, "mm/Hg", CENTER);
        }

        String get_description_condition(String str)
        {
            String retStr = str;
            if (str == "clear")
                retStr = "Ясно";
            if (str == "partly-cloudy")
                retStr = "Малооблачно";
            if (str == "cloudy")
                retStr = "Облачно с прояснениями";
            if (str == "overcast")
                retStr = "Пасмурно";
            if (str == "drizzle")
                retStr = "Моросящий дождь";
            if (str == "light-rain")
                retStr = "Небольшой дождь";
            if (str == "rain")
                retStr = "Дождь";
            if (str == "moderate-rain")
                retStr = "Умеренно сильный дождь";
            if (str == "heavy-rain")
                retStr = "Сильный дождь";
            if (str == "continuous-heavy-rain")
                retStr = "Длительный сильный дождь";
            if (str == "showers")
                retStr = "Ливень";
            if (str == "wet-snow")
                retStr = "Дождь со снегом";
            if (str == "light-snow")
                retStr = "Небольшой снег";
            if (str == "snow")
                retStr = "Снег";
            if (str == "snow-showers")
                retStr = "Снегопад";
            if (str == "hail")
                retStr = "Град";
            if (str == "thunderstorm")
                retStr = "Гроза";
            if (str == "thunderstorm-with-rain")
                retStr = "Дождь с грозой";
            if (str == "thunderstorm-with-hail")
                retStr = "Гроза с градом";
            return retStr;
        }

        int16_t get_wind_angle(String dir)
        {
            if (dir == "nw")
                return 315;
            if (dir == "n")
                return 0;
            if (dir == "ne")
                return 45;
            if (dir == "e")
                return 90;
            if (dir == "se")
                return 135;
            if (dir == "s")
                return 180;
            if (dir == "sw")
                return 225;
            if (dir == "w")
                return 270;
            if (dir == "c")
                return -1;
        }
        // samples functions...  
        int drawString(int x, int y, String text, alignment align)
        {
            char *data = const_cast<char *>(text.c_str());
            int x1, y1; // the bounds of x,y and w and h of the variable 'text' in pixels.
            int w, h;
            int xx = x, yy = y;
            get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);
            if (align == RIGHT)
                x = x - w;
            if (align == CENTER)
                x = x - w / 2;
            int cursor_y = y + h;
            write_string(&currentFont, data, &x, &cursor_y, framebuffer);
            return w;
        }

        void drawStringWithLB(int x, int y, String str, GFXfont font, alignment align)
        {
            int8_t _index = str.indexOf(' ');
            //_index /= 2;
            log_i("str: %s, lenght: %d", str.c_str(), str.length());
            log_i("str.indexOf(\" \"): %d", _index);
            setFont(font);
            if (_index == -1)
            {
                drawString(x, y + font.advance_y / 2, str, align);
                return;
            }
            String _str = str.substring(0, _index);
            drawString(x, y, _str, align);

            _str = str.substring(_index + 1, str.length());
            drawString(x, y + font.advance_y, _str, align);
        }

        bool getIcon(String iconName)
        {
            HTTPClient _http;
            String _host = "yastatic.net";
            String _uri = "/weather/i/icons/funky/dark/";
            WiFiClient _client;
            _client.stop();

            _http.begin(_client, _host, 80, String(_uri + iconName + ".svg"), true);
            int _httpCode = _http.GET();

            if (_httpCode == HTTP_CODE_OK)
            {
                int _size = _client.available();

                File f = SPIFFS.open(String("/" + iconName + ".svg"), FILE_WRITE);
                uint8_t *_data;
                _data = (uint8_t *)ps_calloc(sizeof(uint8_t), _size);
                _client.readBytes(_data, _size);
                f.write(_data, _size);
                f.close();
                free(_data);
                return true;
            }
            else
            {
                return false;
            }
        }

        uint8_t *load_file(String fileName)
        {
            String _fileName = "/resources/" + fileName;
            uint8_t *data;
            if (SPIFFS.exists(_fileName))
            {
                Serial.println(_fileName);

                File f = SPIFFS.open(_fileName, FILE_READ);
                int size = f.size();
                data = (uint8_t *)ps_calloc(sizeof(uint8_t), size);
                f.readBytes((char *)data, size);
                f.close();
                return data;
            }
            else
            {
                Serial.println("file not found");
                data = NULL;
                return data;
            }
        };


        String get_partName(String pName)
        {
            if (pName == "night")
                return "Ночь";
            if (pName == "morning")
                return "Утро";
            if (pName == "day")
                return "День";
            if (pName == "evening")
                return "Вечер";
            return pName;
        }

        void fillCircle(int x, int y, int r, uint8_t color)
        {
            epd_fill_circle(x, y, r, color, framebuffer);
        }

        void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
        {
            epd_write_line(x0, y0, x1, y1, color, framebuffer);
        }

        void drawCircle(int x0, int y0, int r, uint8_t color, bool fill)
        {
        if (fill)
            epd_fill_circle(x0, y0, r, color, framebuffer);
        else
            epd_draw_circle(x0, y0, r, color, framebuffer);
        }

        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
        {
            epd_draw_rect(x, y, w, h, color, framebuffer);
        }

        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
        {
            epd_fill_rect(x, y, w, h, color, framebuffer);
        }

        void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                        int16_t x2, int16_t y2, uint16_t color)
        {
            epd_fill_triangle(x0, y0, x1, y1, x2, y2, color, framebuffer);
        }

        void drawPixel(int x, int y, uint8_t color)
        {
            epd_draw_pixel(x, y, color, framebuffer);
        }

        void setFont(GFXfont const &font)
        {
            currentFont = font;
        }

        void edp_update()
        {
            epd_draw_grayscale_image(epd_full_screen(), framebuffer); // Update the screen
            free(framebuffer);
            displayService.setMemBufferDisplay();
        }
};