#include "KeyInput.h"

KeyInput::KeyInput(int pinL, int pinR) : pinL(pinL),
                                         pinR(pinR),
                                         lastInputL(false),
                                         lastInputR(false),
                                         inputL(false),
                                         inputR(false) {
}

void KeyInput::setup() {
  pinMode(this->pinL, INPUT);
  pinMode(this->pinR, INPUT);
}

void KeyInput::update() {
  this->lastInputL = this->inputL;
  this->lastInputR = this->inputR;

  this->inputL = digitalRead(this->pinL) == LOW;
  this->inputR = digitalRead(this->pinR) == LOW;
}

bool KeyInput::getInputL() {
    return this->inputL;
}

bool KeyInput::getInputR() {
    return this->inputR;
}

bool KeyInput::keyLDown() {
    return this->inputL && this->inputL != this->lastInputL;
}

bool KeyInput::keyRDown() {
    return this->inputR && this->inputR != this->lastInputR;
}

bool KeyInput::keyLUp() {
    return !this->inputL && this->inputL != this->lastInputL;
}

bool KeyInput::keyRUp() {
    return !this->inputR && this->inputR != this->lastInputR;
}
