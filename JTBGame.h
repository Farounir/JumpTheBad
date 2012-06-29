/* * *
 * Jump The Bad Game
 * Author: Ethan Massey
 * 
 * This is the outline for the actual c file
 */
#include <LiquidCrystal.h>

const int numChar = 8; //the number of characters needed to create a custom lcd character
typedef uint8_t custChar[numChar];

//custom graphics
custChar MAN_A = {0xe,0xe,0x4,0xe,0x15,0xe,0x1b,0x00};
const uint8_t MAN = 0;

custChar MAN_JUMP_A = {0xe,0xe,0x5,0xe,0x14,0xe,0x12,0x00};
const uint8_t MAN_JUMP = 1;

custChar MAN_TOP_A = {0xe,0xe,0x15,0xe,0x4,0xe,0x11,0x00};
const uint8_t MAN_TOP= 2;

custChar MAN_LAND_A = {0xe,0xe,0x14,0xe,0x5,0xe,0x9,0x00};
const uint8_t MAN_LAND = 3;

custChar OBS_A = {0x0,0x0,0x4,0xa,0x15,0xa,0x15,0x00};
const uint8_t OBS = 4;

const int WIDTH = 16;  //number of lcd characters long
const int HEIGHT = 2;  //number of lcd characters high

const int JUMPIN = 6;  //pin to read for sweet jumping action!
/*
class JTBGame {

public:
JTBGame(); //constructor
void increment(); //increments everything in the lcdChar array over one to simulate movement
void updateDisplay(); //method to update the display to the state of the lcdChar array
bool isAlive(); //returns if the man is alive or not
int buttonState(); //returns current state of the button
int lastState(); //returns last know state

private:
int buttonState;  //variable for reading the button
int lastState;  //keeps track of the last state of the button

bool alive; //keeps track of whether man is alive or not i.e. if game goes on
int manPos; //keeps track if man is standing, jumping up, has jumped, or is landing using an int 0-4

custChar lcdChar[WIDTH][HEIGHT]; //a 2d array that holds cust characters and is used to keep track of game state
};*/