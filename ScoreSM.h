#ifndef ScoreSM_h
#define ScoreSM_h

#include "LedControl.h"

class Score_STM {
  private:
    enum state { DO_NOTHING_ST, RESET_ST, INC_ST, INC_FAST_ST, DEC_ST, DEC_FAST_ST };
    state currentState = DO_NOTHING_ST;
    
    int piezoPin = A5;
    LedControl lc1 = LedControl(0, 0, 0, 0);
    
    const int DEB_TIME = 1;
    const int LNG_PRESS_TIME = 4;
    
    int debounce = 0;
    int pressTime = 0;
    int curr_score = 0;
    int old_score = -1;
    
    // IO devices:
    int BtnPinUp;     // button increment for P1 score 
    int BtnPinDn;     // button increment for P2 score
    int seg1;
    int seg2;
    int inc_btn_st = 0;         // variable for reading the pushbutton status
    int dec_btn_st = 0;
    int piezo = A5;

    bool reset = false;

    void DisplayScore() {
      if (old_score != curr_score) {
        if(curr_score == 100 || curr_score == -1) {
          curr_score = 0;
        }
//        Serial.print("Score:");
//        Serial.println(curr_score);
          lc1.setDigit(0, seg1, curr_score / 10, false);
          lc1.setDigit(0, seg2, curr_score % 10, false);
        }
      old_score = curr_score;
      // implement 7-seg here
      }


  public:
    Score_STM(int BtnPinDn1, int BtnPinUp1, int seg11, int seg21, int dataPin, int clkPin, int csPin, int numMax7219) {
      BtnPinDn = BtnPinDn1;
      BtnPinUp = BtnPinUp1;
      seg1 = seg11;
      seg2 = seg21;
      LedControl lc_temp(dataPin, clkPin, csPin, numMax7219);
      lc1 = lc_temp;  
    }
    void resetScore() {
      reset = true;
    }
    void incScore() {
      curr_score++;
      tone(piezoPin, 200, 50);
    }
    void decScore() {
      curr_score--;
      tone(piezoPin, 150, 50);
    }
    void tick() {
      inc_btn_st = digitalRead(BtnPinUp);
      dec_btn_st = digitalRead(BtnPinDn);
  
  // ---------- state transitions ----------
  
      switch(currentState) {
          case DO_NOTHING_ST:
              if ((inc_btn_st == HIGH && dec_btn_st == HIGH) || reset == true) {
                  currentState = RESET_ST;
              } else if (inc_btn_st == HIGH && dec_btn_st == LOW) {
                  currentState = INC_ST;
              } else if (inc_btn_st == LOW && dec_btn_st == HIGH) {
                  currentState = DEC_ST;
              } else {
                  currentState = DO_NOTHING_ST;
              }
  
          break;
          case RESET_ST:
              currentState = DO_NOTHING_ST;
              curr_score = 0;
              reset = false;
          break;
  
          case INC_ST:
              if (pressTime == LNG_PRESS_TIME && inc_btn_st == HIGH) {
                  tone(piezoPin, 200, 50);
                  currentState = INC_FAST_ST;
                  pressTime = 0;
              } else if (inc_btn_st == LOW) {
                  currentState = DO_NOTHING_ST;
                  tone(piezoPin, 200, 50);
                  curr_score++;
                  pressTime = 0;
              } else {
                  currentState = INC_ST;
              }
          break;
  
          case INC_FAST_ST:
              if (inc_btn_st == LOW) {
                  currentState = DO_NOTHING_ST;
              } else {
                  currentState = INC_FAST_ST;
              }
          break;
  
          case DEC_ST:
              if (pressTime == LNG_PRESS_TIME && dec_btn_st == HIGH) {
                  currentState = DEC_FAST_ST;
                  tone(piezoPin, 150, 50);
                  pressTime = 0;
              } else if (dec_btn_st == LOW) {
                  currentState = DO_NOTHING_ST;
                  tone(piezoPin, 150, 50);
                  curr_score--;
                  pressTime = 0;
              } else {
                  currentState = DEC_ST;
              }
          break;
  
          case DEC_FAST_ST:
              if (dec_btn_st == LOW) {
                  currentState = DO_NOTHING_ST;
              } else {
                  currentState = DEC_FAST_ST;
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
              pressTime++;
          break;
  
          case INC_FAST_ST:
              curr_score ++;//= 2;
          break;
  
          case DEC_ST:
              pressTime++;
          break;
  
          case DEC_FAST_ST:
              curr_score --;//= 2;
          break;
      }
      DisplayScore();
  }
};

#endif

