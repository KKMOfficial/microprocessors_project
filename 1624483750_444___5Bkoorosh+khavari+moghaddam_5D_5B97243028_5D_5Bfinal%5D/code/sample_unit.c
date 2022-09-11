#include <stm32f4xx.h>

void initiate(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
char keyPressed(void);
void delayMs(int);
void LCD_data(char);
void LCD_command(unsigned char);	

#define RS 0x1     // Pin mask for reg select (e.g. here is pin 5)
#define RW 0x2     // Pin mask for read/write (e.g. here is pin 6)
#define EN 0x4     // Pin mask for enable     (e.g. here is pin 7)

char frame_data[128];
char init_data[128];
int time_unit = 100000;
int max_sample = 200000;
int frame_step = 0;
int sample_num = 0;
int init_data_counter = 0;
	
int main(void){
	
	initiate();
	char keypressed = 12;
	while(1){
		keypressed = keyPressed();
		if(keypressed != 12){
			delayMs(1);
			LCD_data(keypressed);
			delayMs(1);
		}
	}
}
void initiate(void){
	
	// ADC initiation
	RCC->AHB1ENR |= 7;								// enable port A,B,C clock
	GPIOA->MODER |= 0x33;							// set port A0,A1 to analog
	RCC->APB2ENR |= (1<<8);						// enable A2D clock
	ADC1->CR2 |= (1<<10) | (1<<1);		// enable: EOC set at end of conversion | single convert mode
	ADC1->CR2 &= 0xFFFD;							// enable ADC without power saving time
	
	// Timer initiation
	__disable_irq();									// disable interrupts
	RCC->APB1ENR |= 3;								// enable timer 2 and 3
	TIM2->PSC = 160 - 1;							// set period of timer2 = 10us
	TIM2->ARR = 1 - 1;
	TIM3->PSC = 1600 - 1;							// set period of timer3 = 200ms
	TIM3->ARR = 2000 - 1;
	TIM2->CR1 |= 1;										// enable timer2 CounterEnable(CEN)
	TIM3->CR1 |= 1;										// enable timer3 CounterEnable(CEN)	
	TIM2->DIER |= 1;									// send interrupt after update event
	TIM3->DIER |= 1;									// send interrupt after update event
	NVIC_EnableIRQ(TIM2_IRQn);				// set interrupt function for the timer2
	NVIC_EnableIRQ(TIM3_IRQn);				// set interrupt function for the timer3
	__enable_irq();										// enable interrupts
	
	// USART initiation
	RCC->APB2ENR |= (1<<4);						// enable USART1
	GPIOA->AFR[1] = 0x00000770; 			// alt7 for USART1 make A9 tx , A10 rx
  GPIOA->MODER |= 0x00280000;  			// enable alternate function for PA9 and PA10
	USART1->BRR = 0x0683;    					// 9600 baud @ 16 MHz 
	USART1->CR1 = 0x000C;    					// enable Tx,Rx, 8-bit data by default 1 start bit
  USART1->CR2 = 0x0000;    					// 1 stop bit
  USART1->CR3 = 0x0000;    					// no flow control
  USART1->CR1 |= 0x2000;   					// enable USART1
	
	// Keypad initiation
	GPIOB->MODER |= 0x55000000;				//GPIOB init make keypadA... output and Keypad1... input
	
	// LCD initiation
	GPIOC->MODER = 0x00005555;				//GPIOC init make D0..D7 output
	GPIOB->MODER |= 0x00005400;   		// set pin output mode for LCD
	//GPIOB->BSRR = 0x00C00000;   			// turn off EN and R/W
	delayMs(30);            					// initialization sequence
  LCD_command(0x30);
  delayMs(10);
  LCD_command(0x30);
  delayMs(1);
  LCD_command(0x30);
	LCD_command(0x38);								// set 8-bit data, 2-line, 5x7 font
	LCD_command(0x06);								// move cursor right after each char
	LCD_command(0x01);								// clear screen, move cursor to home
	LCD_command(0x0F);								// turn on display, cursor blinking
	
	
	
}
void delayMs(int n) {
    int i;
    for (; n > 0; n--)
        for (i = 0; i < 3195/2; i++){__NOP();} 
}
void LCD_data(char data) {
    //TODO                   				// RS = 1
		GPIOB->ODR |= RS;
    //TODO              			 	    // R/W = 0
		GPIOB->ODR &= !RW;
    //TODO                   				// put data on data bus
		GPIOC->ODR = data;
    //TODO           				        // pulse EN high
		GPIOB->ODR |= EN;
    delayMs(0);             				// Do not change this line!
    //TODO                   				// clear EN
		GPIOB->ODR &= !EN;

    delayMs(1);
}
void LCD_command(unsigned char command) {
    //TODO                          // RS = 0, R/W = 0
		GPIOB->ODR &= !(RS|RW);
    //TODO                          // put command on data bus
		GPIOC->ODR = command;
    //TODO                          // pulse EN high
		GPIOB->ODR |= EN;
    delayMs(0);
    //TODO                          // clear EN
		GPIOB->ODR &= !EN;

    if (command < 4)
        delayMs(2);         				// command 1 and 2 needs up to 1.64ms
    else
        
;         				// all others 40 us
}





char keyPressed(void){
		
	GPIOB->ODR = 0xEFFF;
	delayMs(1000);
	// 1
	if((GPIOB->IDR | 0xFEFF) == 0xFEFF)return 1;
	// 2
	if((GPIOB->IDR | 0xFDFF) == 0xFDFF)return 2;
	// 3
	if((GPIOB->IDR | 0xFBFF) == 0xFBFF)return 3;
	
	GPIOB->ODR = 0xDFFF;
	delayMs(1000);
	// 4
	if((GPIOB->IDR | 0xFEFF) == 0xFEFF)return 4;
	// 5
	if((GPIOB->IDR | 0xFDFF) == 0xFDFF)return 5;
	// 6
	if((GPIOB->IDR | 0xFBFF) == 0xFBFF)return 6;
	
	GPIOB->ODR = 0xBFFF;
	delayMs(1000);
	// 7
	if((GPIOB->IDR | 0xFEFF) == 0xFEFF)return 7;
	// 8
	if((GPIOB->IDR | 0xFDFF) == 0xFDFF)return 8;
	// 9
	if((GPIOB->IDR | 0xFBFF) == 0xFBFF)return 9;
		
	GPIOB->ODR = 0x7FFF;
	delayMs(1000);
	// *
	if((GPIOB->IDR | 0xFEFF) == 0xFEFF)return 10;
	// 0
	if((GPIOB->IDR | 0xFDFF) == 0xFDFF)return 0;
	// #
	if((GPIOB->IDR | 0xFBFF) == 0xFBFF)return 11;
	
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
}