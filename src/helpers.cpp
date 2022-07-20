#include "header.h"

uint32_t DandySynth::noteTime;
byte DandySynth::lastNote;
byte DandySynth::lastChan;
byte DandySynth::lastVel;
uint32_t DandySynth::noteTimes[OSCIS+1];
byte DandySynth::lastNotes[OSCIS+1];



void DandySynth::generateWaveTables()
{
	// fill table
	for (int i = 0; i < 361; i++)
		sine[i] = 0.5 + 0.5 * sin(i * PI / 180);
	for (int i = 0; i < 180; i++)
		square[i] = 1.;
	for (int i = 180; i < 360; i++)
		square[i] = 0;

}

float DandySynth::getNoteSquare(float freq, int now)
{
	int period = 1e6 / freq;
	float t = now % period;
	int idx = (360 * t) / period;
	float pos = square[idx]/2.;
	return pos;
}
float DandySynth::getNoteSine(float freq, int now)
{
	int period = 1e6 / freq;
	float t = now % period;
	int idx = (360 * t) / period;
	float pos = sine[idx]/2.;
	return pos;
}
float DandySynth::getNoteSineR(float freq, int now)
{
	float period = 1e6 / freq;
	float t = fmod(now, period);
	return (sin((16.0*p3*t)/period)/4.)+0.5;
}


void DandySynth::doSomeStuffWithNoteOn(byte channel, byte pitch, byte velocity)
{
	// Do some stuff with NoteOn here
	lastNote = pitch;
	lastChan = channel;
	lastVel = velocity;
	noteTime = micros();

	for (int i = OSCIS; i > 0; i--)
	{
		lastNotes[i] = lastNotes[i-1];
		noteTimes[i] = noteTimes[i-1];
	}
	lastNotes[0] = lastNote;
	noteTimes[0] = noteTime;
}

void DandySynth::setP0(float p)
{
	this->p0 = p;
}
void DandySynth::setP1(float p)
{
	this->p1 = p;
}
void DandySynth::setP2(float p)
{
	this->p2 = p;
}
void DandySynth::setP3(float p)
{
	this->p3 = p;
}
void DandySynth::setP4(float p)
{
	this->p4 = p;
}
void DandySynth::setP5(float p)
{
	this->p5 = p;
}