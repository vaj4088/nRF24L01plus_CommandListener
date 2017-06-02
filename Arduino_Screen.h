/*
 *  Arduino_GPS1.h
 *
 *  Created on: Feb 10, 2016
 *      Author: Ian Shef
 */

// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef Arduino_GPS1_H_
#define Arduino_GPS1_H_
#include "Arduino.h"
//add your includes for the project Arduino_GPS1 here

//end of add your includes here

//add your function definitions for the project Arduino_GPS1 here

bool moveCursorTo(const int row, const int col) ;
bool setBold() ;
bool setNormal() ;
bool eraseScreen() ;
bool eraseInLine(char dir) ;
bool eraseLineToLeft() ;
bool eraseLineToRight() ;
bool eraseWholeLine() ;
bool hideCursor() ;
bool sendCSI() ;
bool doubleSize(const int row, const int col, const char * string) ;
boolean screenSetup(unsigned long rate, void (*f)(void)) ;
boolean screenUpdateItem(byte line, byte column, byte value) ;
boolean screenUpdate() ;
bool setCharacterAttributesOff() ;
bool setCharacterAttributeUnderscore() ;
bool setCharacterAttributeBlink() ;
bool setCharacterAttributeReverseVideo() ;


//Do not add code below this line
#endif /* Arduino_GPS1_H_ */
