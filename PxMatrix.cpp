/*********************************************************************
This is a library for Chinese LED matrix displays

Written by Dominic Buchstaller.
BSD license, check license.txt for more information
*********************************************************************/

//#include <Wire.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef __AVR__
#include <util/delay.h>
#endif

#ifndef _BV
#define _BV(x) (1 << (x))
#endif

#include <stdlib.h>

#include <Adafruit_GFX.h>
#include "PxMatrix.h"

#define max_matrix_width 64
#define max_matrix_height 64
#define color_step 256 / color_depth
#define color_half_step color_step / 2

#define buffer_size max_matrix_width * max_matrix_height * 3 / 8

// the display buffer for the LED matrix
uint8_t PxMATRIX_buffer[color_depth][buffer_size] = {0x00 };



// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t PxMATRIX::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Init code common to both constructors
void PxMATRIX::init(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C){
  _LATCH_PIN = LATCH;
  _OE_PIN = OE;
  _A_PIN= A;
  _B_PIN = B;
  _C_PIN = C;
  _display_color=0;

  _width = width;
  _height = height;

  _test_last_call=0;
  _test_pixel_counter=0;
  _test_line_counter=0;
  _rotate=0;

}
void PxMATRIX::setRotate(bool rotate) {
  _rotate=rotate;
}

PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C) : Adafruit_GFX(width+10, height) {
    init( width,  height,LATCH, OE, A, B, C);
}

PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D) : Adafruit_GFX(width+10, height) {
init( width,  height,LATCH, OE, A, B, C);
  _D_PIN = D;
}

PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D, uint8_t E) : Adafruit_GFX(width+10, height) {
 init( width,  height,LATCH, OE, A, B, C);
   _D_PIN = D;
   _E_PIN = E;
}

void PxMATRIX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  drawPixelRGB565( x,  y,  color);
}

void PxMATRIX::fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b)
{
  if (_rotate){
    uint16_t temp_x=x;
    x=y;
    y=_height-1-temp_x;
  }

  if ((x < 0) || (x > _width) || (y < 0) || (y > _height))
  return;
  x =_width - 1 -x;


  uint16_t base_offset;
  uint16_t total_offset_r=0;
  uint16_t total_offset_g=0;
  uint16_t total_offset_b=0;

  // This only applies to green 32x16, 4-step-displays
  // Not sure if other displays with alternating pattern exits?
  // ... maybe make this generic one day.
  if (_pattern==4)
  {
    // Precomputed row offset values
    base_offset=_row_offset[y]-(x/8)*2;

    // Weird shit access pattern
    if (y<4)
      total_offset_r=base_offset;
    if ((y>=4) && (y<8))
      total_offset_r=base_offset-1;
    if ((y>=8) && (y<12))
      total_offset_r=base_offset-8;
    if (y>=12)
      total_offset_r=base_offset-9;

    total_offset_g=total_offset_r-_pattern_color_bytes;
    total_offset_b=total_offset_g-_pattern_color_bytes;
  }
  else
  {
    // Precomputed row offset values
    base_offset=_row_offset[y]-(x/8);
    if (y<_pattern)
      total_offset_r=base_offset;
    else
      total_offset_r=base_offset-_pattern_color_bytes/2;

    total_offset_g=total_offset_r-_pattern_color_bytes;
    total_offset_b=total_offset_g-_pattern_color_bytes;

  }


  //Color interlacing
  for (int this_color=0; this_color<color_depth; this_color++)
  {
    uint8_t color_tresh = this_color*color_step+color_half_step;

    if (r > color_tresh)
      PxMATRIX_buffer[this_color][total_offset_r] |=_BV(x%8);
    else
      PxMATRIX_buffer[this_color][total_offset_r] &= ~_BV(x%8);

    if (g > color_tresh)
      PxMATRIX_buffer[(this_color+3)%8][total_offset_g] |=_BV(x%8);
    else
      PxMATRIX_buffer[(this_color+3)%8][total_offset_g] &= ~_BV(x%8);

    if (b > color_tresh)
      PxMATRIX_buffer[(this_color+6)%8][total_offset_b] |=_BV(x%8);
    else
      PxMATRIX_buffer[(this_color+6)%8][total_offset_b] &= ~_BV(x%8);
  }
}


void PxMATRIX::drawPixelRGB565(int16_t x, int16_t y, uint16_t color) {
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;

  if (_rotate)
    fillMatrixBuffer( x,  y, r, g,b);
  else
    fillMatrixBuffer( x,  y, r, g,b);
}


void PxMATRIX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b) {
  fillMatrixBuffer(x, y, r, g,b);
}


// the most basic function, get a single pixel
uint8_t PxMATRIX::getPixel(int8_t x, int8_t y) {
  return (0);//PxMATRIX_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;
}


void PxMATRIX::begin()
{
  begin(8);
}

void PxMATRIX::begin(uint8_t pattern) {

  _pattern=pattern;
  _pattern_color_bytes=(_height/_pattern)*(_width/8);
  _send_buffer_size=_pattern_color_bytes*3;


  SPI.begin();

  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(20000000);

  pinMode(_OE_PIN, OUTPUT);
  pinMode(_LATCH_PIN, OUTPUT);
  pinMode(_A_PIN, OUTPUT);
  pinMode(_B_PIN, OUTPUT);
  digitalWrite(_A_PIN, LOW);
  digitalWrite(_B_PIN, LOW);
  digitalWrite(_OE_PIN, HIGH);

  if (_pattern >=8)
  {
    pinMode(_C_PIN, OUTPUT);
    digitalWrite(_C_PIN, LOW);
  }
  if (_pattern >=16)
  {
    pinMode(_D_PIN, OUTPUT);
    digitalWrite(_D_PIN, LOW);
  }
  if (_pattern >=32)
  {
    pinMode(_E_PIN, OUTPUT);
    digitalWrite(_E_PIN, LOW);
  }

  // Precompute row offset values
  for (uint8_t yy=0; yy<_height;yy++)
    _row_offset[yy]=(yy%_pattern)*_send_buffer_size+_send_buffer_size-1;


}

void PxMATRIX::set_mux(uint8_t value)
{
  if (value & 0x01)
    digitalWrite(_A_PIN,HIGH);
  else
    digitalWrite(_A_PIN,LOW);

  if (value & 0x02)
    digitalWrite(_B_PIN,HIGH);
  else
    digitalWrite(_B_PIN,LOW);

  if (_pattern>=8)
  {
    if (value & 0x04)
    digitalWrite(_C_PIN,HIGH);
    else
    digitalWrite(_C_PIN,LOW);
  }

  if (_pattern>=16)
  {
    if (value & 0x08)
        digitalWrite(_D_PIN,HIGH);
    else
        digitalWrite(_D_PIN,LOW);
  }

  if (_pattern>=32)
  {
    if (value & 0x10)
        digitalWrite(_E_PIN,HIGH);
    else
        digitalWrite(_E_PIN,LOW);
  }
}

void PxMATRIX::latch(uint16_t show_time )
{

  digitalWrite(_LATCH_PIN,HIGH);
  //delayMicroseconds(10);
  digitalWrite(_LATCH_PIN,LOW);
  //delayMicroseconds(10);
  digitalWrite(_OE_PIN,0);

  delayMicroseconds(show_time);
  digitalWrite(_OE_PIN,1);

}

void PxMATRIX::display(uint16_t show_time) {
  ESP.wdtFeed();
  for (uint8_t i=0;i<_pattern;i++)

  {
    set_mux(i);

    SPI.writeBytes(&PxMATRIX_buffer[_display_color][i*_send_buffer_size],_send_buffer_size);

    latch(show_time);

  }
  _display_color++;
  if (_display_color>=color_depth)
  _display_color=0;

}

void PxMATRIX::flushDisplay(void) {
  for (int ii=0;ii<_send_buffer_size;ii++)
    SPI.write(0x00);
}

void PxMATRIX::displayTestPattern(uint16_t show_time) {

  if ((millis()-_test_last_call)>100)
  {

    SPI.write(0xFF);
    _test_last_call=millis();
    _test_pixel_counter++;
  }

  if (_test_pixel_counter>_send_buffer_size)

  {
    _test_pixel_counter=0;
    _test_line_counter++;
    flushDisplay();
  }

  if (_test_line_counter> (_height/2))
        _test_line_counter=0;

  digitalWrite(_A_PIN,HIGH);
  digitalWrite(_B_PIN,HIGH);
  digitalWrite(_C_PIN,HIGH);
  digitalWrite(_D_PIN,HIGH);
  digitalWrite(_E_PIN,HIGH);

  digitalWrite(_A_PIN,LOW);
  digitalWrite(_B_PIN,LOW);
  digitalWrite(_C_PIN,LOW);
  digitalWrite(_D_PIN,LOW);
  digitalWrite(_E_PIN,LOW);

  set_mux(_test_line_counter);

  latch(show_time);

}

// clear everything
void PxMATRIX::clearDisplay(void) {
  for(int this_color=0;this_color<color_depth;this_color++)
  for (int j=0;j<(_width*_height*3)/8;j++)
    PxMATRIX_buffer[this_color][j]=0;

}
