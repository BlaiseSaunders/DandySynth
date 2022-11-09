#include "header.h"


#if (SPI_INTERFACES_COUNT == 1)
	SPIClass* spi = &SPI;
#else
	SPIClass* spi = &SPI1;
#endif

__attribute__((optimize("unroll-loops")))
void DandyDisplay::errorScreen()
{
	std::vector<std::string> menuText;

	for (int i = 0; i < 5; i++)
		menuText.push_back("It's cooked!!!!");

	this->menu("RUH ROH", menuText, 0);
}

 void DandyDisplay::mainMenu()
{
	std::vector<std::string> menuText;

	menuText.push_back("Table Select");
	menuText.push_back("Matrix Select");
	menuText.push_back("Waveform View");
	menuText.push_back("Settings");
	menuText.push_back("Test");
	menuText.push_back("Test2");
	menuText.push_back("Test3");
	menuText.push_back("Test4");


	int encPos = this->encoderPos+this->encoderOffset;

	if (encPos < 0)
	{
		this->encoderOffset = -this->encoderPos;
		encPos = this->encoderPos+this->encoderOffset;
	}

	if ((signed int)encPos > (signed int)menuText.size()-1)
	{
		this->encoderOffset = (int)menuText.size()-this->encoderPos-1;
		encPos = this->encoderPos+this->encoderOffset;
	}

	static int prevEncPos = 0;
	if (prevEncPos != encoderPos)
		Serial.printf("P: %d, O: %d, F: %d\n", encoderPos, encoderOffset, encPos);

	prevEncPos = encoderPos;

	this->menu("Main Menu", menuText, encPos);
}

void DandyDisplay::title(std::string title)
{
	tft->setTextColor(ST77XX_ORANGE);
	tft->setTextWrap(true);
	
	//tft->drawFastHLine(0, 0, 240, ST77XX_RED);
	tft->drawRect(1, 1, 239, 49, ST77XX_GREEN);

	tft->setCursor(10, 30);
	tft->print(title.c_str());
}

void DandyDisplay::menu(std::string title, std::vector<std::string> text, int encPos)
{
	this->title(title);


	int menuItemsLength = text.size();

	int menuOffset = min(max(encPos-4, 0), menuItemsLength-5);

	static int previousOff = 0;

	if (menuOffset != previousOff)
		tft->fillScreen(ST77XX_BLACK);
	
	previousOff = menuOffset;

	for (int i = 0; i < abs(min(5, menuItemsLength)); i++) // Check for if there's < 5 items
	{
		tft->setCursor(10, 50*i+80);
		int colour = ST77XX_RED;
		if (i == encPos-menuOffset)
			colour = ST77XX_CYAN;

		tft->drawRect(1, (50*i)+50, 239, 50, colour);
		tft->print(text[menuOffset+i].c_str());
	}
}

void DandyDisplay::encoderPosPush(int pos, int push)
{
	encoderPos = pos;
	encoderPush = push;
}

void DandyDisplay::connMatrix()
{
	this->title("ConnMatrix");
}

void DandyDisplay::waveFormDisplay()
{

}

DandyDisplay::DandyDisplay() // Constructor
{
	this->setupDisplay();
}

void DandyDisplay::setupDisplay()
{
	this->tft = new Adafruit_ST7789(spi, TFT_CS, TFT_DC, TFT_RST);

	pinMode(TFT_BACKLIGHT, OUTPUT);
	digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on

	tft->init(240, 320);                // Init ST7789 240x320
	tft->setRotation(2);

	Serial.println(F("Initialized"));

	uint16_t time = millis();
	tft->fillScreen(ST77XX_BLACK);
	time = millis() - time;

	Serial.println(time, DEC);
	delay(500);

	tft->setFont(&FreeMonoBoldOblique12pt7b);

	// large block of text
	tft->fillScreen(ST77XX_BLACK);

	
	Serial.println("done");
	delay(1000);
}

void DandyDisplay::runDisplay()
{
	uint32_t now = micros();
	static uint32_t lastSwitch = 0;

	switch (currentDisplaymode)
	{
	case MAIN:
		mainMenu();
		if (this->encoderPush && now-lastSwitch > 300000)
		{
			this->encoderPush = false;
			tft->fillScreen(ST77XX_BLACK);
			if (this->encoderPos+this->encoderOffset == 1)
				this->currentDisplaymode = CONN;
			lastSwitch = now;
		}
		break;
	case CONN:
		connMatrix();
		if (this->encoderPush && now-lastSwitch > 300000)
		{
			tft->fillScreen(ST77XX_BLACK);
			this->currentDisplaymode = MAIN;
			this->encoderPush = false;
			lastSwitch = now;
		}
		break;

	default:
		errorScreen();
		break;
	}
}