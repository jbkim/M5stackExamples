#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>     //The DHT12 uses I2C comunication.

#include <WiFi.h>
#include <HTTPClient.h>

DHT12 dht12;          //Preset scale CELSIUS and ID 0x5c.

// TODO: user config
const char* ssid     = "........"; //Wifi SSID
const char* password = "........"; //Wifi Password
String apiKey = "........"; // API Key

const char* host = "api.thingspeak.com";  // host
const int httpPort = 80;      // port

void setup() {
  M5.begin();
  Wire.begin();

  Serial.println("*****************************************************");
  Serial.println("Program Start : Connect ESP32 to Router.");
  Serial.println("Wait for WiFi... ");

  // connecting to the WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // connected
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

void loop() {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  //Read temperature with preset scale.
  float temp = dht12.readTemperature();
  float humid = dht12.readHumidity();
  Serial.print("\r\nTemperature: ");
  M5.Lcd.print("Temperature: ");
  Serial.print(temp);
  M5.Lcd.print(temp);

  //Read humidity.
  Serial.print("*C  Humidity: ");
  M5.Lcd.print("*C  Humidity: ");
  Serial.print(humid);
  M5.Lcd.println(humid);
  Serial.println(" %RH");

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(temp);
    postStr +="&field2=";
    postStr += String(humid);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
    Serial.print("Requesting URL: ");
    Serial.println(postStr);
   
  }

  client.stop();  // stop client
  
  delay(5000);
}
