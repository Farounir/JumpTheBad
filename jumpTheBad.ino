/*                                                       
      _                         _   _            ____            _ _ _ 
     | |                       | | | |          |  _ \          | | | |
     | |_   _ _ __ ___  _ __   | |_| |__   ___  | |_) | __ _  __| | | |
 _   | | | | | '_ ` _ \| '_ \  | __| '_ \ / _ \ |  _ < / _` |/ _` | | |
| |__| | |_| | | | | | | |_) | | |_| | | |  __/ | |_) | (_| | (_| |_|_|
 \____/ \__,_|_| |_| |_| .__/   \__|_| |_|\___| |____/ \__,_|\__,_(_|_)
                       | |                                             
                       |_|                                             
 * The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * Jump pushbutton switch to pin 6 and ground
 * wiper to LCD VO pin (pin 3)
 */

// include the lcd library code:
#include <LiquidCrystal.h>

// CONSTANTS
const int DELAY_TIME = 200;
const int JUMPIN = 6;  // pin to read for sweet jumping action!
const int WIDTH = 16;  // number of lcd characters long
const int HEIGHT = 2;  // number of lcd characters high

// Globals... cause if pacman did it, we can too!
int buttonState;  // variable for reading the button
int lastButton;  // keeps track of the last state of the button
bool alive; // keeps track of whether man is alive or not i.e. if game goes on
int manPos; // keeps track if man is standing, jumping up, has jumped, or is landing using an int 0-4

uint8_t lcdChar[WIDTH][HEIGHT]; //a 2d array that holds cust characters and is used to keep track of game state

//custom graphics!!!!1!!!11!
const int numChar = 8; // the number of characters needed to create a custom lcd character
typedef uint8_t custChar[numChar];

custChar MAN_A = {0xe,0xe,0x4,0xe,0x15,0xe,0x1b,0x00};      // standing man: both arms and legs down
const uint8_t MAN = 0;

custChar MAN_JUMP_A = {0xe,0xe,0x5,0xe,0x14,0xe,0x12,0x00}; // starting to jump: right arm up, legs to left
const uint8_t MAN_JUMP = 1;

custChar MAN_TOP_A = {0xe,0xe,0x15,0xe,0x4,0xe,0x11,0x00};  // spread eagle: both arms and legs up
const uint8_t MAN_TOP= 2;

custChar MAN_LAND_A = {0xe,0xe,0x14,0xe,0x5,0xe,0x9,0x00};  // we have landing: left arm up, legs to right
const uint8_t MAN_LAND = 3;

custChar OBS_A = {0x0,0x0,0x4,0xa,0x15,0xa,0x15,0x00};      // generic obstacle: spike pointing up
const uint8_t OBS = 4;

custChar MAN_DEAD_A = {0x0,0x0,0x0,0x0,0x18,0x19,0x1f,0x00};  // it's dead, Jim!
const uint8_t MAN_DEAD = 5;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
	// setup jump button as input with internal pullup resistor
	// NOTE: grounding pin 6 to pull low will signal switch is triggered
	pinMode(JUMPIN, INPUT);
	digitalWrite(JUMPIN, HIGH);
	
	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);

	// init all the custom characters
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
	delay(2000);
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

/*void increment(); //increments everything in the lcdChar array over one to simulate movement
void updateDisplay(); //method to update the display to the state of the lcdChar array
*/  
