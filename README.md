# EcoCitizens
Network of air quality sensors over the cities of Ukraine.<br>
EGAP challenge.<br>

###STRUCTURE
 - <b>ard_src</b> - Arduino (2560) source files.<br>
 - <b>esp_src</b> - ESP8266 source files.<br>
 - <b>docs</b> - documents: drawings, examples, schemes.<br>
 - <b>libs</b> - used libraries.<br>
 - <b>demo_old</b> - костыльные исходники первого макета датчика (Винница)<br>

###JSON
```c
{
    "_id":"UA-VN-0001",                  //an unique identification number
    "data":{
        "date":{
            "date":"2016-09-09",                 //date and time when first reading was performed
            "time":"09:13:45"
        },
    "readings":{
            "mono":110.0,                //concentration of carbon monoxide, [ppm], MQ-7B
            "dust":0.011,                //concentrarion of dust, [mg/m3], GP2Y10
            "temp":24.5,                 //temperature, [degC], Si7021/T5403
            "pres":100000,               //air pressure, [Pa], T5403
            "hum": 80.0                  //relative humidity, [%], Si7021
        }
    },
    "err":0                              //an error code 
}
```

###Credits
<center>Vinnitsa - Zaporozhye - Krivoy Rog<br>
2016</center>
