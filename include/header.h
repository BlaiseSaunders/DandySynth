#ifndef __HEADER
#define __HEADER

#include <vector>
#include <string>
#include <functional>
#include <MIDI.h>
#include <Encoder.h>
#include "MCP_DAC.h"
#include <SPI.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include "DandyDisplay.h"


#define OSCIS 4
#define BUFSIZE 128
#define WAVETABLE_SIZE 32
#define WAVE_SIZE 360
#define NOTE_RANGE 256

#define POT_START_PIN 14
#define POT_COUNT 5

// Because of the limited number of pins available on the Circuit Playground Boards
// Software SPI is used
#define TFT_CS        31
#define TFT_RST       28 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        29
#define TFT_BACKLIGHT PIN_A3 // Display backlight pin


unsigned int secToMicro(float sec);

// Pseudo Singleton main class for synthesis
class DandySynth
{
private:

    DandyDisplay *display;

    typedef enum WAVE_TYPE { SINE, SQUARE, TRIANGLE, COMBO, REAL, WAVETABLE } WAVE_TYPE;

    struct EnvelopeSettings
    {
        float attackStartValue;
        float attackTimeEnd;
        
        float sustainValue;
        float sustainTimeEnd;

        float releaseValueEnd;
        float releaseTime; // Higher is quicker
    };

    struct EnvelopeSettings basicOneShotEnv = {
        .attackStartValue = 0.0,
        .attackTimeEnd = 0.1,
        .sustainValue = 0.9,
        .sustainTimeEnd = 0.5,
        .releaseValueEnd = 0.0,
        .releaseTime = 0.3
    };
    struct EnvelopeSettings flatEnv = {
        .attackStartValue = 1.0,
        .attackTimeEnd = 0.1,
        .sustainValue = 1.0,
        .sustainTimeEnd = 0.5,
        .releaseValueEnd = 1.0,
        .releaseTime = 0.3
    };
    struct EnvelopeSettings funkyLoopEnv = {
        .attackStartValue = 0.1,
        .attackTimeEnd = 0.2,
        .sustainValue = 0.9,
        .sustainTimeEnd = 0.5,
        .releaseValueEnd = 0.1,
        .releaseTime = 0.6
    };

public:

    float velParam;

    float pi = 3.1415926;

	#define LFOS 4
	float lfoWaveValues[LFOS];

    // LOOKUP TABLES
    float sine[WAVE_SIZE];
    float square[WAVE_SIZE];
    float table[WAVETABLE_SIZE][WAVE_SIZE];

    MCP4921 MCP;


    int16_t outBuffer[BUFSIZE]; // For wave output

    int bufPos = 0;
    int encPos = 0;
    int encPush = 0;
    float fmFreq = 1.;
    int waveTablePos = 0;

    static byte controlValues[256];
    static byte lastCCNum;
    static byte lastCCVal;
    static uint32_t noteTime;
    static byte lastNote;
    static byte lastChan;
    static byte lastVel;
    static uint32_t noteTimes[OSCIS+1];
    static uint32_t noteIndexLookup[NOTE_RANGE];
    static byte lastNotes[OSCIS+1];



    void generateWaveTables();
    void processLFOs(uint32_t now);

    float getOscilatorsOuput(uint32_t now);

    float getNoteWave(float freq, uint32_t now, int slice);
    float getNoteSquare(float freq, uint32_t now);
    float getNoteSine(float freq, uint32_t now);
    float getNoteSineR(float freq, uint32_t now);
    float getNoteSineGoofy(float freq, uint32_t now);
    float freqToTableIndex(float freq, uint32_t now);
    float freqToTableIndex(float freq, uint32_t now, int *rem);
    float freqToRadIndex(float freq, uint32_t now);

    float sampleWave(WAVE_TYPE waveType, float freq, uint32_t now, float param);

    static void handleNoteOn(byte channel, byte pitch, byte velocity);
    static void handleNoteOff(byte channel, byte pitch, byte velocity);
    static void handleMIDIControlChange(byte channel, byte pitch, byte velocity);
    float envelope(float t, struct DandySynth::EnvelopeSettings set);

    void run(uint32_t now);
    void setup();

    float noteToFreq(float note);

    std::vector<float> params;
    void setParameterArray(std::vector<float> params);


    void setEncPos(int pos);
    void setEncPush(int pushed);

};

#endif