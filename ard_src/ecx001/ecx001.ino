#include <stdint.h>
#include "sensors.h"
#include "ecx.h"

Reading avgReadings(Reading*, int){};
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
    if (time_cur - time_prev > dt){
        //sampling & collecting

        if (iter > 5){
            //averaging, json, send to esp
            iter = 0;
        }

        iter++;
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