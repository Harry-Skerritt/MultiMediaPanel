#include <Arduino.h>
#include <Keypad.h>
#include <BleKeyboard.h>
#include <AiEsp32RotaryEncoder.h>

#include "LEDManager/LEDManager.h"
#include "DisplayManager/DisplayManager.h"

// --- PINS ---
#define LED_PIN 2
#define NUM_LEDS 12
#define SCREEN_ADDR 0x3C


// --- INIT ---
DisplayManager display(128, 64);
LEDManager leds(LED_PIN, NUM_LEDS);

AiEsp32RotaryEncoder rotary_encoder = AiEsp32RotaryEncoder(5, 18, 4, -1, 4);
BleKeyboard ble_keyboard("ESP32 MacroPad", "Handmade", 100);


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
int last_encoder_value = 0;
bool sleeping = false;
char last_key = '-';
bool rainbowActive = false;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Keyboard
    ble_keyboard.begin();

    // LEDs
    leds.initialise();

    // Display
    display.initialise();

    // Encoder
    rotary_encoder.begin();
    rotary_encoder.setup([]{ rotary_encoder.readEncoder_ISR(); });
    rotary_encoder.setBoundaries(-1000, 1000, false);
}

void loop() {
    bool connected = ble_keyboard.isConnected();
    char key = keypad.getKey();

    if (key && key != '8') {
        last_key = key;
    }

    if (key == '8') {
        sleeping = !sleeping;
        if (sleeping) {
            display.setSleep(true);
            leds.off();
        } else {
            display.setSleep(false);
        }
        return;
    }

    if (!sleeping && connected) {
        leds.update();

        if (key) {
            rainbowActive = false;

            if (key == '1') {
                ble_keyboard.write(KEY_MEDIA_PLAY_PAUSE);
                leds.fill(0, 255, 0); // Green
            }
            else if (key == '2') {
                ble_keyboard.write(KEY_MEDIA_MUTE);
                leds.fill(255, 0, 0); // Red
            }
            else if (key == '3') {
                ble_keyboard.write(KEY_MEDIA_NEXT_TRACK);
                leds.fill(0, 255, 255); // Teal
            }
            else if (key == '4') {
                ble_keyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
                leds.fill(255, 0, 255); // Purple
            }
            else if (key == '7') {
                leds.setRainbow(true);
            }
            else {
                leds.fill(255, 255, 255); // White
            }
        }

        int current_val = rotary_encoder.readEncoder();
        if (current_val > last_encoder_value) {
            ble_keyboard.write(KEY_MEDIA_VOLUME_UP);
            last_encoder_value = current_val;
        } else if (current_val < last_encoder_value) {
            ble_keyboard.write(KEY_MEDIA_VOLUME_DOWN);
            last_encoder_value = current_val;
        }

        display.update(connected, last_key, last_encoder_value);
    }
}