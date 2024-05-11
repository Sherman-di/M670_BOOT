#include "boot_app.h"
#include "string.h"
#include "main.h"
#include "stm32f1xx.h"
#include "w25x16.h"
#include "internal_flash.h"

static void MX_GPIO_DeInit(void);
static void MX_SPI1_DeInit(void);
static void MX_USART2_UART_DeInit(void);
static uint8_t test_data[5] = {0xfe, 0xfe, 0xfe, 0xfe, 0xfe};

p_fun jump_to_program = NULL;
#pragma pack(1)
static union{
	uint8_t data[5];
	struct{
		uint8_t flag;
		uint32_t data_byte_num;
	}program;
}app;

static union{
	uint8_t data_8[PROGRAM_DATA_LENGTH];
	uint32_t data_32[PROGRAM_DATA_LENGTH/4];
}program;

static union{
	uint8_t data_8[PROGRAM_DATA_LENGTH];
	uint32_t data_32[PROGRAM_DATA_LENGTH/4];
}program_temp;

#pragma pack()

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;
extern void Error_Handler(void);

static uint16_t Cal_CRC16(uint8_t p_data[], uint8_t offset, uint32_t size)
{
    uint8_t u8Cnt;
    uint16_t u16CrcResult = 0xA28C;
    uint32_t i = offset;

    while (size != 0)
    {
        u16CrcResult ^= p_data[i++];
        for (u8Cnt = 0; u8Cnt < 8; u8Cnt++)
        {
            if ((u16CrcResult & 0x1) == 0x1)
            {
                u16CrcResult >>= 1;
                u16CrcResult ^= 0x8408;
            }
            else
            {
                u16CrcResult >>= 1;
            }
        }
        size--;
    }

    return ((uint16_t)(~u16CrcResult));
}
#if 1
static __asm void modify_stack_pointer_and_start_app(uint32_t r0_sp, uint32_t r1_pc)
{
    MOV SP, R0
    BX R1
}
#else
static void jump(void){
	
	uint32_t jump_address = 0;
	
	/* Jump to user application */
	AEGIS_PRINT("  Runing user boot!\r\n\n");
	//__disable_irq();//close irq
	jump_address = *(volatile uint32_t*) (APP_PROGRAM_START_ADDRESS + 4);
	jump_to_program = (p_fun) jump_address;

	__set_MSP(*(volatile uint32_t*) APP_PROGRAM_START_ADDRESS);//初始化堆栈指针

	jump_to_program();
	
	AEGIS_PRINT("  Err to runing user application!\r\n\n");

}
#endif
void boot_app_task(void){
	
	uint8_t temp = 0, num = 0;
	uint16_t crc1 = 0, crc2 = 0; 
	uint32_t w25x16_sector = 0, w25x16_addr = FLASH_APP_HEX_FILE_SECTOR_BASE, i;
	
	HAL_Delay(200);
	W25X16_read_x_byte(app.data, (FLASH_APP_DATA_FLAG_SECTOR_BASE << 12), sizeof(app.data));
	
//	for (i = 0; i < 5; i++){
//		
//		AEGIS_PRINT("  app.data[%d]:0x%x\n", i, app.data[i]);
//	}		
	
	AEGIS_PRINT("  flag:%d byte_num:%d\n", app.program.flag, app.program.data_byte_num);
	
	if (app.program.flag == 0x5a){//有新固件要升级
				
		do {
				
			AEGIS_PRINT("\n  ------>app.program.data_byte_num(%d)w25x16_addr(%x)\n", app.program.data_byte_num, w25x16_addr);	
			if (app.program.data_byte_num >= PROGRAM_DATA_LENGTH){		
				
					W25X16_read_x_byte(program.data_8, (w25x16_addr << 12), PROGRAM_DATA_LENGTH);
					
//					for (i = 0; i < 160; i++){						
//						AEGIS_PRINT("%X ", program.data_8[i]);
//					}	
									
					HAL_Delay(100);
					
					app.program.data_byte_num -= PROGRAM_DATA_LENGTH;
					write_intternal_flash((APP_PROGRAM_START_ADDRESS + PROGRAM_DATA_LENGTH * w25x16_sector), program.data_32, (PROGRAM_DATA_LENGTH / 4));		
					
					w25x16_addr++;
					w25x16_sector++;
					
			} else{
				
				num = 4;
				
				do{
					
					HAL_Delay(100);
					W25X16_read_x_byte(program.data_8, (w25x16_addr << 12), app.program.data_byte_num);
					temp = app.program.data_byte_num % 4;	
				
					for (i = 0; i < (4 - temp); i++){
						program.data_8[app.program.data_byte_num + i] = 0xff;
					}
					
					app.program.data_byte_num += (4 - temp);
					
					crc1 = Cal_CRC16(program.data_8, 0, app.program.data_byte_num);
					HAL_Delay(100);					
					write_intternal_flash((APP_PROGRAM_START_ADDRESS + PROGRAM_DATA_LENGTH * w25x16_sector), program.data_32, (app.program.data_byte_num / 4));
					HAL_Delay(100);	
					read_intternal_flash((APP_PROGRAM_START_ADDRESS + PROGRAM_DATA_LENGTH * w25x16_sector), program_temp.data_32, (app.program.data_byte_num / 4));
					crc2 = Cal_CRC16(program_temp.data_8, 0, app.program.data_byte_num);
					
					if (crc1 == crc2){
						
						app.program.data_byte_num = 0;	
						break;
					}				
					num--;					
					
				}while(num);		

				if(!num) AEGIS_PRINT("\n  write internal flash fail.....\n");				
			} 
			
		} while(app.program.data_byte_num);		

		W25X16_erase_sector(FLASH_APP_DATA_FLAG_SECTOR_BASE);
		HAL_Delay(100);
		W25X16_write_x_byte(test_data, (FLASH_APP_DATA_FLAG_SECTOR_BASE << 12), sizeof(test_data));
		AEGIS_PRINT("  write internal flash success\n");
	} 

	MX_USART2_UART_DeInit();
	MX_SPI1_DeInit();
	MX_GPIO_DeInit();
	HAL_RCC_DeInit();
	HAL_DeInit();
	HAL_Delay(100);
	//jump();		
	modify_stack_pointer_and_start_app(APP_PROGRAM_START_ADDRESS, RESET_HANDLER);
}

static void MX_GPIO_DeInit(void)
{

  /* GPIO Ports Clock Disable */
	__HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOC, FLASH_WP_Pin|FLASH_CS_Pin);
}

static void MX_USART2_UART_DeInit(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_DeInit(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

static void MX_SPI1_DeInit(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_DeInit(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}


