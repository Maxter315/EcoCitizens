#define VERSION 001
#define TEST 1
#define DELTATIME 1000
#define TSAMP 150
#define TSAMPF 150.0
#define DATLEN 5

#define SENSOR_ID "UA-KR-0001"

#include <Arduino.h>
//#include <Time.h>
//#include <DS1307RTC.h>

//#include <aJSON.h>
#include <SD.h>
#include <SPI.h>
//#include <ArduinoJson.h>
//#include <stdint.h>
#include "sensors.h"
#include "graph.h"

#include "RTClib.h"
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int chipSelect = 53;

Reading avgReadings(Reading*, int);

void loading(TFT_HX8357 &tft){
    for(int i=0;i<10;i++){
        tft.print('.');
        delay(500);
    }
}

/*============================================================================*/

// these are the only external variables used by the graph function
// it's a flag to draw the coordinate system only on the first call to the Graph() function
// and will mimize flicker
// also create some variables to store the old x and y, if you draw 2 graphs on the same display
// you will need to store ox and oy per each display
boolean display1 = true;
boolean update1 = true;

double ox = -999, oy = -999; // Force them to be off screen

/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn

  huge arguement list
  &d name of your display object
  x = x data point
  y = y datapont
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x asis label
  ylabel = y asis label
  &redraw = flag to redraw graph on first call only
  color = plotted trace colour
*/


void Graph(TFT_HX8357 &tft, double x, double y, byte dp,
                           double gx, double gy, double w, double h,
                           double xlo, double xhi, double xinc,
                           double ylo, double yhi, double yinc,
                           char *title, char *xlabel, char *ylabel,
                           boolean &redraw, unsigned int color) 
{
  double ydiv, xdiv;
  double i;
  double temp;
  int rot, newrot;

  // gcolor = graph grid colors
  // acolor = axes line colors
  // pcolor = color of your plotted data
  // tcolor = text color
  // bcolor = background color
  unsigned int gcolor = DKBLUE;
  unsigned int acolor = RED;
  unsigned int pcolor = color;
  unsigned int tcolor = WHITE;
  unsigned int bcolor = BLACK;

  if (redraw == true) {

    redraw = false;
    // initialize old x and old y in order to draw the first point of the graph
    // but save the transformed value
    // note my transform funcition is the same as the map function, except the map uses long and we need doubles
    //ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    //oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

    tft.setTextDatum(MR_DATUM);

    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) {
        tft.drawLine(gx, temp, gx + w, temp, acolor);
        tft.setTextColor(acolor, bcolor);
        tft.drawString(xlabel, (int)(gx + w) , (int)temp, 2);
      }
      else {
        tft.drawLine(gx, temp, gx + w, temp, gcolor);
      }
      // draw the axis labels
      tft.setTextColor(tcolor, bcolor);
      // precision is default Arduino--this could really use some format control
      tft.drawFloat(i, dp, gx - 4, temp, 1);
    }

    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform
      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        tft.drawLine(temp, gy, temp, gy - h, acolor);
        tft.setTextColor(acolor, bcolor);
        tft.setTextDatum(BC_DATUM);
        tft.drawString(ylabel, (int)temp, (int)(gy - h - 8) , 2);
      }
      else {
        tft.drawLine(temp, gy, temp, gy - h, gcolor);
      }
      // draw the axis labels
      tft.setTextColor(tcolor, bcolor);
      tft.setTextDatum(TC_DATUM);
      // precision is default Arduino--this could really use some format control
      tft.drawFloat(i, dp, temp, gy + 7, 1);
    }

    //now draw the graph labels
    tft.setTextColor(tcolor, bcolor);
    tft.drawString(title, (int)(gx + w / 2) , (int)(gy - h - 30), 4);
  }

  // the coordinates are now drawn, plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized above
  //x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  //y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  //tft.drawLine(ox, oy, x, y, pcolor);
  // it's up to you but drawing 2 more lines to give the graph some thickness
  //tft.drawLine(ox, oy + 1, x, y + 1, pcolor);
  //tft.drawLine(ox, oy - 1, x, y - 1, pcolor);
  //ox = x;
  //oy = y;

}

void Trace(TFT_HX8357 &tft, double x,  double y,  byte dp,
           double gx, double gy,
           double w, double h,
           double xlo, double xhi, double xinc,
           double ylo, double yhi, double yinc,
           char *title, char *xlabel, char *ylabel,
           boolean &update1, unsigned int color)
{
  double ydiv, xdiv;
  double i;
  double temp;
  int rot, newrot;

  //unsigned int gcolor = DKBLUE;   // gcolor = graph grid color
  unsigned int acolor = RED;        // acolor = main axes and label color
  unsigned int pcolor = color;      // pcolor = color of your plotted data
  unsigned int tcolor = WHITE;      // tcolor = text color
  unsigned int bcolor = BLACK;      // bcolor = background color

  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  if (update1) {
    update1 = false;
    
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

    if ((ox < gx) || (ox > gx+w)) {update1 = true; return;}
    if ((oy < gy-h) || (oy > gy)) {update1 = true; return;}
    

    tft.setTextDatum(MR_DATUM);

    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) {
        tft.setTextColor(acolor, bcolor);
        tft.drawString(xlabel, (int)(gx + w) , (int)temp, 2);
      }
      // draw the axis labels
      tft.setTextColor(tcolor, bcolor);
      // precision is default Arduino--this could really use some format control
      tft.drawFloat(i, dp, gx - 4, temp, 1);
    }

    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform
      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        tft.setTextColor(acolor, bcolor);
        tft.setTextDatum(BC_DATUM);
        tft.drawString(ylabel, (int)temp, (int)(gy - h - 8) , 2);
      }

      // draw the axis labels
      tft.setTextColor(tcolor, bcolor);
      tft.setTextDatum(TC_DATUM);
      // precision is default Arduino--this could really use some format control
      tft.drawFloat(i, dp, temp, gy + 7, 1);
    }

    //now draw the graph labels
    tft.setTextColor(tcolor, bcolor);
    tft.drawString(title, (int)(gx + w / 2) , (int)(gy - h - 30), 4);
  }

  // the coordinates are now drawn, plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

  if ((x < gx) || (x > gx+w)) {update1 = true; return;}
  if ((y < gy-h) || (y > gy)) {update1 = true; return;}
    
    
  tft.drawLine(ox, oy, x, y, pcolor);
  // it's up to you but drawing 2 more lines to give the graph some thickness
  //tft.drawLine(ox, oy + 1, x, y + 1, pcolor);
  //tft.drawLine(ox, oy - 1, x, y - 1, pcolor);
  ox = x;
  oy = y;

}

/*

  End of graphing function

*/


/*============================================================================*/

TFT_HX8357 tft = TFT_HX8357();
SI7021 si7021;
T5403 barometer(MODE_I2C);

uint16_t sensorsError = 0;
uint8_t nsamp = 0;
unsigned long time_cur, time_prev;
unsigned long dt = DELTATIME;

void setup() {
    //Display initialization
        tft.init();
        tft.setRotation(1);
        tft.setCursor(0, 0, 2);
        tft.fillScreen(TFT_BLACK);
        
        tft.setTextColor(TFT_WHITE,TFT_BLACK);
        tft.setTextSize(1);
        tft.print("Hello");
        delay(1000);
    
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_BLACK,TFT_BLACK);
        tft.print("Hello");
    
        tft.setTextColor(TFT_WHITE,TFT_BLACK);
        tft.setCursor(0, 25, 2);

    //Sensors init    
        sensorsError = sensorsInit(tft);
        barometer.begin();
        si7021.setHumidityRes(12);

    tft.setCursor(0, 110, 2);
    if (! rtc.begin()) {
        tft.println("Couldn't find RTC");
    }

    //SDCARD INITIALIZATION
    String dataString = "#time(HH:MM:SS),\tCO(ppm),\tdust(mg/m3),\ttemp(degC),\tpres(hPa),\thum(%)";
    bool sderror = false;
    if (!SD.begin(chipSelect)) {
        tft.println("SD ERROR");
        sderror = true;
    } else {
        SD.remove("datalog.txt") 
        File dataFile = SD.open("datalog.txt", FILE_WRITE);
        dataFile.println(dataString);
        dataFile.close();
        tft.println("SD OK");
    }
    
    //SDCARD END

        tft.setCursor(0, 0, 2);
        tft.fillScreen(TFT_BLACK);
}

void loop() {
    
    time_cur = millis();
    int iter = 0;
    //int nsamp = 0;
    double pressi,humi,tempi,dusti,monoi;

    //loading(tft);
    Reading data[DATLEN];
    Reading accumRead;
    Reading singleRead;
    float monArray[150];

    if (time_cur - time_prev > dt){
        time_prev = time_cur;
        nsamp>=TSAMP?nsamp=0:nsamp++;

            DateTime now = rtc.now();
    /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    */
        //Display some data
        /*
            tft.setCursor(0, 150, 2);
            tft.setTextColor(TFT_BLACK,TFT_BLACK);
            tft.print("Pressure: ");
            tft.println(pressi);
            tft.print("Humidity: ");
            tft.println(humi);
            tft.print("Temperature: ");
            tft.println(tempi);
            tft.print("Dust: ");
            tft.println(monoi);
            tft.print(nsamp);
            */

            tft.print(now.year(), DEC);     tft.print('/');
            tft.print(now.month(), DEC);    tft.print('/');
            tft.print(now.day(), DEC);      tft.print(" ");
            tft.print(now.hour(), DEC);     tft.print(':');
            tft.print(now.minute(), DEC);   tft.print(':');
            tft.println(now.second(), DEC);

            singleRead = getSensorsReadings(nsamp);
            
            pressi = singleRead.pres;
            humi = singleRead.hum;
            tempi = singleRead.temp;
            dusti = singleRead.dust;
            monoi = singleRead.mono;
            monArray[nsamp] = monoi;
            
            tft.setCursor(0, 150, 2);
            tft.setTextColor(TFT_WHITE,TFT_BLACK);
            tft.print("Pressure: ");
            tft.println(pressi);
            tft.print("Humidity: ");
            tft.println(humi);
            tft.print("Temperature: ");
            tft.println(tempi);
            tft.print("Mono: ");
            tft.println(monoi);
            tft.print("Dust: ");
            tft.println(dusti);
            tft.print(nsamp);

        //drawSystem(tft,150,20,250,150,DKRED,YELLOW);
        /*
        double x,y;
        Graph(tft, x, y, 1, 150, 250, 300, 200, 0, 6.5, 1, -1, 1, .25, "", "", "", display1, YELLOW);
        update1 = true;
        for (x = 0; x <= 6.3; x += .1) {
            y = sin(x);
            Trace(tft, x, y, 1, 150, 250, 300, 200, 0, 6.5, 1, -1, 1, .25, "Sin(x)", "x", "fn(x)", update1, YELLOW);
        }
        */
    
        //sampling & collecting, 1000ms
        //Reading curread = getSensorsReadings();
        
        accumRead.mono = accumRead.mono + singleRead.mono;
        accumRead.dust = accumRead.dust + singleRead.dust;
        accumRead.temp = accumRead.temp + singleRead.temp;
        accumRead.pres = accumRead.pres + singleRead.pres;
        accumRead.hum = accumRead.hum + singleRead.hum;

        if (nsamp >= TSAMP){
            //averaging and storing to data[i];
            //float denom = (float)TSAMP;

            accumRead.mono = accumRead.mono / TSAMPF;
            accumRead.dust = accumRead.dust / TSAMPF;
            accumRead.temp = accumRead.temp / TSAMPF;
            accumRead.pres = accumRead.pres / TSAMPF;
            accumRead.hum = accumRead.hum / TSAMPF;

            data[iter] = accumRead;
            String dataString;
            //dataString = String(time_cur/1000,DEC);
            dataString = String(now.hour(),DEC);    dataString += ":";
            dataString += String(now.minute(),DEC);    dataString += ":";
            dataString += String(now.second(),DEC);
            dataString += ",";
            dataString += String(singleRead.mono,1);
            dataString += ",";
            dataString += String(singleRead.dust,3);
            dataString += ",";
            dataString += String(singleRead.temp,1);
            dataString += ",";
            dataString += String(singleRead.pres,2);
            dataString += ",";
            dataString += String(singleRead.hum,1);
            File dataFile = SD.open("today.txt", FILE_WRITE);
            dataFile.println(dataString);
            dataFile.close();
            
            if (iter >= 4){
                //json, send to esp
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
                */
                
                iter = 0;
            } else {
                iter++;
            }
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

