#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial


ESP8266WiFiMulti WiFiMulti;
char APName[20] = "spot_name";
char APPass[20] = "defaultpass";
String settings;
bool set_rcvd = false;
const char* server_rx;
StaticJsonBuffer<600> jsonBuffer;

void setup() {

    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);
    while(!set_rcvd){
      if(USE_SERIAL.available()){
        settings = USE_SERIAL.readStringUntil('\n');
        set_rcvd = true;
      }else{
        delayMicroseconds(500);
        USE_SERIAL.print(".");
      }
    }
    
    JsonObject& root = jsonBuffer.parseObject(settings);
    // Test if parsing succeeds.
    if (!root.success()) {
      USE_SERIAL.println("parseObject() failed");
      return;
    }
    const char* ssid_rx = root["ssid"];
    const char* pass_rx = root["password"];
    server_rx = root["server"];
    delay(2000);
    //USE_SERIAL.print(settings);
    USE_SERIAL.print(ssid_rx);
    USE_SERIAL.println(server_rx);
    delay(1000);
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    //WiFiMulti.addAP(APName, APPass);
    WiFiMulti.addAP(ssid_rx, pass_rx);
}

int var = 0;

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        if(USE_SERIAL.available()){
            String sensorsData;
            sensorsData = USE_SERIAL.readStringUntil('\n');

            HTTPClient http;

            //USE_SERIAL.print("     [HTTP] begin...\n");
    
            // configure server and url
            //http.begin("http://192.168.1.12/test.html");
            //http.begin("http://31.131.22.224:80/");
            //http.begin("http://ecocitizens.online/user/controller/importParameters.php");
            //http.begin("31.131.22.224", 8880, "");
            //http.begin("http://ecocitizens.online/");
            http.begin(server_rx);
    
            //USE_SERIAL.print("[HTTP] POST...\n");
            // start connection and send HTTP header
            int httpCode = http.POST(sensorsData);
            if(httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);
    
            } else {
                USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }

            http.end();
            USE_SERIAL.print(sensorsData);
        }else{
            USE_SERIAL.print("esp:");
            USE_SERIAL.println(var);
            var++;
        }
        delay(500);
    }
    delay(1000);
}

