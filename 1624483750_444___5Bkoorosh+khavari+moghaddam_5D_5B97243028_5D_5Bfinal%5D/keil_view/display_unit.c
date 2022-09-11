#include <stm32f4xx.h>

void GLCD_INIT(void);
void enable_pulse(void);
void delayMs(int);
void data_out(char);
void command_out(char);
void update(int);
void draw_point(int,int,int);
void usart_init(void);
void sendPacket(uint32_t);
uint32_t receivePacket(void);

void set_A_parameter(void);
void set_B_parameter(void);
void referesh_page(void);

//A parameter
float A_param = 1;

//B parameter
float B_param = 0;

//Time Unit
uint32_t Time_Unit = 100;

//USART Protocol
uint32_t Set_Parameter_A = 1;
uint32_t Set_Parameter_B = 2;
uint32_t Channel_1_Frame = 3;


uint8_t data_buffer0[128][8];
uint8_t data_buffer1[128][8];
int refresh_buffer_turn = 1;

int main(void){
	usart_init();
	GLCD_INIT();
	
	
	//change display buffer
	//for(int i=0;i<128;i++){
	//		draw_point(i,(i%64));
	//}

	
	//

	
	//uint8_t x = receivePacket();
	//if(x == 0xFF){
	//	GPIOA->ODR |= 0xFFFFFFFF;
	//	delayMs(1000);
	//}
	
	
	//draw_point(50,50);
	//draw_point(10,50);
	//draw_point(80,50);
	
	//draw_point(51,20);
	//draw_point(11,20);
	//draw_point(81,20);
	//delayMs(3000);
	//update();
	
	uint32_t packet_type;
	
	while(1){
		
		//get packet type
		packet_type = receivePacket();
		
		switch(packet_type){
			case 1:{set_A_parameter(); break;}
			case 2:{set_B_parameter(); break;}
			case 3:{referesh_page(); break;}
		}
		
		
	}
}

void referesh_page(){
	//store 128 x[n]'s
	uint32_t x[128];
	for(int i=0;i<128;i++){
		x[i] = receivePacket();
		delayMs(1);
	}
	//find y values
	float y[128];
	for(int i=0;i<128;i++){
		y[i] = A_param*x[i]+B_param;
	}
	//draw final y[n]'s
	for(int i=0;i<128;i++){
		draw_point(i,(int)y[i],refresh_buffer_turn);
	}
	//fast update screen
	update(refresh_buffer_turn);
	//change turn
	refresh_buffer_turn = 1 - refresh_buffer_turn;
}

void set_A_parameter(){
		uint32_t natural_part, decimal_part;
		natural_part = receivePacket();
		delayMs(100);
		decimal_part = receivePacket();
		delayMs(100);
		A_param = 10*natural_part+decimal_part;
		A_param = A_param / 10;
}

void set_B_parameter(){
		uint32_t natural_part, decimal_part;
		natural_part = receivePacket();
		delayMs(100);
		decimal_part = receivePacket();
		delayMs(100);
		B_param = 10*natural_part+decimal_part;
		B_param = B_param / 10;
}


void usart_init(){
	// USART initiation
	RCC->AHB1ENR = RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR = (1<<4);						// enable USART1
	GPIOA->AFR[1] = 0x770; 						// alt7 for USART1 make A9 tx , A10 rx
  GPIOA->MODER = 0x280000;  				// enable alternate function for PA9 and PA10
	USART1->BRR = 0x008B;    					// 9600 baud @ 16 MHz 
	USART1->CR1 = 0x000C;    					// enable Tx,Rx, 8-bit data by default 1 start bit
  USART1->CR2 = 0x0000;    					// 1 stop bit
  USART1->CR3 = 0x0000;    					// no flow control
  USART1->CR1 |= 0x2000;   
}
void sendPacket(uint32_t s){
	// check buffer to be empty
	while((USART1->SR & (1<<7))!=(1<<7)){}
		
	// set data register
	USART1->DR = s;
		
	// delay 
	delayMs(2);	
}
uint32_t receivePacket(){
	while((USART1->SR & (1<<5))!=(1<<5)){__NOP();}
	return USART1->DR;
}
void draw_point(int x, int y, int buffer_number){
	//find page number
	int page = 7 - (y/8);
	//find page offset
	int line = 7 - y%8;
	if(buffer_number==0){
		data_buffer0[x][page] = ((0xFEFF<<line)&(0xFF00))>>8;
	}else{
		data_buffer1[x][page] = ((0xFEFF<<line)&(0xFF00))>>8;
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

void update(int buffer_num){
	//left half
	GPIOC->ODR |= 0b01000;
	
	
	if(buffer_num == 0){
		// select page
		for(char column=0;column<64;column++){
			command_out(0x40 | column);
			for(char page=0;page<8;page++){
				command_out(0xB8 | page);
				data_out(data_buffer0[column][page]);
				if(data_buffer0[column][page]<0xFF){
					break;
				}
			}
		}
		
		//right half
		GPIOC->ODR &= 0b10111;
		
		for(char column=64;column<128;column++){
			command_out(0x40 | column);
			for(char page=0;page<8;page++){
				command_out(0xB8 | page);
				data_out(data_buffer0[column][page]);
				if(data_buffer0[column][page]<0xFF){
					break;
				}
			}
		}
	}else if(buffer_num == 1){
		// select page
		for(char column=0;column<64;column++){
			command_out(0x40 | column);
			for(char page=0;page<8;page++){
				command_out(0xB8 | page);
				data_out(data_buffer1[column][page]);
				if(data_buffer1[column][page]<0xFF){
					break;
				}
			}
		}
		
		//right half
		GPIOC->ODR &= 0b10111;
		
		for(char column=64;column<128;column++){
			command_out(0x40 | column);
			for(char page=0;page<8;page++){
				command_out(0xB8 | page);
				data_out(data_buffer1[column][page]);
				if(data_buffer1[column][page]<0xFF){
					break;
				}
			}
		}
	}
	
	
	
	//for(char page=0;page<8;page++){
	//	command_out(0xB8 | page);
	//	for(char column=0;column<64;column++){
	//		command_out(0x40 | column);
	//		data_out(data_buffer[column][page]);
			
	//	}
	//}
	//for(char page=0;page<8;page++){
	//	command_out(0xB8 | page);
	//	for(char column=64;column<128;column++){
	//		command_out(0x40 | column);
	//		data_out(data_buffer[column][page]);
			
	//	}
	//}
}

void GLCD_INIT( void ){
	RCC->AHB1ENR |=1<<2;     				//Enable PORTC Clock
	GPIOA->MODER |=0x5555;					//PA0-PA7 Become OUTPUT "DATA PORT"
	GPIOC->MODER |=0x0155;          //PC2-PC6 Become OUTPUT "CTRL PORT"
	GPIOA->BSRR  |=0xFF0000;				//RESET DATA PORT
	GPIOC->BSRR  |=0x1F0000;				//RESET CTRL PORT
	
	GPIOC->ODR &= !0x8;				
	delayMs(30);
	GPIOC->ODR &= 0xEF;
	delayMs(30);
	GPIOC->ODR |= 0x10;
	delayMs(30);
	command_out(0x3F);
	delayMs(30);
	command_out(0x40);
	delayMs(30);
	command_out(0xBA);
	delayMs(30);
	
	for(int i=0;i<128;i++){
		for(int j=0;j<64;j++){
			data_buffer0[i][j] = 0xFF;
			data_buffer1[i][j] = 0xFF;
		}
	}
	
}
void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195/2; i++){__NOP();} 
}