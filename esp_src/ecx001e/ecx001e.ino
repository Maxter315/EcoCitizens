#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial


ESP8266WiFiMulti WiFiMulti;
char APName[20] = "spot_name";
char APPass[20] = "defaultpass";

void setup() {

    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP(APName, APPass);
}


int var = 0;

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        String sensorsData;
        sensorsData = USE_SERIAL.readStringUntil('\n');

        USE_SERIAL.print(var);
        var++;
        HTTPClient http;

        //USE_SERIAL.print("     [HTTP] begin...\n");

        // configure server and url
        //http.begin("http://192.168.1.12/test.html");
        http.begin("http://ecocitizens.online/user/controller/importParameters.php");
        //http.begin("192.168.1.12", 80, "/test.html");

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
    }

    delay(1000);
}

