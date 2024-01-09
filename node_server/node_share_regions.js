require('dotenv').config();
const env = process.env;
const express = require('express');
const path = require('path')
const app = express();
const fetch = require('node-fetch');
const { readFileSync } = require('fs');

app.use('/resources', express.static(path.join(__dirname, 'resources')))

const port = 3000;

app.get('/regions', (req, res) => {
    if (!isConfirmInnerAPIKey(req.query.innerApiKey)) {
        return res.status(403).json({"message":"forbidden"});
    }

    let data = readFileSync('./regions.json');
    res.send(data);
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

app.get('/weather', async(req, res) => {
    if (!isConfirmInnerAPIKey(req.query.innerApiKey)) {
        return res.status(403).json({"message":"forbidden"});
    }
    
    let response = await fetch(`https://api.weather.yandex.ru/v2/forecast?lat=${req.query.lat}&lon=${req.query.lon}`, {
        headers: {
            "X-Yandex-API-Key": env.WEATHER_YANDEX_API_KEY
        }
    });

    let temp = await response.text();
    temp = JSON.parse(temp);

    for (key in temp) {
        if (key == 'now') {
            package.now = temp.now;
            continue;
        }

        if (key == 'fact') {
            for(kkey in temp[key]) {
                if (package[key][kkey] != undefined) {
                    package[key][kkey] = temp[key][kkey];
                }
            }
            continue;
        }

        if (key == 'forecasts') {
            for(kkey in temp[key][0]) {
                if (package[key][kkey] != undefined) {
                    if (kkey == 'parts') {
                        for (i = 0; i < package.forecasts.parts.length; i++) {
                            // console.log(temp[key][0][kkey]);

                            for(kkkey in temp[key][0][kkey]) {
                                let obj = temp[key][0][kkey][kkkey];

                                for (kkkkey in obj) {
                                    // package.forecasts.parts[i]['icon'] = "111";
                                    if (package.forecasts.parts[i][kkkkey] != undefined && package.forecasts.parts[i].part_name == kkkey) {
                                        package[key][kkey][i][kkkkey] = obj[kkkkey];
                                    }       
                                }
                            }
                        }
                    } else {
                        for(kkey in temp[key][0]) {
                            if (kkey != 'parts') {
                                if (package[key][kkey] != undefined) {
                                    package[key][kkey] = temp[key][0][kkey];
                                }
                            }
                        }
                    }
                    
                }
            }
        }
    }
    
    return res.json(package);
});

let isConfirmInnerAPIKey = (apiKey) => {
    return apiKey == env.INNER_API_KEY;
}

app.listen(port, () => {
    console.log(`App listening on port ${port}`)
})