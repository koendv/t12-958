#ifndef _DISPLAY_H_
#define	_DISPLAY_H_

#include <U8g2lib.h>
extern U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2;

#define	BLACK		0
#define	WHITE		1

namespace menu_screen {
void force_redraw();
void setup();
void loop();
}

#endif
