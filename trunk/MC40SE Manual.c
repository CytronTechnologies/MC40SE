/*******************************************************************************
* This is the main program for MC40SE Advance Mobile Robot Controller 
* using PIC16F887 or PIC16F877A
* Compiled with MPLAB IDE v8.63 with HiTECH C Compiler (Lite Mode) V9.80
* Manual Robot sample, please refer to "Remote Controlled Mobile Robot" article 
* at http://tutorial.cytron.com.my for detail description.
* SKPS must be in 9600 baudrate.
* Revision: 1.1
* Date: 19 Sep 2011
* Author: Ober Choo Sui Hong @ Cytron Technologies Sdn. Bhd.
*******************************************************************************/
#include <htc.h>
#include "system.h"		// header of hardware pin, Crystal speed, baud rate
#include "timer1.h"		// header file for timer1, use as counter for encoder
#include "uart.h"		// header file for UART, serial communication
#include "adc.h"		// header file for ADC
#include "pwm.h"		// header file for PWM, speed control
#include "lcd.h"		// header file for LCD
#include "skps.h"		// header file for SKPS

/*******************************************************************************
* DEVICE CONFIGURATION WORDS FOR PIC16F887                                     *
*******************************************************************************/
#if defined (_16F887)	// if the PIC selected is PIC16F887
__CONFIG( INTIO &		// Internal Clock, pins as I/O.
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

#elif defined (_16F877A)// if PIC16F877A is selected, 
//ensure JP27 (BL_R) and JP48 (SK_R) is not connected
__CONFIG(HS &			// External Crystal at High Speed
		 WDTDIS &		// Disable Watchdog Timer
		 PWRTEN &		// Enable Power Up Timer		 
		 BORDIS &		// Disable Brown Out Reset		 
		 LVPDIS);		// Disable Low Voltage Programming
#endif	

/*******************************************************************************
* PRIVATE CONSTANT DEFINE                                                  *
*******************************************************************************/
#define	PORT1 	1
#define	PORT2	2
#define	RUN		0
#define	BRAKE	1
#define	CW		0
#define	CCW		1		
#define SPEED	300		// initial constant speed as 300

/* Gear type, this is due to different gear type, the rotation will be different
* this will further affect the locomotion, please choose the correct gear type
*
* type 0 is applied for:
*Linix Brushless Motor:(XXW - YY ---> XX watt for motor power, YY for gear ratio)
*	a. 30W - 10 
*Vexta Brushless Motor:(XXW - YY ---> XX watt for motor power, YY for gear ratio)
*	a. 15W - 20, 30, 200
*	b. 30W - 30, 50, 100
*	c. 50W - 30, 50, 100
*
*type 1 is applied for:
*Linix Brushless Motor:(XXW - YY ---> XX watt for motor power, YY for gear ratio)
*	a. 10W - 5, 15
*	b. 30W - 20
*Vexta Brushless Motor:(XXW - YY ---> XX watt for motor power, YY for gear ratio)
*	a. 15W - 5, 10, 15, 50, 100
*	b. 30W - 5, 10, 15, 20, 200
*	c. 50W - 5, 10, 15, 20, 200
*/
#define GEAR			1				// gear type

#define left_speed(a)	set_pwm1(a)		// left motor at PORT1
#define right_speed(a) 	set_pwm2(a)		// right motor at PORT2
#define DIRL			DIR1			// direction pin for Left motor
#define	DIRR			DIR2			// direction pin for Right motor
#define RUNL			RUN1			// RUN/BRAKE pin for Left motor
#define RUNR			RUN2			// RUN/BRAKE pin for Right motor

#define	LIMIT1			SEN1			// Upper limit switch for motor 1
#define LIMIT2			SEN2			// Lower limit switch for motor 1
#define	LIMIT3			SEN3			// Upper limit switch for motor 2
#define LIMIT4			SEN4			// Lower limit switch for motor 2


/*******************************************************************************
* PRIVATE FUNCTION PROTOTYPES                                                  *
*******************************************************************************/
//basic function for MC40SE
void delay_ms(unsigned int ui_value);
void beep(unsigned char uc_count);
void mc40se_init(void);

//function for brushless motor, brush motor and relays
void brushless(unsigned char uc_port_number, unsigned char uc_motor_status, unsigned char uc_motor_dir, unsigned int ui_speed);
void brush(unsigned char uc_port_number, unsigned char uc_motor_status, unsigned char uc_motor_dir, unsigned int ui_speed);
void reset_brushless(void);
void relay_on(unsigned char uc_relay_number);
void relay_off(unsigned char uc_relay_number);
void relay_off_all(void);

// robot locomotion, navigation
void forward(void);
void reverse(void);
void pivot_left(void);
void pivot_right(void);
void stop(void);
void run(void);
void motorspeed(unsigned int m_left, unsigned int m_right);

//functions for manual
void manual_demo(void);


/*******************************************************************************
* Global Variables                                                             *
*******************************************************************************/
unsigned char mLeft = 0, mRight = 0;	//motor speed
/*******************************************************************************
* MAIN FUNCTION                                                                *
*******************************************************************************/
int main(void)
{	
	// Initialize PIC16F887 to correct Input/Output based on MC40SE on board interface
	mc40se_init();
	
	// off all relays
	relay_off_all();	
	
	// Initialize ADC.
	adc_init();	
	
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
	lcd_clear_msg(" MC40SE\n Manual");
	delay_ms(1000);	
		
	beep(2);	// beep twice
	// display message, current mode is manual robot demo, press SW1 increase mode
	// press SW2 enter this mode
	lcd_clear_msg(" Manual\n  Demo!");		

	while(1)	//infinite loop
	{
		manual_demo();	// call manual demo function
	}//while(1)
	
	while(1) continue;	// infinite loop to prevent PIC from reset
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
#if defined (_16F887)
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
	TRISA = 0b00111111;
	TRISB = 0b00111111;
	TRISC = 0b10000001;
	TRISD = 0;
	TRISE = 0b00000000;	
}

/*******************************************************************************
* PRIVATE FUNCTION: brushless
*
* PARAMETERS:
* ~ unsigned char uc_port_number - refer to brushless port on MC40SE, PORT1 or PORT2
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
			BL_R = 0;	//after stop, reset the brushless
			__delay_ms(5);	// this reset can only be used if internal crystal is used
			BL_R = 1;		// and JP27 (BL-R) is connected
			__delay_ms(10);				
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
			BL_R = 0;	//after stop, reset the brushless
			__delay_ms(5);	// this reset can only be used if internal crystal is used
			BL_R = 1;		// and JP27 (BL-R) is connected
			__delay_ms(10);				
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
* PRIVATE FUNCTION: reset_brushless
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* reset alarm on Brushless motor, both port. Uses RA7 which is one of OSC pin.
* This function can only work if PIC is using internal oscillator.
* Jumper 27 (BL_R) must be connected.
*******************************************************************************/
void reset_brushless(void)
{
	#if defined (_16F887)
	BL_R = 0;
	__delay_ms(5);
	BL_R = 1;
	__delay_ms(10);	
	#endif	
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
	if(uc_relay_number > 0)
	{
	PORTD = PORTD | (0b00000001<< (uc_relay_number - 1));
	LATCH = 1;	// transfer the new output to relay
	delay_ms(1);
	LATCH = 0;	// hold the output of latch
	}
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
	if(uc_relay_number > 0)
	{
	PORTD = PORTD & (~(0b00000001<< (uc_relay_number -1)));
	LATCH = 1;	// transfer the new output to relay
	delay_ms(1);
	LATCH = 0;	// hold the output of latch
	}
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
	
/*******************************************************************************
* PRIVATE FUNCTION: manual_demo
*
* PARAMETERS:
* ~ void
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* to demo PS2 control using SKPS for manual robot
*
*******************************************************************************/
void manual_demo(void)
{
	unsigned char up_v, down_v, left_v, right_v, speed_up, speed_down; //variable for joy stick value
	unsigned int speed = SPEED;		//variable to store speed value, 	
	lcd_clear_msg(" Manual\nSKPS+PS2");	// SKPS and PS2 must be connected to MC40Se
	delay_ms(500);
	
	while(uc_skps(p_con_status) == 0) continue; //wait until status of PS2 is connected	
	lcd_clear_msg("PS2 OK\nSTART=ON");	// press START on PS2 to get started
	delay_ms(1000);
		
	while(uc_skps(p_start) == 1) continue; //wait until START button of PS2 is press
	
	beep(2);
	lcd_2ndline();
	lcd_putstr("SEL=out ");		// Press SELECT button on PS2 to exit this demo
		
	while(uc_skps(p_select) == 1)
	{
		//read joy stick value process		
		up_v=uc_skps(p_joy_lu);		// read analog value of left joystick, up axis, from 0 - 100
		down_v=uc_skps(p_joy_ld);	// read analog value of left joystick, down axis, from 0 - 100
		left_v=uc_skps(p_joy_ll);	// read analog value of left joystick, left axis, from 0 - 100
		right_v=uc_skps(p_joy_lr);	// read analog value of left joystick, right axis, from 0 - 100	
		speed_up=uc_skps(p_joy_ru);		// read analog value of right joystick, up axis, from 0 - 100
		speed_down=uc_skps(p_joy_rd);	// read analog value of right joystick, down axis, from 0 - 100
	
		
		// Control motor at relay, this is Right 1 front button
		if ((uc_skps(p_r1)== 0) && (LIMIT1 == 1))	//if R1 is press and Limit switch 1 is not touch
		{
			relay_on(1);
			relay_off(2);	
		}	
		
		// this is Right 2 front button
		else if ((uc_skps(p_r2)==0) && (LIMIT2 == 1)) //if R2 is press and limit switch 2 is not touch
		{
			relay_on(2);
			relay_off(1);
		}
		// if both neither switch is press, off relay 1&2		
		else {
			relay_off(1);
			relay_off(2);
		}	
		
		// check if Left front button is pressed
		if ((uc_skps(p_l1)== 0) && (LIMIT3 == 1)) // if L1 is press and limit switch 3 is not touch
		{
			relay_on(3);
			relay_off(4);
		}		
		else if ((uc_skps(p_l2)==0) && (LIMIT4 == 1)) // if L2 is press and limit switch 4 is not touch
		{
			relay_on(4);
			relay_off(3);
		}		
		else {
			relay_off(3);
			relay_off(4);
		}		
		
		//to change speed, default speed is 300, speed range is 10-bit WM, from 0 to 1023
		if(speed_up > 30)	// if right joystick is being push to up axis
		{
			if(speed < 1015) 
			{
				speed += 5;
				delay_ms(1);
			}			
		}
		else if (speed_down > 30) // if right joystick is being push down axis
		{
			if(speed > 5) 
			{
				speed -=5;
				delay_ms(1);
			}	
		}		
		//navigation using left and right top 4 buttons
		if(uc_skps(p_up)== 0)	// if up arrow button is press
		{
			if (uc_skps(p_square)==0) {	// if up & square buttons are press
				//left turn
				forward();
				motorspeed(0,speed);
			}
			else if (uc_skps(p_circle)==0) {
				//right turn
				forward();
				motorspeed(speed,0);
			}
			else {	
				//forward
				forward();
				motorspeed(speed,speed);
			}	
		}
		
		// if down arrow button is press, reverse
		else if(uc_skps(p_down) == 0)
		{	
			//backward
			reverse();
			motorspeed(speed,speed);
		}
		
		// if square button only being press, pivot left
		else if(uc_skps(p_square) == 0)
		{	
			//pivot left
			pivot_left();
			motorspeed(speed,speed);
		}
		
		// if circle button only being press, pivot right
		else if(uc_skps(p_circle) == 0)
		{
			//pivot right
			pivot_right();
			motorspeed(speed,speed);
			
		}	
			
		//analog control for mobility
		// if left joystick being push up	
		else if(up_v > 10)
		{
		 	// if left joystick being push to left too
			if(left_v > 10)
			{
				//turn left	
				forward();
				motorspeed(0,speed);	//turning left with stop at left motor									
			}
			else if(right_v > 0)
			{
				//turn right
				forward();
				motorspeed(speed,0);	//turning right with stop at right motor				
			}
			else
			{
				//normal forward
				forward();	// direction forward	
				motorspeed(speed,speed);	// forward with both left and right speed same		
			}			
			
		}
		//if left joystick being push down
		else if(down_v > 0)
		{
			// if left joystick being push left too
			if(left_v > 0)
			{
				// reverse towards left	
				reverse();	
				motorspeed(0,speed);	//reserve left with stop at left motor				
			}
			else if(right_v > 0)
			{
				// reverse towards right
				reverse();
				motorspeed(speed,0);	//reserve right with stop at right motor			
			}
			else
			{
			//normal reverse
			reverse();	
			motorspeed(speed,speed);	//reverse with normal, same speed on left and right motor								
			}		
		}
		
		// if left up and down axis is not push, only left and right axis being push
		else if(left_v > 0)
		{
			//pivot left
			pivot_left();
			motorspeed(speed,speed);			
		}
		// if right joystick being push to right only
		else if(right_v>0)
		{
			//pivot right
			pivot_right();
			motorspeed(speed,speed);				
		}
		
		// if no joystick or button on top panel being pressed, stop the robot (no navigation)
		else
		{
			stop();		// if left analog joystick is not pushed, both left and right motor will brake	
		}	
	}//while(ps(p_select) == 1)
	
	while(uc_skps(p_select) == 0) continue; //wait for p select to be release
	beep(2);
	delay_ms(100);
}

// ==================== brushless motor control =======================================
//control brushless motor
//==============================================================================================
void forward(void)
{
	run();
	if(GEAR == 1)
	{
	DIRL = 1;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 0;	//Robot will forward depend on gear type
	}
	else
	{
	DIRL = 0;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 1;	//Robot will forward depend on gear type
	}
}

void reverse(void)
{
	run();
	if(GEAR == 1)
	{
	DIRL = 0;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 1;	//Robot will reverse depend on gear type
	}
	else 
	{
	DIRL = 1;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 0;	//Robot will reverse depend on gear type
	}	
}

void pivot_left(void)
{
	run();
	if(GEAR == 1)
	{
	DIRL = 0;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 0;	//Robot will turn left depend on gear type		
	}
	else
	{
	DIRL = 1;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 1;	//Robot will turn left depend on gear type		
	}	
}

void pivot_right(void)
{
	run();
	if(GEAR == 1)
	{
	DIRL = 1;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 1;	//Robot will turn right depend on gear type	
	}
	else
	{	
	DIRL = 0;	//motor at left(PORT1) and right(PORT2) 
	DIRR = 0;	//Robot will turn right depend on gear type	
	}	
}
void stop(void)
{
	RUNL = 1;	//motor at left(PORT1) and right (PORT2)
	RUNR = 1; 	//will brake
	BL_R = 0;	//after stop, reset the brushless
	__delay_ms(5);	// this reset can only be used if internal crystal is used
	BL_R = 1;		// and JP27 (BL-R) is connected
	__delay_ms(10);	
}
void run(void)
{
	RUNL = 0;	//motor at left(PORT1) and right (PORT2)
	RUNR = 0; 	//will run	
}
void motorspeed(unsigned int m_left, unsigned int m_right)
{	
	left_speed(m_left);	//left motor speed
	right_speed(m_right);	// right motor speed
}

