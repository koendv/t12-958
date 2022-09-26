/* This replaces ArduinoMenu encoderIn.h */

#include "t12-common.h"

#ifndef RSITE_ARDUINO_MENU_ENCODER
  #define RSITE_ARDUINO_MENU_ENCODER

  //#include "../menuDefs.h"
  #include "menuDefs.h"

  namespace Menu {

    //emulate a stream based on encoderIn movement returning +/- for every 'sensivity' steps
    //buffer not needer because we have an accumulator
    class encoderInStream:public menuIn {
    public:
      int sensivity;
      int oldPos=0;
      encoderInStream(int sensivity): sensivity(sensivity) {}
      inline void setSensivity(int s) {sensivity=s;}
      int available(void) {return abs(encoder.read()-oldPos)/sensivity;}
      int peek(void) override {
        int d=encoder.read()-oldPos;
        if (d<=-sensivity)return options->navCodes[downCmd].ch;
        if (d>=sensivity) return options->navCodes[upCmd].ch;
        return -1;
      }
      int read() override {
        int d=encoder.read()-oldPos;
        if(d>0 && oldPos<0) d+=sensivity;
        else if(d<0 && oldPos>0) d-=sensivity;
        if (d<=-sensivity) {
          oldPos-=sensivity;
          return options->navCodes[downCmd].ch;
        }
        if (d>=sensivity) {
          oldPos+=sensivity;
          return options->navCodes[upCmd].ch;
        }
        return -1;
      }
      void flush() {oldPos=encoder.read();}
      size_t write(uint8_t v) {oldPos=v;return 1;}

    };

  }//namespace Menu
#endif
