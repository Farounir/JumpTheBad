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
 * Duck pushbutton switch to pin 7 and ground
 * wiper to LCD VO pin (pin 3)
 */

// include the lcd library code:
#include <Wire.h> // some variants of lcd library require Wire.h
#include <LiquidCrystal.h>

// CONSTANTS: HARDCORE, YO
const int DELAY_TIME = 1;
const int JUMPIN = 6;          // pin to read for sweet jumping action!
const int DUCKPIN = 7;         // pin to read for fantastic ducking happenings!
const int WIDTH = 16;          // number of lcd characters long`
const int HEIGHT = 2;          // number of lcd characters high
const int MS_PER_TICK = 250;   // ms per tick
const int NUM_CHAR = 8;        // the number of hex bytes needed to create a custom 5x7 lcd character
const uint8_t BLANK = 254;     // uint8 code of a blank character


// Globals... cause if pacman did it, we can too!
int buttonState0;    // variable for reading the button
int lastButton0;     // keeps track of the last state of the button
int buttonState1;    // variable for reading the button
int lastButton1;     // keeps track of the last state of the button
bool alive;          // keeps track of whether man is alive or not i.e. if game goes on

int manState = 0;    // 0 means standing, 1 means in the process of jumping, and 2 means in the process of ducking
int jumpState = 0;   // 0 means not jumping/standing, 1 means heading up, 2 means top, 3 means heading down
int duckState = 0;   // 0 means not ducking/standing, 1 means heading down, 2 means ducking all the way, 3 means heading back up

long lastTick = 0;   // last time the clock ticked in ms
int tickCycle = 5;   // number of ticks before the array is incremented (the picture moves)
int cyclePos = 0;    // the number of ticks since the cycle reset (resets at the value of tickCycle)

int state = -1;      // state of jumping demo animation
int numStates = 5;   // number of states in the demo


typedef uint8_t custChar[NUM_CHAR];

uint8_t lcdChar[WIDTH][HEIGHT];    //a 2d array that holds cust characters and is used to keep track of game state

// custom graphics!!!!1!!!11!

custChar MAN_A = {0xe,0xe,0x4,0xe,0x15,0xe,0x1b,0x00};          // standing man: both arms and legs down
const uint8_t MAN = 0;

custChar MAN_JUMP_A = {0xe,0xe,0x5,0xe,0x14,0xe,0x12,0x10};     // starting to jump: right arm up, legs to left
const uint8_t MAN_JUMP = 1;                                     // This also can be used as an after slide

custChar MAN_TOP_A = {0xe,0xe,0x15,0xe,0x4,0xe,0x11,0x00};      // spread eagle: both arms and legs up
const uint8_t MAN_TOP= 2;

custChar MAN_LAND_A = {0xe,0xe,0x14,0xe,0x5,0xe,0x9,0x1};      // we have landing: left arm up, legs to right
const uint8_t MAN_LAND = 3;                                     // This also can be used as a pre slide

custChar OBS0_A = {0x0,0x4,0x4,0xe,0xe,0x1f,0x1f,0x1f};              // generic obstacle: spike pointing up
const uint8_t OBS0 = 4;

custChar OBS_BOT_A = {0x15,0x11,0x0,0x0,0x0,0x0,0x0};           // better duck bro: bottom part of obstacle, no man underneath
const uint8_t OBS_BOT = 5;

custChar OBS_TOP_A = {0x1f,0x15,0x1f,0x15,0x1f,0x15,0x15,0x15};      // top part of obstacle, attaches to OBS_BOT
const uint8_t OBS_TOP = 6;

custChar MAN_SLIDE_A = {0x15,0x11,0x0,0x18,0x18,0x6,0xb,0x1};       // sliiiide to the right: bottom part of obstacle, with man underneath
const uint8_t MAN_SLIDE = 7;

// initialize the LCD library with the numbers of the interface pins

// This line of code is for standard LCD interface:
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// And these two are for SPI interface using http://www.ladyada.net/products/i2cspilcdbackpack/:
// #include <Wire.h>
// LiquidCrystal lcd(12, 13, 11);

void setup() {
	// start serial for debugglin'
	Serial.begin(115200);
	//initalize randomness
	randomSeed(analogRead(0));

	// setup jump and duck buttons as input with internal pullup resistor
	// NOTE: grounding pin 6 or 7 to low will trigger switch.
	pinMode(JUMPIN, INPUT);
	digitalWrite(JUMPIN, HIGH);
	pinMode(DUCKPIN, INPUT);
	digitalWrite(DUCKPIN, HIGH);

	pinMode(12, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(2, OUTPUT);
	
	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);

	// init all the custom characters
	lcd.createChar(MAN, MAN_A);
	lcd.createChar(MAN_JUMP, MAN_JUMP_A);
	lcd.createChar(MAN_TOP, MAN_TOP_A);
	lcd.createChar(MAN_LAND, MAN_LAND_A);
	lcd.createChar(OBS0, OBS0_A);
	lcd.createChar(OBS_TOP, OBS_TOP_A);
	lcd.createChar(OBS_BOT, OBS_BOT_A);
	lcd.createChar(MAN_SLIDE, MAN_SLIDE_A);

	//intialize lcdChar[][]
	//REDO THIS probably could use a method
	
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
	lcd.write(OBS0);
	lcd.setCursor(5,0);
	lcd.write(OBS_TOP);
	lcd.setCursor(5,1);
	lcd.write(OBS_BOT);
	lcd.setCursor(6,0);
	lcd.write(OBS_TOP);
	lcd.setCursor(6,1);
	lcd.write(MAN_SLIDE);
	
	lcd.setCursor(0,0);
	//lcd.print("Jump the Bad!");
	delay(3000);
	lcd.clear();
}

void loop() {
	checkTime();
}

void checkTime() {
	if (millis() > lastTick + MS_PER_TICK) {
		Serial.print("Tick! Time (ms): ");
		Serial.println(millis());
		lastTick = millis();
		
		incrementState();
		displayState();
		/*
		if (manState == 0) //he is standing) {
			
		*/
	}
}

void incrementState() {
	state++;
	if (state == numStates) { 
		state = 0; //resets state if all states have been touched
	}
}

void displayState() {
	lcd.clear();
	switch (state) {
		case 0:
			/* diagram:
			 *  #
			 * O^
			 */
			lcd.setCursor(1, 1);
			lcd.write(MAN);
			lcd.setCursor(2, 0);
			lcd.write(OBS_TOP);
			lcd.setCursor(2, 1);
			lcd.write(OBS_BOT);
			break;
		case 1:
			/* diagram:
			 *  #
			 * o^
			 */
			lcd.setCursor(1, 1);
			lcd.write(MAN_LAND);
			lcd.setCursor(2, 0);
			lcd.write(OBS_TOP);
			lcd.setCursor(2, 1);
			lcd.write(OBS_BOT);
			break;
		case 2:
			/* diagram:
			 *  #
			 *  A
			 */
			lcd.setCursor(2, 0);
			lcd.write(OBS_TOP);
			lcd.setCursor(2, 1);
			lcd.write(MAN_SLIDE);
			break;
		case 3:
			/* diagram:
			 *  #
			 *  ^o
			 */
			lcd.setCursor(3, 1);
			lcd.write(MAN_JUMP);
			lcd.setCursor(2, 0);
			lcd.write(OBS_TOP);
			lcd.setCursor(2, 1);
			lcd.write(OBS_BOT);
			break;
		case 4:
			/* diagram:
			 *  #
			 *  ^O
			 */
			lcd.setCursor(3, 1);
			lcd.write(MAN);
			lcd.setCursor(2, 0);
			lcd.write(OBS_TOP);
			lcd.setCursor(2, 1);
			lcd.write(OBS_BOT);
			break;
	}

}


void incrementArray() {
	for (int y = 0; y++; y < HEIGHT) {
		for (int x = 0; x++; x < WIDTH) {
			if (x = WIDTH - 1) { 
				lcdChar[x][y] = BLANK;  //this is the last character and must be blank since array has shifted
			}
			lcdChar[x][y] = lcdChar[x+1][y];  //shift characters over one to simulate movement
		}
	}
	//generates some objects to duck/jump
	//I guess a simple 25% perecent chance per object will work
	int randomNum = random(100);
	if (randomNum < 50) {
		if (randomNum < 25) {  //Duck this bad
			lcdChar[WIDTH-1][0] = OBS_TOP;
			lcdChar[WIDTH-1][1] = OBS_BOT;
		} else {  //Jump this bad
			lcdChar[WIDTH-1][0] = BLANK;
			lcdChar[WIDTH-1][1] = OBS0;
		}
	} else {
		//What's in the box?! Nothing! Absoluetly nothing! STUPID! YOU'RE SOOOO STUPID!
			lcdChar[WIDTH-1][0] = BLANK;
			lcdChar[WIDTH-1][1] = BLANK;
	}
	//now that everything has moved we need to recreate the man
	updateMan();  //puts man back in the picture
}

void updateMan() {
	switch (manState) {
	case 0:  //standing
		if (lcdChar[0][1] != BLANK) {  //makes sure that we aren't drawing a man where an object should be
			alive = false;
		} else {
			lcdChar[0][1] = MAN;
		}
		break;

	case 1: //jumping
		switch (jumpState) {

		case 0: //starting to jump (like no really, he is just standing there)
		    if (lcdChar[0][1] != BLANK) {
		        alive = false;
		    } else {
			    lcdChar[0][1] = MAN;
	        }
			break;
		case 1:
            if (lcdChar[0][1] != BLANK) {
			    alive = false;
		    } else {
			    lcdChar[0][1] = MAN_JUMP; //starting to take off
            }
			break;
		case 2:
            if (lcdChar[0][0] != BLANK) { 
			    alive = false;
		    } else {
			    lcdChar[0][0] = MAN_TOP; //apex of jump
            }
            break;
        case 3:
            if (lcdChar[0][1] != BLANK) { //coming down
			    alive = false;
		    } else {
                lcdChar[0][1] = MAN_LAND;
            }
        }

    case 2: //ducking
        switch(duckState) {
        case 0: //starting to duck (like no really, he is just standing there)
		    if (lcdChar[0][1] != BLANK) {
		        alive = false;
		    } else {
			    lcdChar[0][1] = MAN;
	        }
			break;
		case 1:
            if (lcdChar[0][1] != BLANK) {
			    alive = false;
		    } else {
			    lcdChar[0][1] = MAN_LAND; //starting to duck down
            }
			break;
		case 2:
            if (lcdChar[0][0] != OBS_BOT) { 
			    alive = false;
		    } else {
			    lcdChar[0][0] = MAN_SLIDE; //sliding
            }
            break;
        case 3:
            if (lcdChar[0][1] != BLANK) { //coming out (not a gay joke)
			    alive = false;
		    } else {
                lcdChar[0][1] = MAN_JUMP;
            }
        }
}