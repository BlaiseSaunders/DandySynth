#include "header.h"

float DandySynth::shaper(float t)
{
	t = 1.0-t; // Ranges between 0.0 and 1.0


	float at = 0.1;
	float a = 1.0;
	float st = 0.5;
	float s = 0.9;
	float r = 0.9;
	float rs = 0.3; // Higher is quicker release

	if (t < at)
	{
		t *= 1.0/at; // Get range from 0 to 1
		return max(a*t, (float)0.0); // Ramp up
	}
	else if (t < st)
		return s; // Hold
	else
	{
		t -= st;
		t *= 1.0/(1.0-st)+rs; // Get range from 0 to 1
		t = 1.0-t;
		return max(r*t, (float)0.0); // Ramp down
	}
}


void DandySynth::setEncPos(int pos)
{
	this->encPos = pos;
}

void DandySynth::setEncPush(int pushed)
{
	this->encPush = pushed;
}

float DandySynth::noteToFreq(float note)
{
	double a = pow(2, 1.0/12.0);
	float freq = 40*pow(a, note+3);

	return freq;
}

void DandySynth::run(uint32_t now)
{
	display->encoderPosPush(encPos, encPush);
	uint32_t snow = micros();
	static uint32_t lastTime = 0;
	if (snow - lastTime > 300000)
	{
		display->runDisplay();
		lastTime = snow;
	}

	float pos = 0;
	float slices[OSCIS];
	float amps[OSCIS];
	float sum = 0;
	this->velParam = (float)lastVel/128.0;
	for (int i = 0; i < OSCIS; i++) // Output from each of our oscillators
	{
		float note = (int)lastNotes[i]-36;

		float freq = noteToFreq(note);

		//float mod = getNoteSineR(5.0*p3, now)+1.0;

		if (controlValues[0] == 0)
			slices[i] = getNoteSine(freq, now);
		else if (controlValues[0] == 1)
			slices[i] = getNoteSquare(freq, now)*getNoteSine(freq, now)*2.;
		else if (controlValues[0] == 2)
			slices[i] = getNoteSquare(freq, now);
		else if (controlValues[0] == 3)
			slices[i] = getNoteSineR(freq, now);
		else if (controlValues[0] == 4)
			slices[i] = getNoteWave(freq, now, this->lastVel);
			

		//slices[i] /= 2.0;
		//slices[i] *= mod;


		float notePos = max(1.0-((now-noteTimes[i])/(1000000.0*(1.5*(float)controlValues[2]/128.0))), 0.0); // time frame for shaper
		amps[i] = shaper(notePos);

		sum += amps[i];
	}

	for (int i = 0; i < OSCIS; i++)
	{
		if (sum > 1.0)
			pos += slices[i]*(amps[i]/sum);
		else
			pos += slices[i]*amps[i];
	}
		
	float noteS = getNoteSquare(440, now);

	MCP.fastWriteA(pos*4096.0);

	outBuffer[bufPos % BUFSIZE] = (int)pos * 128 / 4096; // Log what we've generated to a circular buffer for later rendering
	bufPos++;
}

void DandySynth::setup()
{
	generateWaveTables();

	MCP.begin(10);
	MCP.fastWriteA(0);

	display = new DandyDisplay();
}