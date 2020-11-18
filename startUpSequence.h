#ifndef StartUp_h
#define StartUp_h

#include "LedControl.h"

class Startup {
  private:
 
  public:
    
    Startup(int dataPin, int clkPin, int csPin, int numMax7219) {
      LedControl lc = LedControl(dataPin, clkPin, csPin, numMax7219);
      
      // Scroll Hello on timer
      //flash 8's on timer
      //flash 8's on scores
      // flash every light

      // write Game On
      // play buzzer
      for(int i = 7; i >= -5; i--) {
        if (i >= 0) {
          lc.setChar(0, i, 'H', false);
        }
        if (i+1 < 7 && i+1 >= 0) {
          lc.setChar(0, i+1, 'E', false);
        }
        if (i+2 < 7 && i+2 >= 0) {
          lc.setChar(0, i+2, 'L', false);
        }
        if (i+3 < 7 && i+3 >= 0) {
          lc.setChar(0, i+3, 'L', false);
        }
        if (i+4 < 7 && i+4 >= 0) {
          lc.setDigit(0, i+4, 0, false);
        }
        delay(500);
        }
      delay(1000);
      lc.clearDisplay(0);
      
      lc.setChar(0, 0, 'P', false);
      lc.setDigit(0, 1, 0, false);
      lc.setDigit(0, 2, 0, false);
      lc.setChar(0, 3, 'P', false); 
      delay(5000);
      tone(A5, 100, 500);
      lc.clearDisplay(0);
    }
}

#endif

