#ifndef _KEY_INPUT_H_
#define _KEY_INPUT_H_

#include <M5Core2.h>

class KeyInput {
    public:
      KeyInput(int pinL, int pinR);
      void setup();
      void update();
      bool getInputL();
      bool getInputR();
      bool keyLDown();
      bool keyRDown();
      bool keyLUp();
      bool keyRUp();

    private:
      int pinL;
      int pinR;
      bool lastInputL;
      bool lastInputR;
      bool inputL;
      bool inputR;
};

#endif _KEY_INPUT_H_
