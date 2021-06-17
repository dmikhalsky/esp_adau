#include <Wire.h>
#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>



#include <SigmaStudioFW.h>
#include <USER_SETTINGS.h>
#include <volume.h>

#include "json.h"

#if USE_SPI 
#include <SPI.h>
#else    
#include <Wire.h>
#endif

#define ARDUINOJSON_USE_DOUBLE 1

const char* ssid = "xxxxxxxxxx";
const char* password = "yyyyyyyyy";

StaticJsonDocument<384> doc;

ESP8266WebServer server(80);


#define LED_ADDR 0x0023
#define FREQ_ADDR 0x01C
#define NX_ADDR 0x024

#define DELAY_L_ADDR 0x0021
#define DELAY_R_ADDR 0x0022

//#define LED_ON 0
//#define LED_OFF 1

#define SDA_PIN 4
#define SCL_PIN 5




void handleRoot() {
    
    server.send(200, "text/html", root_response);
}

void handlePlain() {
    if (server.method() != HTTP_POST) {
        server.send(405, "text/plain", "Method Not Allowed");
        }
    else {
        server.send(200, "text/plain", "saved");
        String json = server.arg("plain");
        Serial.println(json);
        
        DeserializationError error = deserializeJson(doc, json);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        int delay_left = doc["delay"][0]; 
        int delay_right = doc["delay"][1];

        bool Tone = doc["Tone"]; // false
        int tone_freq = doc["frequency"]; // 500
        int led_status = doc["led"]; // 0

        JsonArray volume = doc["volume"];
        int volume_0 = volume[0]; // 0
        int volume_1 = volume[1]; // 0
        int volume_range = volume[2]; // 9

        SIGMA_SET_LEVEL(volume_0, volume_range, VOLUME0_ADDR, VOLUME0_DATA);
        SIGMA_SET_LEVEL(volume_1, volume_range, VOLUME1_ADDR, VOLUME1_DATA);

        SIGMA_WRITE_REGISTER_INTEGER(LED_ADDR, led_status); //write value to LED register
        
        
        SIGMA_WRITE_REGISTER_INTEGER(DELAY_L_ADDR, delay_left); //write values to delay register
        SIGMA_WRITE_REGISTER_INTEGER(DELAY_R_ADDR, delay_right);


        if (Tone) {
            Serial.print("Frequency="); 
            Serial.println(tone_freq);
            SIGMA_WRITE_REGISTER_INTEGER(NX_ADDR, 2);
            
            double freq_dsp;
    
            freq_dsp = double(tone_freq/24000.0);

            SIGMA_WRITE_REGISTER_FLOAT(FREQ_ADDR, freq_dsp);
        }
        else {
            Serial.println("No tone");
            SIGMA_WRITE_REGISTER_INTEGER(NX_ADDR, 0);
        }
        
    }
}

void handleNotFound() {
    
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void setup(void) {
    
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("my_adau")) {
        Serial.println("MDNS responder started");
    }

    #if USE_SPI                            // Initialize SPI library and toggle DSP into SPI mode
        pinMode(SDA_PIN, INPUT);           // Set the I2C SDA pin high-impedance to avoid contention with SPI
        pinMode(SCL_PIN, INPUT);           // Set the I2C SCL pin high-impedance to avoid contention with SPI
        digitalWrite(DSP_SS_PIN, HIGH);    // Set SPI slave select line to initialize high
        pinMode(DSP_SS_PIN, OUTPUT);       // Enable SPI slave select pin
        SPI.begin();                       // Initialize the SPI library

    for (uint8_t i = 0; i < 3; i++) {
        digitalWrite(DSP_SS_PIN, LOW);
        delayMicroseconds(10);
        digitalWrite(DSP_SS_PIN, HIGH);
        delayMicroseconds(10);
    }
    #else    // Initialize I2C 
        Wire.begin();                // Initialize I2C library
        Wire.setClock(I2C_SPEED);    // Set I2C SCL frequency based on the value in USER_SETTINGS.h
    #endif

    server.on("/", handleRoot);

    server.on("/plain", handlePlain);

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
}