#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <BleKeyboard.h>
#include <AiEsp32RotaryEncoder.h>

#include "LEDManager/LEDManager.h"

// --- PINS ---
#define LED_PIN 2
#define NUM_LEDS 12
#define SCREEN_ADDR 0x3C


// --- INIT ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);
LEDManager leds = LEDManager(LED_PIN, NUM_LEDS);
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

void setup() {
    Serial.begin(115200);
    bleKeyboard.begin();

    // LEDs
    leds.initialise();

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
            display.ssd1306_command(SSD1306_DISPLAYOFF);
            leds.off();
        } else {
            display.ssd1306_command(SSD1306_DISPLAYON);
        }
        return;
    }

    if (!sleeping) {
        leds.update();

        if (key && connected) {
            rainbowActive = false;

            if (key == '1') {
                bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
                leds.fill(0, 255, 0); // Green
            }
            else if (key == '2') {
                bleKeyboard.write(KEY_MEDIA_MUTE);
                leds.fill(255, 0, 0); // Red
            }
            else if (key == '3') {
                bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
                leds.fill(0, 255, 255); // Teal
            }
            else if (key == '4') {
                bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
                leds.fill(255, 0, 255); // Purple
            }
            else if (key == '7') {
                leds.setRainbow(true);
            }
            else {
                leds.fill(255, 255, 255); // White
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