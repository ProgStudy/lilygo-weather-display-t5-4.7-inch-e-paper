require('dotenv').config();
const env = process.env;
const express = require('express');
const path = require('path')
const app = express();
const fetch = require('node-fetch');
const dateFormat = require('date-format');
const seasonService = require('date-season')();
const { readFileSync } = require('fs');

app.use('/resources', express.static(path.join(__dirname, 'resources')))

const port = 3000;

let code_condition = [
    {
        name: 'skc',
        hasStatus: true,
        code: [0],
        message: 'Ясно',
    },
    {
        name: 'ovc',
        code: [1,2],
        message: 'Пасмурно',
    },
    {
        name: 'bkn',
        code: [3],
        hasStatus: true,
        message: 'Переменная облачность',
    },
    {
        name: 'ovc_ra_sn',
        code: [51, 53, 55, 66, 67],
        message: 'Снег с дождем',
    },
    {
        name: 'bkn_-ra',
        code: [61],
        hasStatus: true,
        message: 'Слабый дождь',
    },
    {
        name: 'bkn_ra',
        code: [63],
        hasStatus: true,
        message: 'Дождь',

    },
    {
        name: 'bkn_+ra',
        code: [65],
        hasStatus: true,
        message: 'Сильный дождь',
        
    },
    {
        name: 'bkn_-sn',
        code: [71, 73, 75],
        hasStatus: true,
        message: 'Слабый снег',
    },
    {
        name: 'ovc_sn',
        code: [71, 73, 75],
        message: 'Пасмурно со снегом',
    },
    {
        name: 'bkn_+sn',
        code: [71, 73, 75],
        hasStatus: true,
        message: 'Метель',
    },
    {
        name: 'Град',
        code: [77],
        message: 'Пасмурно со снегом',
    },
    {
        name: 'ovc_ts',
        code: [95],
        message: 'Гроза',
    },
    {
        name: 'ovc_ts_ra',
        code: [96,97,98],
        message: 'Гроза с дождем',
    },
    {
        name: 'ovc_ts_ha',
        code: [99],
        message: 'Гроза с градом',
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
            return {icon: item.name + (item.hasStatus ? (isDay ? '_d' : '_n') : ''), condition: item.message};
        }
    }
}

function getTimeName(time) {

    let hours = time.getHours();    

    if (hours >= 0 && hours < 4) {
        return 'night';
    }

    if (hours >= 4 && hours < 9) {
        return 'morning';
    }

    if (hours >= 9 && hours < 16) {
        return 'day';
    }

    return 'evening';
}
function getDegWindDir(dir) {

    if (dir == 315) {
        return "nw";
    }
    if (dir == 0) {
        return "n";
    }
    if (dir == 45) {
        return "ne";
    }
    if (dir == 90) {
        return "e";
    }
    if (dir == 135) {
        return "se";
    }
    if (dir == 180) {
        return "s";
    }
    if (dir == 225) {
        return "sw";
    }
    if (dir == 270) {
        return "w";
    }
    
    return 'c';
}

function getFactByTime(obj, time) {
    let now = new Date();
    now.setHours(time.split(':')[0]);

    let index = obj.hourly.time.indexOf(dateFormat.asString('yyyy-MM-dd', new Date()) + 'T' + time);
    let iconObj = getIcon(obj.hourly.weather_code[index], '00:00' != time);
    return {
        condition: iconObj.condition,
        temp: parseInt(obj.hourly.temperature_2m[index]),
        feels_like: parseInt(obj.hourly.apparent_temperature[index]),
        wind_dir: getDegWindDir(obj.hourly.wind_direction_10m[index]),
        wind_gust: parseInt(obj.hourly.wind_gusts_10m[index]),
        wind_speed: parseInt(obj.hourly.wind_speed_10m[index]),
        icon: iconObj.icon,
        humidity: obj.hourly.relative_humidity_2m[index],
        part_name: getTimeName(now),
        pressure_mm: obj.hourly.pressure_msl[index] / 1.333,
        prec_prob: 10,
        prec_mm: 10,
        temp_avg: 10,
    }
}

app.get('/weather', async(req, res) => {

    if (!isConfirmInnerAPIKey(req.query.innerApiKey)) {
        return res.status(403).json({"message":"forbidden"});
    }

    let response = await fetch(`https://api.open-meteo.com/v1/forecast?latitude=${req.query.lat}&longitude=${req.query.lon}&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure,wind_speed_10m,wind_direction_10m,wind_gusts_10m&hourly=pressure_msl,relative_humidity_2m,temperature_2m,apparent_temperature,precipitation,rain,showers,snowfall,snow_depth,weather_code,wind_speed_10m,wind_direction_10m,wind_gusts_10m&daily=weather_code,temperature_2m_min,apparent_temperature_min,sunrise,sunset,daylight_duration,wind_speed_10m_max,wind_gusts_10m_max&timezone=Europe%2FMoscow&forecast_days=1`);

    let temp = await response.text();
    temp = JSON.parse(temp);

    let iconObj = getIcon(temp.current.weather_code, temp.current.is_day == 1);

    package.now = (Date.parse(temp.current.time.replace('T', ' ') + ':00')) / 1000;
    package.forecasts.date = temp.daily.time[0];
    package.forecasts.sunrise = temp.daily.sunrise[0].split('T')[1];
    package.forecasts.sunset = temp.daily.sunset[0].split('T')[1];

    package.fact.temp = parseInt(temp.current.temperature_2m);
    package.fact.condition = iconObj.condition;
    package.fact.feels_like = parseInt(temp.current.apparent_temperature);
    package.fact.wind_dir = getDegWindDir(temp.current.wind_direction_10m);
    package.fact.season = seasonService(new Date()).toLowerCase();
    package.fact.wind_gust = parseInt(temp.current.wind_gusts_10m);
    package.fact.wind_speed = parseInt(temp.current.wind_speed_10m);
    package.fact.icon = iconObj.icon;
    package.fact.humidity = temp.current.relative_humidity_2m;
    package.fact.pressure_mm = temp.current.pressure_msl / 1.333;
    
    let datetimeOne = new Date();
    datetimeOne.setMinutes(0)
    datetimeOne.setHours(datetimeOne.getHours() + 5);
    let datetimeTwo = new Date();
    datetimeTwo.setHours(datetimeOne.getHours() + 5);
    datetimeTwo.setMinutes(0)

    package.forecasts.parts = [
        getFactByTime(temp, dateFormat.asString('hh:mm', datetimeOne)),
        getFactByTime(temp, dateFormat.asString('hh:mm', datetimeTwo))
    ];

    console.log(package);
    // console.log(package.forecasts);
    
    return res.json(package);
});

let isConfirmInnerAPIKey = (apiKey) => {
    return apiKey == env.INNER_API_KEY;
}

app.listen(port, () => {
    console.log(`App listening on port ${port}`)
})