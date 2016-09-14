#include <stdint.h>
#include "sensors.h"
#include "ecx.h"

Reading avgReadings(Reading*, int){};
//void genJson(Reading){};
/*============================================================================*/

TFT_HX8357 tft = TFT_HX8357();
SI7021 si7021;
T5403 barometer(MODE_I2C);
uint16_t sensorsError = 0;
unsigned long time_cur, time_prev;
unsigned long dt = DELTATIME;


void setup() {

    tft.init();
    tft.setRotation(1);
    tft.setCursor(0, 0, 2);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.setTextSize(2);

    sensorsError = sensorsInit();

}

void loop() {
    time_cur = millis();
    int iter = 0;
    Reading data[DATLEN];

    if (time_cur - time_prev > dt){
        //sampling & collecting
        data[iter] = getSensorsReadings();

        if (iter >= 4){
            //averaging, json, send to esp
            Reading ans = avgReadings(data,DATLEN);

            //json generating

            
            /*
            StaticJsonBuffer<400> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();

            root["id"] = SENSOR_ID;
            JsonArray& loc = root.createNestedArray("location");
                loc.add(double_with_n_digits(48.756080, 6));
                loc.add(double_with_n_digits(2.302038, 6));
            root["date"] = "2016-01-01";
            root["time"] = "13:00:01";
            root["err"] = sensorsError;
            root["tsamp"] = 180;
            root["samples"] = 5;
            JsonArray& read_j = root.createNestedArray("readings");
                JsonArray& mono_j = read_j.createNestedObject();
                    mono_a = 

            root.printTo(Serial);
            Serial.println();*/

            iter = 0;
        } else {
            iter++;
        }
    }
}

/*============================================================================*/
Reading avgReadings(Reading* array, int num){

    Reading acc;

    for(int i=0; i<num; i++){
        acc.CO += array[i].CO;
        acc.dust += array[i].dust;
        acc.temperature += array[i].temperature;
        acc.pressure += array[i].pressure;
        acc.humidity += array[i].humidity;
    }

    float denom = (float)num;

    acc.CO = acc.CO / denom;
    acc.dust = acc.dust / denom;
    acc.temperature = acc.temperature / denom;
    acc.pressure = acc.pressure / denom;
    acc.humidity = acc.humidity / denom;

    return acc;
}
/*
void genJson(Reading smpl){

}
*/