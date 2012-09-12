/*******************************************************************************
* This is the main program to sanity test the MC40SE Controller using PIC16F887
* This sample code is compiled under MPLAB IDE v8.85 with Hi-Tech C Pro Compiler (Lite Mode) V9.83
* Author: Ober Choo Sui Hong @ Cytron Technologies Sdn. Bhd.
* Date: 31 Aug 2012
*******************************************************************************/
#include <htc.h>
#include "system.h"
#include "timer1.h"
#include "uart.h"
#include "adc.h"
#include "pwm.h"
#include "lcd.h"
#include "skps.h"

/*******************************************************************************
* DEVICE CONFIGURATION WORDS FOR PIC16F887                                     *
*******************************************************************************/

	#if defined (_16F887) // if this file is compile for PIC16F887
	__CONFIG(0x2CE4);	//1st configuration words
	__CONFIG(0x3FFF);	//2nd configuration words
	
	// for reference only
	/*__CONFIG( INTIO &		// Internal Clock, pins as I/O.
			 //HS &			// External Crystal at High Speed
			 WDTDIS &		// Disable Watchdog Timer.
			 PWRTEN &		// Enable Power Up Timer.
			 IESOEN &			// Enabled Internal External Clock Switch Over
			 //IESODIS &		// Disabled Internal External Clock Switch Over
			 BORDIS &		// Disable Brown Out Reset.
			 //FCMDIS	&		// Disable monitor clock fail safe
			 FCMEN &		// Enable monitor clock fail safe
			 MCLREN &		// MCLR function is enabled
			 LVPDIS);		// Disable Low Voltage Programming.
			 
	*/		 
	#elif defined (_16F877A) // if this file is compile for PIC16F877A		 
	__CONFIG(0x3F32);
	
	//for reference only
	/*__CONFIG(HS &			// High Speed Crystal.
			 WDTDIS &		// Disable Watchdog Timer.
			 PWRTEN &		// Enable Power Up Timer.
			 BORDIS &		// Disable Brown Out Reset.
			 LVPDIS);		// Disable Low Voltage Programming.
	*/
	#endif	//end of pre-processor if condition, choosing between PIC16F887 or PIC16F877A




/*******************************************************************************
* PRIVATE CONSTANT DEFINE                                                  *
*******************************************************************************/
#define	PORT1 	1
#define	PORT2	2
#define	RUN		0
#define	BRAKE	1
#define	CW		0
#define	CCW		1


/*******************************************************************************
* PRIVATE FUNCTION PROTOTYPES                                                  *
*******************************************************************************/

void delay_ms(unsigned int ui_value);
void beep(unsigned char uc_count);
void mc40se_init(void);
void test_switch(void);
void test_led(void);
void test_di(void);
void test_adc(void);
void test_brushless(void);
void test_brush(void);
void test_encoder(void);
void test_relay(void);
void test_ex_md(void);
void test_uart(void);
void test_skps(void);


void brushless(unsigned char uc_port_number, unsigned char uc_motor_status, unsigned char uc_motor_dir, unsigned int ui_speed);
void brush(unsigned char uc_port_number, unsigned char uc_motor_status, unsigned char uc_motor_dir, unsigned int ui_speed);
void relay_on(unsigned char uc_relay_number);
void relay_off(unsigned char uc_relay_number);
void relay_off_all(void);


/*******************************************************************************
* Global Variables                                                             *
*******************************************************************************/
char string_SWsError[] = "Other \nSWs Low";
char string_passed[] = "Passed!";


/*******************************************************************************
* MAIN FUNCTION                                                                *
*******************************************************************************/
int main(void)
{
	unsigned char test_number = 1;
	
	// Initialize PIC16F887 to correct Input/Output based on MC40SE on board interface
	mc40se_init();	
	
	// Initialize ADC.
	adc_init();
	
	// off all relays
	relay_off_all();
	
	// Initialize UART.
	uart_init();
		
	// Initialize PWM.
	pwm_init();

	// Initialize PWM.
	timer1_init();
	
	// Initialize the LCD.
	lcd_init();		
	
	// Initialize bruhsless motor port
	brushless(PORT1, BRAKE, CW, 0);
	brushless(PORT2, BRAKE, CCW, 0);
			
	// Display the messages and beep twice.
	lcd_clear_msg(" Cytron \n  Tech");
	beep(2);
	delay_ms(1000);
	
		
	// Need to make sure the push button is useable before perform other test.
	test_switch();
		
	while (1) 
		{
		lcd_2ndline();
		lcd_putstr("1+,2=Run");
	
		// Return cursor to the home position.
		lcd_home();
		
		switch (test_number) 
			{
			case 1:
				lcd_putstr("1:All   ");
				if (SW2 == 0) 			// if SW2 is press
				{	while (SW2 == 0);	// waiting for SW2 to be let go
					test_led();			// test LED or Buzzer
					test_di();			// test digital input
					test_adc();			// test analog input
					test_brushless();	// test brushless motor (Vexta or Linix)
					test_brush();		// test brush motor (MD10X or MD30X)
					test_encoder();		// test external encoder (rotary incremental type)
					test_relay();		// test relay for brush motor
					test_ex_md();		// test external MD, which should be connected at MD3 or MD4 ports
					test_uart();		// tesr UART connection to computer is needed, through UC00A					
					test_skps();		// test SKPS, SKPS, Wired PS2 or wireless PS2 is needed
				}	
				break;
				
			case 2:
				lcd_putstr("2:LED+BZ");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_led();
				}	
				break;
				
			case 3:
				lcd_putstr("3:Dig In");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_di();
				}	
				break;
				
			case 4:
				lcd_putstr("4:ADC   ");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_adc();
				}	
				break;
				
			case 5:
				lcd_putstr("5:B-less");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_brushless();
				}	
				break;
			
			case 6:
				lcd_putstr("6:BrushM");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_brush();
				}	
				break;
				
			case 7:
				lcd_putstr("7:Relays");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_relay();
				}	
				break;
			
			case 8:
				lcd_putstr("8:Ex_MD ");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_ex_md();
				}	
				break;
					
			case 9:
				lcd_putstr("9:ENC   ");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_encoder();
				}	
				break;
			
			case 10:		
				lcd_putstr("10:UART ");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_uart();
				}	
				break;
				
			case 11:				
				lcd_putstr("11:SKPS ");
				if (SW2 == 0) 
				{
					while (SW2 == 0);
					test_skps();
				}	
				break;	
			
		}//switch (test_number) 		
		
		// If SW1 is pressed...
		if (SW1 == 0) 
		{
			if (++test_number > 11) 
			{
				test_number = 1;
			}				
			while (SW1 == 0);
			beep(1);
		}		
	
	} // while (1)
	
}



/*******************************************************************************
* PRIVATE FUNCTION: delay_ms
*
* PARAMETERS:
* ~ ui_value	- The period for the delay in miliseconds.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Delay in miliseconds.
*
*******************************************************************************/
void delay_ms(unsigned int ui_value)
{
	while (ui_value-- > 0) {
		__delay_ms(1);
	}	
}


/*******************************************************************************
* PRIVATE FUNCTION: beep
*
* PARAMETERS:
* ~ uc_count	- How many times we want to beep.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Beep for the specified number of times.
*
*******************************************************************************/
void beep(unsigned char uc_count)
{
	while (uc_count-- > 0) {
		BUZZER = 1;
		delay_ms(50);
		BUZZER = 0;
		delay_ms(50);
	}
}
/*******************************************************************************
* PRIVATE FUNCTION: mc40se_init
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Initialize the PIC16F887 for MC40SE on board device.
*
*******************************************************************************/
void mc40se_init(void)
{
	#if defined(_16F887)	// PIC16F887 internal clock, 16F877A don't have
	// Initialize the Internal Osc, under OSCCON register
	IRCF2 = 1;		// IRCF<2:0> = 111 => 8MHz
	IRCF1 = 1;		// IRCF<2:0> = 110 => 4MHz, default
	IRCF0 = 1;		// IRCF<2:0> = 101 => 2MHz, 
	#endif
		
	// clear port value
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;
	
	// Initialize the I/O port direction.
	TRISA = 0b10111111;
	TRISB = 0b00111111;
	TRISC = 0b10000001;
	TRISD = 0;
	TRISE = 0b00000000;	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_switch
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the push buttons.
*
*******************************************************************************/
void test_switch(void)
{
	// Display the messages.
	lcd_clear_msg("Testing\nSwitches");
	delay_ms(1000);	
	
	// Display the messages.
	lcd_clear_msg("Press\nSW1");
	
	// Waiting for user to press SW1.
	while (SW1 == 1);
	
	// If SW1 is pressed but other switches also become low, trap the error.
	if (SW2 == 0) {
		// Display the error messages and trap the error.
		lcd_clear_msg(string_SWsError);
		beep(10);
		while (1);
	}	
	
	// Waiting for user to release SW1.
	while (SW1 == 0);	
	beep(1);
	// Display the messages.
	lcd_clear_msg("Press\nSW2");
	
	// Waiting for user to press SW2.
	while (SW2 == 1);
	
	// If SW2 is pressed but other switches also become low, trap the error.
	if (SW1 == 0 ) 
	{
		// Display the error messages and trap the error.
		lcd_clear_msg(string_SWsError);
		beep(10);
		while (1);
	}	
	
	// Waiting for user to release SW2.
	while (SW2 == 0);	
		
	// Display the messages.
	lcd_clear_msg(string_passed);
	beep(2);
	delay_ms(500);
}	

/*******************************************************************************
* PRIVATE FUNCTION: test_led
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the LEDs.
*
*******************************************************************************/
void test_led(void)
{	
	unsigned char i = 0;
	lcd_clear_msg("Testing\nLED");
	delay_ms(1000);	
	
	// Waiting for user to press SW1.
	while (SW1 == 1) {
	lcd_clear_msg("Connect\nBuzzer");	//Buzzer and LED share same output pin	
	for (i = 0; i < 200; i++) {
		if (SW1 == 0) {
			break;
		}	
		delay_ms(10);
	}
	
	lcd_clear_msg("SW1\nto test");	
	for (i = 0; i < 200; i++) {
		if (SW1 == 0) {
			break;
		}	
		delay_ms(10);
	}
	}//while (SW1 == 1)
	
	// Waiting for user to release SW1.
	while (SW1 == 0);

	// Testing LED on MC40SE, which share with buzzer.
	lcd_clear_msg("LED1+Buz\n  time");
	for(i=0; i<10; i++)
	{
	lcd_goto(0x40);
	lcd_putchar(i+0x30);	
	LED1 = ~LED1; //toggle LED
	delay_ms(500);	
	}
	delay_ms(500);
	lcd_clear_msg(string_passed);
	beep(2);		// done
	delay_ms(500);
}
/*******************************************************************************
* PRIVATE FUNCTION: test_di
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the digital input.
*
*******************************************************************************/
void test_di(void)
{
	unsigned char i = 0;
	lcd_clear_msg("Testing\nSEN1-8");
	delay_ms(1000);	
	
	// Waiting for user to press SW1.
	while (SW1 == 1) {
	lcd_clear_msg("Connect\nDi Sen");	//Connect digital sensor	
	for (i = 0; i < 200; i++) {
		if (SW1 == 0) {
			break;
		}	
		delay_ms(10);
	}
	
	lcd_clear_msg("SW1\nto test");	
	for (i = 0; i < 200; i++) {
		if (SW1 == 0) {
			break;
		}	
		delay_ms(10);
	}
	}//while (SW1 == 1)
	
	// Waiting for user to release SW1.
	while (SW1 == 0);

	// Testing LED on MC40SE, which share with buzzer.
	lcd_clear_msg("SW2 exit");
	while(SW2 == 1)
	{
		lcd_goto(0x40);		//2nd line, 1st cursor
		if(SEN1 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');
		lcd_goto(0x41);		//2nd line, 1st cursor
		if(SEN2 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');
		lcd_goto(0x42);		//2nd line, 1st cursor
		if(SEN3 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');
		lcd_goto(0x43);		//2nd line, 1st cursor
		if(SEN4 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');
		lcd_goto(0x44);		//2nd line, 1st cursor
		if(SEN5 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');
		lcd_goto(0x45);		//2nd line, 1st cursor
		if(SEN6 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');
		lcd_goto(0x46);		//2nd line, 1st cursor
		if(SEN7 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');
		lcd_goto(0x47);		//2nd line, 1st cursor
		if(SEN8 == 0)	lcd_putchar('X');
		else lcd_putchar(' ');		
	}
	
	// Waiting for user to release SW2.
	while (SW2 == 0);

	delay_ms(500);
	lcd_clear_msg(string_passed);
	beep(2);		// done
	delay_ms(500);
}	
/*******************************************************************************
* PRIVATE FUNCTION: test_adc
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the ADC.
*
*******************************************************************************/
void test_adc(void)
{
	
	unsigned char uc_d1 = 0, uc_d3 = 0, uc_d4 = 0;	
	unsigned int ui_adc = 0, uc_d2 = 0;
	unsigned char i = 0;
	// Display the messages.
	lcd_clear_msg("Testing\nADC");
	delay_ms(1000);	
	
	lcd_clear_msg("ADC:\nSW1 exit");	
	
	ADON= 1; 	//Activate ADC module
	while (SW1 == 1)	// Loop until SW1 is pressed.
	 {	
		ui_adc = 0;	
		for(i = 0; i < 10; i++)		// read 10 times from ADC module and take the average value
		{
		ui_adc = ui_adc + ui_adc_read();	// read adc value from channel 0
		}
		ui_adc = ui_adc/10;		
		
		lcd_goto(0x04);	//goto character after ADC:
		lcd_bcd(4, ui_adc);	
	}//while (SW1 == 1)	
	
	// Waiting for user to release SW1.
	while (SW1 == 0);
	
	ADON = 0;	// Deactivate ADC module
	
	lcd_clear_msg(string_passed);
	beep(2);		// done
	delay_ms(500);
}	
/*******************************************************************************
* PRIVATE FUNCTION: test_brushless
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the control for brushless motor.
*
*******************************************************************************/
void test_brushless(void)
{
	unsigned int ui_counter = 0;
	unsigned int ui_speed = 0;	
	unsigned char i = 0;
	// Display the messages.
	lcd_clear_msg("Testing\nBrushles");
	delay_ms(1000);
		
	// Waiting for user to press SW1.
	while (SW1 == 1) {
		lcd_clear_msg("Connect \nB-less M");
		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
		
		lcd_clear_msg("Jumper\nRC0=ENC1");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
				
		lcd_clear_msg("SW1\nto test");
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}//while (SW1 == 1)
	
	// Waiting for user to release SW1.
	while (SW1 == 0);
	
	// Accelerate clockwise.
	lcd_clear_msg("CW:\nEC:");
	set_encoder(0);							// clear counter
	ui_speed = 20;
	brushless(PORT1, RUN, CW, ui_speed);		// controlling Brushless motor at Brushless 1	
	
	for (; ui_speed < 1000; ui_speed+=10 ) {
		set_pwm1(ui_speed);
		delay_ms(5);		
		lcd_goto(0x03);
		lcd_bcd(4, ui_speed);
		lcd_goto(0x43);	
		lcd_bcd(5, ui_encoder());
	}
	
	// De-accelerate counter clockwise.
	lcd_clear_msg("CCW:\nEC:");
	set_encoder(0);							// clear counter
	ui_speed = 1000;
	brushless(PORT1, RUN, CCW, ui_speed);		// controlling Brushless motor at Brushless 1
	
	// Deaccelerate counter clockwise.
	for (; ui_speed > 50; ui_speed-=10) {
		set_pwm1(ui_speed);
		delay_ms(5);
		lcd_goto(0x04);
		lcd_bcd(4, ui_speed);
		lcd_goto(0x43);	
		lcd_bcd(5, ui_encoder());		
	}	
	delay_ms(100);	

	// Stop motor at brushless port 1
	brushless(PORT1, BRAKE, CCW, 0);
	delay_ms(1000);
	
	// Waiting for user to press SW1.
	while (SW1 == 1) {
		lcd_clear_msg("Jumper\nRC0=ENC2");
		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}		
		lcd_clear_msg("SW1\nto test");
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}//while (SW1 == 1)
	
	// Waiting for user to release SW1.
	while (SW1 == 0);
	
	// Accelerate clockwise.
	lcd_clear_msg("CW:\nEC:");
	set_encoder(0);							// clear counter
	ui_speed = 20;
	brushless(PORT2, RUN, CW, ui_speed);		// controlling Brushless motor at Brushless 2	
	
	for (; ui_speed < 1000; ui_speed+=10 ) {
		set_pwm2(ui_speed);
		delay_ms(5);		
		lcd_goto(0x03);
		lcd_bcd(4, ui_speed);
		lcd_goto(0x43);	
		lcd_bcd(5, ui_encoder());
	}
	
	// De-accelerate counter clockwise.
	lcd_clear_msg("CCW:\nEC:");
	set_encoder(0);							// clear counter
	ui_speed = 1000;
	brushless(PORT2, RUN, CCW, ui_speed);		// controlling Brushless motor at Brushless 2
	
	// Deaccelerate counter clockwise.
	for (; ui_speed > 50; ui_speed-=10) {
		set_pwm2(ui_speed);
		delay_ms(5);
		lcd_goto(0x04);
		lcd_bcd(4, ui_speed);
		lcd_goto(0x43);	
		lcd_bcd(5, ui_encoder());		
	}	
	delay_ms(100);	

	// Stop motor.
	brushless(PORT2, BRAKE, CCW, 0);
	set_encoder(0);	
	delay_ms(1000);
	
	lcd_clear_msg(string_passed);	
	beep(2);	
}	

/*******************************************************************************
* PRIVATE FUNCTION: test_brush
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the brush motor with speed.
*
*******************************************************************************/
void test_brush(void)
{
	unsigned char i;
	unsigned int ui_speed;
	
	// Display the messages.
	lcd_clear_msg("Test\nMDXXX");
	delay_ms(1000);
	
	// Waiting for user to press SW1.
	while (SW1 == 1) 
		{
		lcd_clear_msg("Connect\nMD10/30");		
		for (i = 0; i < 200; i++) 
			{
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
		
		lcd_clear_msg("SW1\nto test");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}	
	
	// Waiting for user to release SW1.
	while (SW1 == 0);	
	
	// Brush motor at Brush 1 accelerate clockwise.
	lcd_clear_msg("BRUSH 1\nCW:");	
		
	ui_speed = 10;
	brush(PORT1, RUN, CW, ui_speed);	//controlling brush motor at BRUSH1, require MD10X or MD30X
	
	for (; ui_speed < 1000; ui_speed+=5 ) 
	{
		set_pwm1(ui_speed);				
		delay_ms(5);
		lcd_goto(0x43);
		lcd_bcd(4, ui_speed);		
	}
	
	// De-accelerate counter clockwise.
	lcd_clear_msg("BRUSH 1\nCCW:");		
	brush(PORT1, RUN, CCW, ui_speed);	//controlling brush motor at BRUSH1, require MD10X or MD30X
	
	// Deaccelerate counter clockwise.
	for (; ui_speed > 10; ui_speed-=5) 
	{
		set_pwm1(ui_speed);				
		delay_ms(5);
		lcd_goto(0x44);
		lcd_bcd(4, ui_speed);			
	}	
	delay_ms(100);	

	// Stop motor.
	brush(PORT1, BRAKE, CCW, 0);	//controlling brush motor at BRUSH1, require MD10X or MD30X
	delay_ms(1000);
	
	
	// Brush motor at Brush 2 accelerate clockwise.
	lcd_clear_msg("BRUSH 2\nCW:");	
		
	ui_speed = 10;
	brush(PORT2, RUN, CW, ui_speed);	//controlling brush motor at BRUSH2, require MD10X or MD30X
	
	for (; ui_speed < 1000; ui_speed+=5 ) 
	{
		set_pwm2(ui_speed);				
		delay_ms(5);
		lcd_goto(0x43);
		lcd_bcd(4, ui_speed);		
	}
	
	// De-accelerate counter clockwise.
	lcd_clear_msg("BRUSH 2\nCCW:");		
	brush(PORT2, RUN, CCW, ui_speed);	//controlling brush motor at BRUSH2, require MD10X or MD30X
	
	// Deaccelerate counter clockwise.
	for (; ui_speed > 10; ui_speed-=5) 
	{
		set_pwm2(ui_speed);				
		delay_ms(5);
		lcd_goto(0x44);
		lcd_bcd(4, ui_speed);			
	}	
	delay_ms(100);	

	// Stop motor.
	brush(PORT2, BRAKE, CCW, 0);	//controlling brush motor at BRUSH1, require MD10X or MD30X
	delay_ms(1000);	
	
	lcd_clear_msg(string_passed);	
	beep(2);	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_encoder
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the external encoder.
*
*******************************************************************************/
void test_encoder(void)
{
	unsigned char i = 0;
	// Display the messages.
	lcd_clear_msg("Test\nE_Enc");
	delay_ms(1000);
		
	// Waiting for user to press SW1.
	while (SW1 == 1) {
		lcd_clear_msg("Connect\nExt_ENC");
		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
		
		lcd_clear_msg("Jumper\nRC0=E_EN");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
				
		lcd_clear_msg("SW1\nto test");
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}//while (SW1 == 1)
	
	// Waiting for user to release SW1.
	while (SW1 == 0);
	
	lcd_clear_msg("500 exit\nEnc:");
	set_encoder(0);							// clear counter

	while (ui_encoder()<=500 ) 				// 500 count will complete the test
	{	
		lcd_goto(0x44);	
		lcd_bcd(4, ui_encoder());
	}
	delay_ms(500);	
	set_encoder(0);	
	
	lcd_clear_msg(string_passed);	
	beep(2);	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_relay
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the relay.
*
*******************************************************************************/
void test_relay(void)
{
	unsigned char i = 0;
	// Display the messages.
	lcd_clear_msg("Test\nRelays");
	delay_ms(1000);
		
	// Waiting for user to press SW1.
	while (SW1 == 1) {
		lcd_clear_msg("Connect\nMTR_Batt");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
		
		lcd_clear_msg("Connect\nMotor1&2");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
				
		lcd_clear_msg("SW1\nto test");
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}//while (SW1 == 1)
	
	// Waiting for user to release SW1.
	while (SW1 == 0);
	
	lcd_clear_msg("SW2 exit");
	relay_off_all();	// off all relay
	lcd_goto(0x40);	//LCD 2nd line
	lcd_putstr("Relay ");
	i = 1;
	while (SW2 == 1) 				// SW2 to exit
	{			
		lcd_goto(0x46);		
		lcd_putchar(i+0x30);
		relay_on(i);
		if(SW2 == 0) break;
		delay_ms(2000);
		relay_off(i);
		i++;
		if(i>=5) i = 1;		
	}
	delay_ms(1000);	
	relay_off_all();	// off all relay	
	
	lcd_clear_msg(string_passed);	
	beep(2);	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_ex_md
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test external MD that connect to MD3 and MD4 port.
* It can be MD10B, MD10C, MD30A, MD30B
*******************************************************************************/
void test_ex_md(void)
{
	unsigned char i = 0;
	// Display the messages.
	lcd_clear_msg("Test\nEx_MD");
	delay_ms(1000);
		
	// Waiting for user to press SW1.
	while (SW1 == 1) {
		lcd_clear_msg("Connect\nMD3&4");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
					
		lcd_clear_msg("SW1\nto test");
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}//while (SW1 == 1)
	
	// Waiting for user to release SW1.
	while (SW1 == 0);
	
	lcd_clear_msg("SW2 exit");
	relay_off_all();	// off all relay, MD3 and MD4 share the lower pin of PORTD (same port for relays)
	
	while (SW2 == 1) 				// SW2 to exit
	{			
		relay_off_all();
		lcd_2ndline();		
		lcd_putstr("MD3=CW ");
		relay_on(5);
		relay_off(6);
		delay_ms(2000);
		if(SW2 == 0) break;
		
		lcd_2ndline();		
		lcd_putstr("MD3=CCW");
		relay_on(6);
		relay_off(5);
		delay_ms(2000);
		if(SW2 == 0) break;
		
		relay_off_all();
		lcd_2ndline();		
		lcd_putstr("MD4=CW ");
		relay_on(7);
		relay_off(8);
		delay_ms(2000);
		if(SW2 == 0) break;
		
		lcd_2ndline();		
		lcd_putstr("MD4=CCW");
		relay_on(8);
		relay_off(7);
		delay_ms(2000);			
	}
	while(SW2 == 0); 	//wait for SW2 to release
	
	relay_off_all();	// off all relay	
	
	lcd_clear_msg(string_passed);	
	beep(2);	
}

/*******************************************************************************
* PRIVATE FUNCTION: test_uart
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the UART.
*
*******************************************************************************/
void test_uart(void)
{	
	unsigned char i;
	char c_received_data;
		
	// Display the messages.
	lcd_clear_msg("Test\nUART");
	delay_ms(1000);
		
	// Waiting for user to press SW1.
	while (SW1 == 1) {
		lcd_clear_msg("Connect\nUC00A");
		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
		
		lcd_clear_msg("SW1\nto test");
		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}	
	
	// Waiting for user to release SW1.
	while (SW1 == 0);	
	
	// Display the messages.
	lcd_clear_msg("Enter to\nexit");
		
	// Sending message to the PC.
	uart_putstr("\r\n\nMC40SE testing UART...\r\n");
	uart_putstr("Press any key to test.\r\n");
	uart_putstr("Press enter to exit.\r\n\n");
	
	do {
		c_received_data = uc_uart_rx();
		uart_tx(c_received_data);
	}
	while (c_received_data != '\r' && c_received_data != '\n');	
	
	// Sending message to the PC.
	uart_putstr("\r\n\nTest Completed.\r\n");
	
	lcd_clear_msg(string_passed);
	beep(2);		// done
	delay_ms(500);
}	


/*******************************************************************************
* PRIVATE FUNCTION: test_skps
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Test the SKPS.
*
*******************************************************************************/

void test_skps(void)
{
	unsigned char uc_skps_ru = 0 , uc_skps_lu = 0;
	unsigned char i = 0;
	// Display the messages.
	lcd_clear_msg("Test\nSKPS");
	delay_ms(1000);
	
	// Waiting for user to press SW1.
	while (SW1 == 1) {
		lcd_clear_msg("Connect\nSKPS");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
		
		lcd_clear_msg("Connect\nPS2");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
		
		lcd_clear_msg("SW1\nto test");		
		for (i = 0; i < 200; i++) {
			if (SW1 == 0) {
				break;
			}	
			delay_ms(10);
		}
	}	
	
	// Waiting for user to release SW1.
	while (SW1 == 0);
	
	// Display the messages.
	skps_reset();	//reset the SKPS
	delay_ms(500);
	lcd_clear_msg("SW2 exit");
	delay_ms(500);
	lcd_2ndline();
	
		
	// While SW1 is not press, keep reading input from SKPS
	while (SW2 == 1) {	
		if(!(uc_skps(p_l1) && uc_skps(p_l2) && uc_skps(p_r1) && uc_skps(p_r2)))
		{
			lcd_2ndline();
			lcd_putstr("Buz On  ");
			BUZZER = 1;
		}	
		else 
		{
			BUZZER = 0;
			lcd_2ndline();
			lcd_putstr("       ");
		}

		uc_skps_ru = uc_skps(p_joy_ru);		// read the value of right joystik, up axis
		uc_skps_lu = uc_skps(p_joy_lu);		// read the value of left joystick, up axis
		
		if (uc_skps_ru >5)
		{
		skps_vibrate (p_motor1, 1);		// send command to vibrate right motor
		lcd_2ndline();
		lcd_putstr("Right M ");
		}
		else 
		{	
		skps_vibrate(p_motor1, 0);	// stop motor
		lcd_2ndline();
		lcd_putstr("        ");
		}
		
		if (uc_skps_lu >10)
		{
		skps_vibrate (p_motor2, uc_skps_lu*2);	// send command to vibrate left motor
		lcd_2ndline();
		lcd_putstr("Left M  ");	
		}
		else 
		{
		skps_vibrate(p_motor2, 0);	// stop motor
		lcd_2ndline();
		lcd_putstr("        ");
		}
	}	
	
	//wait SW1 to be released
	while (SW2 == 0);	
		
	// Display the messages.
	lcd_clear_msg(string_passed);
	beep(2);
	delay_ms(500);
}
/*******************************************************************************
* PRIVATE FUNCTION: brushless
*
* PARAMETERS:
* ~ unsigned char uc_port_number - refer to brushless port in MC40SE, PORT1 or PORT2
* ~ unsigned char uc_motor_status - refer to either RUN or BRAKE brushless motor
* ~ unsigned char uc_motor_dir - to control the direction of brushless motor, CW or CCW
* ~ unsigned int ui_speed - speed for brushless motor, from 0 to 1024
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* control brushless motor, suitable for Vexta or LINIX brushless motor connect to MC40SE
*
*******************************************************************************/
void brushless(unsigned char uc_port_number, unsigned char uc_motor_status, unsigned char uc_motor_dir, unsigned int ui_speed)
{
	if(uc_port_number == PORT1)
	{
		if(uc_motor_status == RUN)
		{		
			RUN1 = 0;	// active low		
		}
		else if(uc_motor_status == BRAKE)
		{
			RUN1 = 1;	// activate low				
		}
		
		if(uc_motor_dir == CW)
		{
			DIR1 = 1;
		}
		else if (uc_motor_dir == CCW)
		{
			DIR1 = 0;
		}
		set_pwm1(ui_speed);
	}
	else if(uc_port_number == PORT2)
	{
		if(uc_motor_status == RUN)
		{		
			RUN2 = 0;	// active low		
		}
		else if(uc_motor_status == BRAKE)
		{
			RUN2 = 1;	// activate low				
		}
		
		if(uc_motor_dir == CW)
		{
			DIR2 = 1;
		}
		else if (uc_motor_dir == CCW)
		{
			DIR2 = 0;
		}
		set_pwm2(ui_speed);
	}
}	

/*******************************************************************************
* PRIVATE FUNCTION: brush
*
* PARAMETERS:
* ~ unsigned char uc_port_number - refer to brush port in MC40SE, PORT1 or PORT2
* ~ unsigned char uc_motor_status - refer to either RUN or BRAKE brush motor
* ~ unsigned char uc_motor_dir - to control the direction of brush motor, CW or CCW
* ~ unsigned int ui_speed - speed for brush motor, from 0 to 1024
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* control brush motor, require MD10X or MD30X as motor driver
*
*******************************************************************************/
void brush(unsigned char uc_port_number, unsigned char uc_motor_status, unsigned char uc_motor_dir, unsigned int ui_speed)
{
	if(uc_port_number == PORT1)
	{
		if(uc_motor_status == RUN)
		{
			if(uc_motor_dir == CW)
			{
				RUN1 = 0;
				DIR1 = 1;
			}
			else if (uc_motor_dir == CCW)
			{
				RUN1 = 1;
				DIR1 = 0;
			}			
		}
		else if(uc_motor_status == BRAKE)
		{
			RUN1 = 0;	// brake to gnd
			DIR1 = 0;	
		}
		set_pwm1(ui_speed);
	}
	else if(uc_port_number == PORT2)
	{
		if(uc_motor_status == RUN)
		{
			if(uc_motor_dir == CW)
			{
				RUN2 = 0;
				DIR2 = 1;
			}
			else if (uc_motor_dir == CCW)
			{
				RUN2 = 1;
				DIR2 = 0;
			}			
		}
		else if(uc_motor_status == BRAKE)
		{
			RUN2 = 0;	// brake to gnd
			DIR2 = 0;	
		}
		set_pwm2(ui_speed);
	}	
}	
/*******************************************************************************
* PRIVATE FUNCTION: relay_on
*
* PARAMETERS:
* ~ unsigned char uc_relay_number - relay that needed to be activate, from 1-8
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* activate relay.
*
*******************************************************************************/
void relay_on(unsigned char uc_relay_number)
{
	LATCH = 0;	// hold the state of latch
	PORTD = PORTD | (0b00000001<< (uc_relay_number - 1));
	LATCH = 1;	// transfer the new output to relay
	delay_ms(1);
	LATCH = 0;	// hold the output of latch
}	
/*******************************************************************************
* PRIVATE FUNCTION: relay_off
*
* PARAMETERS:
* ~ unsigned char uc_relay_number - relay that needed to be deactivate, from 1-8
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Deactivate relay
*
*******************************************************************************/
void relay_off(unsigned char uc_relay_number)
{
	LATCH = 0;	// hold the state of latch
	PORTD = PORTD & (~(0b00000001<< (uc_relay_number -1)));
	LATCH = 1;	// transfer the new output to relay
	delay_ms(1);
	LATCH = 0;	// hold the output of latch
}	
/*******************************************************************************
* PRIVATE FUNCTION: relay_off_all
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* deactivate all relays.
*
*******************************************************************************/
void relay_off_all(void)
{
	LATCH = 0;	// hold the output of latch
	PORTD = 0; 	// Off all relays
	LATCH = 1;	// enable latch to read input and transfer to output
	delay_ms(5);
	LATCH = 0;	// hold the output of latch
}
