#include <stdint.h>
#include "sensors.h"
#include "ecx.h"

TFT_HX8357 tft = TFT_HX8357();
SI7021 si7021;
T5403 barometer(MODE_I2C);
uint16_t sensorsError = 0;

void setup() {
    tft.init();
    tft.setRotation(1);
    sensorsError = sensorsInit();

}

Reading avgReadings(Reading*, int){};

void loop() {
    
  // Fill screen with random colour so we can see the effect of printing with and without 
  // a background colour defined
  tft.fillScreen(random(0xFFFF));
  
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  tft.setCursor(0, 0, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  // We can now plot text on screen using the "print" class
  tft.println("Hello World!");
  
  // Set the font colour to be yellow with no background, set to font 7
  tft.setTextColor(TFT_YELLOW); tft.setTextFont(7);
  tft.println(1234.56);
  
  // Set the font colour to be red with black background, set to font 4
  tft.setTextColor(TFT_RED,TFT_BLACK);    tft.setTextFont(4);
  tft.println((long)3735928559, HEX); // Should print DEADBEEF

  // Set the font colour to be green with black background, set to font 4
  tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.setTextFont(4);
  tft.println("Groop");
  tft.println("I implore thee,");

  // Test some print formatting functions
  float fnumber = 123.45;
   // Set the font colour to be blue with no background, set to font 4
  tft.setTextColor(TFT_BLUE);    tft.setTextFont(4);
  tft.print("Float = "); tft.println(fnumber);           // Print floating point number
  tft.print("Binary = "); tft.println((int)fnumber, BIN); // Print as integer value in binary
  tft.print("Hexadecimal = "); tft.println((int)fnumber, HEX); // Print as integer number in Hexadecimal
}


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