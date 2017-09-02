/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OBD2UART.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
COBD obd;

#define XPOS 0
#define YPOS 30
#define WIDTH 128
#define HEIGHT 31
#define GAP 3

int rpm = 0;
float maxRpm = 6000;
bool run = true;

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {                
  Serial.begin(9600);
  
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  //Start and Initiate OBD-II connection
  while(obd.begin()<=0)
    delay(2000);
  while(!obd.init());

  // Clear the buffer.
  display.clearDisplay();

}

void loop() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(44,5);
  obd.readPID(PID_RPM,rpm);
  if(rpm>maxRpm){
    maxRpm = rpm;
  }
  display.println(rpm);
  display.drawRect(XPOS,YPOS,WIDTH,HEIGHT,WHITE);
  display.fillRect(XPOS+GAP, YPOS+GAP, (int)((WIDTH-(GAP*2))*(rpm/maxRpm)), HEIGHT-(GAP*2), WHITE);
  display.display();
}



