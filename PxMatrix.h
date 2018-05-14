/*********************************************************************
This is a library for Chinese LED matrix displays

Written by Dominic Buchstaller.
BSD license, check license.txt for more information
*********************************************************************/


#ifndef _PxMATRIX_H
#define _PxMATRIX_H

// This is how many color levels the display shows - the more the slower the update
#define color_depth 8
//#define double_buffer

#include "Adafruit_GFX.h"
  #include "Arduino.h"


#include <SPI.h>

// Either the panel handles the multiplexing and we feed BINARY to A-E pins
// or we handle the multiplexing and activate one of A-D pins (STRAIGHT)
enum mux_patterns {BINARY, STRAIGHT};


class PxMATRIX : public Adafruit_GFX {
 public:
  PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B);
  PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C);
  PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D);
  PxMATRIX(uint8_t width, uint8_t height,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B,uint8_t C,uint8_t D,uint8_t E);

  void begin(uint8_t pattern);
  void begin();


  void clearDisplay(void);

  // Updates the display
  void display(uint16_t show_time);

  // Draw pixels
  void drawPixelRGB565(int16_t x, int16_t y, uint16_t color);
  void drawPixelRGB565(int16_t x, int16_t y, uint16_t color, bool selected_buffer);

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color, bool selected_buffer);

  void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b);
  void drawPixelRGB888(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b, bool selected_buffer);

  // Does nothing for now
  uint8_t getPixel(int8_t x, int8_t y);

  // Converts RGB888 to RGB565
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

  // Helpfull for debugging (place in display update loop)
  void displayTestPattern(uint16_t showtime);

  // FLush the buffer of the display
  void flushDisplay();

  // Rotate display
  void setRotate(bool rotate);

  // Helps to reduce display update latency on larger displays
  void setFastUpdate(bool fast_update);

  // Select active buffer to updare display from
  void selectBuffer(bool selected_buffer);

  // Control the minimum color values that result in an active pixel
  void setColorOffset(uint8_t r, uint8_t g,uint8_t b);

  // Set the multiplex pattern
  void setMuxPattern(mux_patterns mux_pattern);

 private:


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

  // Color offset
  uint8_t _color_R_offset;
  uint8_t _color_G_offset;
  uint8_t _color_B_offset;

  // Color pattern that is pushed to the display
  uint8_t _display_color;

  // Holds some pre-computed values for faster pixel drawing
  uint32_t _row_offset[64];

  // Holds the display scan pattern type
  uint8_t _pattern;

  // Number of bytes in one color
  uint8_t _pattern_color_bytes;

  // Total number of bytes that is pushed to the display at a time
  // 3 * _pattern_color_bytes
  uint8_t _send_buffer_size;

  // This is for double buffering
  bool _selected_buffer;
  bool _active_buffer;

  // Hols configuration
  bool _rotate;
  bool _fast_update;

  // Holds multiplex pattern
  mux_patterns _mux_pattern;


  // Used for test pattern
  uint16_t _test_pixel_counter;
  uint8_t _test_line_counter;
  unsigned long _test_last_call;

  // Generic function that draw one pixel
  void fillMatrixBuffer(int16_t x, int16_t y, uint8_t r, uint8_t g,uint8_t b, bool selected_buffer);

  // Init code common to both constructors
  void init(uint8_t width, uint8_t height ,uint8_t LATCH, uint8_t OE, uint8_t A,uint8_t B);

  // Light up LEDs and hold for show_time microseconds
  void latch(uint16_t show_time );

  // Set row multiplexer
  void set_mux(uint8_t value);
};

#endif
