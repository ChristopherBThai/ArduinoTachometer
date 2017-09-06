 #include <Adafruit_GFX.h>    

//LED DISPLAY
#include <Adafruit_TFTLCD.h> 

#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define WIDTH   480
#define HEIGHT  320

bool DEBUG = true;

//RPM VARIABLES
//rpm
int prevrpm = 0;
int rpm = 0;
int maxrpm = 6000;
int dir = 1;
//rpm text
int textGap = 4;
int textWidth = 20;
int textHeight = 28;
int totalTextWidth = textWidth + textGap;
int textx = 160;
int texty = 160;
//rpm bar
int prevLength;
int barw = 400;
int barx = (WIDTH-barw)/2;
int bary = 240;
int barh = 60;
int barg = 3;

//OBD2
#include <OBD2UART.h>
COBD obd;
bool run = true;

void setup() {
  Serial.begin(9600);
  if(DEBUG)
    Serial.println("Running Debug mode");
  Serial.println("Beginning obd...");
  obd.begin();
  tft.reset();
  Serial.println("Beginning tft...");
  tft.begin(0x9481);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  
  tft.setCursor(textx,texty);
  tft.setTextColor(CYAN);
  tft.setTextSize(4);
  tft.print("0000 RPM");
  tft.drawRect(barx-barg,bary-barg,barw+barg*2, barh+barg*2, CYAN);
  //tft.drawRect(0,0,480,320,BLUE);

  Serial.println("Initializing obd...");
  obd.leaveLowPowerMode();
  while(!DEBUG&&!obd.init());
  run = true;

  Serial.println("Setup Successful");
  delay(1000);
}

void loop() {
  if(run){

    //Tachometer
    if(DEBUG)
      testRpm();
    else
      obd.readPID(PID_RPM,rpm);
    writeDigits();
    fillBar();
    prevrpm = rpm;

    //Voltage
    writeVolts();

    //If the car is off... turn off
    if(false){
      discard();
    }
  }else{
    //If the car is on... turn on
    if(false){
      initi();
    }
  }
}

void writeDigits(){
  Serial.print(rpm);
  Serial.println(" rpm");
  
  int temprpm = rpm;
  int tempprpm = prevrpm;
  tft.setTextSize(4);

  for(int i = 3;i>=0;i--){
    if(temprpm%10 != tempprpm%10){
      int tempx = textx + totalTextWidth * i;
      int tempDigit = temprpm%10;
      tft.setCursor(tempx, texty);
      tft.fillRect(tempx,texty,textWidth,textHeight,BLACK);
      tft.print(tempDigit);
    }
    temprpm /= 10;
    tempprpm /= 10;
  }
}

void fillBar(){
  int length = (int)( barw*(((float)rpm)/maxrpm));
  if(length<0)
    length = 0;
  else if(length>barw)
    length = barw;
  int diff = length - prevLength;
  if(diff>0)
    tft.fillRect(barx+prevLength,bary,diff, barh, CYAN);
  else if(diff<0)
    tft.fillRect(barx+length,bary,diff*-1, barh, BLACK);
  prevLength = length;
}

void writeVolts(){
  float volts = 0;
  if(!DEBUG)
    volts = obd.readPID(PID_CONTROL_MODULE_VOLTAGE,rpm);
  else
    volts = 12.0;
  tft.setTextSize(2);
  tft.setCursor(WIDTH - (7*totalTextWidth/2), 3); //12.0 V
  tft.print(volts);
  tft.print(" V");
  Serial.print(volts);
  Serial.println(" V");
}


void initi(){
  obd.begin();
  obd.leaveLowPowerMode();
  while(!obd.init());
  run = true;
}

void discard(){
  obd.enterLowPowerMode();
  obd.end();
  run = false;
}

void testRpm(){
  rpm += 5 * dir;
  if(rpm >= maxrpm)
    dir = -1;
  else if (rpm <= 0)
    dir = 1;
}

