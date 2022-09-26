#ifndef _WAKEUP_H_
#define	_WAKEUP_H_

/*
 * motion detection in soldering iron
 */

class MotionSensor {
public:
  MotionSensor();
  bool moved();
private:
  uint32_t count;
};

#endif
