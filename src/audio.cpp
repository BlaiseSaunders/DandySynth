#include "header.h"

int16_t DandySynth::shaper(int16_t pos, float t)
{
	t = 1.0-t; // Ranges between 0.0 and 1.0


	float at = 0.02;
	float a = 1.0;
	float st = 0.5*p1;
	float s = 0.9;
	float r = 0.4;
	float rs = 0.3; // Higher is quicker release

	if (t < at)
	{
		t *= 1.0/at; // Get range from 0 to 1
		pos *= max(a*t, (float)0.0);
	}
	else if (t < st)
		pos *= s;
	else
	{

		t -= st;
		t *= 1.0/(1.0-st)+rs; // Get range from 0 to 1
		t = 1.0-t;
		pos *= max(r*t, (float)0.0);
	}

	return pos;
}


float DandySynth::noteToFreq(float note)
{
	double a = pow(2, 1.0/12.0);
	float freq = 110*pow(a, note+3);

	return freq;
}

void DandySynth::run(uint32_t now)
{
	int16_t pos = 0;
	for (int i = 0; i < OSCIS; i++) // Output from each of our oscillators
	{
		float note = (int)lastNotes[i]-36;

		float noteAmp = getNoteSquare(noteToFreq(note), now)/1.5;

		float notePos = max(1.0-((now-noteTimes[i])/(1000000.0*(1.5*p0))), 0.0); // time frame for shaper
		
		notePos = shaper(noteAmp, notePos);

		lastAmp[i] = noteAmp;
		pos += notePos;
	}
	MCP.fastWriteA(pos);

	outBuffer[bufPos%BUFSIZE] = (int)pos * 128 / 4096; // Log what we've generated to a circular buffer for later rendering
	bufPos++;
}

void DandySynth::setup()
{
	generateWaveTables();

	MCP.begin(10);
	MCP.fastWriteA(0);
}