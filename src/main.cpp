#include "header.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, DIN_MIDI);

DandySynth *synth;


#include <SPI.h>

float p = 3.1415926;

void setupMIDI()
{
	DIN_MIDI.begin(MIDI_CHANNEL_OMNI);
	DIN_MIDI.setHandleNoteOn(synth->doSomeStuffWithNoteOn);
	delay(100);
}

Encoder myEnc(3, 4);

void setup()
{
	// Setup Serial
	Serial.begin(115200);

	// Setup our pins
	pinMode(14, INPUT_PULLUP);
	pinMode(15, INPUT_PULLUP);
	pinMode(16, INPUT_PULLUP);
	pinMode(17, INPUT_PULLUP);
	pinMode(18, INPUT_PULLUP);
	pinMode(19, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP); // Encoder switch

	synth = new DandySynth();

	setupMIDI();

	synth->setup();
}

long oldPosition  = -999;
bool buttonState = 0;
long lastPress = 0;

void loop()
{
	DIN_MIDI.read();
	yield();


	uint32_t now = micros();

	static uint32_t lastTime = 0;
	//Our Serial Output
	if (now - lastTime > 300000)
	{
		Serial.print("LastNoteval:\t");
		Serial.print(synth->outBuffer[synth->bufPos%BUFSIZE]);
		Serial.print("LastNotevalReal:\t");
		Serial.print(synth->lastNote);
		Serial.print("\tTime:\t");
		Serial.print(now-synth->noteTimes[0]);
		Serial.printf("\tp0: %f\t", synth->p0);
		Serial.printf("\tp1: %f\t", synth->p1);
		Serial.println();
		lastTime = now;
	}

	synth->setP0((analogRead(14)-10)/310.0);
	synth->setP1((analogRead(15)-10)/310.0);
	synth->setP2((analogRead(16)-10)/310.0);
	synth->setP3((analogRead(17)-10)/310.0);
	synth->setP4((analogRead(18)-10)/310.0);
	synth->setP5((analogRead(19)-10)/310.0);


	long newPosition = myEnc.read();
	if (newPosition != oldPosition)
	{
		oldPosition = newPosition;
		Serial.println(newPosition);
	}

	if (!digitalRead(5) && !buttonState && now-lastPress > 300000)
	{
		Serial.println("ENCODED");
		buttonState = 1;
		lastPress = now;
	}
	else if (digitalRead(5))
		buttonState = 0;


	synth->setEncPos(-newPosition/4);
	synth->setEncPush(buttonState); // TODO: DEBOUNCE MORE

	synth->run(now);
}

