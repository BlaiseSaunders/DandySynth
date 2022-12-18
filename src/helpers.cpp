#include "header.h"


// Forward declaration of static vars from class
uint32_t DandySynth::noteTime;
byte DandySynth::lastNote;
byte DandySynth::controlValues[256];
byte DandySynth::lastChan;
byte DandySynth::lastCCVal;
byte DandySynth::lastCCNum;
byte DandySynth::lastVel;
uint32_t DandySynth::noteTimes[OSCIS+1];
uint32_t DandySynth::noteIndexLookup[NOTE_RANGE];
byte DandySynth::lastNotes[OSCIS+1];

float naive_lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

unsigned int secToMicro(float sec)
{
	return (float)pow(10, 6) * sec;
}

void DandySynth::generateWaveTables()
{
	// fill table
	for (int i = 0; i < WAVE_SIZE; i++)
		sine[i] = 0.5 + 0.5 * sin(i * PI / 180);
	for (int i = 0; i < WAVE_SIZE/2; i++)
		square[i] = 1.;
	for (int i = WAVE_SIZE/2; i < WAVE_SIZE; i++)
		square[i] = 0;

	// LERP Table between square and sine
	for (int j = 0; j < WAVETABLE_SIZE; j++)
		for (int i = 0; i < WAVE_SIZE; i++)
		{
			float u = (float)j/(float)WAVETABLE_SIZE;
			float d = 1.0-u;
			table[j][i] = (square[i]*u) + (sine[i]*d);
		}

}

float DandySynth::freqToTableIndex(float freq, uint32_t now)
{
	int period = 1e6 / freq;
	float t = now % period;
	return (WAVE_SIZE * t) / period;
}

float DandySynth::freqToTableIndex(float freq, uint32_t now, int *rem)
{
	int period = 1e6 / freq;
	*rem = (int) now / period;
	float t = now % period;
	return (WAVE_SIZE * t) / period;
}

float DandySynth::freqToRadIndex(float freq, uint32_t now)
{
	float idx = freqToTableIndex(freq, now);
	return idx * PI / 180.0;
}

float DandySynth::getNoteWave(float freq, uint32_t now, int slice)
{
	if (slice < 0)
		slice = 0;
	if (slice >= WAVETABLE_SIZE)
		slice = WAVETABLE_SIZE;
	int rem;
	int idx = freqToTableIndex(freq, now, &rem);
	rem /= 2;
	rem %= WAVETABLE_SIZE-1;

	float pos = table[rem][idx]/2.;
	return pos;
}

float DandySynth::getNoteSquare(float freq, uint32_t now)
{
	int idx = freqToTableIndex(freq, now);
	float pos = square[idx]/2.;
	return pos;
}
float DandySynth::getNoteSine(float freq, uint32_t now)
{
	int idx = freqToTableIndex(freq, now);
	float pos = sine[idx]/2.;
	return pos;
}
float DandySynth::getNoteSineR(float freq, uint32_t now)
{
	float radidx = freqToRadIndex(freq, now);
	float fradidx = freqToRadIndex(params[0]*32.0, now);

	return (sin(radidx + sin(fradidx))/2.0 + 0.5)*0.5;
	//return radidx > 180;
	//return (int)(sin(radidx + sin(fradidx))/2. + 0.5);
	//return (float)square[(int)radidx + (int)(sin(fradidx))] / 2. + 0.5;
}
float DandySynth::getNoteSineGoofy(float freq, uint32_t now)
{
	float period = 1e6 / freq;
	float t = fmod(now, period);
	return (t * PI / 180)/2.;
}


void DandySynth::handleNoteOff(byte channel, byte pitch, byte velocity)
{
	int noteIndex = noteIndexLookup[pitch];
	Serial.print("Note Off: ");
	Serial.print(pitch);

	if (noteIndex < 0 || noteIndex > OSCIS)
		return; // HES GONE

	Serial.print("\tNoteIndex: ");
	Serial.println(noteIndex);


	noteTimes[noteIndex] = micros();
}


void DandySynth::handleNoteOn(byte channel, byte pitch, byte velocity)
{
	// Do some stuff with NoteOn here
	lastNote = pitch;
	noteIndexLookup[pitch] = 0;
	Serial.print("Note On: ");
	Serial.print(pitch);
	lastChan = channel;
	lastVel = velocity;
	noteTime = 0;

	//bool hasInc[NOTE_RANGE];
	//memset(hasInc, 0, sizeof (bool) * NOTE_RANGE);
	for (int i = OSCIS; i > 0; i--)
	{
		Serial.print("\tSetting: ");
		Serial.print(lastNotes[i]);
		Serial.print(" to: ");
		Serial.print(noteIndexLookup[lastNotes[i]]+1);
		int idx = lastNotes[i];
		//if (!hasInc[idx])
		//{
		noteIndexLookup[idx]++;
		//	hasInc[idx] = true;
		//}

		lastNotes[i] = lastNotes[i-1];
		noteTimes[i] = noteTimes[i-1];
	}
	Serial.println();
	lastNotes[0] = lastNote;
	noteTimes[0] = noteTime;
}

void DandySynth::handleMIDIControlChange(byte channel, byte number, byte value)
{
	lastCCNum = number;
	lastCCVal = value;
	controlValues[value] = number;
}

void DandySynth::setParameterArray(std::vector<float> params) { this->params = params; }

void DandySynth::setEncPos(int pos) { this->encPos = pos; }

void DandySynth::setEncPush(int pushed) { this->encPush = pushed; }

float DandySynth::noteToFreq(float note)
{
	float exp = 2.0 * note * (1.0f / 12.0f) + 3.0313597f;
	return powf(2.0f, exp);

	double a = pow(2, 1.0 / 12.0);
	float freq = 40 * pow(a, note + 3);
	return freq;
}