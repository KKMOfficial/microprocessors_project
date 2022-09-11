#include <stm32f4xx.h>

void GLCD_INIT(void);
void enable_pulse(void);
void delayMs(int);
void data_out(char);
void command_out(char);
void update(void);

uint8_t data_buffer[128][8];

int main(void){
	GLCD_INIT();
	update();
	while(1){
		
	}
}

void enable_pulse(void){
	GPIOC->ODR |= 0x11;
	delayMs(1);
	GPIOC->ODR &= 0x1E;
}

void data_out(char data){
	GPIOC->ODR |= 0b00100;
	GPIOC->ODR &= 0b11101;
	GPIOA->ODR = 0;
	delayMs(1);
	GPIOA->ODR = data;
	enable_pulse();
}
void command_out(char command){
	GPIOC->ODR &= 0b11101;
	GPIOC->ODR &= 0b11011;
	GPIOA->ODR = 0;
	delayMs(1);
	GPIOA->ODR = command;
	enable_pulse();
}

void update(void){
	//right half
	GPIOC->ODR |= 0b01000;
	delayMs(1000);
	for(char page=0;page<8;page++){
		command_out(0xB8 | page);
		for(char column=64;column<128;column++){
			command_out(0x40 | column);
			data_out(data_buffer[column][page]);
			delayMs(1);
		}
	}
	//left half
	GPIOC->ODR &= 0b10111;
	delayMs(1000);
	// select page
	for(char page=1;page<9;page++){
		command_out(0xB8 | page);
		for(char column=0;column<64;column++){
			command_out(0x40 | column);
			data_out(data_buffer[column][page]);
			delayMs(1);
		}
	}
}

void GLCD_INIT( void ){
	RCC->AHB1ENR |=1<<0 | 1<<2;     //Enable PORTB & PORTC Clock
	GPIOA->MODER |=0x5555;					//PB0-PB7 Become OUTPUT "DATA PORT"
	GPIOC->MODER |=0x0155;          //PC2-PC6 Become OUTPUT "CTRL PORT"
	GPIOA->BSRR  |=0xFF0000;				//RESET DATA PORT
	GPIOC->BSRR  |=0x1F0000;				//RESET CTRL PORT
	
	GPIOC->ODR &= !0x8;				
	delayMs(1000);
	GPIOC->ODR &= 0xEF;
	delayMs(1000);
	GPIOC->ODR |= 0x10;
	delayMs(1000);
	command_out(0x3F);
	delayMs(1000);
	command_out(0x40);
	delayMs(1000);
	command_out(0xBA);
	delayMs(1000);
	command_out(0xCA);
	delayMs(1000);
	
	for(int i=0;i<128;i++){
		for(int j=0;j<64;j++){
			data_buffer[i][j] = 0x0F;
		}
	}
	
}
void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195/2; i++){__NOP();} 
}