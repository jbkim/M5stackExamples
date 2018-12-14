#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>     //The DHT12 uses I2C comunication.
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>

#define WIFI_TRIAL   10

#define BUTTON_PIN   37    
#define PIXEL_PIN    15    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 10


DHT12 dht12;          //Preset scale CELSIUS and ID 0x5c.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// TODO: user config
// const char* ssid     = "........"; //Wifi SSID
// const char* password = "........"; //Wifi Password
// String apiKey = "........"; // API Key

const char* ssid     = "SmartAP"; //Wifi SSID
const char* password = "20140701"; //Wifi Password
String apiKey = "IUB6AOZRXA7NVIYV"; // API Key

const char* host = "api.thingspeak.com";  // host
const int httpPort = 80;      // port

bool has_wifi = false;

void setup() {
  int trial = 0;

  M5.begin();
  Wire.begin();
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  

  Serial.println("*****************************************************");
  Serial.println("Program Start : Connect ESP32 to Router.");
  Serial.println("Wait for WiFi... ");

  M5.Lcd.setTextSize(2);

  // connecting to the WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (trial++ > WIFI_TRIAL) break;
  }

  if (WiFi.status() == WL_CONNECTED) {
    has_wifi = true;
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); 
  }
  else {
    has_wifi = false;
    Serial.println("NO WiFi");
  }

}

void loop() {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  //Read temperature with preset scale.
  float temp = dht12.readTemperature();
  float humid = dht12.readHumidity();

  // Debuf message
  Serial.print("\r\nTemperature: ");
  Serial.print(temp);
  Serial.print("*C  Humidity: ");
  Serial.print(humid);
  Serial.println(" %RH");

  // LCD Display
  M5.Lcd.setTextColor(YELLOW);  
  M5.Lcd.fillScreen(TFT_BLACK);  
  M5.Lcd.setCursor(0, 0);
  if (has_wifi) {
    M5.Lcd.print("   WiFi: ");
    M5.Lcd.println(WiFi.localIP());     
   colorWipe(strip.Color(0, 255, 0), 0);  // Green
  }else {
    M5.Lcd.print(" No WiFi");    
    colorWipe(strip.Color(255, 0, 0), 0);  // Red    
  }
  
  M5.Lcd.setCursor(0, 80);
  M5.Lcd.print("Temp: ");
  M5.Lcd.print(temp);
  M5.Lcd.print("C  Humid: ");
  M5.Lcd.println(humid);


  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    // return;
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

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
