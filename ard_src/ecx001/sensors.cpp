/*Sensors*/
#include "sensors.h"

//#include "ecx.h"

#define DELAYL 700
#define DELAYS 200

#define DUSTDELAY1 280
#define DUSTDELAY2 40
#define DUSTLED 7
#define DUSTADC A1

#define MONOADC A2
#define MONOHEAT 6
//extern TFT_HX8357 tft;

uint16_t sensorsInit(TFT_HX8357 tft){
    uint16_t error = 0x00;
    extern SI7021 si7021;
    T5403 barometer(MODE_I2C);

    //Si7021. Temperature & Relative humidity. i2c.
        tft.print("Si7021: ");
        si7021.begin();
        delay(DELAYL);
        if(si7021.getDeviceID() == 7021){
        //if(0){
            tft.println("OK");
        }else{
            tft.println("FAIL");
            error |= ERRSI7021;
        }
        delay(DELAYS);

    //T5403. Pressure. i2c.
        tft.print("T5403: ");
        barometer.begin();
        double pressure_abs  = barometer.getPressure(MODE_ULTRA);
        delay(DELAYL);
        if(pressure_abs != 0.0){
            tft.println("OK");
        }else{
            tft.println("FAIL");
            error |= ERRT5403;
        }
        delay(DELAYS);

    //GP2Y10. Dust. Analogue.
        tft.print("GP2Y10: ");
        delay(DELAYL);
        if(0){
            tft.println("OK");
        }else{
            tft.println("???");
            error |= ERRGP2Y10;
        }
        delay(DELAYS);

    //MQ-7B. Carbon monoxyde. Analogue.
        tft.print("MQ7-B: ");
        delay(DELAYL);
        if(0){
            tft.println("OK");
        }else{
            tft.println("???");
            error |= ERRMQ7;
        }
        delay(DELAYS);

    tft.print("ERROR CODE: ");
    tft.print((long)error,HEX);
    return error;
}

Reading getSensorsReadings(uint8_t sec){
    Reading data;
    extern SI7021 si7021;
    T5403 barometer(MODE_I2C);
    barometer.begin();
    data.mono = getMono(sec);
    data.dust = getDust();
    data.temp = si7021.readTemp();
    data.pres = barometer.getPressure(MODE_ULTRA);
    data.hum = si7021.readHumidity();

    return data;
}


float getDust(){
    pinMode(DUSTLED,OUTPUT);
    digitalWrite(DUSTLED,LOW);      //Turn ON LED
    delayMicroseconds(DUSTDELAY1);  //Wait for 0.28ms
    
    int adc = analogRead(DUSTADC);  //Perform adc
    
    delayMicroseconds(DUSTDELAY2);  //Wait for 0.04ms
    digitalWrite(DUSTLED,HIGH);     //Turn OFF LED

    float volt = adc * (5.0/1024.0);
    float dust = 0.17 * volt - 0.1;

    return dust;
}

float getMono(uint8_t sec){
int static adc = 128;
float adcf;
    pinMode(MONOHEAT,OUTPUT);
    if(sec < 60) digitalWrite(MONOHEAT,LOW);
    else if(sec == 60){
        adc = analogRead(MONOADC);
        //digitalWrite(MONOHEAT,HIGH);
    } else if(sec > 60){
        digitalWrite(MONOHEAT,HIGH);
    }

adc = analogRead(MONOADC); //debug
adcf = adc * (5.0/1024.0);
float res;
float ratio;
ratio = (5.0 - adcf) / adcf;
res = -500 * (ratio*10/1.5) + 600;
if (res < 0.0) res = 0.0;
//res = (float) adc;
return res;
}

