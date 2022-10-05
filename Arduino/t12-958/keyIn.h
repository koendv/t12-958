/* -*- C++ -*- */

/* quick and dirty keyboard driver
 * only one key (encoder button)
 * uses interrupt-driven "button.h" 
 */

#ifndef RSITE_KEYIN_DEF_H
#define	RSITE_KEYIN_DEF_H
//#include "../menuDefs.h"
#include "menuDefs.h"
#include "menuBase.h"
#include "button.h"

namespace Menu {
#include "menuIO/keyMapDef.h"

// emulate a stream keyboard

template<int N>
class keyIn : public menuIn {
public:
  keyIn<N>(keyMap k[]) {}
  void begin()
  {
  }


  int available(void)
  {
    if (button::button_click || button::button_longpress) {
      return (1);
    }else{
      return (0);
    }
  }


  int peek(void)
  {
    if (button::button_longpress) {
      return (options->getCmdChar(escCmd));
    } else if (button::button_click) {
      return (options->getCmdChar(enterCmd));
    } else {
      return (-1); // noCmd
    }
  }


  int read()
  {
    if (button::button_longpress) {
      button::button_longpress = false;
      return (options->getCmdChar(escCmd));
    } else if (button::button_click) {
      button::button_click = false;
      return (options->getCmdChar(enterCmd));
    } else {
      return (-1); // noCmd
    }
  }


  void flush() {}
  size_t write(uint8_t v) { return (0); }

};
}  //namespace Menu

#endif
