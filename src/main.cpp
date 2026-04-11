#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <BleKeyboard.h>
#include <AiEsp32RotaryEncoder.h>

// --- PINS ---
#define LED_PIN 2
#define NUM_LEDS 12
#define SCREEN_ADDR 0x3C


// --- INIT ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(5, 18, 4, -1, 4);
BleKeyboard bleKeyboard("ESP32 MacroPad", "Handmade", 100);


// --- Matrix ---
const byte ROWS = 2;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1','2','3','4'},
    {'5','6','7','8'}
};
byte rowPins[ROWS] = {26, 25};
byte colPins[COLS] = {13, 12, 14, 27};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- MAIN ---
int lastEncoderValue = 0;
bool sleeping = false;
char lastKey = '-';
bool rainbowActive = false;

void fillLEDs(byte r, byte g, byte b);
void rainbowCycle();

void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();

    strip.begin();
    strip.setBrightness(150);
    strip.show();

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR)) {
        Serial.println("OLED Failed");
    }
    display.clearDisplay();

    rotaryEncoder.begin();
    rotaryEncoder.setup([]{ rotaryEncoder.readEncoder_ISR(); });
    rotaryEncoder.setBoundaries(-1000, 1000, false);
}

void loop() {
    bool connected = bleKeyboard.isConnected();
    char key = keypad.getKey();

    if (key && key != '8') {
        lastKey = key;
    }

    if (key == '8') {
        sleeping = !sleeping;
        if (sleeping) {
            fillLEDs(0, 0, 0);
            display.ssd1306_command(SSD1306_DISPLAYOFF);
            strip.show();
        } else {
            display.ssd1306_command(SSD1306_DISPLAYON);
        }
        return;
    }

    if (!sleeping) {
        if (key && connected) {
            rainbowActive = false;

            if (key == '1') {
                bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
                fillLEDs(0, 255, 0); // Green
            }
            else if (key == '2') {
                bleKeyboard.write(KEY_MEDIA_MUTE);
                fillLEDs(255, 0, 0); // Red
            }
            else if (key == '3') {
                bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
                fillLEDs(0, 255, 255); // Teal
            }
            else if (key == '4') {
                bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
                fillLEDs(255, 0, 255); // Purple
            }
            else if (key == '7') {
                rainbowActive = true;
            }
            else {
                fillLEDs(255, 255, 255); // White
            }

            strip.show();
        }

        if (rainbowActive) {
            static unsigned long lastRainbowUpdate = 0;
            if (millis() - lastRainbowUpdate > 20) {
                rainbowCycle();
                lastRainbowUpdate = millis();
            }
        }

        if (connected) {
            int currentVal = rotaryEncoder.readEncoder();
            if (currentVal > lastEncoderValue) {
                bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
                lastEncoderValue = currentVal;
            } else if (currentVal < lastEncoderValue) {
                bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
                lastEncoderValue = currentVal;
            }
        }

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.print(connected ? "BT: CONNECTED" : "BT: PAIRING...");

        if(connected && !sleeping) {
            display.setCursor(0, 30);
            display.setTextSize(2);
            display.print("Key:");
            display.print(lastKey);
        }
        display.display();
    }
}

void fillLEDs(byte r, byte g, byte b) {
    for(int i=0; i<NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
}

void rainbowCycle() {
    static uint16_t j = 0;
    for(int i=0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(j + (i * 65536 / strip.numPixels()))));
    }
    strip.show();
    j += 512;
    if (j >= 65536) j = 0;
}