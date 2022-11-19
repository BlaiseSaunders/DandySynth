#include "header.h"

float DandySynth::envelope(float t, struct DandySynth::EnvelopeSettings set)
{
	t = 1.0 - t; // Ranges between 0.0 and 1.0

	if (t < set.attackTimeEnd)
	{
		t *= 1.0 / set.attackTimeEnd; // Get range from 0 to 1
		return max(set.attackEndValue * t, set.attackStartValue); // Ramp up
	}
	else if (t < set.sustainTimeEnd)
		return set.sustainValue; // Hold
	else
	{
		t -= set.sustainTimeEnd;
		t *= 1.0 / (1.0 - set.sustainTimeEnd) + set.releaseTime; // Get range from 0 to 1
		t = 1.0 - t;
		return max(set.releaseValueStart * t, set.releaseValueEnd); // Ramp down
	}
}

float DandySynth::sampleWave(WAVE_TYPE waveType, float freq, int now, float param)
{
	float out = 0.0;

	switch (waveType)
	{
		default:
		case SINE:
			out = getNoteSine(freq, now);
			break;
		case COMBO:
			out = getNoteSquare(freq, now) * getNoteSine(freq, now) * 2.;
			break;
		case SQUARE:
			out = getNoteSquare(freq, now);
			break;
		case REAL:
			out = getNoteSineR(freq, now);
			break;
		case WAVETABLE:
			out = getNoteWave(freq, now, param);
			break;
	}

	return out;
}

void DandySynth::processLFOs(int now)
{
	for (int i = 0; i < LFOS; i++) // Programmable oscis
	{
		float note = controlValues[4+i] * 0.5;

		float freq = noteToFreq(note);
		lfoWaveValues[i] = getNoteSine(freq, now);
	}
}

float DandySynth::getOscilatorsOuput(int now)
{
	float pos = 0;
	float slices[OSCIS];
	float amps[OSCIS];
	float sum = 0;
	this->velParam = (float)lastVel / 128.0;
	for (int i = 0; i < OSCIS; i++) // Output from each of our oscillators
	{
		float note = (int)lastNotes[i] - 36;

		float freq = noteToFreq(note);

		this->waveTablePos = lfoWaveValues[0] * 20.0 + 1.0;

		sampleWave((DandySynth::WAVE_TYPE)(controlValues[0]), freq, now, this->waveTablePos);

		float noteDurationPercent = (float)controlValues[2] / 128.0;
		float actualNoteDuration = secToMicro(noteDurationPercent);

		float notePos = max(1.0 - (now - noteTimes[i]) / actualNoteDuration, 0.0); // time frame for envelope

		amps[i] = envelope(notePos, basicOneShotEnv);

		sum += amps[i];
	}

	// Make sure amplitudes of waves sum to < 1
	for (int i = 0; i < OSCIS; i++)
	{
		if (sum > 1.0)
			pos += slices[i] * (amps[i] / sum);
		else
			pos += slices[i] * amps[i];
	}

	return pos;

}

void DandySynth::run(uint32_t now)
{
	display->encoderPosPush(encPos, encPush);
	uint32_t snow = micros();
	static uint32_t lastTime = 0;

	if (snow - lastTime > secToMicro(0.3))
	{
		display->runDisplay();
		lastTime = snow;
	}

	this->fmFreq = controlValues[3];

	this->processLFOs(now);

	float oscilatorOutput = getOscilatorsOuput(now);

	// Output our 0-4096 wave position to DAC
	MCP.fastWriteA(oscilatorOutput * 4096.0);

	// Log what we've generated to a circular buffer for later rendering
	outBuffer[bufPos++ % BUFSIZE] = (int)oscilatorOutput * 128 / 4096;
}

void DandySynth::setup()
{
	generateWaveTables();

	MCP.begin(10);
	MCP.fastWriteA(0);

	display = new DandyDisplay();
}