/*
*
* Inspired by and based on a loading animation for Prismata by Lunarch Studios:
* http://www.reddit.com/r/gifs/comments/2on8si/connecting_to_server_so_mesmerizing/cmow0sz
*
* Lunarch Studios Inc. hereby publishes the Actionscript 3 source code pasted in this
* comment under the Creative Commons CC0 1.0 Universal Public Domain Dedication.
* Lunarch Studios Inc. waives all rights to the work worldwide under copyright law,
* including all related and neighboring rights, to the extent allowed by law.
* You can copy, modify, distribute and perform the work, even for commercial purposes,
* all without asking permission.
*
* Aurora: https://github.com/pixelmatix/aurora
* Copyright (c) 2014 Jason Coon
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PatternPendulumWave_H
#define PatternPendulumWave_H

class PatternPendulumWave : public Drawable {
  public:
    PatternPendulumWave() {
      name = (char *)"Pendulum Wave";
    }

    unsigned int drawFrame() {
      effects.DimAll(170); effects.ShowFrame();

      for (int x = 0; x < MATRIX_WIDTH; x++)
      {
        uint8_t y = beatsin8(x + MATRIX_WIDTH, 0, MATRIX_HEIGHT);
        effects.drawBackgroundFastLEDPixelCRGB(x, y, effects.ColorFromCurrentPalette(x * 7));
      }

      return 15;
    }
};

#endif
