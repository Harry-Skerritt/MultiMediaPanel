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

bool last_connection_state = true;

void loop() {
    const bool connected = ble_keyboard.isConnected();
    const char key = keypad.getKey();

    if (connected != last_connection_state) {
        if (!connected) {
            leds.setPulse(true, COLOUR_BLUE);
        } else {
            leds.fill(COLOUR_GREEN);
        }
        last_connection_state = connected;
    }

    if (!sleeping) {
        leds.update();
        display.update(connected, last_key, last_encoder_value);
    }

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

            if (!connected) leds.setPulse(true, COLOUR_BLUE);
            else leds.fill(COLOUR_GREEN); // Todo: Decide a colour
        }
        return;
    }

    if (!sleeping && connected) {
        if (key) {
            rainbowActive = false;

            if (key == '1') {
                ble_keyboard.write(KEY_MEDIA_PLAY_PAUSE);
                leds.fill(COLOUR_GREEN);
            }
            else if (key == '2') {
                ble_keyboard.write(KEY_MEDIA_MUTE);
                leds.fill(COLOUR_RED);
            }
            else if (key == '3') {
                ble_keyboard.write(KEY_MEDIA_NEXT_TRACK);
                leds.fill(RGBColour(0, 255, 255)); // Teal
            }
            else if (key == '4') {
                ble_keyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
                leds.fill(RGBColour(255, 0, 255)); // Purple
            }
            else if (key == '7') {
                leds.setRainbow(true);
            }
            else {
                leds.fill(COLOUR_WHITE); // White
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

        if (rotary_encoder.isEncoderButtonClicked()) {
            leds.onInteraction(RGBColour(255, 0, 212), 1000, true);
        }
    }
}