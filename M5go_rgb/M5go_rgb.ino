// This is a demonstration on how to use an input device to trigger changes on your neo pixels.
// You should wire a momentary push button to connect from ground to a digital IO pin.  When you
// press the button it will change to a new pixel animation.  Note that you need to press the
// button once to start the first animation!

#include <M5Stack.h>
#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN   37    // Digital IO pin connected to the button.  This will be
                          // driven with a pull-up resistor so the switch should
                          // pull the pin to ground momentarily.  On a high -> low
                          // transition the button press logic will execute.

#define PIXEL_PIN    26    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT  3

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB);

bool oldState = HIGH;
int showType = 0;

void setup() {
  M5.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);  

  // Display info
  M5.Lcd.setTextFont(2);
  M5.Lcd.setTextColor(WHITE, BLACK);  
  M5.Lcd.setBrightness(30);
  M5.Lcd.clear();

  M5.Lcd.println("M5Stack RGB LED Test");

  strip.begin();
  colorWipe(strip.Color(0, 0, 0), 0);    // Black/off
  strip.show(); 
}

void loop() {
  // Get current button state.
  bool newState = digitalRead(BUTTON_PIN);

  // Check if state changed from high to low (button press).
  if (newState == LOW && oldState == HIGH) {
    // Short delay to debounce button.
    delay(20);
    // Check if button is still low after debounce.
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      showType++;
      if (showType > 9)
        showType=0;
      startShow(showType);
    }
  }

  // Set the last button state to the old state.
  oldState = newState;
  M5.update();

}

void startShow(int i) {
  switch(i){
    case 0: M5.Lcd.println("Black");
            colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: M5.Lcd.println("Red");
            colorWipe(strip.Color(255, 0, 0), 50);  // Red        
            break;
    case 2: M5.Lcd.println("Green");            
            colorWipe(strip.Color(0, 255, 0), 50);  // Green
            break;
    case 3: M5.Lcd.println("Blue");
            colorWipe(strip.Color(0, 0, 255), 50);  // Blue        
            break;
    case 4: M5.Lcd.println("theaterChase - White");
            theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: M5.Lcd.println("theaterChase - Red");
            theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: M5.Lcd.println("theaterChase - Blue");
            theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: M5.Lcd.println("rainbow");
            rainbow(20);
            break;
    case 8: M5.Lcd.println("rainbow Cycle");
            rainbowCycle(20);
            break;
    case 9: M5.Lcd.println("theaterChase - rainbow");
            theaterChaseRainbow(50);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
