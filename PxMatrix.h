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

#ifndef PxMATRIX_DEFAULT_SHOWTIME
#define PxMATRIX_DEFAULT_SHOWTIME 30
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

#ifdef ESP8266
  #define GPIO_REG_SET(val) GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS,val)
  #define GPIO_REG_CLEAR(val) GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS,val)
#endif


#ifdef ESP32
  #define GPIO_REG_SET(val) GPIO.out_w1ts = val
  #define GPIO_REG_CLEAR(val) GPIO.out_w1tc = val
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

// Specify s speciffic driver chip. Most panels implement a standard shifted
// register (SHIFT). Other chips/panels may need special treatment in oder to work
enum driver_chips {SHIFT, FM6124, FM6126A};


#define max_matrix_pixels (PxMATRIX_MAX_HEIGHT * PxMATRIX_MAX_WIDTH)
#define color_step (256 / PxMATRIX_COLOR_DEPTH)
#define color_half_step (int(color_step / 2))
#define color_third_step (int(color_step / 3))
#define color_two_third_step (int(color_third_step*2))

#define buffer_size (max_matrix_pixels * 3 / 8)

class PxMATRIX : public Adafruit_GFX {
 public:
  inline PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B);
  inline PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C);
  inline PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D);
  inline PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D,uint8_t E);

  inline void begin(uint8_t row_pattern, uint8_t CLK, uint8_t MOSI, uint8_t MISO, uint8_t SS);
  inline void begin(uint8_t row_pattern);
  inline void begin();

  inline void clearDisplay(void);
  inline void clearDisplay(bool selected_buffer);

  // Updates the display
  inline void display(uint16_t show_time);
  inline void display();

  // Draw pixels
  inline void drawPixelRGB565(int16_t x, int16_t y, uint16_t color);

  inline void drawPixel(int16_t x, int16_t y, uint16_t color);

  inline void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b);

  // Does nothing for now (always returns 0)
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

  // When using double buffering, this displays the draw buffer
  inline void showBuffer();

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

  // Set driver chip type
  inline void setDriverChip(driver_chips driver_chip);


 private:

 // the display buffer for the LED matrix
  uint8_t PxMATRIX_buffer[PxMATRIX_COLOR_DEPTH][buffer_size];
#ifdef double_buffer
  uint8_t PxMATRIX_buffer2[PxMATRIX_COLOR_DEPTH][buffer_size];
#endif

  // GPIO pins
  uint8_t _LATCH_PIN;
  uint8_t _OE_PIN;
  uint8_t _A_PIN;
  uint8_t _B_PIN;
  uint8_t _C_PIN;
  uint8_t _D_PIN;
  uint8_t _E_PIN;

  // SPI pins
  uint8_t _SPI_CLK = SPI_BUS_CLK;
  uint8_t _SPI_MOSI = SPI_BUS_MOSI;
  uint8_t _SPI_MISO = SPI_BUS_MISO;
  uint8_t _SPI_SS = SPI_BUS_SS;

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
  bool _active_buffer;

  // Hols configuration
  bool _rotate;
  bool _fast_update;

  // Holds multiplex pattern
  mux_patterns _mux_pattern;

  // Holds the scan pattern
  scan_patterns _scan_pattern;

  // Holds the used driver chip
  driver_chips _driver_chip;

  // Used for test pattern
  uint16_t _test_pixel_counter;
  uint16_t _test_line_counter;
  unsigned long _test_last_call;

  // Generic function that draw one pixel
inline void fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b, bool selected_buffer);

  // Init code common to both constructors
inline void init(uint16_t width, uint16_t height ,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B);

  // Light up LEDs and hold for show_time microseconds
inline void latch(uint16_t show_time );

  // Set row multiplexer
inline void set_mux(uint8_t value);

inline void spi_init();

// Write configuration register in some driver chips
inline void writeRegister(uint16_t reg_value, uint8_t reg_position);
};

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
inline uint16_t PxMATRIX::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Init code common to both constructors
inline void PxMATRIX::init(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A, uint8_t B){
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

  _active_buffer=false;

  _color_R_offset=0;
  _color_G_offset=0;
  _color_B_offset=0;

  _test_last_call=0;
  _test_pixel_counter=0;
  _test_line_counter=0;
  _rotate=0;
  _fast_update=0;

  _row_pattern=0;
  _scan_pattern=LINE;
  _driver_chip=SHIFT;

  clearDisplay(0);
#ifdef double_buffer
  clearDisplay(1);
#endif
}



inline void PxMATRIX::writeRegister(uint16_t reg_value, uint8_t reg_position)
{
  if (_driver_chip == FM6124 || _driver_chip == FM6126A){

    if (_driver_chip == FM6124) {
       Serial.println("\nFM6124 - REG: " + String(reg_position));
    } else {
       Serial.println("\nFM6126A - REG: " + String(reg_position));
    }

    // All FM6126A code is based on the excellent guesswork by shades66 in https://github.com/hzeller/rpi-rgb-led-matrix/issues/746
    // Register 12 - brightness/gain settings, three 6bit values, aaaaaabbbbbbcccccc a= darkness?
    //               seems to add red to the background when the leds are off, b=main brightness c=finer brightness
    //               (i'm not sure if b & c are actually as 12 bit value but with b set to all 1's the value in c doesn't seem to make much difference)

    // Register 13 - not sure what it's doing yet, just that 1 specific bit within seems to be an overall enable function.

    // Now set all the values at the top to the same value for each of register 12/13 to get the same settings across the panel, the current code loads different settings into each 32 columns.
    // clocking in the register is simply clocking in the value (i've 2 panels so 128bits of data) and for the last 12/13 bits depending on the register setting the latch to high. the final drop of latch to low clocks in the configuration. this is done by sending the same value to r1/r2/g1/g2/b1/b2 at the same time to load the config into all the FM6126 chips

    // Some necessary magic bit fields
    // b12  - 1  adds red tinge
    // b12  - 9/8/7/6/5  =  4 bit brightness
    // b13  - 9   =1 screen on
    // b13  - 6   =1 screen off
    pinMode(_SPI_CLK,OUTPUT);
    pinMode(_SPI_MOSI,OUTPUT);
    digitalWrite(_SPI_CLK,HIGH); // CCK LOW
    digitalWrite(_OE_PIN,LOW);
    digitalWrite(_LATCH_PIN,HIGH);
    digitalWrite(_A_PIN,HIGH);
    digitalWrite(_B_PIN,LOW);
    digitalWrite(_C_PIN,LOW);
    digitalWrite(_D_PIN,LOW);

    uint8_t reg_bit=0;
    for (uint32_t bit_counter=0; bit_counter < _send_buffer_size*8; bit_counter++)
    {
      reg_bit=bit_counter%16;
      if ((reg_value>>reg_bit)&1)
        digitalWrite(_SPI_MOSI,HIGH);
      else
        digitalWrite(_SPI_MOSI,LOW);

      delay(1);
      digitalWrite(_SPI_CLK,LOW); // CLK HIGH
      delay(1);
      digitalWrite(_SPI_CLK,HIGH); // CLK LOW
      delay(1);
      if ((bit_counter ==  (_send_buffer_size*8 - reg_position-1)))
      {
        digitalWrite(_LATCH_PIN,LOW);
      }
    }
    digitalWrite(_LATCH_PIN,HIGH);
  }
  digitalWrite(_OE_PIN,HIGH);

}


inline void PxMATRIX::setDriverChip(driver_chips driver_chip)
{
  _driver_chip=driver_chip;

  if (driver_chip == FM6124 || driver_chip == FM6126A){

    uint16_t b12a=0b0111111111111111;
    uint16_t b12b=0b0111100000111111;
    uint16_t b12c=0b0111111111111111;
    uint16_t b12d=0b0111100000111111;

    uint16_t b13a=0b0000000001000000;
    uint16_t b13b=0b0000000001000000;
    uint16_t b13c=0b0000000001000000;
    uint16_t b13d=0b0000000001000000;


    writeRegister(b12a, 12);
    writeRegister(b13a, 13);



  }
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


inline PxMATRIX::PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  init(width, height, LATCH, OE, A, B);
}

inline PxMATRIX::PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  _C_PIN = C;
  init(width, height, LATCH, OE, A, B);
}

inline PxMATRIX::PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  _C_PIN = C;
  _D_PIN = D;
  init(width, height, LATCH, OE, A, B);
}

inline PxMATRIX::PxMATRIX(uint16_t width, uint16_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D, uint8_t E) : Adafruit_GFX(width+ADAFRUIT_GFX_EXTRA, height)
{
  _C_PIN = C;
  _D_PIN = D;
  _E_PIN = E;
  init(width, height, LATCH, OE, A, B);
}

inline void PxMATRIX::drawPixel(int16_t x, int16_t y, uint16_t color) {
  drawPixelRGB565(x, y, color);
}

inline void PxMATRIX::showBuffer() {
  _active_buffer=!_active_buffer;
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
    base_offset=_row_offset[y]-(x/8)*2;
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
  }

  total_offset_g=total_offset_r-_pattern_color_bytes;
  total_offset_b=total_offset_g-_pattern_color_bytes;

  uint8_t bit_select = x%8;
  if ((_scan_pattern==ZAGGIZ) && ((y%(_row_pattern*2))<_row_pattern))
      bit_select = 7-bit_select;

  uint8_t (*PxMATRIX_bufferp)[PxMATRIX_COLOR_DEPTH][buffer_size] = &PxMATRIX_buffer;

#ifdef double_buffer
  PxMATRIX_bufferp = selected_buffer ? &PxMATRIX_buffer2 : &PxMATRIX_buffer;
#endif

  //Color interlacing
  for (int this_color=0; this_color<PxMATRIX_COLOR_DEPTH; this_color++)
  {
    uint8_t color_tresh = this_color*color_step+color_half_step;

    if (r > color_tresh+_color_R_offset)
      (*PxMATRIX_bufferp)[this_color][total_offset_r] |=_BV(bit_select);
    else
      (*PxMATRIX_bufferp)[this_color][total_offset_r] &= ~_BV(bit_select);

    if (g > color_tresh+_color_G_offset)
      (*PxMATRIX_bufferp)[(this_color+color_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_g] |=_BV(bit_select);
    else
      (*PxMATRIX_bufferp)[(this_color+color_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_g] &= ~_BV(bit_select);

    if (b > color_tresh+_color_B_offset)
      (*PxMATRIX_bufferp)[(this_color+color_two_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_b] |=_BV(bit_select);
    else
      (*PxMATRIX_bufferp)[(this_color+color_two_third_step)%PxMATRIX_COLOR_DEPTH][total_offset_b] &= ~_BV(bit_select);
  }
}

inline void PxMATRIX::drawPixelRGB565(int16_t x, int16_t y, uint16_t color) {
  uint8_t r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
  uint8_t g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
  uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
#ifdef double_buffer
  fillMatrixBuffer(x, y, r, g, b, !_active_buffer);
#else
  fillMatrixBuffer(x, y, r, g, b, false);
#endif
}

inline void PxMATRIX::drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b) {
#ifdef double_buffer
  fillMatrixBuffer(x, y, r, g, b, !_active_buffer);
#else
  fillMatrixBuffer(x, y, r, g, b, false);
#endif
}

// the most basic function, get a single pixel
inline uint8_t PxMATRIX::getPixel(int8_t x, int8_t y) {
  return (0);//PxMATRIX_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;
}

inline void PxMATRIX::begin()
{
  begin(8);

}

void PxMATRIX::begin(uint8_t row_pattern, uint8_t CLK, uint8_t MOSI, uint8_t MISO, uint8_t SS)
{
  _SPI_CLK = CLK;
  _SPI_MOSI = MOSI;
  _SPI_MISO = MISO;
  _SPI_SS = SS;
  begin(row_pattern);

}

void PxMATRIX::spi_init(){

  #ifdef ESP8266
    SPI.begin();
  #endif
  #ifdef ESP32
    SPI.begin(_SPI_CLK, _SPI_MISO, _SPI_MOSI, _SPI_SS);
  #endif

    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setFrequency(20000000);

}

void PxMATRIX::begin(uint8_t row_pattern) {

  _row_pattern=row_pattern;
  if (_row_pattern==4)
    _scan_pattern=ZIGZAG;
  _mux_pattern = BINARY;

  _pattern_color_bytes=(_height/_row_pattern)*(_width/8);
  _row_sets_per_buffer = _rows_per_buffer/_row_pattern;
  _send_buffer_size=_pattern_color_bytes*3;



  spi_init();


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

  if (_driver_chip==SHIFT)
  {
    //digitalWrite(_OE_PIN,0); // <<< remove this
    digitalWrite(_LATCH_PIN,HIGH);
    //delayMicroseconds(10);
    digitalWrite(_LATCH_PIN,LOW);

    //delayMicroseconds(10);
    if (show_time >0)
    {
      //delayMicroseconds(show_time);
      digitalWrite(_OE_PIN,0);
      unsigned long start_time=micros();
      while ((micros()-start_time)<show_time)
        asm volatile (" nop ");
      digitalWrite(_OE_PIN,1);
    }



  }

  if (_driver_chip == FM6124 || _driver_chip==FM6126A)
  {
    //digitalWrite(_OE_PIN,0); // <<< remove this
    digitalWrite(_LATCH_PIN,LOW);
    digitalWrite(_SPI_CLK,LOW);
    for (uint8_t latch_count=0; latch_count<3; latch_count++)
    {
      digitalWrite(_SPI_CLK,HIGH);
      delayMicroseconds(1);
      digitalWrite(_SPI_CLK,LOW);
      delayMicroseconds(1);
    }
    digitalWrite(_LATCH_PIN,HIGH);
    digitalWrite(_OE_PIN,0); //<<<< insert this
    delayMicroseconds(show_time);
    digitalWrite(_OE_PIN,1);
  }
}

 void PxMATRIX::display()
{
  display(PxMATRIX_DEFAULT_SHOWTIME);
}


void PxMATRIX::display(uint16_t show_time) {
  if (show_time < 10)
    show_time =10;
  unsigned long start_time=0;
#ifdef ESP8266
  ESP.wdtFeed();
#endif

uint8_t (*bufferp)[PxMATRIX_COLOR_DEPTH][buffer_size] = &PxMATRIX_buffer;

#ifdef double_buffer
  if(_active_buffer)
    bufferp=&PxMATRIX_buffer2;
  else
    bufferp=&PxMATRIX_buffer;
#endif

  for (uint8_t i=0;i<_row_pattern;i++)
  {
    if(_driver_chip == SHIFT) {
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

        SPI.writeBytes(&(*bufferp)[_display_color][i*_send_buffer_size],_send_buffer_size);

        while ((micros()-start_time)<show_time)
          delayMicroseconds(1);
        digitalWrite(_OE_PIN,1);
      }
      else
      {
        set_mux(i);
        SPI.writeBytes(&(*bufferp)[_display_color][i*_send_buffer_size],_send_buffer_size);
        latch(show_time*((uint16_t)_brightness)/255);
      }
    }
    if (_driver_chip == FM6124 || _driver_chip == FM6126A) // _driver_chip == FM6124
    {

      // for (uint32_t xx = 0; xx < _send_buffer_size - 1; xx++) {
      //   uint8_t v = PxMATRIX_buffer[_display_color][i*_send_buffer_size + xx];
      //   for (uint8_t bb = 0; bb < 8; bb++) {
      //     if (((v >> (7 - bb)) & 1) == 1)
      //       GPIO_REG_SET( 1 << _SPI_MOSI);
      //     else
      //       GPIO_REG_CLEAR( 1 << _SPI_MOSI);
      //     GPIO_REG_SET( 1 << _SPI_CLK);
      //     GPIO_REG_CLEAR( 1 << _SPI_CLK);
      //   }
      // }

      pinMode(_SPI_CLK, SPECIAL);
      pinMode(_SPI_MOSI, SPECIAL);


      SPI.writeBytes(&(*bufferp)[_display_color][i*_send_buffer_size],_send_buffer_size-1);

      pinMode(_SPI_CLK, OUTPUT);
      pinMode(_SPI_MOSI, OUTPUT);
      pinMode(_SPI_MISO, OUTPUT);
      pinMode(_SPI_SS, OUTPUT);

      set_mux(i);

      uint8_t v = (*bufferp)[_display_color][i*_send_buffer_size + _send_buffer_size - 1];
      for (uint8_t this_byte = 0; this_byte < 8; this_byte++) {
        if (((v >> (7 - this_byte)) & 1))
          GPIO_REG_SET( 1 << _SPI_MOSI);
        else
          GPIO_REG_CLEAR( 1 << _SPI_MOSI);
        GPIO_REG_SET( 1 << _SPI_CLK);
        GPIO_REG_CLEAR( 1 << _SPI_CLK);


        if (this_byte == 4)
          //GPIO_REG_SET( 1 << _LATCH_PIN);
          digitalWrite(_LATCH_PIN, HIGH);
      }
      //GPIO_REG_WRITE(GPIO_  spi_init();

      digitalWrite(_LATCH_PIN, LOW);
      //GPIO_REG_SET( 1 << _OE_PIN);
      digitalWrite(_OE_PIN, 0); //<<<< insert this
      unsigned long start_time = micros();



      while ((micros()-start_time)<show_time)
        delayMicroseconds(1);
      //GPIO_REG_CLEAR( 1 << _OE_PIN);
      digitalWrite(_OE_PIN, 1);
      //latch(show_time*(uint16_t)_brightness/255);

    }
  }
  _display_color++;
  if (_display_color>=PxMATRIX_COLOR_DEPTH)
  {
    _display_color=0;
  }
}

void PxMATRIX::flushDisplay(void) {
  for (int ii=0;ii<_send_buffer_size;ii++)
    SPI.write(0x00);
}

void PxMATRIX::displayTestPattern(uint16_t show_time) {

  if ((millis()-_test_last_call)>500)
  {
    flushDisplay();
    for (int ii=0;ii<=_test_pixel_counter;ii++)
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

void PxMATRIX::clearDisplay(void) {
#ifdef double_buffer
  clearDisplay(!_active_buffer);
#else
  clearDisplay(false);  
#endif
}
//    void * memset ( void * ptr, int value, size_t num );

// clear everything
void PxMATRIX::clearDisplay(bool selected_buffer) {
#ifdef double_buffer
  if(selected_buffer)
    memset(PxMATRIX_buffer2, 0, PxMATRIX_COLOR_DEPTH*buffer_size);
  else
    memset(PxMATRIX_buffer, 0, PxMATRIX_COLOR_DEPTH*buffer_size);
#else
    memset(PxMATRIX_buffer, 0, PxMATRIX_COLOR_DEPTH*buffer_size);
#endif
}
#endif
