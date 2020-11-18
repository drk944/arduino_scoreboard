#ifndef Bonus_h
#define Bonus_h

#include "LedControl.h"

class Bonus_STM {
  private:
    enum state { DO_NOTHING_ST, RESET_ST, INC_ST, DEC_ST };
    state currentState = DO_NOTHING_ST;
  
    LedControl lc1 = LedControl(0, 0, 0, 0);
    const int ROW_DP = 0;
    const int ROW_A = 1;  
    int curr_bonus = 0;
    int old_bonus = 0;

    bool incBonus = false;
    bool decBonus = false;
    bool resetBonus = false;
    
    int segNum;
    int digNum1;
    int digNum2;

    void DisplayBonus() {
      if (old_bonus != curr_bonus) {
//        Serial.print("Score:");
//        Serial.println(curr_score);
          if (curr_bonus == 1) {
            lc1.setLed(1, segNum, digNum1, true);
            lc1.setLed(1, segNum, digNum2, false);            
          } else if (curr_bonus == 2) {
            lc1.setLed(1, segNum, digNum1, true);
            lc1.setLed(1, segNum, digNum2, true);            
          } else {
            lc1.setLed(1, segNum, digNum1, false);
            lc1.setLed(1, segNum, digNum2, false);
          }
      old_bonus = curr_bonus;
      // implement 7-seg here
      }
    }


  public:
    Bonus_STM(int segNum1, int digNum11, int digNum22, int dataPin, int clkPin, int csPin, int numMax7219) {
      segNum = segNum1;
      digNum1 = digNum11;
      digNum2 = digNum22;
      LedControl lc_temp(dataPin, clkPin, csPin, numMax7219);
      lc1 = lc_temp;  
    }

    void IncBonus() {
      incBonus = true;
    }
    void DecBonus() {
      decBonus = true;
    }
    void ResetBonus() {
      resetBonus = true;
    }
    
    void tick() {
  
  // ---------- state transitions ----------
  
      switch(currentState) {
          case DO_NOTHING_ST:
              if (resetBonus) {
                  currentState = RESET_ST;
                  resetBonus = false;
              } else if (incBonus) {
                  currentState = INC_ST;
                  incBonus = false;
              } else if (decBonus) {
                  currentState = DEC_ST;
                  decBonus = false;
              } else {
                  currentState = DO_NOTHING_ST;
              }
  
          break;
          case RESET_ST:
              currentState = DO_NOTHING_ST;
              curr_bonus = 0;
          break;
  
          case INC_ST:
              currentState = DO_NOTHING_ST;
              if (curr_bonus < 2) {
                curr_bonus++;
              } else {
                curr_bonus = 0;
              }
          break;

           case DEC_ST:
              currentState = DO_NOTHING_ST;
              if (curr_bonus > 0) {
                curr_bonus--;
              } else {
                curr_bonus = 2;
              }
          break;
      }
  // ---------- state actions ----------
      switch(currentState) {
          case DO_NOTHING_ST:
              // nothing happens, wait for input
          break;
          case RESET_ST:
              // actions done in transition
          break;

          case INC_ST:
             // actions done in transition
          break;

          case DEC_ST:
              // actions done in transition
          break;
      }
      DisplayBonus();
  }
};

#endif

