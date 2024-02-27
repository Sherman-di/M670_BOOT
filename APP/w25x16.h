#ifndef _W25X16_H_
#define _W25X16_H_

#include "stdint.h"
//W25X16∂¡–¥÷∏¡Ó±Ì
#define FLASH_ID 									(0XEF14)
#define W25X16_WRITE_ENABLE				(0x06) 
#define W25X16_WRITE_DISABLE			(0x04) 
#define W25X16_READ_STATUS_REG		(0x05) 
#define W25X16_WRITE_STATUS_REG		(0x01) 
#define W25X16_READ_DATA					(0x03) 
#define W25X16_FAST_READ_DATA			(0x0B) 
#define W25X16_FAST_READ_DUAL			(0x3B) 
#define W25X16_PAGE_PROGRAM				(0x02) 
#define W25X16_BLOCK_ERASE				(0xD8) 
#define W25X16_SECTOR_ERASE				(0x20) 
#define W25X16_CHIP_ERASE					(0xC7) 
#define W25X16_POWER_DOWN					(0xB9) 
#define W25X16_RELEASE_POWER_DOWN	(0xAB) 
#define W25X16_DEVICE_ID					(0xAB) 
#define W25X16_MANUFACT_DEVICE_ID	(0x90) 
#define W25X16_JEDEC_DEVICE_ID		(0x9F) 

#define FLASH_APP_DATA_FLAG_SECTOR_BASE   (279ul)
#define FLASH_APP_HEX_FILE_SECTOR_BASE   	(280ul)//280~405

extern void W25X16_wait_busy(void);
extern void W25X16_write_enable(void);
extern void W25X16_write_disable(void);
extern void W25X16_read_x_byte(uint8_t read_buf[], uint32_t read_addr, uint16_t read_byte_num);
extern void W25X16_write_one_page(uint8_t write_buf[], uint32_t write_addr, uint16_t write_byte_num);
extern void W25X16_write_x_byte(uint8_t write_buf[], uint32_t write_addr, uint16_t write_byte_num);
extern void W25X16_erase_sector(uint32_t dst_addr);



#endif









