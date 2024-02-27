#include "w25x16.h"
#include "stm32f1xx_hal.h"
#include "boot_app.h"

extern SPI_HandleTypeDef hspi1;

static uint8_t spi_write_one_byte(uint8_t tx_data){
	
	uint8_t rx_data = 0;
	
	HAL_SPI_TransmitReceive(&hspi1, &tx_data, &rx_data, 1, 100);//ms
	return rx_data;
}


void W25X16_wait_busy(void){
	
	W25X16_CS_LOW
	spi_write_one_byte(W25X16_READ_STATUS_REG);
	while ((spi_write_one_byte(0xFF) & 0x01) == 0x01);
	W25X16_CS_HIGH
}

void W25X16_write_enable(void){
	
	W25X16_CS_LOW
	spi_write_one_byte(W25X16_WRITE_ENABLE);
	W25X16_CS_HIGH
}

void W25X16_write_disable(void){
	
	W25X16_CS_LOW
	spi_write_one_byte(W25X16_WRITE_DISABLE);
	W25X16_CS_HIGH
}

void W25X16_read_x_byte(uint8_t read_buf[], uint32_t read_addr, uint16_t read_byte_num){//
	
	uint16_t i;    							
							
	W25X16_CS_LOW  //片选
	
	spi_write_one_byte(W25X16_READ_DATA);         //发送读取命令   
	spi_write_one_byte((uint8_t)((read_addr) >> 16));  //发送24bit地址    
	spi_write_one_byte((uint8_t)((read_addr) >> 8));   
	spi_write_one_byte((uint8_t)read_addr); 
  
	for(i = 0; i < read_byte_num; i++){ 
			 read_buf[i] = spi_write_one_byte(0XFF);   //循环读数  
			 //if(((i+1)%1000)==0)
			 //Feed_Dog_In_Task();
	}
	
	W25X16_CS_HIGH   //取消片选   
}

void W25X16_write_one_page(uint8_t write_buf[], uint32_t write_addr, uint16_t write_byte_num){//
	
	uint16_t i;    	
	
	W25X16_write_enable();
	W25X16_CS_LOW
							
	spi_write_one_byte(W25X16_PAGE_PROGRAM);         //发送读取命令   
	spi_write_one_byte((uint8_t)((write_addr) >> 16));  //发送24bit地址    
	spi_write_one_byte((uint8_t)((write_addr) >> 8));   
	spi_write_one_byte((uint8_t)write_addr); 
  
	for(i = 0; i < write_byte_num; i++){ 
			 spi_write_one_byte(write_buf[i]);   //循环读数  
			 //if(((i+1)%1000)==0)
			 //Feed_Dog_In_Task();
	}
	
	W25X16_CS_HIGH   
	W25X16_wait_busy();//等待写结束
}

void W25X16_write_x_byte(uint8_t write_buf[], uint32_t write_addr, uint16_t write_byte_num){
	
	uint16_t page_remain_byte;    	
	
	W25X16_WP_High//写保护关闭
	page_remain_byte = 256 - write_addr % 256;//单页剩余字节数
	
	do {
		
		if (write_byte_num <= page_remain_byte){		
			
			page_remain_byte = write_byte_num;
			W25X16_write_one_page(write_buf, write_addr, page_remain_byte);
			write_byte_num -= page_remain_byte;
			
		} else{
			
			W25X16_write_one_page(write_buf, write_addr, page_remain_byte);
			write_byte_num -= page_remain_byte;
			page_remain_byte = 256;
			
		}
		
	} while(write_byte_num);
	
	W25X16_WP_Low //写保护打开
}

void W25X16_erase_sector(uint32_t dst_addr){//
	  		
	W25X16_WP_High
	dst_addr *= 4096;					
	
	W25X16_write_enable();	
	W25X16_wait_busy();
	W25X16_CS_LOW
	
	spi_write_one_byte(W25X16_SECTOR_ERASE);         //发送读取命令   
	spi_write_one_byte((uint8_t)((dst_addr) >> 16));  //发送24bit地址    
	spi_write_one_byte((uint8_t)((dst_addr) >> 8));   
	spi_write_one_byte((uint8_t)dst_addr); 
	
	W25X16_CS_HIGH   
	W25X16_wait_busy();//等待写结束
	W25X16_WP_Low
}














