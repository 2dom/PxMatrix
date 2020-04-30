
#define double_buffer
#include <PxMatrix.h>

#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif
// Pins for LED MATRIX

//PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  display.display(70);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(70);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

struct Text {
	char *text;
	uint16_t width, height;
	int16_t x, y;
	int16_t dx, dy;
} text = {"Hello", 0, 0, 0, 0, 1, 1};


uint16_t textColor = display.color565(0, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);
uint16_t lineColor = display.color565(255, 0, 0);
uint16_t backgroundColor = display.color565(0, 255, 0);

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  display.begin(16);
  display.flushDisplay();
  display.setTextWrap(false);

  #ifdef ESP8266
    display_ticker.attach(0.004, display_updater);
  #endif

  #ifdef ESP32
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  #endif

  int16_t x1 = 0, y1 = 0;
  display.getTextBounds(text.text, 0, 0, &x1, &y1, &text.width, &text.height);
  text.width-=2;
  text.height-=2;
}

int16_t x=0, dx=1;

void loop() {
//  display.clearDisplay();
  display.fillScreen(myBLACK);

  if(x+dx>=display.width() || x+dx<0)
  	dx=-dx;
  x+=dx;
  display.drawLine(x,0, display.width()-x-1, display.height()-1, lineColor);

  if(text.x+text.dx+text.width>=display.width() || text.x+text.dx<0)
  	text.dx=-text.dx;
  if(text.y+text.dy+text.height>=display.height() || text.y+text.dy<0)
  	text.dy=-text.dy;
  text.x+=text.dx;
  text.y+=text.dy;
  display.setTextColor(textColor);
  display.setCursor(text.x, text.y);
  display.print(text.text);

  display.showBuffer();
  delay(20);
}
