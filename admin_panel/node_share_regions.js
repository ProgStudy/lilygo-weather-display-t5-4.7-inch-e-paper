const express = require('express')
const app = express()
const port = 3000

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
})

app.listen(port, () => {
    console.log(`Example app listening on port ${port}`)
})