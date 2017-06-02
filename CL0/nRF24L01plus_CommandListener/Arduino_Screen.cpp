/*
 *  Arduino_Screen.cpp
 *
 *  Created on: May 12, 2017
 *      Author: Ian Shef
 *
 */

// Do not remove the include below
#include "Arduino_Screen.h"

#include "VT100_Control_Codes.h"

unsigned long millisOfLastScreenUpdate ;

/*
void loop() {
	if (true) {
		if (true) // this also sets the
			// newNMEAreceived() flag to false
			return; // we can fail to parse a sentence in which case
		// we should just wait for another
		screenUpdate() ;
		millisOfLastScreenUpdate = millis() ;
	}
	unsigned long millisCurrentTime = millis() ;
	if (millisCurrentTime-millisOfLastScreenUpdate>millisScreenUpdateInterval) {
		screenUpdate() ;
		millisOfLastScreenUpdate = millisCurrentTime ;
	}
}
*/
//
// Parameter 1 is an unsigned long, such as 9600 or 115200.
// Parameter 2 is the name of a function that takes no parameters and
// returns no value.
boolean screenSetup(unsigned long rate, void (*f)(void)) {
	Serial.begin(rate) ;
	eraseScreen() ;
	f() ;
	return true ;
}

bool moveCursorTo(const int row, const int col) {
	if ((row<1)||(row>24)||(col<1)||(col>80)) return false ;
	sendCSI() ;
	Serial.print(row) ;
	Serial.print(';') ;
	Serial.print(col) ;
	Serial.print('f') ;
	return true ;
}

bool setBold() {
	sendCSI() ;
	Serial.print("1m") ;
	return true ;
}

bool setNormal() {
	sendCSI() ;
	Serial.print("m") ;
	return true ;
}

bool eraseScreen() {
	sendCSI() ;
	Serial.print("2J") ;
	return true ;
}

bool eraseInLine(char dir) {
	sendCSI() ;
	Serial.print(dir) ;
	Serial.print('K') ;
	return true ;
}

bool eraseLineToLeft() {
	return eraseInLine('1') ;
}

bool eraseLineToRight() {
	return eraseInLine('0') ;
}

bool eraseWholeLine() {
	return eraseInLine('2') ;
}

bool hideCursor() {
	sendCSI() ;
	Serial.print("?25l") ;
	return true ;
}

bool sendCSI() {
	Serial.print((char)ESC) ;
	Serial.print('[') ;
	return true ;
}

bool doubleSize(const int row, const int col, const char * string) {
	moveCursorTo(row, col/2) ;
	eraseWholeLine() ;
	Serial.print((char)ESC) ;
	Serial.print("#6") ;      // DEC double-width line (DECDWL)
	Serial.print((char)ESC) ;
	Serial.print("#3") ;      // DEC double-height line, top half (DECDHL)
	Serial.print(string) ;
	moveCursorTo(row+1, col/2) ;
	eraseWholeLine() ;
	Serial.print((char)ESC) ;
	Serial.print("#6") ;      // DEC double-width line (DECDWL)
	Serial.print((char)ESC) ;
	Serial.print("#4") ;      // DEC double-height line, bottom half (DECDHL)
	Serial.print(string) ;
	return true ;
}

bool setCharacterAttributesOff() {
	sendCSI() ;
	Serial.print("0m") ;
	return true ;
}

bool setCharacterAttributeBold() {
	sendCSI() ;
	Serial.print("1m") ;
	return true ;
}

bool setCharacterAttributeUnderscore() {
	sendCSI() ;
	Serial.print("4m") ;
	return true ;
}

bool setCharacterAttributeBlink() {
	sendCSI() ;
	Serial.print("5m") ;
	return true ;
}

bool setCharacterAttributeReverseVideo() {
	sendCSI() ;
	Serial.print("7m") ;
	return true ;
}
