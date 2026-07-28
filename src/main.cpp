#include "BleHID.h"
#include <Arduino.h>

BleHID keyboard;

constexpr uint8_t BUTTON_PIN = 4;

uint8_t test = 0;
bool lastButton = HIGH;

void loopTest()
{
  switch (test)
  {
  case 0:
    Serial.println("Shift + A");
    keyboard.press(KEY_LEFT_SHIFT);
    keyboard.press('a');
    delay(20);
    keyboard.release('a');
    keyboard.release(KEY_LEFT_SHIFT);
    break;

  case 1:
    Serial.println("Ctrl + Enter");
    keyboard.press(KEY_LEFT_CTRL);
    keyboard.press(KEY_ENTER);
    delay(20);
    keyboard.release(KEY_ENTER);
    keyboard.release(KEY_LEFT_CTRL);
    break;

  case 2:
    Serial.println("Alt + Tab");
    keyboard.press(KEY_LEFT_ALT);
    keyboard.press(KEY_TAB);
    delay(20);
    keyboard.release(KEY_TAB);
    keyboard.release(KEY_LEFT_ALT);
    break;
  }

  test = (test + 1) % 3;
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  keyboard.begin("ESP32 Keyboard");
}

void loop()
{
  bool pressed = digitalRead(BUTTON_PIN) == LOW;

  if (pressed && lastButton && keyboard.isConnected())
  {
    loopTest();
  }

  lastButton = !pressed;
}