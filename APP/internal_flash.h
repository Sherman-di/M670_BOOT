#ifndef _INTERNAL_FLASH_H_
#define _INTERNAL_FLASH_H_

#include "stdint.h"

extern void read_intternal_flash(uint32_t flash_addr, uint32_t* flash_data, uint16_t data_num);
extern uint8_t write_intternal_flash(uint32_t flash_addr, uint32_t* flash_data, uint16_t data_num);

#endif
