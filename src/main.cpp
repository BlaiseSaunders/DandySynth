#include "header.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, DIN_MIDI);

DandySynth *synth;

void setup()
{
	pinMode(14, INPUT_PULLUP);
	pinMode(15, INPUT_PULLUP);
	
	Serial.begin(115200);

	synth = new DandySynth();

	DIN_MIDI.begin(MIDI_CHANNEL_OMNI);
	DIN_MIDI.setHandleNoteOn(synth->doSomeStuffWithNoteOn);
	delay(100);

	synth->setup();
}
void loop()
{
	DIN_MIDI.read();
	yield();

	float p0 = (analogRead(14)-10)/310.0;
	float p1 = (analogRead(15)-10)/310.0;
	synth->setP0(p0);
	synth->setP1(p1);

	uint32_t now = micros();
	static uint32_t lastTime = 0;
	// Our Serial Output
	if (now - lastTime > 300000)
	{
		Serial.print("LastNoteval:\t");
		Serial.print(synth->outBuffer[synth->bufPos%BUFSIZE]);
		Serial.print("\tTime:\t");
		Serial.print(now-synth->noteTime);
		for (int i = 0; i < OSCIS; i++)
		{
			Serial.printf("\tAmp %d:\t", i);
			Serial.print(synth->lastAmp[i]);
		}

		Serial.printf("\tp0: %f\t", p0);
		Serial.printf("\tp1: %f\t", p1);
		Serial.println();
		lastTime = now;
	}

	synth->run(now);

}

