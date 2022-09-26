#ifndef _FSM_H_
#define	_FSM_H_

namespace main_screen {
enum gui_enum {
  STATE_INIT, STATE_RUNNING, STATE_SETPOINT, STATE_BOOST, STATE_STANDBY, STATE_SLEEP
};
extern gui_enum gui_state;
void setup();
void loop();
}

#endif
