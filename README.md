## Overview

This driver controls chinese RGB LED Matrix modules without any additional components whatsoever. These panels are commonly used in large-scale LED displays and come in diffent layouts and resolutions:

Pitch (mm)| Size | Resolution | Rows-scan pattern
----|----|----|----
P6   | 192mmx96mm  | 32x16 | 1/2, 1/4 or 1/8
P10  | 320mmx160mm | 32x16 | 1/2, 1/4 or 1/8
P4   | 128mmx128mm | 32x32 | 1/8 or 1/16
P5   | 160mmx160mm | 32x32 | 1/8 or 1/16
P6   | 192mmx192mm | 32x32 | 1/8 or 1/16
P7.62| 244mmx244mm | 32x32 | 1/8 or 1/16
P2.5 | 160mmx80mm  | 64x32 | 1/16
P4   | 256mmx128mm | 64x32 | 1/16
P5   | 320mmx160mm | 64x32 | 1/16
P2.5 | 160mmx160mm | 64x64 | 1/32
P3   | 192mmx192mm | 64x64 | 1/32

Multiple panels may be chained together to build larger displays. The driver is Adafruit GFX compatible and currently works with ESP8266 and ESP32 microcontrollers. However, it should be rather straightforward to port it to Atmel-based Arduinos. This readme gives an overview over the the library - for a more detailed guide you may consider witnessmenow's detailed [tutorial](http://www.instructables.com/id/RGB-LED-Matrix-With-an-ESP8266/).

![P10](/images/P10_matrix.jpg)



## Display structure

The display basically consists of 6 large shift register. On the input connector you will find the inputs to the shift register (two for each color - Rx,Gx,Bx), a 2 to 5 bit latch address input (A,B,C,D,E), a latch enable input (LAT/STB), a clock input (CLK) and the output enable input (OE).

There are a few basic row scanning layouts: 1/4, 1/8, 1/16 and 1/32 row scan. You can enable the correct pattern for your display with display.begin(n) where n={4,8,16,32} defines the pattern.

For example, the 32x16 displays work like this (other varieties operate accordingly): Each of the shift register is 64(1/4 row scan) / 32(1/8 row scan) bits long. R1 and R2 will together therefore cover 128(1/4 row scan) / 64(1/8 row scan) bits or 4(1/4 row scan) / 2(1/8 row scan) lines respectively. The rows are, however, not next to each other but have a spacing of 4(1/4 row scan) / 8(1/8 row scan). In case of 1/4 the row scan pattern may also alternate between rows or even reverse bit order.

Such LED matrix are usually used as a sub-module for larger displays and therefore features an output connector for daisy chaining. On the output connector you will find the identical signals to the input connector where A,B,C,LAT,CLK are simply routed through and (R,G,B) pins are the outputs of the shift registers on the module.

## Configure the library for your panel
There are three parameters that define how the panel works. The first one is the basic row scanning layout explained above. You can specify this in the `display.begin(x)` call where x={4,8,16,32} is the scanning layout. Secondly, you may have to specify a different scanning pattern to the default LINE scanning. This can be achieved by calling `display.setScanPattern(x)` where x={LINE, ZIGZAG, ZAGGIZ, WZAGZIG, VZAG}. Finally, your panel may not handle BINARY row multiplexing by itself but we need to handle it in the library and select rows STRAIGHT via the A,B,C,D lines. This can be achieved by calling `display.setMuxPattern(x)` where x={BINARY, STRAIGHT}.
 So for some very strange displays you may have execute:

```
display.begin(4);
display.setScanPattern(ZAGGIZ);
display.setMuxPattern(STRAIGHT);
```
The number of required address lines (A,B,C ...) in the constructor depends on the row-scan pattern and the multiplex pattern. For example a 1/4 scan display with scan pattern BINARY will require A,B where a STRAIGHT display will require A,B,C,D. A good hint is usally the connector labeling on your matrix.

## Set-up and cabling

When driving a long chain of LED modules in a row, parallel color data lines make a lot of sense since it reduces the data rate. But since we are only driving a few modules here, we really don't need that. We can therefore use jumper wires between input connector (PI) and output connector (PO) to chain all shift registers together and create one big shift register. This has two advantages: it reduces the number of required GPIO pins on the microcontroller and we can use the hardware SPI interface to drive it.

* If your panel input connector has "R1" in the top left corner:

  ![P10_conn_A](/images/32x16_con_A.jpg)

  Connect PI and PO as follows:

  PI | PO
  ---|---
  R2 | R1
  G1 | R2
  G2 | G1
  B1 | G2
  B2 | B1

  Connect panel input (PI) to the ESP8266 / ESP32 as follows:

  PI  | ESP8266 (NodeMCU) | ESP32 | NOTE
  ----|----|----|----
  STB/LAT |  16 - (D0) | 22
  A   |  05 - (D1) | 19
  B   |  04 - (D2) | 23
  C   |  15 - (D8) | 18 | only for 1/8, 1/16, 1/32 scan BINARY mux pattern or 1/4 STRAIGHT mux pattern
  D   |  12 - (D6) | 5 | only for 1/16, 1/32 scan BINARY mux pattern or 1/4 STRAIGHT mux pattern
  E   |  00 - (D3) | 15 | only for 1/32 scan
  P_OE|  02 - (D4) | 2
  CLK |  14 - (D5) | 14
  R1  |  13 - (D7) | 13

* If your panel input connector has "R0" in the top left corner:

  ![P10_conn_B](/images/32x16_con_B.jpg)

  Connect PI and PO as follows:

  PI | PO
  ---|---
  R1 | R0
  G0 | R1
  G1 | G0
  B0 | G1
  B1 | B0

  Connect panel input (PI) to the ESP8266 / ESP32 as follows:

  PI  | ESP8266 (NodeMCU) | ESP32 | NOTE
  ----|----|----|----
  STB/LAT |  16 - (D0) | 22
  A   |  05 - (D1) | 19
  B   |  04 - (D2) | 23
  C   |  15 - (D8) | 18 | only for 1/8, 1/16, 1/32 scan BINARY mux pattern or 1/4 STRAIGHT mux pattern
  D   |  12 - (D6) | 5 | only for 1/16, 1/32 scan BINARY mux pattern or 1/4 STRAIGHT mux pattern
  E   |  00 - (D3) | 15 | only for 1/32 scan
  P_OE|  02 - (D4) | 2
  CLK |  14 - (D5) | 14
  R0  |  13 - (D7) | 13

You should end up with something like this (VCC/supply not connected here yet):

![Cabling](/images/P10_cables.jpg)

If you want it more professional, some users have created custom PCBs to gets rid of all those lose cables.

  * [Mike](https://github.com/mike-rankin/ESP8266_RGB_Matrix_Cable_Version)
  * [Pepe](http://www.instructables.com/id/tabuled)

## Colors
The number of color levels can be selected in the header file. The default (8 color levels per primary RGB color) works well with hardly any flickering. Note that the number of color levels determines the achievable display refresh rate. Hence, the more color levels are selected, the more flickering is to be expected. If you run into problems with flickering it is a good idea to increase the CPU frequency to 160MHz. This way the processor has more headroom to compute the display updates and refresh the display in time.

## Chaining
 Chaining any number of displays together horizontally is rather straightforward. Simply use the supplied flat band connector between the panels and then treat the entire chain as one display. For example, three 32x16 displays would result in one 96x16 display where we use the input connector (PI) on the first and the output connector (PO) on the last panel as explained above. Don't forget to specify the correct resolution in the constructor, i.e. PxMATRIX display(96,16,...), and how many panels you chained together, i.e. display.setPanelsWidth(3). Initializing this chaining set-up with 1/4 scan panels would look like this:

```
#include <PxMatrix.h>
PxMATRIX display(96,16,...);
...
[]
...
void setup(){
display.begin(4);
display.setPanelsWidth(3);
...
```

![Chaining](/images/chain.png)


## Troubleshooting

  * Some panels require grounding of unused (multiplex) inputs. For example, some 1/16 scan panels expose an (sometimes unlabeled) E input that needs grounding where only ABCD is connected to the ESP. If left open the display typically shows shifted images and/or ghosting.

  * Check you cabling with a multimeter (diode-test). You can measure the connection between the input/ouput panel connector and the NodeMCU/ESP8266 via the exposed SMD pads/legs.

  * Your display may have a different scanning pattern. Make sure that you have selected the correct scanning pattern in the display.begin call
  * Run the "pattern_test.ino" and check if the scanning pattern is appearing ok. For a 8 row-step display it should look like this (red then yellow then white line progressing):
  ![8step](/images/8step.gif)

  * It is possible that the LED multiplex chip is defective (this was the case with one of my modules). You can verify this by selecting a bit pattern on the A,B,C inputs and measuring that the corresponing row outputs are low , e.g. a 4 row-step display typically uses a ([PR4538](/docs/pr4538.pdf)) chip. Setting (A=1,B=0) should give you (LINE0=1,LINE1=0,LINE2=1,LINE3=1).  This chip can easily be replaced. Spare part available [here](https://www.aliexpress.com/item/Free-shipping-10pcs-lot-PR4538DW-SOP-20-original-authentic/32594044891.html?spm=a2g0s.9042311.0.0.bjr5BY).

  * If you have any problems with ghosting or randomly lit-up pixels, please double-check the ground connection between ESP and your panel and make sure that your power supply can deliver >2A. Also make sure that your cabling between power suppply and power connector (center of the panel) is sufficient to carry the current.

## Thanks to

  * Kinsey Moore for improving the code
  * Jean Carlos for testing the chaining
  * Dave Davenport for FM62126A support

If you would like to buy me a beer: [![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=TXLWSPUD9X2KL)


## Examples



Animated weather clock from [here](https://2dom.github.io/PixelTime/)  (ESP8266)

![Example](/images/PixelTime_small.jpg)

Animated picture frame from [here](https://2dom.github.io/PixelTimes/) (ESP8266 - Pixels from [eboy](http://hello.eboy.com/eboy/category/everything/explore/animations/))

![Example](/images/front_anim2.gif)



Decoding 10 FPS MJPEG on 64x64 RGB LED matrix (ESP32)

<nobr>![Example](/images/starwars.gif)![Example](/images/starwars_back.jpeg)</nobr>
