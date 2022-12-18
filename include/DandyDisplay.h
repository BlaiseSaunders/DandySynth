#pragma once
#include "header.h"

// Display Handling Class
class DandyDisplay
{
public:
    DandyDisplay();
    void setupDisplay();
    void runDisplay();

    void encoderPosPush(int pos, int push);

    int16_t *outBuffer;

private:
    void mainMenu();
    void connMatrix();
    void waveFormDisplay();
    void errorScreen();
    void title(std::string title);

    void menu(std::string title, std::vector<std::string> text, int encPos);

    enum displayMode
    {
        MAIN,
        TABLE_SELECT,
        CONNECITON,
        WAVEFORM,
        SETTINGS
    };

    typedef struct displayState {
        enum displayMode currentDisplaymode = MAIN;
        signed int encoderOffset = 0;
        signed int encoderPos = 0;
        int encoderPush = 0;
    } displayState;

    displayState state;
    displayState preState;

    Adafruit_ST7789 *tft;
};

