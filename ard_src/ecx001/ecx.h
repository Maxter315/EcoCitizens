#define VERSION 001
#define TEST

/* Libraries for installed modules */
#include <Time.h>
#include <DS1307RTC.h>

#include <TFT_HX8357.h>

/* Structures */
typedef struct{
    float CO;
    float dust;
    float temperature;
    float pressure;
    float humidity;
}Reading;
