#include <avr/io.h> 
#include <avr/interrupt.h>
#include <stdlib.h>

static volatile uint8_t startFlag = 0, countFlag = 0, microCounter = 0;
static volatile uint16_t miliCounter = 0, counter = 0;
uint8_t aux, randNum;

//100uS 

//#define RAND_MAX 9

void restartCount();
void UART_Ini();
void Timer0_Ini();
uint8_t checkFlag (uint8_t flag);
char UART0_getchar();
void UART0_putchar(char dato);
void UART0_puts(char *string);
void restartCount();

int main(){
	char teclazo;
	
	UART_Ini();
	
	while (1){
		
		uint8_t loopFlag = 1;
		
		while (loopFlag){
			
			UART0_puts("\n\rPress enter to continue\n\r");
			
			teclazo = UART0_getchar();
			if (teclazo == 13)
				loopFlag = 0;
			
		}
		
		Timer0_Ini();
		
		while(!checkFlag(startFlag));
		TCCR0B = 0;	
		
		//rand() % (max_number + 1 - minimum_number) + minimum_number
		randNum = ('a' + (rand() % (25 + 1)));
		UART0_puts("Quickly! Press '");
		UART0_putchar(randNum);
		UART0_putchar('\'');
		Timer0_Ini();
		//restartCount();
		
		while (!checkFlag(countFlag) && !((UCSR0A>>RXC0)&1));
			
			TCCR0B = 0;			
			
			if (countFlag)
				UART0_puts("Too slow!, you didn't press the key in time\n\r");
			
			else if (UDR0 == randNum){
				
				UART0_puts("Key pressed, time elapsed: ");
				
				aux = (miliCounter / 100);
				//if (aux != 0)
					UART0_putchar(aux + '0');
				
				miliCounter = miliCounter % 100;
				aux = (miliCounter / 10);
				UART0_putchar(aux + '0');
				
				miliCounter = miliCounter % 10; 
				aux = miliCounter;
				UART0_putchar(aux + '0');
				
				UART0_putchar('.');
				
				UART0_putchar(microCounter + '0');
				UART0_puts("ms");
				
			}
			else 
				UART0_puts("\r\n oops, wrong key");
	}
	//teclazo = UDR0;
	return 0;
}

void UART_Ini(){
	
	UCSR0A = 0;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (3<<UCSZ00);
	UBRR0 = 51;
	
}


void Timer0_Ini(){
	
	
	
	TCNT0  = 0;
	TIMSK0 = (1<<OCIE0A);
	TIFR0  = (1<<OCF0A);
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS01);
	OCR0A = 199;
	restartCount();
	sei();
		
}

ISR (TIMER0_COMPA_vect){
	
	microCounter++;
	counter++;
	if ( counter >= 5000)
		startFlag = 1;
	
	if (microCounter == 10){
		miliCounter++;
		microCounter = 0;		
	}
	
	if (counter == 10000)
		countFlag =1;
		
}

uint8_t checkFlag (uint8_t flag){
	
	if (flag){
		flag = 0;
		return 1;
	}
	return 0;	
}



char UART0_getchar(){
	//sei(); 
	while(!((UCSR0A>>RXC0)&1))
		;
	
	return UDR0;
}

void UART0_putchar(char dato){
	
	while(!((UCSR0A>>UDRE0)&1))
		;
	UDR0 = dato;
	
}

void UART0_puts(char *string){
	
	while (*string){
		UART0_putchar(*string++);
	}
	
}


void restartCount(){
	
	startFlag 	 = 0;
	countFlag 	 = 0;
	counter   	 = 0;
	microCounter = 0;
	miliCounter  = 0;
	UCSR0A &= ~(1>>RXC0);	
	
}
