#include "internal_flash.h"
#include "stm32f1xx_hal.h"

//从指定地址读出数据
static uint32_t read_flash_word_data(uint32_t flash_addr){
	
	return *(__IO uint32_t*)flash_addr;
}

//从指定地址开始读出指定长度的数据
void read_intternal_flash(uint32_t flash_addr, uint32_t* flash_data, uint16_t data_num){
	
	uint16_t i = 0;
	
	for(i = 0; i < data_num; i++){
		
		flash_data[i] = read_flash_word_data(flash_addr + (i * 4));
	}
}

//一次最多写入2k byte 
uint8_t write_intternal_flash(uint32_t flash_addr, uint32_t* flash_data, uint16_t data_num){

	uint16_t i = 0;
	uint32_t data_buf = 0, page_error = 0;
	
	//解锁
	HAL_FLASH_Unlock();
	
	//初始化FLASH_EraseInitTypeDef
	FLASH_EraseInitTypeDef flash_init;
	flash_init.TypeErase = FLASH_TYPEERASE_PAGES;//页擦除
	flash_init.PageAddress = flash_addr;
	flash_init.NbPages = 2;
	
	//擦除
	HAL_FLASHEx_Erase(&flash_init, &page_error);
	
	//对FLASH烧写
	for(i = 0; i < data_num; i++){
		
		data_buf = *(flash_data + i);
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (flash_addr + (i * 4)), data_buf))
			return 1;
	}
	
	//上锁
	HAL_FLASH_Lock();

}



