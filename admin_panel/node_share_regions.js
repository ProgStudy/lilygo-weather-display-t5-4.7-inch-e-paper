const express = require('express');
const path = require('path')
const app = express();
const fetch = require('node-fetch');
// app.use(express.static('resources'));
app.use('/resources', express.static(path.join(__dirname, 'resources')))

const port = 3000;

let data = {"regions": [
        {"lat": 47.09412, "lon": 47.49324, "name": "Селитренский сельсовет", "isActive": true},
        {"lat": 11.11, "lon": 22.22, "name": "Москва, Ховрино", "isActive": true},
        {"lat": 11.11, "lon": 22.22, "name": "Мос-обл, Клин", "isActive": true},
        {"lat": 0.0, "lon": 0.0, "name": "Резервный регион", "isActive": false},
        {"lat": 0.0, "lon": 0.0, "name": "Резервный регион", "isActive": false},
    ]
}

app.get('/', (req, res) => {
    console.log('this request...');
    res.send(JSON.stringify(data));
    console.log('request finish!');
});

let temp = '';

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

app.get('/yandex-api-weather', async(req, res) => {
    
    if (temp == "") {
        let response = await fetch('https://api.weather.yandex.ru/v2/forecast?lat=55.86334&lon=37.50611', {
            headers: {
                "X-Yandex-API-Key": "6ddec004-0413-4060-9b06-586cfc5f9299"
            }
        });

        temp = await response.text();
        temp = JSON.parse(temp);
    }


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

app.listen(port, () => {
    console.log(`Example app listening on port ${port}`)
})