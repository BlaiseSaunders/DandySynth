#include "header.h"

float DandySynth::envelope(float t, struct DandySynth::EnvelopeSettings set)
{
	t = 1.0 - t; // Ranges between 0.0 and 1.0

	if (t < set.attackTimeEnd)
	{
		t *= 1.0 / set.attackTimeEnd; // Get range from 0 to 1
		return max(set.sustainValue * t, set.attackStartValue); // Ramp up
	}
	else if (t < set.sustainTimeEnd)
		return set.sustainValue; // Hold
	else
	{
		t -= set.sustainTimeEnd;
		t *= 1.0 / (1.0 - set.sustainTimeEnd) + set.releaseTime; // Get range from 0 to 1
		t = 1.0 - t;
		return max(set.sustainValue * t, set.releaseValueEnd); // Ramp down
	}
}

float DandySynth::sampleWave(WAVE_TYPE waveType, float freq, uint32_t now, float param)
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

void DandySynth::processLFOs(uint32_t now)
{
	for (int i = 0; i < LFOS; i++) // Programmable oscis
	{
		static_assert(LFOS <= POT_COUNT, "Need enough Pots for all the LFOs");
		float freq = params[i]*32.0;
		lfoWaveValues[i] = getNoteSine(freq, now);
	}
}

float DandySynth::getOscilatorsOuput(uint32_t now)
{
	float pos = 0;
	float slices[OSCIS];
	float amps[OSCIS];
	float sum = 0;
	this->velParam = (float)lastVel / 128.0;
	for (int i = 0; i < OSCIS; i++) // Output from each of our oscillators
	{
		float note = (int)lastNotes[i] - 36;

		float freq = noteToFreq(note);//+lfoWaveValues[0];

		this->waveTablePos = 0;//lfoWaveValues[0] * 20.0 + 1.0;

		slices[i] = sampleWave((DandySynth::WAVE_TYPE)(controlValues[0]), freq, now, this->waveTablePos);

		float noteDurationPercent = (float)controlValues[2] / 128.0;
		float actualNoteDuration = secToMicro(noteDurationPercent);

		float notePos = 1.0;

		if (noteTimes[i])
		{
			notePos = max(1.0 - (now - noteTimes[i]) / actualNoteDuration, 0.0); // time frame for envelope
			float envStartValue = envelope(fmod(noteTimes[i] / (float)secToMicro(0.7), 1.0), funkyLoopEnv);
			struct EnvelopeSettings fallOffEnv = basicOneShotEnv;
			fallOffEnv.attackStartValue = envStartValue;
			amps[i] = envelope(notePos, fallOffEnv);
		}
		else
			amps[i] = envelope(fmod(now / (float)secToMicro(0.7), 1.0), flatEnv);
			// amps[i] = envelope(fmod(now / (float)secToMicro(0.7), 1.0), funkyLoopEnv); // TODO: PUT BACK


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

	display->runDisplay();

	this->fmFreq = controlValues[3];

	this->processLFOs(now);

	float oscilatorOutput = getOscilatorsOuput(now);

	// Output our 0-4096 wave position to DAC
	MCP.fastWriteA(oscilatorOutput * 4096.0);

	// Log what we've generated to a circular buffer for later rendering
	outBuffer[bufPos++ % BUFSIZE] = (float)oscilatorOutput * 32.0;
}

void DandySynth::setup()
{
	generateWaveTables();

	MCP.begin(10);
	MCP.fastWriteA(0);

	display = new DandyDisplay();
	display->outBuffer = this->outBuffer;
}