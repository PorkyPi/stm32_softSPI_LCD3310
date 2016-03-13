#include "lcd3310.h"
#include "stm32f1xx_hal.h"

/*
*Setup pins for initiation starting software SPI
*It disable for initialization pins (MOSI, RES, SCE, DC) and enable
*pin CLK for rising toggle
*/
void SPI_Software_Init(void) { 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);     //CLK enable 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);   //MOSI disable
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);   //RES disable
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);    //SCE disable
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);    //DC disable
}

/*
*thet function send one bite to SPI
*May be rewrite in future with Toggle_Pin
*/
void SPI_send_byte(char data)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);  //SCE disable
	//set counter fo 8 bite
	for(char i = 0; i<8; i++)
	{
		//CLK = 0
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);  //send one bit
		HAL_Delay(0);
		
		if(data & 0x80)
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); 
			HAL_Delay(0);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			HAL_Delay(0);
		}
		//CLK = 1
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		data <<= 1; //
		HAL_Delay(0);
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   //SCE Enable
	HAL_Delay(0);
}

/*
* Function send data to display
* The pin DC must be HIGH for LCD reception data
*/
void display_data(char data) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET); //Enable DC for the LCD start recive data
	HAL_Delay(1);
	SPI_send_byte(data);
}


/*
* Funtion send comand to display
* The pin DC and the pin SCE must be low for LCD reception comand
*/
void display_cmd(char data) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); //disable DC for the LCD start recive comand
	HAL_Delay(1);
	SPI_send_byte(data);

}

/*
*LCD initialazing
*/
void display_init(void) {
	SPI_Software_Init();
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET); // Low level on RES
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);  // HIGH level on RES
	
	display_cmd(0x21);	// Extended instruction set
	display_cmd(0x80 + 56);	  // Voltage offset
	
	display_cmd(0x04);	      // temperature compensation 0
	display_cmd(0x13);	      // 1:48 offset scheme
	display_cmd(0x20);
	display_cmd(0x0c);	      // Normal display
}

/*
* Function set cursor position  
* y - Row
* x - Column
*/
void display_setpos(char y, char x) {
	display_cmd(0x40 + y);
	display_cmd(0x80 + x);
}
 
/*
* Function clears the screen and puts the cursor in the first position
*
*/
void display_clear(void) {
	display_setpos(0, 0);
	for (char y = 0; y < 6; y++) {
		for (char x = 0; x < 84; x++) {
			display_data(0);
		}
	}
	display_setpos(0, 0);
}

/*
*function print one symbol
*/
void char_print(char ch)
{
	if(ch<32) //????????? ???????? ??? ??????? ???????? ??????? ??? ? ????????
	{
		for(char i=0; i<5; i++)
		{
			display_data(0);
		}
	}
	if(ch < 128)
	{
		ch -= 32;
		for(char i=0; i<5; i++)
		{
			display_data(char_arrey[ch][i]);
		}
	}
	else
	{
		ch -= 96;
		for(char i=0; i<5; i++)
		{
			display_data(char_arrey[ch][i]);
		}
	}
}
 
/*
*function print string
*/
void print_string(char *a)
{
	while(*(a) != '\0')
	{
		char_print(*(a++));
	}
}
 
