#include "header.h"

uint32_t DandySynth::noteTime;
byte DandySynth::lastNote;
byte DandySynth::controlValues[256];
byte DandySynth::lastChan;
byte DandySynth::lastCCVal;
byte DandySynth::lastCCNum;
byte DandySynth::lastVel;
uint32_t DandySynth::noteTimes[OSCIS+1];
byte DandySynth::lastNotes[OSCIS+1];

float naive_lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

void DandySynth::generateWaveTables()
{
	// fill table
	for (int i = 0; i < 361; i++)
		sine[i] = 0.5 + 0.5 * sin(i * PI / 180);
	for (int i = 0; i < 180; i++)
		square[i] = 1.;
	for (int i = 180; i < 360; i++)
		square[i] = 0;

	// LERP Table between square and sine
	for (int j = 0; j < WAVETABLE_SIZE; j++)
		for (int i = 0; i < WAVE_SIZE-1; i++)
		{
			float u = (float)j/(float)WAVETABLE_SIZE;
			float d = 1.0-u;
			table[j][i] = (square[i]*u) + (sine[i]*d);
		}

}

float DandySynth::freqTo360Index(float freq, int now)
{
	int period = 1e6 / freq;
	float t = now % period;
	return (360 * t) / period;
}
float DandySynth::freqToRadIndex(float freq, int now)
{
	float idx = freqTo360Index(freq, now);
	return idx * PI / 180.0;

}

float DandySynth::getNoteWave(float freq, int now, int slice)
{
	if (slice < 0)
		slice = 0;
	if (slice >= WAVETABLE_SIZE-1)
		slice = WAVETABLE_SIZE-1;
	int idx = freqTo360Index(freq, now);
	float pos = table[slice][idx]/2.;
	return pos;
}

float DandySynth::getNoteSquare(float freq, int now)
{
	int idx = freqTo360Index(freq, now);
	float pos = square[idx]/2.;
	return pos;
}
float DandySynth::getNoteSine(float freq, int now)
{
	int idx = freqTo360Index(freq, now);
	float pos = sine[idx]/2.;
	return pos;
}
float DandySynth::getNoteSineR(float freq, int now)
{
	float radidx = freqToRadIndex(freq, now);;
	float fradidx = freqToRadIndex(this->fmFreq, now);

	//return sin(radidx + sin(fradidx))/2. + 0.5;
	return radidx > 180;
	//return (int)(sin(radidx + sin(fradidx))/2. + 0.5);
	//return (float)square[(int)radidx + (int)(sin(fradidx))] / 2. + 0.5;
}
float DandySynth::getNoteSineGoofy(float freq, int now)
{
	float period = 1e6 / freq;
	float t = fmod(now, period);
	return (t * PI / 180)/2.;
}



void DandySynth::handleNoteOn(byte channel, byte pitch, byte velocity)
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

void DandySynth::handleMIDIControlChange(byte channel, byte number, byte value)
{
	lastCCNum = number;
	lastCCVal = value;
	controlValues[value] = number;
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


void DandySynth::setEncPos(int pos) { this->encPos = pos; }

void DandySynth::setEncPush(int pushed) { this->encPush = pushed; }

float DandySynth::noteToFreq(float note)
{
	double a = pow(2, 1.0 / 12.0);
	float freq = 40 * pow(a, note + 3);

	return freq;
}