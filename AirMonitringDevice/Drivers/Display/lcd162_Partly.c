/*-------------------------------------------------------------------
  File name: "lcd162.c"

  Driver for "LCD Keypad Shield" alphanumeric display.
  Display controller = HD44780U (LCD-II).
  
  Max. uC clock frequency = 16 MHz (Tclk = 62,5 ns)

  Connection : PORTx (4 bit mode) :
  [LCD]        [Portx]
  RS   ------  PH 5
  RW   ------  GND
  E    ------  PH 6
  DB4  ------  PG 5
  DB5  ------  PE 3
  DB6  ------  PH 3
  DB7  ------  PH 4

  Henning Hargaard, February 5, 2018
---------------------------------------------------------------------*/
#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
// Enabling us to use macro _NOP() to insert the NOP instruction
#include <avr/cpufunc.h>
#include <stdbool.h>
#include <stdio.h>
#include "lcd162.h"

// library function itoa() is needed
#include <stdlib.h>

//*********************** PRIVATE (static) operations *********************
static void waitBusy()
{
	_delay_ms(2);
}

static void pulse_E()
{
	PORTH = (PORTH | 0b01000000);
	for(int i = 0; i<4; i++){
		_NOP();
	}
	
	PORTH = (PORTH & 0b10111111);
	for(int i = 0; i<4; i++){
		_NOP();
	}
}

// Sets the display data pins according to the 4 lower bits of data
static void set4DataPins(unsigned char data)
{
  PORTH = (PORTH & 0b11100111) | ((data<<1) & 0b00011000);
  PORTE = (PORTE & 0b11110111) | ((data<<2) & 0b00001000);
  PORTG = (PORTG & 0b11011111) | ((data<<5) & 0b00100000);  
}

static void sendInstruction(unsigned char data)
{      
	waitBusy();
	
	//set Rs and E low
	PORTH &= 0b10011111;
	
	set4DataPins(data>>4);
	pulse_E();

	set4DataPins(data);
	pulse_E();
}

static void sendData(unsigned char data)
{      
  	waitBusy();
  	
  	//set Rs and E low
  	PORTH &= 0b10111111;
	setRS(true);
  	
  	set4DataPins(data>>4);
  	pulse_E();

  	set4DataPins(data);
  	pulse_E();
}

void setRS(bool isSet){
	if(isSet){
		PORTH = (PORTH | 0b00100000);
	}else{
		PORTH = (PORTH & 0b11011111);
	}
}



//*********************** PUBLIC functions *****************************

// Initializes the display, blanks it and sets "current display position"
// at the upper line, leftmost character (cursor invisible)
// Reference: Page 46 in the HD44780 data sheet
void LCDScreenOff(){
	sendInstruction(0b00001000);
}
void LCDScreenOn(){
	sendInstruction(0b00001100);
}

void LCDInit()
{
  // Initializing the used port
  DDRH |= 0b01111000;  // Outputs
  DDRE |= 0b00001000;
  DDRG |= 0b00100000;
  
  // Wait 50 ms (min. 15 ms demanded according to the data sheet)
  _delay_ms(50);
  // Function set (still 8 bit interface)
  PORTG |= 0b00100000;
  PORTE |= 0b00001000;
  pulse_E();
  
  // Wait 10 ms (min. 4,1 ms demanded according to the data sheet)
  _delay_ms(10);
  // Function set (still 8 bit interface)
  pulse_E();

  // Wait 10 ms (min. 100 us demanded according to the data sheet)
  _delay_ms(10);
  // Function set (still 8 bit interface)
  pulse_E();

  // Wait 10 ms (min. 100 us demanded according to the data sheet)
  _delay_ms(10);
  // Function set (now selecting 4 bit interface !)
  PORTG &= 0b11011111;
  pulse_E();

  // Function Set : 4 bit interface, 2 line display, 5x8 dots
  sendInstruction( 0b00101000 );
  // Display, cursor and blinking OFF
  sendInstruction( 0b00001000 );
  // Clear display and set DDRAM adr = 0	
  sendInstruction( 0b00000001 );
  // By display writes : Increment cursor / no shift
  sendInstruction( 0b00000110 );
  // Display ON, cursor and blinking OFF
  sendInstruction( 0b00001111 );
}

// Blanks the display and sets "current display position" to
// the upper line, leftmost character
void LCDClear()
{
  sendInstruction( 0b00000001 );
}

// Sets DDRAM address to character position x and line number y
void LCDGotoXY( unsigned char x, unsigned char y )
{
	unsigned char instruction;
	instruction = (0b10000000+x)+y*0x40;
	
	sendInstruction(instruction);
}

// Display "ch" at "current display position"
void LCDDispChar(char ch)
{
	sendData(ch);
}

// Displays the string "str" starting at "current display position"
void LCDDispString(char* str)
{
	for (char* t = str; *t != '\0'; t++) {
		sendData(*t);
	}
}

// Displays the value of integer "i" at "current display position"
void LCDDispInteger(int i)
{
	char number[7];
	itoa(i, number, 10);
	LCDDispString(number);
}

// Loads one of the 8 user definable characters (UDC) with a dot-pattern,
// pre-defined in an 8 byte array in FLASH memory
void LCDLoadUDC(unsigned char UDCNo, const unsigned char *UDCTab)
{
	unsigned char i;
	
	sendInstruction(0b01000000 | (UDCNo<<3));
	
	for(i=0; i<8; i++)
	sendData(*UDCTab++);

	LCDGotoXY(0,0);
}

// Selects, if the cursor has to be visible, and if the character at
// the cursor position has to blink.
// "cursor" not 0 => visible cursor.
// "blink" not 0 => the character at the cursor position blinks.
void LCDOnOffControl(unsigned char cursor, unsigned char blink)
{
  // To be implemented
}

// Moves the cursor to the left
void LCDCursorLeft()
{
  sendInstruction(0b00010000);
}

// Moves the cursor to the right
void LCDCursorRight()
{
  sendInstruction(0b00010100);
}

// Moves the display text one position to the left
void LCDShiftLeft()
{
  // To be implemented
}

// Moves the display text one position to the right
void LCDShiftRight()
{
  // To be implemented
}

// Sets the backlight intensity to "percent" (0-100)
void setBacklight(unsigned char percent)
{
  // To be implemented
}

// Reads the status for the 5 on board keys
// Returns 0, if no key pressed
unsigned char readKeys()
{
  // To be implemented
}