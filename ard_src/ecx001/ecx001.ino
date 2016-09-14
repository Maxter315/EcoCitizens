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
        //data[iter] = getSensorsReadings();
        //Reading ans = avgReadings(data,10);

        if (iter >= 4){
            //json, send to esp

            //json generating
            /*
            aJsonObject* amono,adust,atemp,apres,ahum;
                amono = aJson.createArray();
                adust = aJson.createArray();
                atemp = aJson.createArray();
                apres = aJson.createArray();
                ahum = aJson.createArray();

            for (int i=0;i<5;i++){
                aJsonObject* foo;

                foo = aJson.createItem(data[i].mono);
                aJson.addItemToArray(amono, foo);
                foo = aJson.createItem(data[i].dust);
                aJson.addItemToArray(adust, foo);
                foo = aJson.createItem(data[i].temp);
                aJson.addItemToArray(atemp, foo);
                foo = aJson.createItem(data[i].pres);
                aJson.addItemToArray(apres, foo);
                foo = aJson.createItem(data[i].hum);
                aJson.addItemToArray(ahum, foo);
            }
            

            aJson.addItemToObject(root)



            aJsonObject* root = aJson.createObject();
            aJson.addItemToObject(root, "id", aJson.createItem(SENSOR_ID));
            aJsonObject* loc = aJson.createArray();

            aJson.addItemToObject(root, "id", );
            aJsonObject* fmt = aJson.createObject();
            */
            /***************************/
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
                JsonObject& mono_o = read_j.createNestedObject();
                JsonObject& dust_o = read_j.createNestedObject();
                JsonObject& temp_o = read_j.createNestedObject();
                JsonObject& pres_o = read_j.createNestedObject();
                JsonObject& hum_o = read_j.createNestedObject();
                

            JsonArray& amono = mono_o.createNestedArray("mono");
            JsonArray& adust = dust_o.createNestedArray("dust");
            JsonArray& atemp = temp_o.createNestedArray("temp");
            JsonArray& apres = pres_o.createNestedArray("pres");
            JsonArray& ahum = hum_o.createNestedArray("hum");

            for (int i=0;i<5;i++){
                amono.add(double_with_n_digits(data[i].mono, 1));
                adust.add(double_with_n_digits(data[i].dust, 1));
                atemp.add(double_with_n_digits(data[i].temp, 1));
                apres.add(double_with_n_digits(data[i].pres, 1));
                ahum.add(double_with_n_digits(data[i].hum, 1));
            }

            root.printTo(Serial);
            Serial.println();
            /**********************************/

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
        acc.mono += array[i].mono;
        acc.dust += array[i].dust;
        acc.temp += array[i].temp;
        acc.pres += array[i].pres;
        acc.hum += array[i].hum;
    }

    float denom = (float)num;

    acc.mono = acc.mono / denom;
    acc.dust = acc.dust / denom;
    acc.temp = acc.temp / denom;
    acc.pres = acc.pres / denom;
    acc.hum = acc.hum / denom;

    return acc;
}
/*
void genJson(Reading smpl){

}
*/