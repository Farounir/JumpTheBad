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
const int JUMPIN = 6;          // pin to read for sweet jumping action!
const int DUCKPIN = 7;         // pin to read for fantastic ducking happenings!
const int WIDTH = 16;          // number of lcd characters long`
const int HEIGHT = 2;          // number of lcd characters high
const int NUM_CHAR = 8;        // the number of hex bytes needed to create a custom 5x7 lcd character
const uint8_t LCD_BLANK = 254; // uint8 code of a blank character


// Globals... cause if pacman did it, we can too!
int buttonState0;              // variable for reading the button
int lastButton0;               // keeps track of the last state of the button
int buttonState1;              // variable for reading the button
int lastButton1;               // keeps track of the last state of the button
bool alive;                    // keeps track of whether man is alive or not i.e. if game goes on

int manState = 0;
/* manState is important, as it defines what the player is doing and what he can do
 * States:
 *     0: standing
 *     1: preparing to jump
 *     2: in air
 *     3: coming down from jump
 *     4: preparing to duck
 *     5: ducking under gate
 *     6: straightening up from duck
 */
int lastManState = 0;          // only redraw man if his state changes

int ms_per_stage_tick = 100;   // every x ms, the stage moves left
long lastTick = 0;             // last time the clock ticked in ms

uint8_t stageData[16] = {0};
const uint8_t OBS_NONE  = 0;
const uint8_t OBS_SPIKE = 1;
const uint8_t OBS_GATE  = 2;



typedef uint8_t custChar[NUM_CHAR];

// custom graphics!!!!1!!!11!

custChar LCD_MAN_DATA = {0xe,0xe,0x4,0xe,0x15,0xe,0x1b,0x00};               // standing man: both arms and legs down
const uint8_t LCD_MAN = 0;

custChar LCD_MAN_JUMP_DATA = {0xe,0xe,0x5,0xe,0x14,0xe,0x12,0x10};          // starting to jump: right arm up, legs to left
const uint8_t LCD_MAN_JUMP = 1;                                             // This also can be used as an after slide

custChar LCD_MAN_TOP_DATA = {0xe,0xe,0x15,0xe,0x4,0xe,0x11,0x00};           // spread eagle: both arms and legs up
const uint8_t LCD_MAN_TOP= 2;

custChar LCD_MAN_LAND_DATA = {0xe,0xe,0x14,0xe,0x5,0xe,0x9,0x1};            // we have landing: left arm up, legs to right
const uint8_t LCD_MAN_LAND = 3;                                             // This also can be used as a pre slide

custChar LCD_SPIKE_DATA = {0x0,0x4,0x4,0xe,0xe,0x1f,0x1f,0x1f};         // generic obstacle: spike pointing up
const uint8_t LCD_SPIKE = 4;

custChar LCD_GATE_BOT_DATA = {0x15,0x11,0x0,0x0,0x0,0x0,0x0};           // better duck bro: bottom part of obstacle, no man underneath
const uint8_t LCD_GATE_BOT = 5;

custChar LCD_GATE_TOP_DATA = {0x1f,0x15,0x1f,0x15,0x1f,0x15,0x15,0x15}; // top part of obstacle, attaches to OBS_BOT
const uint8_t LCD_GATE_TOP = 6;

custChar LCD_MAN_SLIDE_DATA = {0x15,0x11,0x0,0x18,0x18,0x6,0xb,0x1};        // sliiiide to the right: bottom part of obstacle, with man underneath
const uint8_t LCD_MAN_SLIDE = 7;




// initialize the LCD library with the numbers of the interface pins

// This line of code is for standard LCD interface:
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// And these two are for SPI interface using http://www.ladyada.net/products/i2cspilcdbackpack/:
// #include <Wire.h>
// LiquidCrystal lcd(12, 13, 11);

void setup() {

	// start serial for debugglin'
	Serial.begin(115200);

	// initalize randomness
	randomSeed(analogRead(0));

	// setup lcd and input pins
	setupPins();
	
	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);

	// set up the custom symbols displayed on screen
	initChars();

	// display the symbols used in the game for 3 seconds, then clear
	showSymbols();

	Serial.println("Ready to go!");
}

void loop() {
	checkTime();
}

void setupPins() {
	// setup jump and duck buttons as input with internal pullup resistor
	// NOTE: grounding pin 6 or 7 to low will trigger switch.
	pinMode(JUMPIN, INPUT);
	digitalWrite(JUMPIN, HIGH);
	pinMode(DUCKPIN, INPUT);
	digitalWrite(DUCKPIN, HIGH);

	// setup lcd pins
	pinMode(12, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(2, OUTPUT);
}

void initChars() {
	// init all the custom characters
	lcd.createChar(LCD_MAN, LCD_MAN_DATA);
	lcd.createChar(LCD_MAN_JUMP, LCD_MAN_JUMP_DATA);
	lcd.createChar(LCD_MAN_TOP, LCD_MAN_TOP_DATA);
	lcd.createChar(LCD_MAN_LAND, LCD_MAN_LAND_DATA);
	lcd.createChar(LCD_SPIKE, LCD_SPIKE_DATA);
	lcd.createChar(LCD_GATE_TOP, LCD_GATE_TOP_DATA);
	lcd.createChar(LCD_GATE_BOT, LCD_GATE_BOT_DATA);
	lcd.createChar(LCD_MAN_SLIDE, LCD_MAN_SLIDE_DATA);
}

void showSymbols() {
	lcd.clear();
	lcd.setCursor(0,1);
	lcd.write(LCD_MAN);
	lcd.setCursor(1,1);
	lcd.write(LCD_MAN_JUMP);
	lcd.setCursor(2,1);
	lcd.write(LCD_MAN_TOP);
	lcd.setCursor(3,1);
	lcd.write(LCD_MAN_LAND);
	lcd.setCursor(4,1);
	lcd.write(LCD_SPIKE);
	lcd.setCursor(5,0);
	lcd.write(LCD_GATE_TOP);
	lcd.setCursor(5,1);
	lcd.write(LCD_GATE_BOT);
	lcd.setCursor(6,0);
	lcd.write(LCD_GATE_TOP);
	lcd.setCursor(6,1);
	lcd.write(LCD_MAN_SLIDE);
	
	lcd.setCursor(0,0);
	delay(3000);
	lcd.clear();
}

void checkTime() {
	if (millis() > lastTick + ms_per_stage_tick) {
		lastTick = millis();
		moveMan();
		shiftStage();
		dispStage();
	}
}

void moveMan() {
	
}

void shiftStage() {
	// shift everything on the stage to the left:
	// 1 ... 15 -> 0 ... 14
	for (int i = 1; i < WIDTH; i++) {
		stageData[i-1] = stageData[i];
	}
	// generate an obstacle at far right (position 15)
	rndMakeObstacle();
}

void dispStage() {
	// draw obstacles
	for (int i = 0; i < WIDTH; i++) {
		Serial.print(stageData[i]);
		switch (stageData[i]) {
			case OBS_NONE:
				lcd.setCursor(i, 0);
				lcd.write(LCD_BLANK);
				lcd.setCursor(i, 1);
				lcd.write(LCD_BLANK);
				break;
			case OBS_SPIKE:
				lcd.setCursor(i, 0);
				lcd.write(LCD_BLANK);
				lcd.setCursor(i, 1);
				lcd.write(LCD_SPIKE);
				break;
			case OBS_GATE:
				lcd.setCursor(i, 0);
				lcd.write(LCD_GATE_TOP);
				lcd.setCursor(i, 1);
				lcd.write(LCD_GATE_BOT);
				break;
		}
	}
	Serial.println();

	// draw man
	lcd.setCursor(0, 1);
	lcd.write(LCD_MAN);
}

void rndMakeObstacle() {
	int rnd = random(20); // generates numbers from 0 to 19
	if (rnd == 0) { // 5% chance
		stageData[15] = OBS_SPIKE;
	} else if (rnd == 1) {
		stageData[15] = OBS_GATE;
	} else {
		stageData[15] = OBS_NONE;
	}
}