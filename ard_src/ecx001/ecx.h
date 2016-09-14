#define VERSION 001
#define TEST
#define DELTATIME 1000
#define DATLEN 5

#define SENSOR_ID "UA-VN-0001"

/* Libraries for installed modules */
#include <Time.h>
#include <DS1307RTC.h>
#include <TFT_HX8357.h>
#include <aJSON.h>
//#include <ArduinoJson.h>


/* Structures */
typedef struct{
    float CO;
    float dust;
    float temperature;
    float pressure;
    float humidity;
}Reading;
