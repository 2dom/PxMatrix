/*********************************************************************
This is a library for Chinese LED matrix displays

Written by Dominic Buchstaller.
BSD license, check license.txt for more information
*********************************************************************/


#ifndef _PxMATRIX_H
#define _PxMATRIX_H

// This is how many color levels the display shows - the more the slower the update
#ifndef PxMATRIX_COLOR_DEPTH
#define PxMATRIX_COLOR_DEPTH 8
#endif

#ifndef PxMATRIX_MAX_HEIGHT
#define PxMATRIX_MAX_HEIGHT 64
#endif

#ifndef PxMATRIX_MAX_WIDTH
#define PxMATRIX_MAX_WIDTH 64
#endif

//#define double_buffer

#include "Adafruit_GFX.h"
#include "Arduino.h"
#include <SPI.h>

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

// Sometimes some extra width needs to be passed to Adafruit GFX constructor
// to render text close to the end of the display correctly
#ifndef ADAFRUIT_GFX_EXTRA
#define ADAFRUIT_GFX_EXTRA 0
#endif

// HW SPI PINS
#define SPI_BUS_CLK 14
#define SPI_BUS_MOSI 13
#define SPI_BUS_MISO 12
#define SPI_BUS_SS 4

// Either the panel handles the multiplexing and we feed BINARY to A-E pins
// or we handle the multiplexing and activate one of A-D pins (STRAIGHT)
enum mux_patterns {BINARY, STRAIGHT};

// This is how the scanning is implemented. LINE just scans it left to right,
// ZIGZAG jumps 4 rows after every byte, ZAGGII alse revereses every second byte
enum scan_patterns {LINE, ZIGZAG, ZAGGIZ, WZAGZIG, VZAG};

#define max_matrix_pixels PxMATRIX_MAX_HEIGHT * PxMATRIX_MAX_WIDTH
#define color_step 256 / PxMATRIX_COLOR_DEPTH
#define color_half_step int(color_step / 2)
#define color_third_step int(color_step / 3)
#define color_two_third_step int(color_third_step*2)

#define buffer_size max_matrix_pixels * 3 / 8

class PxMATRIX : public Adafruit_GFX {
 public:
  inline PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B);
  inline PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C);
  inline PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D);
  inline PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D,uint8_t E);

  inline void begin(uint8_t row_pattern);
  inline void begin();

  inline void clearDisplay(void);

  // Updates the display
  inline void display(uint16_t show_time);

  // Draw pixels
  inline void drawPixelRGB565(int16_t x, int16_t y, uint16_t color);
  inline void drawPixelRGB565(int16_t x, int16_t y, uint16_t color, bool selected_buffer);

  inline void drawPixel(int16_t x, int16_t y, uint16_t color);
  inline void drawPixel(int16_t x, int16_t y, uint16_t color, bool selected_buffer);

  inline void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b);
  inline void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b, bool selected_buffer);

  // Does nothing for now
  uint8_t getPixel(int8_t x, int8_t y);

  // Converts RGB888 to RGB565
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

  // Helpful for debugging (place in display update loop)
  inline void displayTestPattern(uint16_t showtime);

  // Helpful for debugging (place in display update loop)
  inline void displayTestPixel(uint16_t show_time);

  // FLush the buffer of the display
  inline void flushDisplay();

  // Rotate display
  inline void setRotate(bool rotate);

  // Helps to reduce display update latency on larger displays
  inline void setFastUpdate(bool fast_update);

  // Select active buffer to updare display from
  inline void selectBuffer(bool selected_buffer);

  // Control the minimum color values that result in an active pixel
  inline void setColorOffset(uint8_t r, uint8_t g,uint8_t b);

  // Set the multiplex pattern
  inline void setMuxPattern(mux_patterns mux_pattern);

  // Set the multiplex pattern
  inline void setScanPattern(scan_patterns scan_pattern);

  // Set the number of panels that make up the display area width
  inline void setPanelsWidth(uint8_t panels);

  // Set the brightness of the panels
  inline void setBrightness(uint8_t brightness);


 private:

 // the display buffer for the LED matrix
#ifdef double_buffer
  uint8_t PxMATRIX_buffer[PxMATRIX_COLOR_DEPTH][2*buffer_size];// = {0x00 };
#else
  uint8_t PxMATRIX_buffer[PxMATRIX_COLOR_DEPTH][buffer_size];// = {0x00 };
#endif

  // GPIO pins
  uint8_t _LATCH_PIN;
  uint8_t _OE_PIN;
  uint8_t _A_PIN;
  uint8_t _B_PIN;
  uint8_t _C_PIN;
  uint8_t _D_PIN;
  uint8_t _E_PIN;
  uint8_t _width;
  uint8_t _height;
  uint8_t _panels_width;
  uint8_t _rows_per_buffer;
  uint8_t _row_sets_per_buffer;
  uint8_t _panel_width_bytes;

  // Color offset
  uint8_t _color_R_offset;
  uint8_t _color_G_offset;
  uint8_t _color_B_offset;

  // Panel Brightness
  uint8_t _brightness;

  // Color pattern that is pushed to the display
  uint8_t _display_color;

  // Holds some pre-computed values for faster pixel drawing
  uint32_t _row_offset[PxMATRIX_MAX_HEIGHT];

  // Holds the display row pattern type
  uint8_t _row_pattern;

  // Number of bytes in one color
  uint8_t _pattern_color_bytes;

  // Total number of bytes that is pushed to the display at a time
  // 3 * _pattern_color_bytes
  uint16_t _send_buffer_size;

  // This is for double buffering
  bool _selected_buffer;
  bool _active_buffer;

  // Hols configuration
  bool _rotate;
  bool _fast_update;

  // Holds multiplex pattern
  mux_patterns _mux_pattern;

  // Holds the scan pattern
  scan_patterns _scan_pattern;

  // Used for test pattern
  uint16_t _test_pixel_counter;
  uint16_t _test_line_counter;
  unsigned long _test_last_call;

  // Generic function that draw one pixel
inline void fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b, bool selected_buffer);

  // Init code common to both constructors
inline void init(uint8_t width, uint8_t height ,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B);

  // Light up LEDs and hold for show_time microseconds
inline void latch(uint16_t show_time );

  // Set row multiplexer
inline void set_mux(uint8_t value);
};

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
inline uint16_t PxMATRIX::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Init code common to both constructors
inline void PxMATRIX::init(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A, uint8_t B){
  _LATCH_PIN = LATCH;
  _OE_PIN = OE;
  _display_color=0;

  _A_PIN = A;
  _B_PIN = B;

  if (width > PxMATRIX_MAX_WIDTH){
    #ifdef DEBUG_ESP_PORT
      DEBUG_ESP_PORT.print("[PxMatrix] Width larger than PxMATRIX_MAX_WIDTH.\n");
    #endif
  }

 if (height > PxMATRIX_MAX_HEIGHT){
    #ifdef DEBUG_ESP_PORT
      DEBUG_ESP_PORT.print("[PxMatrix] Height larger than PxMATRIX_MAX_HEIGHT.\n");
    #endif
  }

  _width = width;
  _height = height;
  _brightness=255;
  _panels_width = 1;

  _rows_per_buffer = _height/2;
  _panel_width_bytes = (_width/_panels_width)/8;

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

  _row_pattern=BINARY;
  _scan_pattern=LINE;

  clearDisplay();
}

inline void PxMATRIX::setMuxPattern(mux_patterns mux_pattern)
{
  _mux_pattern=mux_pattern;

  // We handle the multiplexing in the library and activate one of for
  // row drivers --> need A,B,C,D pins
  if (_mux_pattern==STRAIGHT)
  {
    pinMode(_C_PIN, OUTPUT);
    pinMode(_D_PIN, OUTPUT);
  }
}

inline void PxMATRIX::setScanPattern(scan_patterns scan_pattern)
{
  _scan_pattern=scan_pattern;
}

inline void PxMATRIX::setPanelsWidth(uint8_t panels) {
  _panels_width=panels;
  _panel_width_bytes = (_width/_panels_width)/8;
}

inline void PxMATRIX::setRotate(bool rotate) {
  _rotate=rotate;
}

inline void PxMATRIX::setFastUpdate(bool fast_update) {
  _fast_update=fast_update;
}

inline void PxMATRIX::setBrightness(uint8_t brightness) {
  _brightness=brightness;
}


inline PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  init(width, height, LATCH, OE, A, B);
}

inline PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  _C_PIN = C;
  init(width, height, LATCH, OE, A, B);
}

inline PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  _C_PIN = C;
  _D_PIN = D;
  init(width, height, LATCH, OE, A, B);
}

inline PxMATRIX::PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D, uint8_t E) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  _C_PIN = C;
  _D_PIN = D;
  _E_PIN = E;
  init(width, height, LATCH, OE, A, B);
}

inline void PxMATRIX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  drawPixelRGB565( x,  y,  color,0 );
}

inline void PxMATRIX::drawPixel(int16_t x, int16_t y, uint16_t color, bool selected_buffer) {
  drawPixelRGB565( x,  y,  color, selected_buffer);
}

inline void PxMATRIX::selectBuffer(bool selected_buffer)
{

  _selected_buffer=selected_buffer;
}

inline void PxMATRIX::setColorOffset(uint8_t r, uint8_t g,uint8_t b)
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

inline void PxMATRIX::fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g, uint8_t b,bool selected_buffer)
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

  if (_scan_pattern==WZAGZIG || _scan_pattern==VZAG)
  {
    // get block coordinates and constraints
    uint8_t rows_per_buffer = _height/2;
    uint8_t rows_per_block = rows_per_buffer/2;
    // this is a defining characteristic of WZAGZIG and VZAG:
    // two byte alternating chunks bottom up for WZAGZIG
    // two byte up down down up for VZAG
    uint8_t cols_per_block = 16;
    uint8_t panel_width = _width/_panels_width;
    uint8_t blocks_x_per_panel = panel_width/cols_per_block;
    uint8_t panel_index = x/panel_width;
    // strip down to single panel coordinates, restored later using panel_index
    x = x%panel_width;
    uint8_t base_y_offset = y/rows_per_buffer;
    uint8_t buffer_y = y%rows_per_buffer;
    uint8_t block_x = x/cols_per_block;
    uint8_t block_x_mod = x%cols_per_block;
    uint8_t block_y = buffer_y/rows_per_block; // can only be 0/1 for height/pattern=4
    uint8_t block_y_mod = buffer_y%rows_per_block;

    // translate block address to new block address
    // invert block_y so remaining translation will be more sane
    uint8_t block_y_inv = 1 - block_y;
    uint8_t block_x_inv = blocks_x_per_panel - block_x - 1;
    uint8_t block_linear_index;
    if (_scan_pattern==WZAGZIG)
    {
      // apply x/y block transform for WZAGZIG, only works for height/pattern=4
      block_linear_index = block_x_inv * 2 + block_y_inv;
    }
    else if (_scan_pattern==VZAG)
    {
      // apply x/y block transform for VZAG, only works for height/pattern=4 and 32x32 panels until a larger example is found
      block_linear_index = block_x_inv * 3 * block_y + block_y_inv  * (block_x_inv + 1);
    }
    // render block linear index back into normal coordinates
    uint8_t new_block_x = block_linear_index % blocks_x_per_panel;
    uint8_t new_block_y = 1 - block_linear_index/blocks_x_per_panel;
    x = new_block_x * cols_per_block + block_x_mod + panel_index * panel_width;
    y = new_block_y * rows_per_block + block_y_mod + base_y_offset * rows_per_buffer;
  }

  // This code sections supports panels that have a row-changin scanning pattern
  // It does support chaining however only of height/pattern=2
  if (_scan_pattern!=LINE && _scan_pattern!=WZAGZIG && _scan_pattern!=VZAG)
  {
    // Precomputed row offset values
#ifdef double_buffer
    base_offset=buffer_size*selected_buffer+_row_offset[y]-(x/8)*2;
#else
    base_offset=_row_offset[y]-(x/8)*2;
#endif
    uint8_t row_sector=0;
    uint16_t row_sector__offset=_width/4;
    for (uint8_t yy = 0; yy<_height; yy+=2*_row_pattern)
    {
      if ((yy<=y) && (y<yy+_row_pattern))
        total_offset_r=base_offset-row_sector__offset*row_sector-(_scan_pattern==ZAGGIZ ? 1: 0);
      if ((yy+_row_pattern<=y) && (y<yy+2*_row_pattern))
        total_offset_r=base_offset-row_sector__offset*row_sector-(_scan_pattern==ZIGZAG ? 1: 0);
      row_sector++;
    }
  }
  else
  {
    // can only be non-zero when _height/(2 inputs per panel)/_row_pattern > 1
    // i.e.: 32x32 panel with 1/8 scan (A/B/C lines) -> 32/2/8 = 2
    uint8_t vert_index_in_buffer = (y%_rows_per_buffer)/_row_pattern; // which set of rows per buffer
    // can only ever be 0/1 since there are only ever 2 separate input sets present for this variety of panels (R1G1B1/R2G2B2)
    uint8_t which_buffer = y/_rows_per_buffer;
    uint8_t x_byte = x/8;
    // assumes panels are only ever chained for more width
    uint8_t which_panel = x_byte/_panel_width_bytes;
    uint8_t in_row_byte_offset = x_byte%_panel_width_bytes;
    // this could be pretty easily extended to vertical stacking as well
    total_offset_r = _row_offset[y] - in_row_byte_offset - _panel_width_bytes*(_row_sets_per_buffer*(_panels_width*which_buffer + which_panel) + vert_index_in_buffer);
#ifdef double_buffer
    total_offset_r -= buffer_size*selected_buffer;
#endif
  }

  total_offset_g=total_offset_r-_pattern_color_bytes;
  total_offset_b=total_offset_g-_pattern_color_bytes;

  uint8_t bit_select = x%8;
  if ((_scan_pattern==ZAGGIZ) && ((y%(_row_pattern*2))<_row_pattern))
      bit_select = 7-bit_select;

  //Color interlacing
  for (int this_color=0; this_color<PxMATRIX_COLOR_DEPTH; this_color++)
  {
    uint8_t color_tresh = this_color*color_step+color_half_step;

    if (r > color_tresh+_color_R_offset)
      PxMATRIX_buffer[this_color][total_offset_r] |=_BV(bit_select);
    else
      PxMATRIX_buffer[this_color][total_offset_r] &= ~_BV(bit_select);

    if (g > color_tresh+_color_G_offset)
      PxMATRIX_buffer[(this_color+color_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_g] |=_BV(bit_select);
    else
      PxMATRIX_buffer[(this_color+color_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_g] &= ~_BV(bit_select);

    if (b > color_tresh+_color_B_offset)
      PxMATRIX_buffer[(this_color+color_two_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_b] |=_BV(bit_select);
    else
      PxMATRIX_buffer[(this_color+color_two_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_b] &= ~_BV(bit_select);
  }
}

inline void PxMATRIX::drawPixelRGB565(int16_t x, int16_t y, uint16_t color, bool selected_buffer) {
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
  fillMatrixBuffer( x,  y, r, g,b, selected_buffer);
}

inline void PxMATRIX::drawPixelRGB565(int16_t x, int16_t y, uint16_t color) {
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
  fillMatrixBuffer( x,  y, r, g,b, 0);
}

inline void PxMATRIX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b, bool selected_buffer) {
  fillMatrixBuffer(x, y, r, g,b, selected_buffer);
}

inline void PxMATRIX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b) {
  fillMatrixBuffer(x, y, r, g,b, 0);
}

// the most basic function, get a single pixel
inline uint8_t PxMATRIX::getPixel(int8_t x, int8_t y) {
  return (0);//PxMATRIX_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;
}

inline void PxMATRIX::begin()
{
  begin(8);

}

void PxMATRIX::begin(uint8_t row_pattern) {

  _row_pattern=row_pattern;
  if (_row_pattern==4)
    _scan_pattern=ZIGZAG;

  _pattern_color_bytes=(_height/_row_pattern)*(_width/8);
  _row_sets_per_buffer = _rows_per_buffer/_row_pattern;
  _send_buffer_size=_pattern_color_bytes*3;

#ifdef ESP8266
  SPI.begin();
#endif
#ifdef ESP32
  SPI.begin(SPI_BUS_CLK, SPI_BUS_MISO, SPI_BUS_MOSI, SPI_BUS_SS);
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

  if (_row_pattern >=8)
  {
    pinMode(_C_PIN, OUTPUT);
    digitalWrite(_C_PIN, LOW);
  }
  if (_row_pattern >=16)
  {
    pinMode(_D_PIN, OUTPUT);
    digitalWrite(_D_PIN, LOW);
  }
  if (_row_pattern >=32)
  {
    pinMode(_E_PIN, OUTPUT);
    digitalWrite(_E_PIN, LOW);
  }

  // Precompute row offset values
  for (uint8_t yy=0; yy<_height;yy++)
      _row_offset[yy]=((yy)%_row_pattern)*_send_buffer_size+_send_buffer_size-1;

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

    if (_row_pattern>=8)
    {
      if (value & 0x04)
      digitalWrite(_C_PIN,HIGH);
      else
      digitalWrite(_C_PIN,LOW);
    }

    if (_row_pattern>=16)
    {
      if (value & 0x08)
          digitalWrite(_D_PIN,HIGH);
      else
          digitalWrite(_D_PIN,LOW);
    }

    if (_row_pattern>=32)
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

    if (value==3)
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
  for (uint8_t i=0;i<_row_pattern;i++)
  {
    if (_fast_update && (_brightness==255)){

      // This will clock data into the display while the outputs are still
      // latched (LEDs on). We therefore utilize SPI transfer latency as LED
      // ON time and can reduce the waiting time (show_time). This is rather
      // timing sensitive and may lead to flicker however promises reduced
      // update times and increased brightness

      set_mux((i+_row_pattern-1)%_row_pattern);
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
      latch(show_time*(uint16_t)_brightness/255);
    }
  }
  _display_color++;
  if (_display_color>=PxMATRIX_COLOR_DEPTH)
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

void PxMATRIX::displayTestPixel(uint16_t show_time) {

  if ((millis()-_test_last_call)>500)
  {
    flushDisplay();
    uint16_t blanks = _test_pixel_counter/8;
    SPI.write(1<<_test_pixel_counter%8);
    while (blanks){
      SPI.write(0x00);
      blanks--;
    }
    _test_last_call=millis();
    _test_pixel_counter++;
  }

  if (_test_pixel_counter>_send_buffer_size/3*8)

  {
    _test_pixel_counter=0;
    _test_line_counter++;
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
  for(int this_color=0;this_color<PxMATRIX_COLOR_DEPTH;this_color++)
  for (int j=0;j<(_width*_height*3)/8;j++)
    PxMATRIX_buffer[this_color][j]=0;
}
#endif
