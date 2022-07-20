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
    


public:
    float p0, p1, p2, p3, p4, p5;

    float pi = 3.1415926;

    // LOOKUP TABLES
    float sine[361];
    float square[361];

    MCP4921 MCP;


    int16_t outBuffer[BUFSIZE]; // For wave output

    int bufPos = 0;


    static uint32_t noteTime;
    static byte lastNote;
    static byte lastChan;
    static byte lastVel;
    static uint32_t noteTimes[OSCIS+1];
    static byte lastNotes[OSCIS+1];


    void generateWaveTables();
    float getNoteSquare(float freq, int now);
    float getNoteSine(float freq, int now);
    float getNoteSineR(float freq, int now);
    static void doSomeStuffWithNoteOn(byte channel, byte pitch, byte velocity);
    float shaper(float t);

    void run(uint32_t now);
    void setup();

    float noteToFreq(float note);


    void setP0(float v);
    void setP1(float v);
    void setP2(float v);
    void setP3(float v);
    void setP4(float v);
    void setP5(float v);

};

#endif