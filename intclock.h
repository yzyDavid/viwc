#ifndef VIC_INTCLOCK_H
#define VIC_INTCLOCK_H

#include <graphics.h>

extern InterruptFunctionPointer old_clock_int;

void interrupt clock_int();

#endif //VIC_INTCLOCK_H
