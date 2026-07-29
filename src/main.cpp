#include "BleHID.h"
#include "KeyboardMatrix.h"
#include "KeyboardLayout.h"
#include <Arduino.h>

BleHID keyboard;
KeyboardMatrix matrix;

constexpr uint8_t BUTTON_PIN = 4;

uint8_t test = 0;
bool lastButton = HIGH;

void onMatrixKeyChange(uint8_t row, uint8_t col, bool pressed)
{
  uint8_t key = keycodeFromMatrix(row, col);

  if (key == 0)
    return;

  Serial.print("Matrix key ");
  Serial.print(row);
  Serial.print(",");
  Serial.print(col);
  Serial.print(pressed ? " pressed -> " : " released -> ");
  Serial.println((char)key);

  if (pressed)
  {
    keyboard.press(key);
  }
  else
  {
    keyboard.release(key);
  }
}

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

  case 3:
    Serial.println("Simultaneous combo: Ctrl + Shift + Enter");
    {
      const uint8_t combo[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_ENTER};
      keyboard.pressMany(combo, 3);
      delay(20);
      keyboard.release(KEY_ENTER);
      keyboard.release(KEY_LEFT_SHIFT);
      keyboard.release(KEY_LEFT_CTRL);
    }
    break;

  case 4:
    Serial.println("6-key rollover demo: a s d f g h");
    {
      const uint8_t rollover[] = {'a', 's', 'd', 'f', 'g', 'h'};
      keyboard.pressMany(rollover, 6);
      delay(20);
      for (size_t i = 0; i < 6; ++i)
      {
        keyboard.release(rollover[i]);
      }
    }
    break;

  case 5:
    Serial.println("Stress typing burst");
    for (int i = 0; i < 10; ++i)
    {
      keyboard.print("abc ");
      delay(5);
    }
    break;

  case 6:
    Serial.println("Rapid key changes");
    for (int i = 0; i < 20; ++i)
    {
      keyboard.press(KEY_LEFT_SHIFT);
      keyboard.press('a');
      keyboard.release('a');
      keyboard.release(KEY_LEFT_SHIFT);
      delay(2);
    }
    break;
  }

  test = (test + 1) % 7;
}

void setup()
{
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  matrix.begin();
  matrix.setKeyChangeCallback(onMatrixKeyChange);
  keyboard.begin("ESP32 Keyboard");
}

void loop()
{
  matrix.scan();

  if (matrix.changed())
  {
    Serial.println("Matrix state changed:");
    matrix.printState();
    Serial.println();
  }

  bool pressed = digitalRead(BUTTON_PIN) == LOW;

  if (pressed && lastButton && keyboard.isConnected())
  {
    loopTest();
  }

  lastButton = !pressed;
}