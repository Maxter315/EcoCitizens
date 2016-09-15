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
    "id":"UA-VN-0001",                  //an unique identification number
    "location":[102.20595,-35.00454],   //lattitude and longitude of place where sensor is mounted
    "date":"2016-09-09",                //date and time when first reading was performed
    "time":"09:13:45",
    "err":0,                            //an error code 
    "tsamp":180,                        //a time between readings in seconds
    "samples":5,                        //an amount of readings 
    "readings":[
        {
            "mono":[398,399,450,484,500]            //concentration of carbon monoxide, [ppm], MQ-7B
        },
        {
            "dust":[0.011,0.015,0.018,0.02,0.03]    //concentrarion of dust, [mg/m3], GP2Y10
        },
        {
            "temp":[24.5,25.0,26.0,28.4,31.5]       //temperature, [degC], Si7021/T5403
        },
        {
            "pres":[760,764,762,759,761]            //air pressure, [mmhg], T5403
        },
        {
            "hum":[80,90,80,85,81]                  //relative humidity, [%], Si7021
        }
    ]
}
```

<center>Vinnitsa - Zaporozhye - Krivoy Rog<br>
2016</center>
