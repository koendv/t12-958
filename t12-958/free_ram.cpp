#include <stdint.h>
#include <malloc.h>
#include "free_ram.h"
extern "C" char *sbrk(int i);

/* from linker script */

extern char _end;
extern char _sdata;
extern char _estack;
extern char _Min_Stack_Size;
static char *ramstart = &_sdata;
static char *ramend = &_estack;
static char *minSP = (char *)(ramend - &_Min_Stack_Size);

/* from cmsis_gcc.h */

/**
 * \brief   Get Main Stack Pointer
 * \details Returns the current value of the Main Stack Pointer (MSP).
 * \return               MSP Register value
 */
__attribute__((always_inline)) static inline uint32_t __get_MSP(void)
{
  register uint32_t result;

  __asm volatile ("MRS %0, msp" : "=r" (result));

  return (result);
}


uint32_t free_ram(void)
{
  char *heapend = (char *)sbrk(0);
  char *stack_ptr = (char *)__get_MSP();
  struct mallinfo mi = mallinfo();

  return (((stack_ptr < minSP) ? stack_ptr : minSP) - heapend + mi.fordblks);
}
