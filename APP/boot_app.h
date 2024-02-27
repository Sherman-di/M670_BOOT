#ifndef __BOOT_APP__H_
#define __BOOT_APP__H_

#include "stdint.h"
#include "aegis_debug.h"


#define W25X16_CS_LOW										HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);         
#define W25X16_CS_HIGH									HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_SET);  
#define W25X16_WP_Low    								HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET); 
#define W25X16_WP_High   								HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET); 

#define PROGRAM_DATA_LENGTH             (4096)//4k
#define APP_PROGRAM_START_ADDRESS       (0x8005000)

typedef void (*p_fun)(void);

extern void boot_app_task(void);

#endif
