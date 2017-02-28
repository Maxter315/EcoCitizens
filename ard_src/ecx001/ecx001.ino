#define VERSION 001
#define DELTATIME 1000
//!!debug!! TSAMP = 150
#define TSAMP 150
#define TSAMPF 150.0
#define DATLEN 5

#define SENSOR_ID "UA-KR-0001"

#include <Arduino.h>
//#include <Time.h>

#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <stdint.h>

#include "graph.h"
#include "sensors.h"

#include "RTClib.h"

/*============================================================================*/

// these are the only external variables used by the graph function
// it's a flag to draw the coordinate system only on the first call to the Graph() function
// and will mimize flicker
// also create some variables to store the old x and y, if you draw 2 graphs on the same display
// you will need to store ox and oy per each display
boolean display1 = true;
boolean update1 = true;

double ox = -999, oy = -999; // Force them to be off screen

/*============================================================================*/

TFT_HX8357 tft = TFT_HX8357();
SI7021 si7021;
//T5403 barometer(MODE_I2C);
RTC_DS1307 rtc;

const int chipSelect = 53;

uint16_t sensorsError = 0;
uint8_t nsamp = 1, iter = 0;
unsigned long currentTime, prevTime;
unsigned long dt = DELTATIME;
DateTime timeOfFirstRead;

double buffer_A[96],buffer_B[96];
int indx = 0;

void setup() {

        Serial.begin(9600);
        Serial.println("Serial OK");
        Serial3.begin(115200);
        Serial3.setTimeout(150);
    
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
        //barometer.begin();
        delay(10);
        si7021.setHumidityRes(12);
        delay(100);
        //tft.setCursor(0, 110, 2);
        if (! rtc.begin()) {
            tft.println("Couldn't find RTC");
        } else {
            tft.println("RTC OK");
        }

    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

delay(200);
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
            
            File wifiFile = SD.open("WIFI.TXT");
            if (wifiFile) {
              while (wifiFile.available()) {
                Serial3.write(wifiFile.read());
              }

              Serial3.println();
              wifiFile.close();
            } else {
              tft.println("error opening WIFI.TXT");
            }
            
            tft.println("SD OK");
        }
    
    delay(750);
    tft.setCursor(0, 0, 2);
    tft.fillScreen(TFT_BLACK);
}

    Reading accumRead;
    Reading data[DATLEN];
    Reading singleRead;
    String jsonTime;

void loop() {
    DateTime now;

    now = rtc.now();
    currentTime = millis();

    if (currentTime - prevTime > dt){
        //tft.fillScreen(TFT_BLACK);

        prevTime = currentTime;
        nsamp>=TSAMP? nsamp=1 : nsamp++;

        //if (!(nsamp % 5)){
            now = rtc.now();
        //}
                
        //Display some data
            tft.setCursor(0, 0, 2);
            tft.println(getTimeString(now));
            tft.println(getTimeString(timeOfFirstRead));

            singleRead = getSensorsReadings(nsamp);
            tft.fillRect(0,2*16,140,5*16,TFT_BLACK);
            displayInfo(tft, singleRead, 0);

            tft.fillRect(0,7*16,140,2*16,TFT_BLACK);
            tft.print("[sample]: ");
            tft.println(nsamp);
            tft.print("[time]: ");
            jsonTime = getTimeJ(timeOfFirstRead);
            tft.println(jsonTime);
              
        accumRead.mono = accumRead.mono + singleRead.mono;
        accumRead.dust = accumRead.dust + singleRead.dust;
        accumRead.temp = accumRead.temp + singleRead.temp;
        accumRead.pres = accumRead.pres + singleRead.pres;
        accumRead.hum = accumRead.hum + singleRead.hum;

        if (nsamp >= TSAMP){
            //averaging and storing to data[i];
            //float denom = (float)TSAMP;
            //if (iter == 0) {
            timeOfFirstRead = rtc.now();
            jsonTime = getTimeJ(timeOfFirstRead);
            //}

            accumRead.mono = accumRead.mono / TSAMPF;
            accumRead.dust = accumRead.dust / TSAMPF;
            accumRead.temp = accumRead.temp / TSAMPF;
            accumRead.pres = accumRead.pres / TSAMPF;
            accumRead.hum = accumRead.hum / TSAMPF;

            //data[iter] = accumRead;

            // graph ======================================
                if(indx<96){
                  buffer_A[indx] = accumRead.temp;
                  buffer_B[indx] = accumRead.dust;
                  indx++;
                }else{
                  for(int i=0;i<95;i++){
                    buffer_A[i] = buffer_A[i+1];
                    buffer_B[i] = buffer_B[i+1];
                  }
                  buffer_A[95] = accumRead.temp;
                  buffer_B[95] = accumRead.dust;
                }


                double x,y,xll,xlh;
                int gx=175,gy=95,w=295,h=90;
                
                tft.fillRect(140,0,340,230,TFT_BLACK);
                
                double tmp_ind;
                tmp_ind = (96-indx)*2.5;
                xll = -240 + tmp_ind;
                xlh = tmp_ind;

                update1 = true;
                for (int i=0;i<96;i++) {
                  //y = sin(i);
                  x = -240 + (96-indx + i)*2.5;
                  Trace(tft, x, buffer_A[i], 1, gx, gy, w, h, xll, xlh, 60, -10, 40, 10, "temp", "t", "", update1, YELLOW);
                }

                update1 = true;
                for (int i=0;i<96;i++) {
                  //y = sin(i);
                  x = -240 + (96-indx + i)*2.5;
                  Trace(tft, x, buffer_B[i], 1, gx, gy+115, w, h, xll, xlh, 60, 0, 0.6, 0.1, "dust", "t", "", update1, YELLOW);
                }
            // end graph ==================================

            String dataString = genDatalog(now, accumRead);
            String temp = genFilename(now);

            char filename[16]="20160101.csv";
            temp.toCharArray(filename,sizeof(filename));
            //tft.println(filename);
            File dataFile = SD.open(filename, FILE_WRITE);
            dataFile.println(dataString);
            dataFile.close();
            
            //if (iter >= 4){
                //json, send to esp
                //json generating
                //Serial.println("json starts here:");
                
                StaticJsonBuffer<1400> jsonBuffer;
                JsonObject& root = jsonBuffer.createObject();
    
                root["_id"] = SENSOR_ID;
                JsonObject& data_j = root.createNestedObject("data");
                JsonObject& date_j = data_j.createNestedObject("date");
                date_j["date"] = getDateJ(timeOfFirstRead);
                date_j["time"] = jsonTime;
                //root["tsamp"] = TSAMP;
                
                JsonObject& read_j = data_j.createNestedObject("readings");
                /*
                JsonArray& amono = read_j.createNestedArray("mono");
                JsonArray& adust = read_j.createNestedArray("dust");
                JsonArray& atemp = read_j.createNestedArray("temp");
                JsonArray& apres = read_j.createNestedArray("pres");
                JsonArray& ahum = read_j.createNestedArray("hum");
                */
                //for (int i=0;i<5;i++){
                read_j["mono"] = double_with_n_digits(accumRead.mono, 1);
                read_j["dust"] = double_with_n_digits(accumRead.dust, 3);
                read_j["temp"] = double_with_n_digits(accumRead.temp, 2);
                read_j["pres"] = (uint32_t)accumRead.pres;
                read_j["hum"] = double_with_n_digits(accumRead.hum, 2);

                root["err"] = sensorsError;
                /*
                amono.add(double_with_n_digits(data[i].mono, 1));
                adust.add(double_with_n_digits(data[i].dust, 3));
                atemp.add(double_with_n_digits(data[i].temp, 2));
                apres.add((uint32_t)data[i].pres);
                ahum.add(double_with_n_digits(data[i].hum, 2));
                */
                //}
                tft.fillRect(0,16*16,480,4*16,TFT_BLACK);
                root.printTo(Serial3);
                Serial3.println();
                
                iter = 0;
            //} else {
                //iter++;
            //}
            accumRead.mono = 0.0;
            accumRead.dust = 0.0;
            accumRead.temp = 0.0;
            accumRead.pres = 0.0;
            accumRead.hum = 0.0;


        }
        tft.setCursor(0, 16*16, 2);
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
