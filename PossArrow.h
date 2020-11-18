#ifndef PossArrow_h
#define PossArrow_h

#include "LedControl.h"

class Poss_STM {
  private:
    enum state { DO_NOTHING_ST, RESET_ST, T1_ST, T2_ST };
    state currentState = DO_NOTHING_ST;
  
    LedControl lc1 = LedControl(0, 0, 0, 0);
    const int ROW_DP = 0;
    const int ROW_A = 1;
    const int ROW_B = 2;
    const int DIG_0 = 0;
    const int DIG_1 = 1;
      
    bool changePoss = false;
    bool resetPoss = false;
    
    bool t1Poss = false;
    bool t2Poss = false;
    int poss = 0;
    int prev_poss = -1;


    void DisplayPoss() {
      if (prev_poss != poss) {
          prev_poss = poss;
          if (poss == 1) {
            lc1.setLed(1, DIG_0, ROW_DP, true);
            lc1.setLed(1, DIG_0, ROW_A, true);
            lc1.setLed(1, DIG_0, ROW_B, true); 
            
            lc1.setLed(1, DIG_1, ROW_DP, false);
            lc1.setLed(1, DIG_1, ROW_A, false);
            lc1.setLed(1, DIG_1, ROW_B, false);
                 
            // Colon between mins and secs :
            lc1.setLed(1, 0, 3, true);
            lc1.setLed(1, 1, 3, true);
                    
          } else if (poss == 2) {
            lc1.setLed(1, DIG_0, ROW_DP, false);
            lc1.setLed(1, DIG_0, ROW_A, false);
            lc1.setLed(1, DIG_0, ROW_B, false); 
            
            lc1.setLed(1, DIG_1, ROW_DP, true);
            lc1.setLed(1, DIG_1, ROW_A, true);
            lc1.setLed(1, DIG_1, ROW_B, true); 

            // Colon between mins and secs :
            lc1.setLed(1, 0, 3, true);
            lc1.setLed(1, 1, 3, true);
            
          } else if (poss == 0) {
            lc1.setLed(1, DIG_0, ROW_DP, false);
            lc1.setLed(1, DIG_0, ROW_A, false);
            lc1.setLed(1, DIG_0, ROW_B, false); 
            
            lc1.setLed(1, DIG_1, ROW_DP, false);
            lc1.setLed(1, DIG_1, ROW_A, false);
            lc1.setLed(1, DIG_1, ROW_B, false);   

            // Colon between mins and secs :
            lc1.setLed(1, 0, 3, true);
            lc1.setLed(1, 1, 3, true);
          }
      }
    }


  public:
    Poss_STM(int dataPin, int clkPin, int csPin, int numMax7219) {
      LedControl lc_temp(dataPin, clkPin, csPin, numMax7219);
      lc1 = lc_temp;  
    }

    void ChangePoss() {
      changePoss = true;
    }
    void ResetPoss() {
      resetPoss = true;
    }
    
    void tick() {
  
  // ---------- state transitions ----------
  
      switch(currentState) {
          Serial.println("Nothing_ST");
          case DO_NOTHING_ST:
              if (resetPoss) {
                  currentState = RESET_ST;
              } else if (t1Poss || changePoss) {
                  currentState = T1_ST;
                  poss = 1;
                  changePoss = false;
              } else if (t2Poss) {
                  currentState = T2_ST;
                  poss = 2;
                  changePoss = false;
              } else {
                  currentState = DO_NOTHING_ST;
              }
  
          break;
          case RESET_ST:
              Serial.println("Reset_ST");
              currentState = DO_NOTHING_ST;
              t1Poss = false;
              t2Poss = false;
              resetPoss = false;
              poss = 0;
              DisplayPoss();
          break;
  
          case T1_ST:
              Serial.println("T1_ST");
              if (resetPoss) {
                currentState = RESET_ST;
              } else if (changePoss) {
                poss = 2;  
                currentState = T2_ST;
                changePoss = false;
              } else {
                poss = 1;
                currentState = T1_ST;
              }
          break;

           case T2_ST:
              Serial.println("T2_ST");
              if (resetPoss) {
                currentState = RESET_ST;
              } else if (changePoss) {
                currentState = T1_ST;
                poss = 1;
                changePoss = false;
              } else {
                poss = 2;
                currentState = T2_ST;
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

          case T1_ST:
             // actions done in transition
          break;

          case T2_ST:
              // actions done in transition
          break;
      }
      DisplayPoss();
  }
};

#endif


