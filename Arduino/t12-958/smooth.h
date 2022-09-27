#ifndef _SMOOTH_H_
#define	_SMOOTH_H_
#include <stdint.h>

class smooth {
public:
  int32_t data(int32_t x);
  void reset();

private:
  bool first = true;
  int32_t y = 0;
};

#endif
