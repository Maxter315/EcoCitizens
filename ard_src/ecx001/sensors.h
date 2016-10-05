/*Sensors*/
#define ERRSI7021 0x01
#define ERRT5403  0x02
#define ERRMQ7    0x04
#define ERRGP2Y10 0x08

#define DUSTLED 7
#define DUSTADC A1

#include <Wire.h>
#include <Si7021.h>
#include <t5403.h>

/* returns error code */
uint16_t sensorsInit(void);

float getMono(void);
float getDust(void);
float getTemp(void);
float getPres(void);
float getHum(void);

Reading getSensorsReadings(void);