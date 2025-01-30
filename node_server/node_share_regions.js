require('dotenv').config();
const env = process.env;
const express = require('express');
const path = require('path')
const app = express();
const fetch = require('node-fetch');
const dateFormat = require('date-format');
const { readFileSync } = require('fs');

app.use('/resources', express.static(path.join(__dirname, 'resources')))

const port = 3000;

let code_condition = [
    {
        name: 'skc',
        hasStatus: true,
        code: [0]
    },
    {
        name: 'ovc',
        code: [1,2]
    },
    {
        name: 'bkn',
        code: [3],
        hasStatus: true,
    },
    {
        name: 'ovc_ra_sn',
        code: [51, 53, 55]
    },
    {
        name: 'bkn_-ra',
        code: [61],
        hasStatus: true,
    },
    {
        name: 'bkn_ra',
        code: [63],
        hasStatus: true,
    },
    {
        name: 'bkn_+ra',
        code: [65],
        hasStatus: true,
    },
    {
        name: 'ovc_ra_sn',
        code: [66, 67]
    },
    {
        name: 'bkn_-sn',
        code: [71, 73, 75],
        hasStatus: true,
    },
    {
        name: 'ovc_sn',
        code: [71, 73, 75]
    },
    {
        name: 'bkn_+sn',
        code: [71, 73, 75],
        hasStatus: true,
    },
    {
        name: 'ovc_ha',
        code: [77]
    },
    {
        name: 'ovc_ts',
        code: [95]
    },
    {
        name: 'ovc_ts_ra',
        code: [96,97,98]
    },
    {
        name: 'ovc_ts_ha',
        code: [99]
    },
];

app.get('/regions', (req, res) => {

    if (!isConfirmInnerAPIKey(req.query.innerApiKey)) {
        return res.status(403).json({"message":"forbidden"});
    }
    
    let data = readFileSync('./regions.json');
    return res.send(data);
});

let package = {
    now: 0,
    fact: {
        condition: "",
        feels_like: "",
        humidity: "",
        icon: "",
        pressure_mm: "",
        season: "",
        temp: "",
        wind_dir: "",
        wind_gust: "",
        wind_speed: "",
    },
    forecasts: {
        date: "",
        parts: [
            {
                icon: "",
                condition: "",
                prec_prob: "",
                prec_mm: "",
                temp_avg: "",
                feels_like: "",
                pressure_mm: "",
                part_name: "night",
                wind_dir: "",
                wind_gust: "",
                wind_speed: ""
            },
            {
                icon: "",
                condition: "",
                prec_prob: "",
                prec_mm: "",
                temp_avg: "",
                feels_like: "",
                pressure_mm: "",
                part_name: "morning",
                wind_dir: "",
                wind_gust: "",
                wind_speed: ""
            },
            {
                icon: "",
                condition: "",
                prec_prob: "",
                prec_mm: "",
                temp_avg: "",
                feels_like: "",
                pressure_mm: "",
                part_name: "day",
                wind_dir: "",
                wind_gust: "",
                wind_speed: ""
            },
            {
                icon: "",
                condition: "",
                prec_prob: "",
                prec_mm: "",
                temp_avg: "",
                feels_like: "",
                pressure_mm: "",
                part_name: "evening",
                wind_dir: "",
                wind_gust: "",
                wind_speed: ""
            },
        ],
        sunrise: "",
        sunset: "",
    }
}

function getIcon(code, isDay = false) {
    for (let item of code_condition) {
        if (item.code.includes(code)) {
            return item.name + (item.hasStatus ? (isDay ? '_d' : '_n') : '');
        }
    }
}

function getFactByTime(obj, time) {
    let index = obj.hourly.time.indexOf(dateFormat.asString('yyyy-MM-dd', new Date()) + 'T' + time);
    return {
        condition: "",
        temp: parseInt(obj.hourly.temperature_2m[index]),
        feels_like: parseInt(obj.hourly.apparent_temperature[index]),
        wind_dir: 'N',
        wind_gust: parseInt(obj.hourly.wind_gusts_10m[index]),
        wind_speed: parseInt(obj.hourly.wind_speed_10m[index]),
        icon: getIcon(obj.hourly.weather_code[index], '00:00' != time),
        humidity: obj.hourly.relative_humidity_2m[index],
        part_name: '00:00' != time ? 'day' : 'night',
        pressure_mm: obj.hourly.pressure_msl[index] / 1.333,
        prec_prob: "",
        prec_mm: "",
        temp_avg: "",
    }
}

app.get('/weather', async(req, res) => {

    if (!isConfirmInnerAPIKey(req.query.innerApiKey)) {
        return res.status(403).json({"message":"forbidden"});
    }

    let response = await fetch(`https://api.open-meteo.com/v1/forecast?latitude=${req.query.lat}&longitude=${req.query.lon}&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&hourly=pressure_msl,relative_humidity_2m,temperature_2m,apparent_temperature,precipitation,rain,showers,snowfall,snow_depth,weather_code,wind_speed_10m,wind_direction_10m,wind_gusts_10m&daily=weather_code,temperature_2m_min,apparent_temperature_min,sunrise,sunset,daylight_duration,wind_speed_10m_max,wind_gusts_10m_max&timezone=Europe%2FMoscow&forecast_days=1`);

    let temp = await response.text();
    temp = JSON.parse(temp);

    package.now = (Date.parse(temp.current.time.replace('T', ' ') + ':00')) / 1000;
    package.forecasts.date = temp.daily.time[0];
    package.forecasts.sunrise = temp.daily.sunrise[0].split('T')[1];
    package.forecasts.sunset = temp.daily.sunset[0].split('T')[1];

    package.fact.temp = parseInt(temp.current.temperature_2m);
    package.fact.feels_like = parseInt(temp.current.apparent_temperature);
    package.fact.wind_dir = 'N';
    package.fact.wind_gust = parseInt(temp.current.wind_gusts_10m);
    package.fact.wind_speed = parseInt(temp.current.wind_speed_10m);
    package.fact.icon = getIcon(temp.current.weather_code, temp.current.is_day == 1);
    package.fact.humidity = temp.current.relative_humidity_2m;
    package.fact.pressure_mm = temp.current.pressure_msl / 1.333;
    
    package.forecasts.parts = [
        getFactByTime(temp, '00:00'),
        getFactByTime(temp, '12:00'),
    ];

    console.log(package);
    
    return res.json(package);
});

let isConfirmInnerAPIKey = (apiKey) => {
    return apiKey == env.INNER_API_KEY;
}

app.listen(port, () => {
    console.log(`App listening on port ${port}`)
})