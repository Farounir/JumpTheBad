/*
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

// include the library code:
#include <LiquidCrystal.h>
#include "JTBGame.h"

const int DELAY_TIME = 200;
int jumPin = 6;  //pin to read for sweet jumping action!
int buttonState = 0;  //variable for reading the button
int lastState = 0;  //keeps track of the last state of the button

//custom graphics
uint8_t manArray[] = {0xe,0xe,0x4,0xe,0x15,0xe,0x1b,0x00};
uint8_t man = 0;

uint8_t obsArray0[] = {0x0,0x0,0x4,0xa,0x15,0xa,0x15, 0x00};
uint8_t obs0 = 1;

uint8_t obsArray1[] = {0x2,0xd,0x3,0xd,0x3,0xd,0x2, 0x00};
uint8_t obs1 = 2;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  //setup jump button
  pinMode(JUMPIN, INPUT);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.createChar(MAN, MAN_A);
  lcd.createChar(MAN_JUMP, MAN_JUMP_A);
  lcd.createChar(MAN_TOP, MAN_TOP_A);
  lcd.createChar(MAN_LAND, MAN_LAND_A);
  lcd.createChar(OBS, OBS_A);
  
	lcd.clear();
	lcd.setCursor(0,1);
	lcd.write(MAN);
	lcd.setCursor(1,1);
	lcd.write(MAN_JUMP);
	lcd.setCursor(2,1);
	lcd.write(MAN_TOP);
	lcd.setCursor(3,1);
	lcd.write(MAN_LAND);
	lcd.setCursor(4,1);
	lcd.write(OBS);
  
  lcd.setCursor(0,0);
  lcd.print("Jump the Bad!");
  delay(200);
  lcd.clear();
}

void loop() {
	/*lcd.setCursor(0,1);
	lcd.write(MAN);

	lcd.setCursor(1,1);
	lcd.write(MAN_JUMP);

	lcd.setCursor(2,0);
	lcd.write(MAN_JUMP);

	lcd.setCursor(3,0);
	lcd.write(MAN_TOP);

	lcd.setCursor(3,1);
	lcd.write(OBS);

	lcd.setCursor(4,0);
	lcd.write(MAN_LAND);

	lcd.setCursor(5,1);
	lcd.write(MAN_LAND);

	lcd.setCursor(6,1);
	lcd.write(MAN);
	delay(200);
	lcd.clear();*/




	lcd.setCursor(8,1);
	lcd.write(MAN);
	lcd.setCursor(9,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();

	lcd.setCursor(8,1);
	lcd.write(MAN_JUMP);
	lcd.setCursor(9,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();

	/*lcd.setCursor(8,0);
	lcd.write(MAN_JUMP);
	lcd.setCursor(9,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();

	lcd.setCursor(8,0);
	lcd.write(MAN_JUMP);
	lcd.setCursor(8,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();*/

	lcd.setCursor(8,0);
	lcd.write(MAN_TOP);
	lcd.setCursor(8,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();

	/*lcd.setCursor(8,0);
	lcd.write(MAN_LAND);
	lcd.setCursor(8,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();*/

	lcd.setCursor(8,1);
	lcd.write(MAN_LAND);
	lcd.setCursor(7,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();

	lcd.setCursor(8,1);
	lcd.write(MAN);
	lcd.setCursor(7,1);
	lcd.write(OBS);
	delay(DELAY_TIME);
	lcd.clear();
}
  
