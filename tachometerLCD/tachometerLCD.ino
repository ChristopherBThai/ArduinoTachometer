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

int prevrpm = 0;
int rpm = 0;
int maxrpm = 6000;
int textGap = 4;
int textWidth = 20;
int textHeight = 28;
int totalTextWidth = textWidth + textGap;
int textx = 160;
int texty = 100;
int prevLength;

//OBD2
#include <OBD2UART.h>
COBD obd;
bool run = true;

void setup() {
  Serial.begin(9600);
  obd.begin();
  tft.reset();
  tft.begin(0x9481);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  
  tft.setCursor(textx,texty);
  tft.setTextColor(CYAN);
  tft.setTextSize(4);
  tft.print(rpm);
  tft.print(" RPM");
  tft.drawRect(77,197, 327, 66, CYAN);
  tft.drawRect(0,0,480,320,BLUE);

  obd.leaveLowPowerMode();
  while(!obd.init());
  run = true;
  
  delay(1000);
}

void loop() {
  if(run){
    obd.readPID(PID_RPM,rpm);
    checkDigits();
    tft.setCursor(textx,texty);
    tft.print(rpm);
    fillBar();
    prevrpm = rpm;

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

void checkDigits(){
  
  int temprpm = rpm;
  int tempprpm = prevrpm;

  for(int i = 3;i>=0;i--){
    if(temprpm%10 != tempprpm%10||rpm<1000)
      tft.fillRect(textx+totalTextWidth*i,texty,textWidth,textHeight,BLACK);
    temprpm /= 10;
    tempprpm /= 10;
  }
}

void fillBar(){
  int length = (int)( 321*(((float)rpm)/maxrpm));
  if(length<0)
    length = 0;
  else if(length>321)
    length = 321;
  int diff = length - prevLength;
  if(diff>0)
    tft.fillRect(80+prevLength,200,diff, 60, CYAN);
  else if(diff<0)
    tft.fillRect(80+length,200,diff*-1, 60, BLACK);
  prevLength = length;
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

