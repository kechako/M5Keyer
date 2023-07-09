#include <M5Core2.h>
#include "KeyInput.h"
#include "Tone.h"

#define PIN_L 32
#define PIN_R 33

#define ST_NONE    0
#define ST_PLAYING 1
uint8_t state = ST_NONE;

#define TONE_NONE 0
#define TONE_DASH 1
#define TONE_DOT  2
uint8_t toneState = TONE_NONE;

#define OPTION_UNIT   0
#define OPTION_VOLUME 1
int optionMode = OPTION_UNIT;
int unitMs = 40;
int volume = 100;

float getToneGain() {
  return static_cast<float>(volume) / 100.0f;
}

KeyInput input(PIN_L, PIN_R);
Tone _tone(800, unitMs, getToneGain());

void printKeyState(bool l, bool r) {
  M5.Lcd.setTextSize(10);
  M5.Lcd.setCursor(0, 0);

  if (l && r) {
    M5.Lcd.print("[L] [R]");
  } else if (l) {
    M5.Lcd.print("[L] [ ]");
  } else if (r) {
    M5.Lcd.print("[ ] [R]");
  } else {
    M5.Lcd.print("[ ] [ ]");
  }
}

void printUnitMs() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 80);
  M5.Lcd.printf("Unit: %d ms   ", unitMs);
}

void printVolume() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 80);
  M5.Lcd.printf("Volume: %3d   ", volume);
}

void printOption() {
  switch (optionMode) {
    case OPTION_UNIT:
      printUnitMs();
      break;
    case OPTION_VOLUME:
      printVolume();
      break;
  }
}

void refreshScreen() {
  if (input.keyLDown() || input.keyLUp() || input.keyRDown() || input.keyRUp()) {
    bool l = input.getInputL();
    bool r = input.getInputR();

    printKeyState(l, r);
  }
}

void playDash() {
  _tone.playDash();
  state = ST_PLAYING;
  toneState = TONE_DASH;
}

void playDot() {
  _tone.playDot();
  state = ST_PLAYING;
  toneState = TONE_DOT;
}

void finishPlay() {
  state = ST_NONE;
  toneState = TONE_NONE;
}

void updateState() {
  bool r = input.getInputR();
  bool l = input.getInputL();

  switch (state) {
    case ST_NONE:
      if (r) {
        playDash();
      } else if (l) {
        playDot();
      }
      break;
    case ST_PLAYING:
      if (!_tone.getPlaying()) {
        if (r && l) {
          switch (toneState) {
            case TONE_DASH:
              playDot();
              break;
            case TONE_DOT:
              playDash();
              break;
            default:
              finishPlay();
              break;
          }
        } else if (r) {
          playDash();
        } else if (l) {
          playDot();
        } else {
          finishPlay();
        }
      }
      break;
  }
}

void applyUnitMs() {
  if (_tone.getPlaying()) {
    return;
  }

  _tone.setUnit(unitMs);
}

void applyVolume() {
  if (_tone.getPlaying()) {
    return;
  }

  _tone.setGain(getToneGain());
}

void increaseUnit() {
  if (_tone.getPlaying()) {
    return;
  }

  if (unitMs < 100) {
    unitMs += 5;
    printUnitMs();
  }
}

void decreaseUnit() {
  if (_tone.getPlaying()) {
    return;
  }

  if (unitMs > 5) {
    unitMs -= 5;
    printUnitMs();
  }
}

void increaseVolume() {
  if (_tone.getPlaying()) {
    return;
  }

  if (volume < 100) {
    volume++;
    printVolume();
  }
}

void decreaseVolume() {
  if (_tone.getPlaying()) {
    return;
  }

  if (volume > 0) {
    volume--;
    printVolume();
  }
}

void captureButtons() {
  switch (optionMode) {
    case OPTION_UNIT:
      if (M5.BtnA.pressedFor(1000, 200)) {
        decreaseUnit();
      } else if (M5.BtnA.wasReleased()) {
        decreaseUnit();
        applyUnitMs();
      } else if (M5.BtnB.pressedFor(1000, 200)) {
        increaseUnit();
      } else if (M5.BtnB.wasReleased()) {
        increaseUnit();
        applyUnitMs();
      } else if (M5.BtnC.wasReleased()) {
        optionMode = OPTION_VOLUME;
        printVolume();
      }
      break;
    case OPTION_VOLUME:
      if (M5.BtnA.pressedFor(1000, 200)) {
        decreaseVolume();
      } else if (M5.BtnA.wasReleased()) {
        decreaseVolume();
        applyVolume();
      } else if (M5.BtnB.pressedFor(1000, 200)) {
        increaseVolume();
      } else if (M5.BtnB.wasReleased()) {
        increaseVolume();
        applyVolume();
      } else if (M5.BtnC.wasReleased()) {
        optionMode = OPTION_UNIT;
        printUnitMs();
      }
      break;
  }
}

void setup() {
  // Initialize M5 Core2
  M5.begin(
    true,  // LCD
    false, // SD
    true, // Serial
    false  // I2C
  );

  input.setup();
  _tone.setup();

  printKeyState(false, false);
  printUnitMs();
}

void loop() {
  M5.update();

  input.update();
  _tone.update();

  captureButtons();

  refreshScreen();

  updateState();
}
