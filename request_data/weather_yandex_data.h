typedef struct
{
    String condition;
    String daytime;
    int8_t feels_like;
    uint8_t humidity;
    String icon;
    int obs_time;
    bool polar;
    uint16_t pressure_mm;
    uint16_t pressure_pa;
    String season;
    int8_t temp;
    int8_t temp_water;
    String wind_dir;
    float wind_gust;
    float wind_speed;
} fact_weather_yandex_t;

typedef struct
{
    String condition;
    String daytime;
    int8_t feels_like;
    uint8_t humidity;
    String icon;
    String part_name;
    bool polar;
    float prec_mm;
    uint16_t prec_period;
    uint8_t prec_prob;
    uint16_t pressure_mm;
    uint16_t pressure_pa;
    int8_t temp_avg;
    int8_t temp_max;
    int8_t temp_min;
    int8_t temp_water;
    String wind_dir;
    float wind_gust;
    float wind_speed;
} forecast_part_t;

typedef struct
{
    String date;
    int date_ts;
    uint8_t moon_code;
    String moon_text;
    forecast_part_t parts[2];
    String sunrise;
    String sunset;
    uint16_t week;
} forecast_weather_yandex_t;

typedef struct
{
    float lat;
    float lon;
    String url;
} info_t;

typedef struct
{
    fact_weather_yandex_t fact;
    forecast_weather_yandex_t forecast;
    info_t info;
    int now;
    String now_dt;
} weather_yandex_t;
