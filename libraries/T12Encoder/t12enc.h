/* Rotary encoder handler for t12-958 */
#ifndef _T12ENC_UPDATE_H_
#define	_T12ENC_UPDATE_H_
#include <stdint.h>

void t12enc_init(void);
void t12enc_update(void);

extern volatile int32_t t12enc_position;

#endif
