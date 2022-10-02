#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define max_digit_1 10
#define max_digit_2 6
#define display( bin_number )\
{\
	PORTA = (PORTA &0xC0) | (1<<bin_number);\
	switch (bin_number)\
	{\
	case 0:\
		PORTC = (PORTC & 0xF0) | digit_1_of_seconds;\
		break;\
	case 1:\
		PORTC = (PORTC & 0xF0) | digit_2_of_seconds;\
		break;\
	case 2:\
		PORTC = (PORTC & 0xF0) | digit_1_of_minutes;\
		break;\
	case 3:\
		PORTC = (PORTC & 0xF0) | digit_2_of_minutes;\
		break;\
	case 4:\
		PORTC = (PORTC & 0xF0) | digit_1_of_hours;\
		break;\
	case 5:\
		PORTC = (PORTC & 0xF0) | digit_2_of_hours;\
		break;\
	}\
}
unsigned char digit_1_of_seconds =0,digit_2_of_seconds =0,digit_1_of_minutes =0;
unsigned char digit_2_of_minutes =0,digit_1_of_hours =0,digit_2_of_hours =0;
ISR(INT1_vect)
{
	TCCR1B &= ~(1<<CS10) & ~(1<<CS12); // stop the timer by stopping the clock
}
ISR(INT2_vect)
{
	TCCR1B |= (1<<CS10) | (1<<CS12); // stop the timer by stopping the clock
}
ISR(INT0_vect)
{
	 digit_1_of_seconds =0;
	 digit_2_of_seconds =0;
	 digit_1_of_minutes =0;
	 digit_2_of_minutes =0;
	 digit_1_of_hours =0;
	 digit_2_of_hours =0;
}

ISR(TIMER1_COMPA_vect)
{
	digit_1_of_seconds++;
	if(digit_1_of_seconds==max_digit_1)
	{
		digit_1_of_seconds=0;
		digit_2_of_seconds++;
	}
	if(digit_2_of_seconds==max_digit_2)
	{
		digit_2_of_seconds=0;
		digit_1_of_minutes++;
	}
	if(digit_1_of_minutes==max_digit_1)
	{
		digit_1_of_minutes=0;
		digit_2_of_minutes++;
	}
	if(digit_2_of_minutes==max_digit_2)
	{
		digit_2_of_minutes=0;
		digit_1_of_hours++;
	}
	if(digit_1_of_hours==max_digit_1)
	{
		digit_1_of_hours=0;
		digit_2_of_hours++;
	}
	if(digit_2_of_hours==max_digit_1)
		{
			digit_2_of_hours=0;

		}

}
void INT0_Init(void)
{
	DDRD  &= (~(1<<PD2));               // Configure INT0/PD2 as input pin
	PORTD |=(1<<PD2);                   // activate pull up resistor
	MCUCR |=  (1<<ISC01);               // Trigger INT0 with the failing edge
	GICR  |= (1<<INT0);                 // Enable external interrupt pin INT0
}
void INT1_Init(void)
{
	DDRD  &= (~(1<<PD3));               // Configure INT1/PD3 as input pin
	MCUCR |=  (1<<ISC11)|(1<<ISC10);       // Trigger INT1 with the raising edge
	GICR  |= (1<<INT1);                 // Enable external interrupt pin INT1
}
void INT2_Init(void)
{
	DDRB  &= (~(1<<PB2));               // Configure INT2/PB2 as input pin
	PORTB |=(1<<PB2);                   // activate pull up resistor
	MCUCSR &= ~ (1<<ISC2);              // Trigger INT2 with the failing edge
	GICR  |= (1<<INT2);                 // Enable external interrupt pin INT2
}
void Timer1_Init_CTC_Mode(void)
{
	TCNT1=0;  // Set Timer Initial Value 0

	/*    Configure timer control register
	 * 1. Non PWM mode FOC1A=1
	 * 2. CTC Mode WGM10=0 , WGM11=0 , WGM12=1 & WGM13=0
	 * 3.1 For System Clock=1Mhz and timer prescaler is F_CPU/1024.
	 * 3.2 Timer frequency will be around 1Khz, Ttimer = 1ms
	 * 3.3 we need time to count to 1000 to get 1 second
	 */
	TCCR1A |= (1<<FOC1A);
	TCCR1B |=(1<<WGM12) | (1<< CS10) | (1<<CS12);
	OCR1A = 1000;
	TIMSK |= (1<<OCIE1A); // Enable Timer1A Compare Interrupt
}



int main(void)
{
	unsigned char i;

	DDRC |= 0x0F;
	PORTC &= 0xF0;
	DDRA |= 0x3F;
	PORTA &=~(0x3F);
	Timer1_Init_CTC_Mode();
	INT0_Init();
	INT1_Init();
	INT2_Init();
	SREG |=(1<<7);

	while(1){
		for( i=0;i<6;i++)
		{
			display(i);
			_delay_ms(3);
		}
	}
}
