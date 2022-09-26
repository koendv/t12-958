#ifndef _BUTTON_H_
#define	_BUTTON_H_

namespace button {
extern bool button_pressed;
extern bool button_click;
extern bool button_longpress;

inline bool pressed()
{
  return (button_pressed);
}


inline bool click()
{
  bool ret = button_click;

  button_click = false;
  return (ret);
}


inline bool longpress()
{
  bool ret = button_longpress;

  button_longpress = false;
  return (ret);
}


void setup();

}
#endif
