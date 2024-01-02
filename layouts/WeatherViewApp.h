#include "epd_driver.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define weatherYandex_VIEW_APP (2)

#define PRINT_PARAM 1
#define PRINT_DATA 0
#define SAVE_LAST_DATA 1

#define AP_SSID "weatherYandex_STATION"
#define AP_PASS "0123456789"

#define White 0xFF
#define LightGrey 0xBB
#define Grey 0x88
#define DarkGrey 0x44
#define Black 0x00

GFXfont currentFont;

const char *ntpServer = "0.europe.pool.ntp.org";

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

        }

    private:

        // bool requestWeatherByLatLon(float lat, float lon) {
            
        //     HTTPClient _http;
        //     String _host = "api.weather.yandex.ru";
        //     String _uri = "/v2/forecast?lat=" + String(lat, 6) + "&lon=" + String(lon, 6);
        //     WiFiClient _client;
        //     _client.stop();

        //     _http.begin(_client, _host, 80, _uri, true);
        //     _http.addHeader("X-Yandex-API-Key", configService.getYandexApiKey());
        //     int _httpCode = _http.GET();

        //     if (_httpCode == HTTP_CODE_OK)
        //     {
        //         int _size = _client.available();
        //         char _data[_size];
        //         _http.getStream().readBytes(_data, _size);

        //         yandexJsonDecode(_data, _size);
        //         _client.stop();
        //         _http.end();

        //         return true;
        //     }

        //     return false;
        // }

        // bool yandexJsonDecode(char *jsonStr, int size)
        // {
        //     DynamicJsonDocument jsonDoc(size);                              // allocate the JsonDocument
        //     DeserializationError error = deserializeJson(jsonDoc, jsonStr); // Deserialize the JSON document
        //     if (error)
        //     { // Test if parsing succeeds.
        //         log_i("deserializeJson() failed: %s", error.c_str());
        //         return false;
        //     }
        //     // convert it to a JsonObject
        //     JsonObject jo = jsonDoc.as<JsonObject>();
        //     // read from JsonObject
        //     weatherYandex.fact.condition = jo["fact"]["condition"].as<char *>();
        //     weatherYandex.fact.daytime = jo["fact"]["daytime"].as<char *>();
        //     weatherYandex.fact.feels_like = jo["fact"]["feels_like"].as<int8_t>();
        //     weatherYandex.fact.humidity = jo["fact"]["humidity"].as<uint8_t>();
        //     weatherYandex.fact.icon = jo["fact"]["icon"].as<char *>();
        //     weatherYandex.fact.obs_time = jo["fact"]["obs_time"].as<int>();
        //     weatherYandex.fact.polar = jo["fact"]["polar"].as<bool>();
        //     weatherYandex.fact.pressure_mm = jo["fact"]["pressure_mm"].as<uint16_t>();
        //     weatherYandex.fact.pressure_pa = jo["fact"]["pressure_pa"].as<uint16_t>();
        //     weatherYandex.fact.season = jo["fact"]["season"].as<char *>();
        //     weatherYandex.fact.temp = jo["fact"]["temp"].as<int8_t>();
        //     weatherYandex.fact.temp_water = jo["fact"]["temp_water"].as<int8_t>();
        //     weatherYandex.fact.wind_dir = jo["fact"]["wind_dir"].as<char *>();
        //     weatherYandex.fact.wind_gust = jo["fact"]["wind_gust"].as<float>();
        //     weatherYandex.fact.wind_speed = jo["fact"]["wind_speed"].as<float>();

        //     weatherYandex.forecast.date = jo["forecast"]["date"].as<String>();
        //     weatherYandex.forecast.date_ts = jo["forecast"]["date_ts"].as<int>();
        //     weatherYandex.forecast.moon_code = jo["forecast"]["moon_code"].as<uint8_t>();
        //     weatherYandex.forecast.moon_text = jo["forecast"]["moon_text"].as<char *>();
        //     for (uint8_t i = 0; i < 2; i++)
        //     {
        //         weatherYandex.forecast.parts[i].condition = jo["forecast"]["parts"][i]["condition"].as<char *>();
        //         weatherYandex.forecast.parts[i].daytime = jo["forecast"]["parts"][i]["daytime"].as<char *>();
        //         weatherYandex.forecast.parts[i].feels_like = jo["forecast"]["parts"][i]["feels_like"].as<int8_t>();
        //         weatherYandex.forecast.parts[i].humidity = jo["forecast"]["parts"][i]["humidity"].as<uint8_t>();
        //         weatherYandex.forecast.parts[i].icon = jo["forecast"]["parts"][i]["icon"].as<char *>();
        //         weatherYandex.forecast.parts[i].part_name = jo["forecast"]["parts"][i]["part_name"].as<char *>();
        //         weatherYandex.forecast.parts[i].polar = jo["forecast"]["parts"][i]["polar"].as<bool>();
        //         weatherYandex.forecast.parts[i].prec_mm = jo["forecast"]["parts"][i]["prec_mm"].as<float>();
        //         weatherYandex.forecast.parts[i].prec_period = jo["forecast"]["parts"][i]["prec_period"].as<uint16_t>();
        //         weatherYandex.forecast.parts[i].prec_prob = jo["forecast"]["parts"][i]["prec_prob"].as<uint8_t>();
        //         weatherYandex.forecast.parts[i].pressure_mm = jo["forecast"]["parts"][i]["pressure_mm"].as<uint16_t>();
        //         weatherYandex.forecast.parts[i].pressure_pa = jo["forecast"]["parts"][i]["pressure_pa"].as<uint16_t>();
        //         weatherYandex.forecast.parts[i].temp_avg = jo["forecast"]["parts"][i]["temp_avg"].as<int8_t>();
        //         weatherYandex.forecast.parts[i].temp_max = jo["forecast"]["parts"][i]["temp_max"].as<int8_t>();
        //         weatherYandex.forecast.parts[i].temp_min = jo["forecast"]["parts"][i]["temp_min"].as<int8_t>();
        //         weatherYandex.forecast.parts[i].temp_water = jo["forecast"]["parts"][i]["temp_water"].as<uint8_t>();
        //         weatherYandex.forecast.parts[i].wind_dir = jo["forecast"]["parts"][i]["wind_dir"].as<char *>();
        //         weatherYandex.forecast.parts[i].wind_gust = jo["forecast"]["parts"][i]["wind_gust"].as<float>();
        //         weatherYandex.forecast.parts[i].wind_speed = jo["forecast"]["parts"][i]["wind_speed"].as<float>();
        //     }
        //     weatherYandex.forecast.sunrise = jo["forecast"]["sunrise"].as<char *>();
        //     weatherYandex.forecast.sunset = jo["forecast"]["sunset"].as<char *>();
        //     weatherYandex.forecast.week = jo["forecast"]["week"].as<uint8_t>();
        //     weatherYandex.info.lat = jo["info"]["lat"].as<float>();
        //     weatherYandex.info.lon = jo["info"]["lon"].as<float>();
        //     weatherYandex.info.url = jo["info"]["url"].as<char *>();
        //     weatherYandex.now = jo["now"].as<int>();
        //     weatherYandex.now_dt = jo["now_dt"].as<char *>();
        //     return true;
        // }
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
        }
};