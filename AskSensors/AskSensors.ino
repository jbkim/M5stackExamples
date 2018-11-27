#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>     //The DHT12 uses I2C comunication.

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti WiFiMulti;
HTTPClient ask;

DHT12 dht12;          //Preset scale CELSIUS and ID 0x5c.
/*
For configuration library:
DHT12 dht12("Scale temperature","ID device for I2C");
On "Scale temperature" you can select the preset scale:
CELSIUS, FAHRENHEIT or KELVIN.
And on "ID device", you can put ID sensor, on DHT12
normally is 0x5c.
Examples:
  DHT12 dht12;
The preset scale is CELSIUS and ID is 0x5c.
  DHT12 dht12(KELVIN);
the preset scale is KELVIN and ID is 0x5c.
  DHT12 dht12(FAHRENHEIT,0x53);
The preset scale is FAHRENHEIT and ID is 0x53.
*/

// TODO: user config
const char* ssid     = "......."; //Wifi SSID
const char* password = "......."; //Wifi Password
String apiKeyIn = "........."; // API Key

const char* host = "asksensors.com";  // host
const int httpPort = 80;      // port

float temp, humid;

void setup() {
  M5.begin();
  Wire.begin();

  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Connect ESP32 to AskSensors.");
  Serial.println("Wait for WiFi... ");

  // connecting to the WiFi network
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
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
  temp = dht12.readTemperature();
  humid = dht12.readHumidity();
  Serial.print("\r\nTemperature: ");
  M5.Lcd.print("Temperature: ");
  Serial.print(temp);
  M5.Lcd.print(temp);

  //Read humidity.
  Serial.print("*C  Humidity: ");
  M5.Lcd.print("*C  Humidity: ");
  Serial.print(humid);
  M5.Lcd.println(humid);
  Serial.println("%RH");

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }else {

    // Create a URL for the request
  String url = "https://asksensors.com/api.asksensors/write/";
  url += apiKeyIn;
  url += "?module1=";
  url += temp;
  url += "&module2=";
  url += humid;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
 
   // ask.begin("https://asksensors.com/api.asksensors/write/ONKMHCC38YT74K5NGT8JC1GF8B1O9QKT?module1=100"); //Specify the URL
   ask.begin(url); //Specify the URL
  
    //Check for the returning code
    int httpCode = ask.GET();          
 
    if (httpCode > 0) { 
 
        String payload = ask.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      } else {
      Serial.println("Error on HTTP request");
    }
 
    ask.end(); //End 
  }

  client.stop();  // stop client
  
  delay(5000);
}
