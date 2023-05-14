#include <twi.h>
#include <twi_pins.h>
#include <Wire.h>
#include <SoftWire.h>
#include <AsyncDelay.h>
#include <tinyNeoPixel.h>

#define LED_PIN    PIN_PA7
#define NUM_LEDS   12
#define SENSE_PIN  PIN_PA4 // Sense pin in
#define SENSE_POUT PIN_PA6 // Sense pin out
#define BRIGHTNESS 100

#define FIRST_I2C_ADDR 0x1
#define LAST_I2C_ADDR  0x5

tinyNeoPixel strip = tinyNeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);
SoftWire master(PIN_PA1, PIN_PA2);

bool enableOut;
int  inNo;
bool connectedOutputs[5] = {false, false, false, false, false};

void setup() {
  Serial.begin(115200);
  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(SENSE_PIN, INPUT);
  inNo = map(analogRead(SENSE_PIN), 0, 1023, 5, 0); // Get I2C address from sense pin voltage, 0-5v, inverted
  if (inNo == 0) {
    setAll(strip.Color(255, 0, 0, 0));
    while (true) {}
  }
  Wire.begin(inNo);
  master.begin();
  master.setTimeout_ms(200);

  /*
    - I2C ADDRESSES -
        0x1 - 0x2
       /         \
     IN ->       0x3
       \         /
        0x5 - 0x6
  */
  
  for (uint8_t addr = FIRST_I2C_ADDR; addr <= LAST_I2C_ADDR; addr++) {
    delayMicroseconds(50);

    uint8_t startResult = master.llStart((addr << 1) + 1); // Signal a read
    master.stop();

    if (startResult == 0) {
      // Line found at address
      connectedOutputs[addr - 1] = true;
    }

    delay(50);
  }
}

void loop() {

}

void setAll(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}
