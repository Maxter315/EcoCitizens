#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
char APName[20] = "spot_name";
char APPass[20] = "defaultpass";
const char* server_rx;
bool set_rcvd = false;
String settings;
String ardTimeJson = "{\"date\":\"Mar 12 2017\",\"time\":\"11:12:00\",}";
StaticJsonBuffer<200> ardJsonBuf;
StaticJsonBuffer<600> jsonBuffer;

unsigned int localPort = 2390; // local port to listen for UDP packets
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP udp;
unsigned long sendNTPpacket(IPAddress& address);


void setup() {

    USE_SERIAL.begin(115200);
    USE_SERIAL.setTimeout(2000);

    // Receiving settings from WIFI.TXT file on SD card
    while(!set_rcvd){
      if(USE_SERIAL.available()){
        settings = USE_SERIAL.readStringUntil('\n');
        set_rcvd = true;
      }else{
        delayMicroseconds(500);
        //USE_SERIAL.print(".");
      }
    }
    //settings = "{\"ssid\":\"spot_name\",\"password\":\"defaultpass\",\"server\":\"http://ecocitizens.online:80/\"}";
    JsonObject& root = jsonBuffer.parseObject(settings);
    // Test if parsing succeeds.
    if (!root.success()) {
      USE_SERIAL.println("parseObject() failed");
      return;
    }
    const char* ssid_rx = root["ssid"];
    const char* pass_rx = root["password"];
    server_rx = root["server"];

    delay(1000);
/*
    USE_SERIAL.print(ssid_rx);
    USE_SERIAL.println(server_rx);
*/
//    delay(1000);
/*  
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(500);
    }
*/
    WiFiMulti.addAP(ssid_rx, pass_rx);


    if((WiFiMulti.run() == WL_CONNECTED)) {
        udp.begin(localPort);
        WiFi.hostByName(ntpServerName, timeServerIP); 
        sendNTPpacket(timeServerIP); // send an NTP packet to a time server
        delay(500);
        int cb = udp.parsePacket();
        if (!cb) {
            //Serial.println("no packet yet");
        }else {
            udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

            unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
            unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
            unsigned long secsSince1900 = highWord << 16 | lowWord;
   
            //Serial.print("Unix time = ");
            const unsigned long seventyYears = 2208988800UL;
            unsigned long epoch = secsSince1900 - seventyYears;
            //Serial.println(epoch);

            JsonObject& ard_time = ardJsonBuf.createObject();
            ard_time["epoch"] = epoch + 3600*2;
            Serial.flush();
            ard_time.printTo(USE_SERIAL);
            USE_SERIAL.print("\n");

        }
        
    }

    //USE_SERIAL.println(ardTimeJson);

}


void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        if(USE_SERIAL.available()){

            String sensorsData;
            sensorsData = USE_SERIAL.readStringUntil('\n');
            USE_SERIAL.println(sensorsData.length());
            HTTPClient http;

            // configure server and url
            //http.begin("http://192.168.1.12/test.html");
            //http.begin("http://31.131.22.224:80/");
            //http.begin("http://ecocitizens.online/user/controller/importParameters.php");
            //http.begin("31.131.22.224", 8880, "");
            //http.begin("http://ecocitizens.online/");
            http.begin(server_rx);

            // start connection and send HTTP header
            String ct = "Content-Type";
            String app = "application/json"; 
            http.addHeader(ct,app,0,0);
            int httpCode = http.POST(sensorsData);

            if(httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);
            } else {
                USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }

            http.end();
            
        }else{
            delayMicroseconds(100);
        }
        
    }
    delay(1);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  //Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
