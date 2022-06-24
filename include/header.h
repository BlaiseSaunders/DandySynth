#ifndef __HEADER
#define __HEADER

#include <MIDI.h>
#include "MCP_DAC.h"
#include <SPI.h>

#define OSCIS 4
#define BUFSIZE 64

class DandySynth
{
private:
    float p0, p1;


public:
    float pi = 3.1415926;

    // LOOKUP TABLES
    uint16_t sine[361];
    uint16_t square[361];

    MCP4921 MCP;


    int16_t outBuffer[BUFSIZE]; // For wave output

    int bufPos = 0;


    static uint32_t noteTime;
    static byte lastNote;
    static byte lastChan;
    static byte lastVel;
    static uint32_t noteTimes[OSCIS+1];
    static byte lastNotes[OSCIS+1];
    static float lastAmp[OSCIS];


    void generateWaveTables();
    float getNoteSquare(float freq, int now);
    float getNoteSquareSine(float freq, int now);
    static void doSomeStuffWithNoteOn(byte channel, byte pitch, byte velocity);
    int16_t shaper(int16_t pos, float t);

    void run(uint32_t now);
    void setup();

    float noteToFreq(float note);


    void setP0(float v);
    void setP1(float v);

};

#endif