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
#define color_half_step int(color_step / 2)
#define color_third_step int(color_step / 3)
#define color_two_third_step int(color_third_step*2)


#define buffer_size max_matrix_width * max_matrix_height * 3 / 8

// the display buffer for the LED matrix
#ifdef double_buffer
uint8_t PxMATRIX_buffer[color_depth][2*buffer_size] = {0x00 };
#else
uint8_t PxMATRIX_buffer[color_depth][buffer_size] = {0x00 };
#endif

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t PxMATRIX::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Init code common to both constructors
void PxMATRIX::init(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A, uint8_t B){
  _LATCH_PIN = LATCH;
  _OE_PIN = OE;
  _display_color=0;

  _A_PIN = A;
  _B_PIN = B;

  _width = width;
  _height = height;

  _selected_buffer=false;
  _active_buffer=false;

  _color_R_offset=0;
  _color_G_offset=0;
  _color_B_offset=0;

  _test_last_call=0;
  _test_pixel_counter=0;
  _test_line_counter=0;
  _rotate=0;
  _fast_update=0;

  _mux_pattern=BINARY;

}

void PxMATRIX::setMuxPattern(mux_patterns mux_pattern)
{
  _mux_pattern=mux_pattern;

  // We handle the multiplexing in the library and activate one of for
  // row drivers --> need A,B,C,D pins
  if (_mux_pattern==STRAIGHT)
  {
    pinMode(_C_PIN, OUTPUT);
    digitalWrite(_C_PIN, LOW);
    pinMode(_D_PIN, OUTPUT);
    digitalWrite(_D_PIN, LOW);

  }

}

void PxMATRIX::setRotate(bool rotate) {
  _rotate=rotate;
}

void PxMATRIX::setFastUpdate(bool fast_update) {
  _fast_update=fast_update;
}

PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B) : Adafruit_GFX(width+10, height)
{
  init(width, height, LATCH, OE, A, B);
}

PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C) : Adafruit_GFX(width+10, height)
{
  _C_PIN = C;
  init(width, height, LATCH, OE, A, B);
}

PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D) : Adafruit_GFX(width+10, height)
{
  _C_PIN = C;
  _D_PIN = D;
  init(width, height, LATCH, OE, A, B);
}

PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D, uint8_t E) : Adafruit_GFX(width+10, height)
{
  _C_PIN = C;
  _D_PIN = D;
  _E_PIN = E;
  init(width, height, LATCH, OE, A, B);
}

void PxMATRIX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  drawPixelRGB565( x,  y,  color,0 );
}

void PxMATRIX::drawPixel(int16_t x, int16_t y, uint16_t color, bool selected_buffer) {
  drawPixelRGB565( x,  y,  color, selected_buffer);
}

void PxMATRIX::selectBuffer(bool selected_buffer)
{

  _selected_buffer=selected_buffer;
}

void PxMATRIX::setColorOffset(uint8_t r, uint8_t g,uint8_t b)
{


  if ((color_half_step+r)<0)
    r=-color_half_step;
  if ((color_half_step+r)>255)
      r=255-color_half_step;

  if ((color_half_step+g)<0)
    g=-color_half_step;
  if ((color_half_step+g)>255)
      g=255-color_half_step;

  if ((color_half_step+b)<0)
    b=-color_half_step;
  if ((color_half_step+b)>255)
      b=255-color_half_step;

    _color_R_offset=r;
    _color_G_offset=g;
    _color_B_offset=b;


}

void PxMATRIX::fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b,bool selected_buffer)
{
  if (_rotate){
    uint16_t temp_x=x;
    x=y;
    y=_height-1-temp_x;
  }

  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
  return;
  x =_width - 1 -x;


  uint32_t base_offset;
  uint32_t total_offset_r=0;
  uint32_t total_offset_g=0;
  uint32_t total_offset_b=0;

  // This only applies to green 32x16, 4-step-displays
  // Not sure if other displays with alternating pattern exits?
  // ... maybe make this generic one day.
  if (_pattern==4)
  {
    // Precomputed row offset values
#ifdef double_buffer
    base_offset=buffer_size*selected_buffer+_row_offset[y]-(x/8)*2;
#else
    base_offset=_row_offset[y]-(x/8)*2;
#endif
    // Weird shit access pattern
    if (y<4)
      total_offset_r=base_offset;
    if ((y>=4) && (y<8))
      total_offset_r=base_offset-1;
    if ((y>=8) && (y<12))
      total_offset_r=base_offset-_width/4;
    if (y>=12)
      total_offset_r=base_offset-_width/4-1;

    total_offset_g=total_offset_r-_pattern_color_bytes;
    total_offset_b=total_offset_g-_pattern_color_bytes;
  }
  else
  {
    // Precomputed row offset values
#ifdef double_buffer
    base_offset=buffer_size*selected_buffer+_row_offset[y]-(x/8);
#else
    base_offset=_row_offset[y]-(x/8);
#endif

    // relies on integer truncation, do not simplify
    uint8_t vert_sector = y/_pattern;
    total_offset_r=base_offset-vert_sector*_width/8;

    total_offset_g=total_offset_r-_pattern_color_bytes;
    total_offset_b=total_offset_g-_pattern_color_bytes;

  }

  //Color interlacing
  for (int this_color=0; this_color<color_depth; this_color++)
  {
    uint8_t color_tresh = this_color*color_step+color_half_step;

    if (r > color_tresh+_color_R_offset)
      PxMATRIX_buffer[this_color][total_offset_r] |=_BV(x%8);
    else
      PxMATRIX_buffer[this_color][total_offset_r] &= ~_BV(x%8);

    if (g > color_tresh+_color_G_offset)
      PxMATRIX_buffer[(this_color+color_third_step)%color_depth][total_offset_g] |=_BV(x%8);
    else
      PxMATRIX_buffer[(this_color+color_third_step)%color_depth][total_offset_g] &= ~_BV(x%8);

    if (b > color_tresh+_color_B_offset)
      PxMATRIX_buffer[(this_color+color_two_third_step)%color_depth][total_offset_b] |=_BV(x%8);
    else
      PxMATRIX_buffer[(this_color+color_two_third_step)%color_depth][total_offset_b] &= ~_BV(x%8);
  }
}

void PxMATRIX::drawPixelRGB565(int16_t x, int16_t y, uint16_t color, bool selected_buffer) {
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
  fillMatrixBuffer( x,  y, r, g,b, selected_buffer);
}

void PxMATRIX::drawPixelRGB565(int16_t x, int16_t y, uint16_t color) {
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
  fillMatrixBuffer( x,  y, r, g,b, 0);
}

void PxMATRIX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b, bool selected_buffer) {
  fillMatrixBuffer(x, y, r, g,b, selected_buffer);
}

void PxMATRIX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b) {
  fillMatrixBuffer(x, y, r, g,b, 0);
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

#ifdef ESP8266
  SPI.begin();
#endif
#ifdef ESP32
  //void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
  SPI.begin(14, 12, 13, 4);
#endif

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
      _row_offset[yy]=((yy)%_pattern)*_send_buffer_size+_send_buffer_size-1;


}

void PxMATRIX::set_mux(uint8_t value)
{

  if (_mux_pattern==BINARY)
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


  if (_mux_pattern==STRAIGHT)
  {
    if (value==0)
      digitalWrite(_A_PIN,LOW);
    else
      digitalWrite(_A_PIN,HIGH);

    if (value==1)
      digitalWrite(_B_PIN,LOW);
    else
      digitalWrite(_B_PIN,HIGH);


    if (value==2)
      digitalWrite(_C_PIN,LOW);
    else
      digitalWrite(_C_PIN,HIGH);

    if (value ==3)
      digitalWrite(_D_PIN,LOW);
    else
      digitalWrite(_D_PIN,HIGH);

  }
}

void PxMATRIX::latch(uint16_t show_time )
{
  //digitalWrite(_OE_PIN,0); // <<< remove this
  digitalWrite(_LATCH_PIN,HIGH);
  //delayMicroseconds(10);
  digitalWrite(_LATCH_PIN,LOW);
  //delayMicroseconds(10);
 digitalWrite(_OE_PIN,0); //<<<< insert this
  delayMicroseconds(show_time);
  digitalWrite(_OE_PIN,1);

}

void PxMATRIX::display(uint16_t show_time) {
  unsigned long start_time=0;
#ifdef ESP8266
  ESP.wdtFeed();
#endif
  for (uint8_t i=0;i<_pattern;i++)

  {

    if (_fast_update){

      // This will clock data into the display while the outputs are still
      // latched (LEDs on). We therefore utilize SPI transfer latency as LED
      // ON time and can reduce the waiting time (show_time). This is rather
      // timing sensitive and may lead to flicker however promises reduced
      // update times and increased brightness

      set_mux((i+_pattern-1)%_pattern);
      digitalWrite(_LATCH_PIN,HIGH);
      digitalWrite(_OE_PIN,0);
      start_time = micros();
      digitalWrite(_LATCH_PIN,LOW);
      delayMicroseconds(1);

#ifdef double_buffer
      SPI.writeBytes(&PxMATRIX_buffer[_display_color][buffer_size*_active_buffer+i*_send_buffer_size],_send_buffer_size);
#else
      SPI.writeBytes(&PxMATRIX_buffer[_display_color][i*_send_buffer_size],_send_buffer_size);
#endif
      while ((micros()-start_time)<show_time)
        delayMicroseconds(1);
      digitalWrite(_OE_PIN,1);
    }
    else
    {

      set_mux(i);
#ifdef double_buffer
      SPI.writeBytes(&PxMATRIX_buffer[_display_color][buffer_size*_active_buffer+i*_send_buffer_size],_send_buffer_size);
#else
      SPI.writeBytes(&PxMATRIX_buffer[_display_color][i*_send_buffer_size],_send_buffer_size);
#endif
      latch(show_time);
    }
  }
  _display_color++;
  if (_display_color>=color_depth)
  {
    _display_color=0;
#ifdef double_buffer
    _active_buffer=_selected_buffer;
#endif
  }
}

void PxMATRIX::flushDisplay(void) {
  for (int ii=0;ii<_send_buffer_size;ii++)
    SPI.write(0x00);
}

void PxMATRIX::displayTestPattern(uint16_t show_time) {

  if ((millis()-_test_last_call)>500)
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
