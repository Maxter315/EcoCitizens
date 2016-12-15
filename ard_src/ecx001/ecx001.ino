#define VERSION 001
#define DELTATIME 1000
//!!debug!! TSAMP = 150
#define TSAMP 150
#define TSAMPF 150.0
#define DATLEN 5

#define SENSOR_ID "UA-KR-0001"
#define LOC_LONG (48.756080)
#define LOC_LAT (2.302038)

#include <Arduino.h>
//#include <Time.h>
//#include <DS1307RTC.h>

//#include <aJSON.h>
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>
//#include <stdint.h>
#include "sensors.h"
#include "graph.h"

#include "RTClib.h"
RTC_DS1307 rtc;

const int chipSelect = 53;

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
uint8_t nsamp = 0, iter = 0;
unsigned long currentTime, prevTime;
unsigned long dt = DELTATIME;
DateTime timeOfFirstRead;

void setup() {

        Serial.begin(9600);
        Serial.println("Serial OK");
        Serial3.begin(115200);
        Serial3.setTimeout(50);
    
    //Display initialization
        tft.init();
        tft.setRotation(1);
        tft.setCursor(0, 0, 2);
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE,TFT_BLACK);

        //tft.setCursor(0, 25, 2);

    //Sensors init    
        sensorsError = sensorsInit(tft);
        barometer.begin();
        si7021.setHumidityRes(12);
        //tft.setCursor(0, 110, 2);

        if (! rtc.begin()) {
            tft.println("Couldn't find RTC");
        } else {
            tft.println("RTC OK");
        }
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


    //SDCARD INITIALIZATION
        String dataString = "#time(HH:MM:SS),\tCO(ppm),\tdust(mg/m3),\ttemp(degC),\tpres(Pa),\thum(%)";
        bool sderror = false;
        if (!SD.begin(chipSelect)) {
            tft.println("SD ERROR");
            sderror = true;
        } else {
            SD.remove("datalog.txt"); 
            File dataFile = SD.open("datalog.txt", FILE_WRITE);
            dataFile.println(dataString);
            dataFile.close();
            tft.println("SD OK");
        }
    
    delay(2000);
    tft.setCursor(0, 0, 2);
    tft.fillScreen(TFT_BLACK);
}

void loop() {

    Reading data[DATLEN];
    Reading accumRead;
    Reading singleRead;
    float monArray[150];
    DateTime now;

    now = rtc.now();
    currentTime = millis();

    if (currentTime - prevTime > dt){
        //tft.fillScreen(TFT_BLACK);

        prevTime = currentTime;
        nsamp>=TSAMP? nsamp=0 : nsamp++;

        if (!(nsamp % 5)){
            now = rtc.now();
        }
        //DateTime now = rtc.now();
        
        //Display some data
            tft.setCursor(0, 0, 2);
            tft.println(getTimeString(now));
            tft.println(getTimeString(timeOfFirstRead));

            singleRead = getSensorsReadings(nsamp);
            displayInfo(tft, singleRead, 0);
            tft.println(nsamp);
            tft.println(iter);

        //drawSystem(tft,150,20,250,150,DKRED,YELLOW);
        /*
        double x,y;
        Graph(tft, x, y, 1, 150, 250, 300, 200, 0, 6.5, 1, -1, 1, .25, "", "", "", display1, YELLOW);
        update1 = true;
        for (x = 0; x <= 6.3; x += .1) {
            y = nsamp*sin(x);
            Trace(tft, x, y, 1, 150, 250, 300, 200, 0, 6.5, 1, -1, 1, .25, "Sin(x)", "x", "fn(x)", update1, YELLOW);
        }
        */
        
        accumRead.mono = accumRead.mono + singleRead.mono;
        accumRead.dust = accumRead.dust + singleRead.dust;
        accumRead.temp = accumRead.temp + singleRead.temp;
        accumRead.pres = accumRead.pres + singleRead.pres;
        accumRead.hum = accumRead.hum + singleRead.hum;

        if (nsamp >= TSAMP){
            //averaging and storing to data[i];
            //float denom = (float)TSAMP;
            if (iter == 0) {
                timeOfFirstRead = rtc.now();
            }

            accumRead.mono = accumRead.mono / TSAMPF;
            accumRead.dust = accumRead.dust / TSAMPF;
            accumRead.temp = accumRead.temp / TSAMPF;
            accumRead.pres = accumRead.pres / TSAMPF;
            accumRead.hum = accumRead.hum / TSAMPF;

            data[iter] = singleRead;

            String dataString = genDatalog(now, singleRead);
            String temp = genFilename(now);

            char filename[16]="20160101.csv";
            temp.toCharArray(filename,sizeof(filename));
            tft.println(filename);
            File dataFile = SD.open(filename, FILE_WRITE);
            dataFile.println(dataString);
            dataFile.close();
            
            if (iter >= 4){
                //json, send to esp
                //json generating
                //Serial.println("json starts here:");
                
                StaticJsonBuffer<1400> jsonBuffer;
                JsonObject& root = jsonBuffer.createObject();
    
                root["id"] = SENSOR_ID;
                root["date"] = getDateJ(timeOfFirstRead);
                root["time"] = getTimeJ(timeOfFirstRead);
                root["err"] = sensorsError;
                root["tsamp"] = TSAMP;
                JsonObject& read_j = root.createNestedObject("readings");
                JsonArray& amono = read_j.createNestedArray("mono");
                JsonArray& adust = read_j.createNestedArray("dust");
                JsonArray& atemp = read_j.createNestedArray("temp");
                JsonArray& apres = read_j.createNestedArray("pres");
                JsonArray& ahum = read_j.createNestedArray("hum");

                for (int i=0;i<5;i++){
                    amono.add(double_with_n_digits(data[i].mono, 1));
                    adust.add(double_with_n_digits(data[i].dust, 3));
                    atemp.add(double_with_n_digits(data[i].temp, 2));
                    apres.add(double_with_n_digits(data[i].pres, 1));
                    ahum.add(double_with_n_digits(data[i].hum, 2));
                }
                root.printTo(Serial3);
                Serial3.println();
                
                iter = 0;
            } else {
                iter++;
            }


        }

        String temps = Serial3.readString();
        tft.println(temps);
    }
}

/*============================================================================*/

String addzero(int a){
    String out;
    if (a < 10) {
        out = "0";
        out += String(a, DEC);
    }else{
        out = String(a, DEC);
    }
return out;
}

String getTimeString(DateTime now){
    String out;
    out = String(now.year(), DEC);      out += '/';
    out += addzero(now.month());        out += '/';
    out += addzero(now.day());          out += ' ';
    out += addzero(now.hour());         out += ':';
    out += addzero(now.minute());       out += ':';
    out += addzero(now.second());

    return out;
}

String getTimeJ(DateTime now){
    String out;
    out = addzero(now.hour());          out += ':';
    out += addzero(now.minute());       out += ':';
    out += addzero(now.second());
    return out;
}

String getDateJ(DateTime now){
    String out;
    out = String(now.year(), DEC);      out += '-';
    out += addzero(now.month());        out += '-';
    out += addzero(now.day());
    return out;
}

String genFilename(DateTime now){
    String out;
    out = String(now.year(), DEC);
    out += addzero(now.month());
    out += addzero(now.day());
    out += ".csv";
    return out;
}

String genDatalog(DateTime now, Reading sample){
    String out;

    out = addzero(now.hour());              out += ":";
    out += addzero(now.minute());           out += ":";
    out += addzero(now.second());           out += ",";

    out += String(sample.mono,1);           out += ",";
    out += String(sample.dust,3);           out += ",";
    out += String(sample.temp,1);           out += ",";
    out += String(sample.pres/100.0,2);     out += ",";
    out += String(sample.hum,1);
    return out;
}

void displayInfo(TFT_HX8357& tft, Reading sample, uint8_t pos){
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.print("Pressure: ");
    tft.println(sample.pres);
    tft.print("Humidity: ");
    tft.println(sample.hum);
    tft.print("Temperature: ");
    tft.println(sample.temp);
    tft.print("Mono: ");
    tft.println(sample.mono);
    tft.print("Dust: ");
    tft.println(sample.dust);
}
