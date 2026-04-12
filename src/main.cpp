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
    Wire.begin();

    // Load Settings
    settings_manager.begin();
    DeviceSettings saved_settings = settings_manager.load();

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
        if (action == EncoderAction::CLICK_HOLD) {
            static bool is_settings = false;
            is_settings = !is_settings;
            page_manager.switchPage(is_settings ? PageID::SETTINGS : PageID::MEDIA);
            leds.onInteraction(RGBColour(255, 100, 0), 600, true);
            return;
        }

        if (connected) {
            page_manager.update(action, key);
        }

        // 5. UPDATE: Background tasks
        leds.update();
        page_manager.draw(*display.getDisplay());
    }

    /*
    if (!sleeping && connected) {

        // --- BTN Matrix ---
        if (key) {
            rainbowActive = false;

            if (key == '1') {
                ble_keyboard.write(KEY_MEDIA_PLAY_PAUSE);
                leds.onInteraction(COLOUR_GREEN, 800, true);
            }
            else if (key == '2') {
                ble_keyboard.write(KEY_MEDIA_NEXT_TRACK);
                leds.onInteraction(RGBColour(0, 255, 255), 800, true); // Teal
            }
            else if (key == '3') {
                ble_keyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
                leds.onInteraction(RGBColour(255, 0, 255), 800, true); // Purple
            }
            else if (key == '4') {
                ble_keyboard.write(KEY_MEDIA_STOP);
                leds.onInteraction(COLOUR_RED, 800, true);
            }
            else if (key == '7') {
                leds.setRainbow(true);
            }
            else {
                leds.onInteraction(COLOUR_WHITE, 800, true); // White
            }
        }

        // --- Encoder ---
        if (action != EncoderAction::NONE) {
            switch (action) {
                case EncoderAction::CLOCKWISE:
                    ble_keyboard.write(KEY_MEDIA_VOLUME_UP);
                    leds.onInteraction(RGBColour(187, 255, 0), 50);
                    break;

                case EncoderAction::COUNTER_CLOCKWISE:
                    ble_keyboard.write(KEY_MEDIA_VOLUME_DOWN);
                    leds.onInteraction(RGBColour(187, 255, 0), 50);
                    break;

                case EncoderAction::SINGLE_CLICK:
                    ble_keyboard.write(KEY_MEDIA_MUTE);
                    leds.onInteraction(COLOUR_RED, 500, true);
                    break;

                case EncoderAction::DOUBLE_CLICK:
                    display.showMessage("Double Click!", "Page Switch");
                    leds.onInteraction(COLOUR_WHITE, 500, true);
                    break;

                case EncoderAction::CLICK_HOLD:
                    static bool is_settings = false;
                    is_settings = !is_settings;
                    page_manager.switchPage(is_settings ? PageID::SETTINGS : PageID::MEDIA);
                    leds.onInteraction(RGBColour(255, 100, 0), 1000, true);
                    break;

                case EncoderAction::DOUBLE_CLICK_HOLD:
                    display.showMessage("Double Click Hold!", "Secret Menu");
                    leds.onInteraction(RGBColour(255, 255, 0), 2000, true);
                    break;

                default: break;
            }
        }
    }
    */
}