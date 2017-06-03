/* myRF24RC
 * Copyright (C) 2016 Michael Melchior <Michael.MelchiorGM@gmail.com>
 *
 * It is free software: You can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// Do not remove the include below
#include "nRF24L01plus_CommandListener.h"
/*
 * This program is using the Optimized High Speed NRF24L01+ Driver Class
 * An optimized fork of the NRF24L01+ Driver written by
 * J.Coliz <maniacbug@ymail.com>
 * licensed under the GNU GPL v2 license.
 * It can be found in Js' github repository under
 * http://tmrh20.github.io/RF24/index.html.
 * Check the License.txt for more information.
 * This program was modified by IBS  Ian Shef  11 May 2017.
 */
#include "RF24.h"

// Comment to not get Serial debug message.
// Uncomment for Serial debug message.
// #define RC_DEBUG

const unsigned long version = 20170601 ; //
const char versionSuffix = 'a' ;

RF24 myRF24(8, 10);

// pipes share the	four most significant bytes
// 					least significant byte is unique for each pipe
// Hex is DF 19 55 57 C1 / C3 / C4 / C5 / C6
// Command: MSBit first to LSBit
// Data: LSByte first to MSByte, MSBit first to LSBit in each byte
uint8_t myAddress[][5] = {{223, 25, 85, 87, 193}, {195}, {196}, {197}, {198}};

uint8_t commandMessage[] = {0, 128, 128, 128, 128, 128, 128, 0};

// buffer for receiving
const int bufferSize = 8;
uint8_t myBuffer[bufferSize];

// User commands and input position.
byte hotItemNumber = 0 ;
int userCommand[8] = {0, 0, 0, 0, 0, 0, 0, 0} ;
float userCommandTimeSeconds ;
float userCommandTimeSecondsInitial ;

unsigned long commandTimer;
unsigned long commandThresh = 20;

// initialize the receiver buffer
void initBuffer() {
	memset(myBuffer, 0, bufferSize);
} // void initBuffer() {

// initialize the RF24 transceiver
void initRF24() {
	myRF24.begin();

	// needed, since the nRF24L01 is not resetting itself at startup!
	myRF24.stopListening();

	myRF24.setAutoAck(true);

	myRF24.setAddressWidth(5);

	// setting for 10 retries with 500ms limit
	myRF24.setRetries(2, 10);

	myRF24.setChannel(60);

	myRF24.setPayloadSize(8);

	myRF24.openReadingPipe(2, myAddress[1]);
	myRF24.openReadingPipe(3, myAddress[2]);
	myRF24.openReadingPipe(4, myAddress[3]);
	myRF24.openReadingPipe(5, myAddress[4]);

	// RF24_PA_MIN RF24_PA_LOW RF24_PA_HIGH RF24_PA_MAX
	myRF24.setPALevel(RF24_PA_HIGH);

	myRF24.setDataRate(RF24_1MBPS);

	myRF24.enableDynamicPayloads();
	myRF24.enableDynamicAck();
	myRF24.enableAckPayload();

	myRF24.setCRCLength(RF24_CRC_8);
}

// Setting up the writing and the reading pipes.
void setUpPipes() {
	myRF24.openWritingPipe(myAddress[0]);
	myRF24.openReadingPipe(0, myAddress[0]);
} // void pair() {

// Return number of seconds.
float seconds() {
	return millis()/1000.0 ;  //  The ".0" is important to make this a float.
}

boolean screenUpdateItem(byte _line, byte _column, int _value) {
	moveCursorTo(_line, _column) ;
	int x = _value ;
	if (x>=0) {
		Serial.print("+") ;
	} else {
		Serial.print("-") ;
	}
	x = abs(x) ;
	byte leadingSpaces = 2-(int)log10(x) ;
	for (byte j = 1; j<=leadingSpaces; j++) {
		Serial.print(" ") ;
	}
	Serial.print(x) ;
	return true ;
}

boolean screenUpdate(int _hotItemNumber, int _item[8]) {
	//
	// hotItemNumber is in the range [0,7].
	// Each item is in the range [-128, 127].
	//
	moveCursorTo(3, 1);
	eraseWholeLine();
	setCharacterAttributesOff() ;
	byte column = 2 ;
	for (byte i = 0; i<8; i++) {
		if (i == _hotItemNumber) setCharacterAttributeReverseVideo() ;
		screenUpdateItem(3, column, _item[i]) ;
		if (i == _hotItemNumber) setCharacterAttributesOff() ;
		column +=9 ;
	}

	return true ;
}

void localScreenSetup() {
	moveCursorTo(1, 1) ;
	setBold() ;
	userCommandTimeSecondsInitial = seconds() ;
	//
	// Width for each item is 9 characters (9 columns).
	//
	Serial.println(F("0->Full  Fwd/Aft  Left/Rt")) ;
	Serial.print(  F("Throttle Pitch    Roll     PitchTrm RollTrm? ")) ;
	Serial.print(F("???????? ???????? Flags (128=>Lights Off)")) ;
	setNormal() ;
	moveCursorTo(10, 1) ;
	Serial.print(F("Use up & down arrows to change settings, and ")) ;
	Serial.print(F("use left & right arrows to change function.")) ;
	moveCursorTo(12, 1) ;
	Serial.print(F("Version ")) ;
	Serial.print(version) ;
	Serial.print(versionSuffix) ; hideCursor() ;
}

void setup() {
	screenSetup(115200UL, localScreenSetup) ;
	screenUpdate(hotItemNumber, userCommand) ;

	// init receiver buffer
	initBuffer();

	// init transceiver
	initRF24();

	// Set up the reading pipe (and the unnecessary writing pipe).
	setUpPipes();

//	Wait a few seconds before sending the first command.
 	moveCursorTo(14, 1) ; Serial.println(F("Delaying...")) ;
	delay(4000) ;

	// set the timer for the command intervals
	commandTimer = millis();

	Serial.println("OK.");
} // void setup() {
// get the data for commands every <commandThresh> ms
// calculate the controller values
// send the data to the quadcopter
void loop() {
	// check timer for command input
	if (millis() - commandTimer > commandThresh) {

		// reset command timer
		commandTimer = millis();
	} // if (millis() - commandTimer > commandThresh) {
} // void loop() {
