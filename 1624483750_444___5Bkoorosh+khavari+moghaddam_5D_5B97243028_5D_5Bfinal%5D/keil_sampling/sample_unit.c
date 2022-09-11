#include <stm32f4xx.h>

void initiate(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
char keyPressed(void);
void delayMs(int);
void LCD_data(char);
void LCD_command(unsigned char);	
void LDC_init(void);
void enable_pulse(void);
uint32_t receivePacket(void);
void sendPacket(uint32_t);
void usart_init(void);

void increase_and_send_A(void);
void decrease_and_send_A(void);
void increase_and_send_B(void);
void decrease_and_send_B(void);
void increase_and_send_T(void);
void decrease_and_send_T(void);
void timer2_init(void);
void timer3_init(void);
void lcd_update_A(void);
void lcd_update_B(void);
void lcd_update_T(void);
void increase_time_unit(void);
void decrease_time_unit(void);



char frame_data[128];
char init_data[128];
int max_sample = 200000;
int frame_step = 0;
int sample_num = 0;
int init_data_counter = 0;

//A parameter
float A_param = 1;
float A_Change = 0.1;

//B parameter
float B_param = 0;
float B_Change = 0.1;

//Time Unit
uint32_t time_unit = 156;
uint32_t TU_Change = 100000;

//USART Protocol
uint32_t Set_Parameter_A = 1;
uint32_t Set_Parameter_B = 2;
uint32_t Channel_1_Frame = 3;


	
int main(void){
	
	initiate();
	usart_init();
	LDC_init();
	char result = 12;
	
	//sendPacket(0xFF);
	//delayMs(1000);
	
	timer2_init();
	delayMs(100);
	timer3_init();
	delayMs(100);
	
	//initiate lcd
	LCD_data('A');
	delayMs(1);
	LCD_data('=');
	delayMs(1);
	lcd_update_A();
	LCD_command(0x14);
	LCD_data('B');
	delayMs(1);
	LCD_data('=');
	delayMs(1);
	lcd_update_B();
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_data('T');
	delayMs(1);
	LCD_data('i');
	delayMs(1);
	LCD_data('m');
	delayMs(1);
	LCD_data('e');
	delayMs(1);
	LCD_data(' ');
	delayMs(1);
	LCD_data('U');
	delayMs(1);
	LCD_data('n');
	delayMs(1);
	LCD_data('i');
	delayMs(1);
	LCD_data('t');
	delayMs(1);
	LCD_data('=');
	delayMs(1);
	lcd_update_T();
	
	while(1){
		result = keyPressed();
		switch(result){
			case 1:{increase_and_send_A(); break;}
			case 2:{decrease_and_send_A(); break;}
			case 3:{increase_and_send_B(); break;}
			case 4:{decrease_and_send_B(); break;}
			case 5:{increase_time_unit(); break;}
			case 6:{decrease_time_unit(); break;}
			case 12:{break;}
		}
		result = 12;
		//delayMs(10);
	}
}
void increase_time_unit(void){
	time_unit = time_unit + TU_Change;
	lcd_update_T();
}
void decrease_time_unit(void){
	time_unit = time_unit - TU_Change;
	lcd_update_T();
}
void lcd_update_T(void){
	//go to specific position
	LCD_command(0x02);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	
	uint32_t copy = Time_Unit;
	char digit;
	while(copy != 0){
		digit = copy % 10;
		copy = copy / 10;
		LCD_data(digit);
		delayMs(5);
	}
	
}
void lcd_update_A(void){
	//go to specific position
	LCD_command(0x02);
	LCD_command(0x14);
	LCD_command(0x14);
	//write natural part
	LCD_data((char)A_param+'0');
	delayMs(5);
	LCD_data('.');
	delayMs(5);
	LCD_data((uint32_t)((A_param-(float)(uint32_t)A_param)*10)+'0');
	delayMs(5);
}

void lcd_update_B(void){
	//go to specific position
	LCD_command(0x02);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	LCD_command(0x14);
	//write natural part
	LCD_data((char)B_param+'0');
	delayMs(5);
	LCD_data('.');
	delayMs(5);
	LCD_data((uint32_t)((B_param-(float)(uint32_t)B_param)*10)+'0');
	delayMs(5);
}


void increase_and_send_A(void){
	A_param = A_param + A_Change;
	sendPacket(Set_Parameter_A);
	delayMs(100);
	sendPacket((uint32_t)A_param);
	delayMs(100);
	sendPacket((uint32_t)((A_param-(float)(uint32_t)A_param)*10));
	delayMs(100);
	lcd_update_A();
}
void decrease_and_send_A(void){
	A_param = A_param - A_Change;
	sendPacket(Set_Parameter_A);
	delayMs(100);
	sendPacket((uint32_t)A_param);
	delayMs(100);
	sendPacket((uint32_t)((A_param-(float)(uint32_t)A_param)*10));
	delayMs(100);
	lcd_update_A();
}
void increase_and_send_B(void){
	B_param = B_param + B_Change;
	sendPacket(Set_Parameter_B);
	delayMs(100);
	sendPacket((uint32_t)B_param);
	delayMs(100);
	sendPacket((uint32_t)((B_param-(float)(uint32_t)B_param)*10));
	delayMs(100);
	lcd_update_B();
}
void decrease_and_send_B(void){
	B_param = B_param - B_Change;
	sendPacket(Set_Parameter_B);
	delayMs(100);
	sendPacket((uint32_t)B_param);
	delayMs(100);
	sendPacket((uint32_t)((B_param-(float)(uint32_t)B_param)*10));
	delayMs(100);
	lcd_update_B();
}

void enable_pulse(void){
	GPIOB->ODR |= 0b100;
	delayMs(1);             				// Do not change this line!
	//TODO                   				// clear EN
	GPIOB->ODR &= 0b011;
}
void initiate(void){
	
	// ADC initiation
	RCC->AHB1ENR |= 7;								// enable port A,B,C clock
	GPIOA->MODER  = 0x00000000;  			  
	GPIOA->MODER |= 0x33;							// set port A0,A1 to analog
	RCC->APB2ENR |= (1<<8);						// enable A2D clock
	ADC1->CR2 |= (1<<10);							// enable: EOC set at end of conversion | single convert mode
	ADC1->CR2 &= 0b1111111111111110;	// enable ADC without power saving time
	ADC1->CR2 |= 1;										//ENABLE ADC
	
								
	
	// Keypad initiation
	GPIOA->MODER |= 0x55000000;  			  //GPIOA init make Keypad1... output
	
}
void timer2_init(void){
	// Timer2 initiation
	__disable_irq();									// disable interrupts
	RCC->APB1ENR |= 1;								// enable timer 2 
	TIM2->PSC = 16 - 1;							// set period of timer2 = 10us
	TIM2->ARR = 10 - 1;
	TIM2->CR1 = 1;										// enable timer2 CounterEnable(CEN)
	TIM2->DIER |= 1;									// send interrupt after update event
	NVIC_EnableIRQ(TIM2_IRQn);				// set interrupt function for the timer2
	__enable_irq();
}
void timer3_init(void){
	// Timer3 initiation
	__disable_irq();
	RCC->APB1ENR |= 2;								// enable timer 3
	TIM3->PSC = 1600 - 1;							// set period of timer3 = 200ms
	TIM3->ARR = 2000 - 1;
	TIM3->CR1 = 1;										// enable timer3 CounterEnable(CEN)	
	TIM3->DIER |= 1;									// send interrupt after update event
	NVIC_EnableIRQ(TIM3_IRQn);				// set interrupt function for the timer3
	__enable_irq();										// enable interrupts
}
void usart_init(void){
	// USART initiation
	RCC->AHB1ENR |= 1;
	RCC->APB2ENR |= (1<<4);						// enable USART1
	GPIOA->AFR[1] = 0x770; 						// alt7 for USART1 make A9 tx , A10 rx
  GPIOA->MODER |= 0x280000;  				// enable alternate function for PA9 and PA10
	USART1->BRR = 0x008B;    					// 9600 baud @ 16 MHz 
	USART1->CR1 = 0x000C;    					// enable Tx,Rx, 8-bit data by default 1 start bit
  USART1->CR2 = 0x0000;    					// 1 stop bit
  USART1->CR3 = 0x0000;    					// no flow control
  USART1->CR1 |= 0x2000;   					// enable USART1
}
uint32_t receivePacket(){
	while((USART1->SR & (1<<5))!=(1<<5)){}
	return USART1->DR;
}
void sendPacket(uint32_t s){
	// check buffer to be empty
	while((USART1->SR & (1<<7))!=(1<<7)){}
		
	// set data register
	USART1->DR = s;
		
	// delay 
	delayMs(2);	
		
	// wait till operation ends
	while((USART1->SR & (1<<6))!=(1<<6)){}	
			
}
void LDC_init(void){
	// LCD initiation
	GPIOC->MODER = 0x00005555;				//GPIOC init make D0..D7 output
	GPIOB->MODER |= 0x00000015;   		// set pin output mode for LCD
	
	GPIOB->BSRR = 0x00C00000;   			// turn off EN and R/W
	delayMs(300);            					// initialization sequence
  LCD_command(0x30);
  delayMs(100);
  LCD_command(0x30);
  delayMs(10);
  LCD_command(0x30);
	delayMs(10);
	LCD_command(0x38);								// set 8-bit data, 2-line, 5x7 font
	delayMs(10);
	LCD_command(0x06);								// move cursor right after each char
	delayMs(10);
	LCD_command(0x01);								// clear screen, move cursor to home
	delayMs(10);
	LCD_command(0x0F);								// turn on display, cursor blinking
	delayMs(10);
	
}
void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195/2; i++){__NOP();} 
}
void LCD_data(char data) {
    //TODO                   				// RS = 1
		GPIOB->ODR |= 0b001;
    //TODO              			 	    // R/W = 0
		GPIOB->ODR = 0b101;
    //TODO                   				// put data on data bus
		GPIOC->ODR = 0xFF00 | data;
    //TODO           			
		enable_pulse();

    delayMs(10);
}
void LCD_command(unsigned char command) {
    //TODO                          // RS = 0, R/W = 0
		GPIOB->ODR &= 0b100;
    //TODO                          // put command on data bus
		GPIOC->ODR = 0xFF00 | command;
    //TODO                          // pulse EN high
		enable_pulse();

    if (command < 4)
        delayMs(20);         				// command 1 and 2 needs up to 1.64ms
    else
        delayMs(10);         				// all others 40 us
}





char keyPressed(void){
		
	GPIOA->ODR = 0xEFFF;
	delayMs(15);
	// 1
	delayMs(5);
	if((GPIOB->IDR | 0xEFFF) == 0xEFFF){return 1;}
	// 2
	delayMs(5);
	if((GPIOB->IDR | 0xDFFF) == 0xDFFF){return 2;}
	// 3
	delayMs(5);
	if((GPIOB->IDR | 0xBFFF) == 0xBFFF){return 3;}
	
	GPIOA->ODR = 0xDFFF;
	delayMs(15);
	// 4
	delayMs(5);
	if((GPIOB->IDR | 0xEFFF) == 0xEFFF){return 4;}
	// 5
	delayMs(5);
	if((GPIOB->IDR | 0xDFFF) == 0xDFFF){return 5;}
	// 6
	delayMs(5);
	if((GPIOB->IDR | 0xBFFF) == 0xBFFF){return 6;}
	
	GPIOA->ODR = 0xBFFF;
	delayMs(15);
	// 7
	delayMs(5);
	if((GPIOB->IDR | 0xEFFF) == 0xEFFF){return 7;}
	// 8
	delayMs(5);
	if((GPIOB->IDR | 0xDFFF) == 0xDFFF){return 8;}
	// 9
	delayMs(5);
	if((GPIOB->IDR | 0xBFFF) == 0xBFFF){return 9;}
		
	GPIOA->ODR = 0x7FFF;
	delayMs(15);
	// *
	delayMs(5);
	if((GPIOB->IDR | 0xEFFF) == 0xEFFF){return 10;}
	// 0
	delayMs(5);
	if((GPIOB->IDR | 0xDFFF) == 0xDFFF){return 0;}
	// #
	delayMs(5);
	if((GPIOB->IDR | 0xBFFF) == 0xBFFF){return 11;}
	
	// if nothing pressed
	return 12;
}
void TIM2_IRQHandler(void){
	TIM2->SR = 0;											// clear UIF bit(start timer again after finishing)
	//this timer intended for convertor strobe software simulator
	ADC1->CR2 |= (1<<30);							// set SWSTART = 1 to convert and make sample
	while(!(ADC1->SR & 2)){}					// wait untile sample is done
	//where to store the sample
	if(sample_num % time_unit == 0 && init_data_counter < 128){
		init_data[init_data_counter]=ADC1->DR;
		init_data_counter +=1;
	}
	sample_num += 1;
}
void TIM3_IRQHandler(void){
	TIM3->SR = 0;											// clear UIF bit(start timer again after finishing)
	//this timer intended for sending frames using USART port
	frame_step = 128 / (sample_num / time_unit);
	int index = 0;
	while(index < 128){
		frame_data[index] = init_data[index];
		index += frame_step;
	}
	init_data_counter = 0;
	sample_num = 0;
	// send the 128 data packet through usart port
	sendPacket(Channel_1_Frame);
	delayMs(1);
	for(int i=0;i<128;i++){
		sendPacket(frame_data[index]);
		delayMs(1);
	}
	
}
