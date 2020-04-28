
#ifndef PatternTest_H
#define PatternTest_H

class PatternTest : public Drawable {
  private:

  public:
    PatternTest() {
      name = (char *)"Test Pattern";
    }

    unsigned int drawFrame() {

       display.fillScreen(display.color565(128, 0, 0));  
    
    }
};

#endif
