/*Sensors*/
#define DELAYL 700
#define DELAYS 200

uint16_t sensorsInit(void){
    int error = 0x00;

    //Si7021. Temperature & Relative humidity. i2c.
    tft.print("Si7021\t\t");
    si7021.begin();
    delay(DELAYL);
    if(si7021.getDeviceID() == 7021){
        tft.println("OK");
    }else{
        tft.println("FAIL");
        error |= ERRSI7021;
    }
    delay(DELAYS);

    //T5403. Pressure. i2c.
    tft.print("T5403\t\t");
    delay(DELAYL);
    if(0){
        tft.println("OK");
    }else{
        tft.println("FAIL");
        error |= ERRT5403;
    }
    delay(DELAYS);

    //GP2Y10. Dust. Analogue.
    tft.print("GP2Y10\t\t");
    delay(DELAYL);
    if(0){
        tft.println("OK");
    }else{
        tft.println("FAIL");
        error |= ERRGP2Y10;
    }
    delay(DELAYS);

    //GP2Y10. Dust. Analogue.
    tft.print("MQ7-B\t\t");
    delay(DELAYL);
    if(0){
        tft.println("OK");
    }else{
        tft.println("FAIL");
        error |= ERRMQ7;
    }
    delay(DELAYS);

    return error;
};