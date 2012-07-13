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
#include <Bounce.h>



// CONSTANTS: HARDCORE, YO
const int JUMPIN = 6;          // pin to read for sweet jumping action!
const int DUCKPIN = 7;         // pin to read for fantastic ducking happenings!
const int WIDTH = 16;          // width in chars of lcd screen
const int HEIGHT = 2;          // height in chars of lcd screen
const int NUM_CHAR = 8;        // the number of hex bytes needed to create a custom 5x8 lcd character

const int MS_PER_STAGE_TICK = 120;        // every x ms, the stage moves left
const int TICKS_TILL_SPEEDUP = 30;        // every x ticks, speed up the game
const int SPEEDUP_AMOUNT = 8;             // speed up by this many ticks
const int CHG_TICKS_TILL_SPEEDUP = 5;     // increase ticksTillSpeedup by this much each time the game speeds up
const int LAST_TICK_SPEEDUP = 0;          // last tick at which the speed increased

long currTick = 0;
int ticksTillSpeedup = TICKS_TILL_SPEEDUP;
int lastTickSpeedup = LAST_TICK_SPEEDUP;
int msPerStageTick = MS_PER_STAGE_TICK;

const int MIN_TICKS_BTWN_OBSTACLES = 4;   // cooldown time before another obstacle can be spawned
const int MAX_TICKS_BTWN_OBSTACLES = 10;  // longest time game will wait between two obstacles
const int TICKS_PER_JUMP = 2;             // hang time per jump, in ticks
const int MAX_TICKS_DUCKING = 3;          // max amount of time man can duck before standing up again

long lastObstacleTick = 0;

// Globals... cause if pacman did it, we can too!

uint8_t manState = 0;
/* manState is important, as it defines what the player is doing and what he can do
 * States:
 *     0: standing
 *     1: preparing to jump
 *     2: in air
 *     3: coming down from jump
 *     4: ducking
 *     5: straightening up from duck
 */
const uint8_t MAN_STANDING  = 0;
const uint8_t MAN_PRE_JUMP  = 1;
const uint8_t MAN_JUMP      = 2;
const uint8_t MAN_POST_JUMP = 3;
const uint8_t MAN_DUCK      = 4;
const uint8_t MAN_POST_DUCK = 5;

uint8_t lastManState = -1;     // only redraw man if his state changes
bool obstaclePassed = false;   // redraws man manually after each obstacle
bool doPostDuck = false;       // after each gate, do a post-duck recovery to normal

bool manRunningDisp = false;   // toggle two display states to show the man "running"

int jumpTimer = 0;             // keeps track of how far into the jump's hangtime the man is
int duckTimer = 0;             // keeps track of how long the man has been ducking

int obstacleCooldown = false;  // doesn't draw obstacles too close to each other

long lastTickMs = 0;           // last time the clock ticked in ms

uint8_t stageData[WIDTH] = {0}; // holds the stage obstacle data
const uint8_t OBS_NONE  = 0;
const uint8_t OBS_SPIKE = 1;
const uint8_t OBS_GATE  = 2;



const uint8_t LCD_BLANK = 254; // uint8 code of a blank character

typedef uint8_t custChar[NUM_CHAR];

// custom graphics!!!!1!!!11!

custChar LCD_MAN_DATA = {0xe,0xe,0x4,0x1e,0x5,0xe,0x1b,0x0};                // standing man: both arms and legs down
const uint8_t LCD_MAN = 0;

custChar LCD_MAN_JUMP_DATA = {0xe,0xe,0x5,0xe,0x14,0xe,0x12,0x10};          // starting to jump: right arm up, legs to left
const uint8_t LCD_MAN_JUMP = 1;                                             // This also can be used as an after slide

custChar LCD_MAN_TOP_DATA = {0xe,0xe,0x15,0xe,0x4,0xe,0x11,0x00};           // spread eagle: both arms and legs up
const uint8_t LCD_MAN_TOP = 2;

custChar LCD_MAN_LAND_DATA = {0xe,0xe,0x14,0xe,0x5,0xe,0x9,0x1};            // we have landing: left arm up, legs to right
const uint8_t LCD_MAN_LAND = 3;                                             // This also can be used as a pre slide

custChar LCD_SPIKE_DATA = {0x0,0x4,0x4,0xe,0xe,0x1f,0x1f,0x1f};             // generic obstacle: spike pointing up
const uint8_t LCD_SPIKE = 4;

custChar LCD_GATE_BOT_DATA = {0x15,0x11,0x0,0x0,0x0,0x0,0x0};               // better duck bro: bottom part of obstacle, no man underneath
const uint8_t LCD_GATE_BOT = 5;

custChar LCD_GATE_TOP_DATA = {0x1f,0x15,0x1f,0x15,0x1f,0x15,0x15,0x15};     // top part of obstacle, attaches to OBS_BOT
const uint8_t LCD_GATE_TOP = 6;

custChar LCD_MAN_SLIDE_DATA = {0x15,0x11,0x0,0x18,0x18,0x6,0xb,0x1};        // sliiiide to the right: bottom part of obstacle, with man underneath
const uint8_t LCD_MAN_SLIDE = 7;




// initialize the LCD library with the numbers of the interface pins

// This line of code is for standard LCD interface:
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// And these two are for SPI interface using http://www.ladyada.net/products/i2cspilcdbackpack/:
// #include <Wire.h>
// LiquidCrystal lcd(12, 13, 11);



// Bounce library, to take care of input buttons
Bounce btnJump = Bounce(JUMPIN, 5); // 5ms bounce
Bounce btnDuck = Bounce(DUCKPIN, 5);



void setup() {
	// start serial for debugglin'
	Serial.begin(115200);

	// initalize randomness
	randomSeed(analogRead(0));

	// setup lcd and input pins
	setupPins();
	
	// set up the LCD's number of columns and rows: 
	lcd.begin(WIDTH, HEIGHT);

	// set up the custom symbols displayed on screen
	initChars();
	
	// reinitialize game, just to make sure everything's ok
	reInitGame();

	// display the symbols used in the game for 3 seconds, then clear
	showSymbols();

	Serial.println("Ready to go!");
}

void loop() {
	checkTime();
}

void reInitGame() {
	// reset to standing
	manState = MAN_STANDING;
	lastManState = -1;
	// clear stage
	for (int i = 0; i < WIDTH; i++) {
		stageData[i] = 0;
	}
	// reset game ticks
	currTick = 0;
	msPerStageTick = MS_PER_STAGE_TICK;
	ticksTillSpeedup = 30;
	lastTickSpeedup = 0;
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
	delay(2000);
	lcd.clear();
}

void checkTime() {
	if (millis() > lastTickMs + msPerStageTick) {
		lastTickMs = millis();
		currTick++;

		if (currTick >= ticksTillSpeedup + lastTickSpeedup) {
			msPerStageTick -= CHG_TICKS_TILL_SPEEDUP;
			ticksTillSpeedup += CHG_TICKS_TILL_SPEEDUP;
			lastTickSpeedup = currTick;
		}

		readButtons();
		drawMan();
		shiftStage();
		dispStage();
		checkCollide();
	}
}

void readButtons() {
	btnJump.update();
	btnDuck.update();
	if (btnJump.fallingEdge()) {
		Serial.println("Jump pressed.");
		// only jump if man is ready to jump (standing or just finished jumping/ducking)
		if (manState == MAN_STANDING ||
			manState == MAN_POST_JUMP ||
			manState == MAN_POST_DUCK) {
			manState = MAN_PRE_JUMP;	
		}
	}
	if (btnJump.risingEdge()) {
		Serial.println("Jump released.");
		// only land if man was jumping
		if (manState == MAN_JUMP) {
			manState = MAN_POST_JUMP;
		}
	}
	if (btnDuck.fallingEdge()) {
		Serial.println("Duck pressed.");
		// only duck if man is ready to duck (standing or just finished jumping/ducking)
		if (manState == MAN_STANDING ||
			manState == MAN_POST_JUMP ||
			manState == MAN_POST_DUCK) {
			manState = MAN_DUCK;
			duckTimer = MAX_TICKS_DUCKING;
		}
	}
	if (btnDuck.risingEdge()) {
		Serial.println("Duck released.");
		// only stand if man was ducking
		if (manState == MAN_DUCK) {
			manState = MAN_STANDING;	
		}
	}
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
				if (i != 0) { // don't overwrite man with blank
					lcd.setCursor(i, 0);
					lcd.write(LCD_BLANK);
					lcd.setCursor(i, 1);
					lcd.write(LCD_BLANK);
				}
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
}

void checkCollide() {
	if (stageData[0] == OBS_SPIKE) {
		if (manState != MAN_JUMP && manState != MAN_POST_JUMP) {
			doCollide(OBS_SPIKE);
		} else {
			obstaclePassed = true;
			lcd.setCursor(0, 0);
			lcd.write(LCD_MAN_TOP);
		}
	} else if (stageData[0] == OBS_GATE) {
		if (manState != MAN_DUCK) {
			doCollide(OBS_GATE);
		} else {
			obstaclePassed = true;
			doPostDuck = true;
			lcd.setCursor(0, 1);
			lcd.write(LCD_MAN_SLIDE);
		}
	}
}

void doCollide(uint8_t typeObstacle) {
	Serial.print("Collision: ");
	if (typeObstacle == OBS_SPIKE) {
		Serial.println("Spike");
	} else if (typeObstacle == OBS_GATE) {
		Serial.println("Gate");
	} else {
		Serial.println("Unknown");
	}
	delay(10);
	setup();
}

void drawMan() {

	// if man was starting to jump last tick, make him jump and set the hang timer
	if (lastManState == MAN_PRE_JUMP) {
		manState = MAN_JUMP;
		jumpTimer = TICKS_PER_JUMP;
	}

	// if man is in the air, check the hang time
	if (manState == MAN_JUMP) {
		if (jumpTimer == 0) {
			manState = MAN_POST_JUMP;
		} else {
			jumpTimer--;
		}
	}

	// if man was landing last tick, make him standing
	if (lastManState == MAN_POST_JUMP) {
		manState = MAN_STANDING;
	}

	// if man just passed under a gate, finish the slide
	if (doPostDuck) {
		doPostDuck = false;
		manState = MAN_POST_DUCK;
	}

	// if man is ducking, check the duck timer and stand him up if duck has been held too long
	if (lastManState == MAN_DUCK) {
		if (duckTimer == 0) {
			manState = MAN_STANDING;
		} else {
			duckTimer--;
		}
	}

	if (lastManState == MAN_POST_DUCK) {
		manState = MAN_STANDING;
	}


	if (manState == MAN_STANDING ||
	    manState != lastManState ||
	    obstaclePassed) { // man state has changed, redraw man
		lastManState = manState;
		switch (manState) {
			case MAN_STANDING:
				lcd.setCursor(0, 0);
				lcd.write(LCD_BLANK);
				lcd.setCursor(0, 1);
				if (manRunningDisp) {
					lcd.write(LCD_MAN);
					manRunningDisp = false;
				} else {
					lcd.write(LCD_MAN_JUMP);
					manRunningDisp = true;
				}
				break;
			case MAN_PRE_JUMP:
				lcd.setCursor(0, 0);
				lcd.write(LCD_BLANK);
				lcd.setCursor(0, 1);
				lcd.write(LCD_MAN_JUMP);
				break;
			case MAN_JUMP:
				lcd.setCursor(0, 0);
				lcd.write(LCD_MAN_TOP);
				lcd.setCursor(0, 1);
				lcd.write(LCD_BLANK);
				// super hacky, this handles the edge case where you release jump before the jump even starts
				if (btnJump.read() == HIGH) {
					manState = MAN_POST_JUMP;
				}
				break;
			case MAN_POST_JUMP:
				lcd.setCursor(0, 0);
				lcd.write(LCD_MAN_LAND);
				lcd.setCursor(0, 1);
				lcd.write(LCD_BLANK);
				break;
			case MAN_DUCK:
				lcd.setCursor(0, 0);
				lcd.write(LCD_BLANK);
				lcd.setCursor(0, 1);
				lcd.write(LCD_MAN_LAND);
				break;
			case MAN_POST_DUCK:
				lcd.setCursor(0, 0);
				lcd.write(LCD_BLANK);
				lcd.setCursor(0, 1);
				lcd.write(LCD_MAN_JUMP);
				break;
		}
	}
}

void rndMakeObstacle() {
	if (obstacleCooldown > 0) {
		obstacleCooldown--;
		stageData[15] = OBS_NONE;
	} else {
		int rnd = random(20); // generates numbers from 0 to 19
		if (rnd == 0) { // 5% chance
			lastObstacleTick = currTick;
			obstacleCooldown = TICKS_BTWN_OBSTACLES;
			stageData[15] = OBS_SPIKE;
		} else if (rnd == 1) {
			lastObstacleTick = currTick;
			obstacleCooldown = TICKS_BTWN_OBSTACLES;
			stageData[15] = OBS_GATE;
		} else {
			stageData[15] = OBS_NONE;
		}
	}
}