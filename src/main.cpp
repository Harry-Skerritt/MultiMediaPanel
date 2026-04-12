#include <Arduino.h>
#include <Keypad.h>
#include <BleKeyboard.h>


#include "LEDManager/LEDManager.h"
#include "DisplayManager/DisplayManager.h"
#include "EncoderManager/EncoderManager.h"
#include "Pages/PageManager.h"
#include "SettingsManager/SettingsManager.h"

// --- PINS ---
#define LED_PIN 2
#define NUM_LEDS 12
#define SCREEN_ADDR 0x3C


// --- INIT ---
SettingsManager settings_manager;

DisplayManager display(128, 64);

LEDManager leds(LED_PIN, NUM_LEDS);

AiEsp32RotaryEncoder rotary_encoder(5, 18, 4, -1, 4);
EncoderManager encoder(rotary_encoder);

BleKeyboard ble_keyboard("ESP32 MacroPad", "Handmade", 100);

PageManager page_manager(leds, ble_keyboard, settings_manager);



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
    while (!Serial) delay(10);
    delay(1000);
    Serial.println("--- Booting MacroPad ---");

    // Load Settings
    settings_manager.begin();
    const DeviceSettings saved_settings = settings_manager.load();
    page_manager.initialise(saved_settings);

    // Keyboard
    ble_keyboard.begin();

    // LEDs
    leds.initialise(saved_settings.led_brightness);
    leds.setEnabled(saved_settings.leds_enabled);

    // Display
    display.initialise();
    display.setContrast(saved_settings.screen_contrast);

    // Encoder
    rotary_encoder.begin();
    rotary_encoder.setup([]{ rotary_encoder.readEncoder_ISR(); });
    rotary_encoder.setBoundaries(-1000, 1000, false);
}

bool last_connection_state = true;

void loop() {
    const EncoderAction action = encoder.update();
    const bool connected = ble_keyboard.isConnected();
    const char key = keypad.getKey();

    // --- BT Connection ---
    if (connected != last_connection_state) {
        if (!connected) {
            leds.setPulse(true, COLOUR_BLUE);
        } else {
            leds.fill(RGBColour(255, 0, 119));
        }
        last_connection_state = connected;
    }

    // --- Sleeping ---
    if (key == '8') {
        sleeping = !sleeping;
        display.setSleep(sleeping);
        if (sleeping) {
            leds.off();
        } else {
            if (!connected) leds.setPulse(true, COLOUR_BLUE);
            else leds.fill(RGBColour(255, 0, 119)); // Todo: Decide a colour
        }
        return;
    }

    if (!sleeping) {
        if (action == EncoderAction::DOUBLE_CLICK_HOLD) {
            if (page_manager.getCurrentPageID() == PageID::SETTINGS) {
                page_manager.switchPage(PageID::MEDIA);
            } else {
                page_manager.switchPage(PageID::SETTINGS);
            }
            leds.onInteraction(RGBColour(255, 100, 0), 600, true);
            return;
        }

        if (connected || page_manager.getCurrentPageID() == PageID::SETTINGS) {
            page_manager.update(action, key);
        }

        // Background tasks
        leds.update();
        page_manager.draw(*display.getDisplay());
    }
}