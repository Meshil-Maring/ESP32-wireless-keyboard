#include "BleHID.h"
#include <Arduino.h>

BleHID keyboard;

constexpr uint8_t TEST_BUTTON_PIN = 4; // Connect button between GPIO 4 and GND

bool testRequested = false;
bool buttonWasReleased = true;

void setup()
{
  Serial.begin(115200);

  while (!Serial)
  {
    delay(10);
  }

  pinMode(TEST_BUTTON_PIN, INPUT_PULLUP);
  keyboard.begin("ESP32 Keyboard");
}

void runKeyboardTest()
{
  static bool testStarted = false;

  if (testStarted)
    return;

  testStarted = true;

  Serial.println("Starting keyboard test sequence...");

  Serial.println("Test 1: Enter key");
  keyboard.press(KEY_ENTER);
  delay(20);
  keyboard.release(KEY_ENTER);

  delay(500);

  Serial.println("Test 2: Shift + A");
  keyboard.press(KEY_LEFT_SHIFT);
  keyboard.press('a');
  delay(20);
  keyboard.release('a');
  keyboard.release(KEY_LEFT_SHIFT);

  delay(500);

  Serial.println("Test 3: Ctrl + Enter");
  keyboard.press(KEY_LEFT_CTRL);
  keyboard.press(KEY_ENTER);
  delay(20);
  keyboard.release(KEY_ENTER);
  keyboard.release(KEY_LEFT_CTRL);

  Serial.println("Keyboard test sequence complete.");
}

void loop()
{
  const bool buttonPressed = (digitalRead(TEST_BUTTON_PIN) == LOW);

  if (buttonPressed && buttonWasReleased)
  {
    buttonWasReleased = false;
    testRequested = true;
    Serial.println("Test button pressed.");
  }
  else if (!buttonPressed)
  {
    buttonWasReleased = true;
  }

  if (keyboard.isConnected() && testRequested)
  {
    runKeyboardTest();
  }
}
