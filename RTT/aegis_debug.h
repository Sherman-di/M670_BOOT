#ifndef __DEBUGE__H_
#define __DEBUGE__H_
#include "stdlib.h"
#include "SEGGER_RTT.h"

#define AEGIS_PRINT(format, ...)    SEGGER_RTT_printf(0, format, ##__VA_ARGS__); 

#define AEGIS_PRINT_COLOR(color, format, ...)     {\
SEGGER_RTT_printf(0,color);                  \
SEGGER_RTT_printf(0, format, ##__VA_ARGS__); \
}


#endif
