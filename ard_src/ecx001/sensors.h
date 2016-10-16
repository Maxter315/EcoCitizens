/*Sensors*/
#define ERRSI7021 0x01
#define ERRT5403  0x02
#define ERRMQ7    0x04
#define ERRGP2Y10 0x08

#include <stdint.h>
#include <Wire.h>
#include <Si7021.h>
#include <t5403.h>
#include <TFT_HX8357.h>

/* returns error code */
uint16_t sensorsInit(TFT_HX8357);

float getMono(uint8_t);
float getDust();
//float getTemp(SI7021);
//float getPres(T5403);
//float getHum(SI7021);

/* Structures */
/*
typedef struct{
    float mono;
    float dust;
    float temp;
    float pres;
    float hum;
}Reading;
*/
struct Reading{
    float mono;
    float dust;
    float temp;
    float pres;
    float hum;
};

Reading getSensorsReadings(uint8_t);
