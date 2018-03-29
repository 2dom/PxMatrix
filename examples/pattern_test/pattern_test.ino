#include <PxMatrix.h>
#include <Ticker.h>

// Pins for LED MATRIX
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2

PxMATRIX display(32,16, P_LAT, P_OE,P_A,P_B,P_C);

Ticker display_ticker;

void display_updater()
{
   //display.display(70);
   
   display.displayTestPattern(70);
}
uint16_t myCYAN = display.color565(0, 255, 255);
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  display.begin(8);
  display.flushDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2,0);
  display.print("Pixel");
  Serial.println("hello");
  display_ticker.attach(0.001, display_updater);
  delay(1000);
}


void loop() {

 delay(100);

}
