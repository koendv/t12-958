
#include "t12encoder.h"
#include <zf_exti.h>
#include <isr.h>

// Enable this to emit codes twice per step.
//#define HALF_STEP

// Values returned by 'direction'
// No complete step yet.
#define	DIR_NONE	0x0
// Clockwise step.
#define	DIR_CW		0x10
// Anti-clockwise step.
#define	DIR_CCW		0x20

/*
 * The below state table has, for each state (row), the new state
 * to set based on the next encoder output. From left to right in,
 * the table, the encoder outputs are 00, 01, 10, 11, and the value
 * in that position is the new state to set.
 */

#define	R_START    0x0

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define	R_CCW_BEGIN		0x1
#define	R_CW_BEGIN		0x2
#define	R_START_M		0x3
#define	R_CW_BEGIN_M		0x4
#define	R_CCW_BEGIN_M		0x5
static const uint8_t ttable[6][4] =
{
  { R_START_M,		 R_CW_BEGIN,	R_CCW_BEGIN,  R_START		}, // R_START (00)
  { R_START_M | DIR_CCW, R_START,	R_CCW_BEGIN,  R_START		}, // R_CCW_BEGIN
  { R_START_M | DIR_CW,	 R_CW_BEGIN,	R_START,      R_START		}, // R_CW_BEGIN
  { R_START_M,		 R_CCW_BEGIN_M, R_CW_BEGIN_M, R_START		}, // R_START_M (11)
  { R_START_M,		 R_START_M,	R_CW_BEGIN_M, R_START | DIR_CW	}, // R_CW_BEGIN_M
  { R_START_M,		 R_CCW_BEGIN_M, R_START_M,    R_START | DIR_CCW }, // R_CCW_BEGIN_M
};
#else
// Use the full-step state table (emits a code at 00 only)
#define	R_CW_FINAL	0x1
#define	R_CW_BEGIN	0x2
#define	R_CW_NEXT	0x3
#define	R_CCW_BEGIN	0x4
#define	R_CCW_FINAL	0x5
#define	R_CCW_NEXT	0x6

static const uint8_t ttable[7][4] =
{
  { R_START,	R_CW_BEGIN,  R_CCW_BEGIN, R_START	    }, // R_START
  { R_CW_NEXT,	R_START,     R_CW_FINAL,  R_START | DIR_CW  }, // R_CW_FINAL
  { R_CW_NEXT,	R_CW_BEGIN,  R_START,	  R_START	    }, // R_CW_BEGIN
  { R_CW_NEXT,	R_CW_BEGIN,  R_CW_FINAL,  R_START	    }, // R_CW_NEXT
  { R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START	    }, // R_CCW_BEGIN
  { R_CCW_NEXT, R_CCW_FINAL, R_START,	  R_START | DIR_CCW }, // R_CCW_FINAL
  { R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START	    }, // R_CCW_NEXT
};
#endif

static volatile int32_t state = R_START;
volatile int32_t t12enc_position = 0;

void t12enc_init()
{
  // initialize fsm
  state = R_START;
  t12enc_position = 0;
  // register t12enc_update() as interrupt handler for EXTI line 3 and 4 (PB3 and PB4)
  extisr_ptr[3] = &t12enc_update;
  extisr_ptr[4] = &t12enc_update;
  // enable EXTI interrupts for gpio PB3 and PB4
  exti_interrupt_init(B3, EXTI_Trigger_Rising_Falling, 0x01);
  exti_interrupt_init(B4, EXTI_Trigger_Rising_Falling, 0x01);
  return;
}


// interrupt handler. needs c calling convention.
void t12enc_update()
{
  // Grab state of input pins PB3 and PB4
  uint8_t pinstate = (GPIOB->IDR >> 3) & 0x3;

  // Determine new state from the pins and state table.
  state = ttable[state & 0xf][pinstate];
  // Determine fsm output
  uint8_t direction = state & 0x30;

  // Update position
  if (direction == DIR_CW) {
    t12enc_position++;
  }else if (direction == DIR_CCW) {
    t12enc_position--;
  }
}
