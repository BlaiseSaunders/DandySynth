#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

// Because of the limited number of pins available on the Circuit Playground Boards
// Software SPI is used
#define TFT_CS        38
#define TFT_RST       28 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        29
#define TFT_BACKLIGHT PIN_A3 // Display backlight pin

#if (SPI_INTERFACES_COUNT == 1)
	SPIClass* spi = &SPI;
#else
	SPIClass* spi = &SPI1;
#endif

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable.
Adafruit_ST7789 tft = Adafruit_ST7789(spi, TFT_CS, TFT_DC, TFT_RST);



void mainMenu()
{
	tft.setTextColor(ST77XX_ORANGE);
	tft.setTextWrap(true);
	
	//tft.drawFastHLine(0, 0, 240, ST77XX_RED);
	tft.drawRect(1, 1, 239, 49, ST77XX_CYAN);

	tft.setCursor(10, 30);
	tft.print("Main Menu");

	for (int i = 0; i < 5; i++)
	{
		tft.setCursor(10, 50*i+80);
		tft.drawRect(1, (50*i)+50, 239, 50, ST77XX_RED);
		tft.print("Ping pong");
	}
}

void connMatrix()
{

}

void waveFormDisplay()
{

}

void testdrawtext(char *text, uint16_t color)
{
	tft.setCursor(0, 0);
	tft.setTextColor(color);
	tft.setTextWrap(true);
	tft.print(text);
}

void setupDisplay(void) {
	Serial.print(F("Hello! Gizmo TFT Test"));

	pinMode(TFT_BACKLIGHT, OUTPUT);
	digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on

	tft.init(240, 320);                // Init ST7789 240x320
	tft.setRotation(2);

	Serial.println(F("Initialized"));

	uint16_t time = millis();
	tft.fillScreen(ST77XX_BLACK);
	time = millis() - time;

	Serial.println(time, DEC);
	delay(500);

	tft.setFont(&FreeMonoBoldOblique12pt7b);

	// large block of text
	tft.fillScreen(ST77XX_BLACK);

	
	Serial.println("done");
	delay(1000);
}

void runDisplay() {
	mainMenu();
}