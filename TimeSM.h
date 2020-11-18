#ifndef TimerSM_h
#define TimerSM_h

#include "LedControl.h"
#include "IRremote.h"

#define NEXT 0xFF02FD

// IR Setup
#define RECV_PIN 5
static unsigned long int remoteVal = 0;
IRrecv irrecv(RECV_PIN);
decode_results results;


#define MS_2_TICK 10
#define DUTY 2400  // duty cycle of the Buzzer, change to adjust pitch

class Timer_STM {
  private:
      LedControl lc1 = LedControl(0, 0, 0, 0);

    // IO Devices
    const int play_pause_pin = 9;
    int btnPinUp = 10;     // button increment for time 
    int btnPinDn = 8;
    int piezoPin = A5;
    int pp_btn_st = 0;
    int inc_btn_st = 0;    // variable for reading the pushbutton status
    int dec_btn_st = 0;

    const int DEB_TIME = 1;
    const int LNG_PRESS_TIME = 4;
    
    int debounce = 0;
    int pressTime = 0;

    bool gameOn = false;    // boolean to determine whether the game has begun
    bool gameOver = false;  // boolean to determine whether the game has ended
    bool reset = false;
    int startingMins = 12;  // Set default QTR time here
    int startingSec = 00;
   
    enum state { CLK_ENABLED_ST, DO_NOTHING_ST, DEC_GAME_CLK_ST, INC_GAME_CLK_ST, 
        END_OF_REG_ST, INC_QRT_ST, INC_FAST_ST, DEC_BTN_ST, DEC_FAST_ST, RESET_ST };
    enum bzState {WAIT_ST, PLAY_SOUND_ST };
    state currentState = DO_NOTHING_ST;
    state nextState = currentState;
    bzState bzCurrentState = WAIT_ST;
    int msInt = 0;
    int numQtr = 1;
    bool clkEnabled;
    bool buzzerEnabled = false;
    int gc_min = startingMins; // game clock
    int gc_sec = startingSec;
    
    int decGameClkSec(int seconds){
      if (seconds == 0) {
        seconds = 59;
      } else {
        seconds--;
      }
      return seconds;
    }
    
    int decGameClkMin(int minutes) {
      if (minutes > 0) {
        minutes--;
      }
      return minutes;
    }
    
    int incGameClkSec(int seconds) {
      if (seconds == 59) {
        seconds = 0;
      } else {
        seconds++;
      }
      return seconds;
    }
    
    int incGameClkMin(int minutes) {
      minutes++;
      return minutes;
    }
    
    void EOQ_Buzzer(){

      tone(piezoPin, 100, 1000);
      //Flash_Numbers();
      //Serial.print("Buzzer!");
    }
    
    void Flash_Numbers() {  // This is just wrong, FIXME
      for (unsigned int i = 0; i < 4; i++) {
        lc1.setDigit(0, 3, 0, false);
        lc1.setDigit(0, 2, 0, false);
        lc1.setDigit(0, 1, 0, false);
        lc1.setDigit(0, 0, 0, false);
        delay(500);
        lc1.setDigit(0, 3, 0, true);
        lc1.setDigit(0, 2, 0, true);
        lc1.setDigit(0, 1, 0, true);
        lc1.setDigit(0, 0, 0, true);
        delay(500);
      }
    }

    void updateTime() {
      if (!gameOn && !gameOver) {
        startingMins = gc_min; // This feature updates the starting initial time for each qtr
        startingSec = gc_sec;   
      }
    
                // Print the game clock
  //        Serial.print("Q:");
  //        Serial.print(numQtr);
  //        Serial.print(" ");
  //        Serial.print(gc_min);
  //        Serial.print(":");
  //        Serial.print(gc_sec);
  //        Serial.print("\n");

//      // Colon between mins and secs :
//      lc1.setLed(1, 0, 3, true);
//      lc1.setLed(1, 1, 3, true);
      // mins
      lc1.setDigit(0, 3, gc_min / 10, false);
      lc1.setDigit(0, 2, gc_min % 10, false);
      // seconds
      lc1.setDigit(0, 1, gc_sec / 10, false);
      lc1.setDigit(0, 0, gc_sec % 10, false);
      // set QTR
      if (numQtr == 1) {
          lc1.setLed(1, 2, 0, true);
          lc1.setLed(1, 2, 1, false);
          lc1.setLed(1, 2, 2, false);
          lc1.setLed(1, 2, 3, false);
      } else if (numQtr == 2) {
          lc1.setLed(1, 2, 0, false);
          lc1.setLed(1, 2, 1, true);
          lc1.setLed(1, 2, 2, false);
          lc1.setLed(1, 2, 3, false);
      } else if (numQtr == 3) {
          lc1.setLed(1, 2, 0, false);
          lc1.setLed(1, 2, 1, false);
          lc1.setLed(1, 2, 2, true);
          lc1.setLed(1, 2, 3, false);
      } else if (numQtr == 4) {
          lc1.setLed(1, 2, 0, false);
          lc1.setLed(1, 2, 1, false);
          lc1.setLed(1, 2, 2, false);
          lc1.setLed(1, 2, 3, true);        
      } else {
          lc1.setLed(1, 2, 0, false);
          lc1.setLed(1, 2, 1, false);
          lc1.setLed(1, 2, 2, false);
          lc1.setLed(1, 2, 3, false);        
      }
    }

  public:
    Timer_STM(int dataPin, int clkPin, int csPin, int numMax7219) {
      LedControl lc_temp(dataPin, clkPin, csPin, numMax7219);
      lc1 = lc_temp;
      irrecv.enableIRIn();
      irrecv.blink13(true);
    }

    void Play_Pause_Clk() {
      clkEnabled = !clkEnabled;
      currentState = DO_NOTHING_ST;
      Pause_Buzzer();
    }
    
    bool isGameOver() {
      return gameOver;
    }

    void resetClock() {
      reset = true;
    }

    void DecGameClk() {
      gc_sec = decGameClkSec(gc_sec);  
      if (gc_sec == 59) {
        gc_min = decGameClkMin(gc_min);
      }
    }

    void IncGameClk() {
      if (gc_sec == 59) {
          gc_min = incGameClkMin(gc_min);
        }
        gc_sec = incGameClkSec(gc_sec);
    }

    void IncQtr() {
      numQtr++;
      if (numQtr == 5) {
        numQtr = 1;
      }
    }

    void DecQtr() {
      numQtr--;
      if (numQtr == 0) {
        numQtr = 4;
      }
    }
    void Pause_Buzzer() {
      tone(piezoPin, 100, 200);
    }
    
    void tick() {
      pp_btn_st = digitalRead(play_pause_pin);
      inc_btn_st = digitalRead(btnPinUp);
      dec_btn_st = digitalRead(btnPinDn);   

      // State transitions
      switch(currentState) {
      case DO_NOTHING_ST:
        if (pp_btn_st == HIGH || remoteVal == NEXT) {
          gameOn = true;
          Pause_Buzzer();
          currentState = CLK_ENABLED_ST;
          msInt = 0; 
        } 
        else if (reset) {
          currentState = RESET_ST;
        }
        else if (inc_btn_st == HIGH && dec_btn_st == LOW) {
            currentState = INC_GAME_CLK_ST;
        } 
        else if (inc_btn_st == LOW && dec_btn_st == HIGH) {
            currentState = DEC_BTN_ST;
        }
        else if (pp_btn_st == LOW && clkEnabled && (msInt % MS_2_TICK) == 0 && (gc_min != 0 || gc_sec != 0)) {
          currentState = DEC_GAME_CLK_ST;
          msInt = 0;
        }
        else if (pp_btn_st == LOW && clkEnabled && (msInt % MS_2_TICK) == 0 && gc_sec == 0 && gc_min == 0 && numQtr == 4) {
          currentState = END_OF_REG_ST;
          EOQ_Buzzer();
          //Serial.print("Game Over!\n");
          msInt = 0;
        }
        else if (pp_btn_st == LOW && clkEnabled && (msInt % MS_2_TICK) == 0 && gc_sec == 0 && gc_min == 0 && numQtr != 4) {
           EOQ_Buzzer();
           currentState = INC_QRT_ST;
        }
      break;
      case DEC_GAME_CLK_ST:
        currentState = DO_NOTHING_ST;
      break;
      
      case INC_GAME_CLK_ST:
        if (pressTime == LNG_PRESS_TIME && inc_btn_st == HIGH) {
            currentState = INC_FAST_ST;
            pressTime = 0;
            tone(piezoPin, 300, 50);
        } else if (inc_btn_st == LOW) {
            currentState = DO_NOTHING_ST;
            IncGameClk();
            pressTime = 0;
            tone(piezoPin, 300, 50);
        } else {
            currentState = INC_GAME_CLK_ST;
        }
        break;
    
      case END_OF_REG_ST:
        Flash_Numbers();
        gameOn = false;
        gameOver= true;
        if (pp_btn_st == HIGH || remoteVal == NEXT) {
          currentState = DO_NOTHING_ST;
          gameOver = false;
          gc_sec = startingSec;
          gc_min = startingMins;
          numQtr = 1;
          // Reset score
          clkEnabled = false;
        } 
        else {
          currentState = END_OF_REG_ST;  
        }
      break;
    
      case CLK_ENABLED_ST:
        currentState = DO_NOTHING_ST;
      break;
    
      case INC_QRT_ST:
        currentState = DO_NOTHING_ST;
        gc_min = startingMins;
        gc_sec = startingSec;
        clkEnabled = false;
      break;

      case INC_FAST_ST:
        if (inc_btn_st == LOW) {
            currentState = DO_NOTHING_ST;
        } else {
            currentState = INC_FAST_ST;
        }
      break;
      
      case DEC_BTN_ST:
          if (pressTime == LNG_PRESS_TIME && dec_btn_st == HIGH) {
              currentState = DEC_FAST_ST;
              pressTime = 0;
              tone(piezoPin, 250, 50);
          } else if (dec_btn_st == LOW) {
              currentState = DO_NOTHING_ST;
              DecGameClk();
              pressTime = 0;
              tone(piezoPin, 250, 50);
          } else {
              currentState = DEC_BTN_ST;
          }
      break;

      case DEC_FAST_ST:
        if (dec_btn_st == LOW) {
            currentState = DO_NOTHING_ST;
        } else {
            currentState = DEC_FAST_ST;
        }
      break;

      case RESET_ST:
        currentState = DO_NOTHING_ST;
        gc_min = 12;
        gc_sec = 0;
        numQtr = 1;

        reset = false;
        clkEnabled = false;
        gameOn = false;  
        gameOver = false; 
      break;
      }
      // state actions ---------------------------------------
      switch(currentState) {
      case DO_NOTHING_ST:
      if (clkEnabled) {
         msInt++;
      }
      break;
      case DEC_GAME_CLK_ST:
        DecGameClk();
        if (gc_min == 0 && gc_sec == 0) {
          EOQ_Buzzer();
        }    
      break;

      case INC_GAME_CLK_ST: // INC_GAME_CLK_ST is for when the inc button is pressed
        pressTime++;
      break;

      case END_OF_REG_ST:
      break;
    
      case CLK_ENABLED_ST:
        clkEnabled = !clkEnabled;
      break;
    
      case INC_QRT_ST:
        IncQtr();
      break;

      case INC_FAST_ST:
        IncGameClk();
        IncGameClk();
      break;
      
      case DEC_BTN_ST:
        pressTime++;
      break;

      case DEC_FAST_ST:
        DecGameClk();
        DecGameClk();
      break;

      case RESET_ST:
      break;
      }
      updateTime();
    //  currentState = nextState;
//    #define DEFUALT_VAL 0xFFFFFF
//    if (irrecv.decode(&results)){
//        //irrecv.resume();
//      if (results.value != DEFUALT_VAL) {
//        remoteVal = results.value;
//      }
//      }
    }
};

#endif

