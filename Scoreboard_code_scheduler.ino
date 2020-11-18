/*
 * Complete Scoreboard code, revised 8/20/2020
 * Author: Derek Benham
 * 
 * Revision: 1
 * 
 * Todo:
 * Implement OT
 * Flash numbers at end of QTR
 * Flashy Intro other than Poop, test all LED's
 * Beep countdown when under 10 seconds
 * Seconds shifted to minutes and print milliseconds when under 1 minute?
 */


#include "IRremote.h"

#include "TimeSM.h"
#include "ScoreSM.h"
#include "Bonus.h"
#include "PossArrow.h"
#include "LedControl.h"
//#include "startUpSequence.h"

#define NUM_MAX_7219 2
#define LD_PIN 2
#define CLK_PIN 3
#define DIN_PIN 4

#define T1_LEFT_SEG 5
#define T1_RIGHT_SEG 4
#define T2_LEFT_SEG 7
#define T2_RIGHT_SEG 6

#define T1_SCORE_UP 12
#define T1_SCORE_DN 11
#define T2_SCORE_UP 7
#define T2_SCORE_DN 6

// Hex Codes for IR Remote
#define CH_MINUS 0xFFA25D
#define CHANNEL 0xFF629D
#define CH_PLUS 0xFFE21D
#define PREV 0xFF22DD
#define NEXT 0xFF02FD
#define PLAY_PAUSE 0xFFC23D
#define VOL_DN 0xFFE01F
#define VOL_UP 0xFFA857
#define EQ 0xFF906F
#define BTN_0 0xFF6897
#define BTN_100 0xFF9867
#define BTN_200 0xFFB04F
#define BTN_1 0xFF30CF
#define BTN_2 0xFF18E7
#define BTN_3 0xFF7A85
#define BTN_4 0xFF10EF
#define BTN_5 0xFF38C7
#define BTN_6 0xFFFF5AA5
#define BTN_7 0xFFFF42BD
#define BTN_8 0xFF4AB5
#define BTN_9 0xFF52AD
#define DEFAULT_VAL 0xFFFFFFFF


LedControl lc = LedControl(LD_PIN,DIN_PIN,CLK_PIN,NUM_MAX_7219);

Timer_STM timerSM(LD_PIN, DIN_PIN, CLK_PIN, NUM_MAX_7219);

Score_STM team1SM(T1_SCORE_DN, T1_SCORE_UP, T1_LEFT_SEG, T1_RIGHT_SEG, LD_PIN, DIN_PIN, CLK_PIN, NUM_MAX_7219);
Score_STM team2SM(T2_SCORE_DN, T2_SCORE_UP, T2_LEFT_SEG, T2_RIGHT_SEG, LD_PIN, DIN_PIN, CLK_PIN, NUM_MAX_7219);

Bonus_STM bonusT1(4, 0, 1, LD_PIN, DIN_PIN, CLK_PIN, NUM_MAX_7219);
Bonus_STM bonusT2(5, 2, 0, LD_PIN, DIN_PIN, CLK_PIN, NUM_MAX_7219);

Poss_STM possArrow(LD_PIN, DIN_PIN, CLK_PIN, NUM_MAX_7219);

//Startup startUpSeq(LD_PIN, DIN_PIN, CLK_PIN, NUM_MAX_7219)

int SMCnter = 0;
#define timerSMTick 2

// IR Setup
#define RECV_PIN 5
unsigned long int remote_Val = 0;
IRrecv irrecv1(RECV_PIN);
decode_results results1;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello");
  
  irrecv1.enableIRIn();
  irrecv1.blink13(true);
  
  int devices=lc.getDeviceCount();
  //we have to init all devices in a loop
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* Set the brightness to a medium values */
    lc.setIntensity(address,15);
    /* and clear the display */
    lc.clearDisplay(address);
  }

  //runStartupTest();
  // make sure to clear display after test
  lc.setChar(0, 0, 'P', false);
  lc.setDigit(0, 1, 0, false);
  lc.setDigit(0, 2, 0, false);
  lc.setChar(0, 3, 'P', false); 

  lc.setChar(0, 4, 'P', false);
  lc.setDigit(0, 5, 0, false);
  lc.setDigit(0, 6, 0, false);
  lc.setChar(0, 7, 'P', false); 
  delay(5000);
  tone(A5, 100, 500);
  lc.clearDisplay(0);
  

  // ISR
cli();//stop interrupts
//set timer1 interrupt at 5Hz
  TCCR0A = 0;// set entire TCCR1A register to 0
  TCCR0B = 0;// same for TCCR1B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR0A = 250;
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR0B |= (1 << CS01); //(1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  sei();//allow interrupts
} 
    ISR(TIMER0_COMPA_vect){  //change the 0 to 1 for timer1 and 2 for timer2
     //interrupt commands here
      

     if((SMCnter % 1360) == 0) {
//      myLoop();
     }
     if((SMCnter % 600) == 0) {
       myLoop();
     }
     if((SMCnter % 680) == 0) {
       timerSM.tick();
       team1SM.tick();
       team2SM.tick();
//       bonusT1.tick();
//       bonusT2.tick();
//       possArrow.tick(); 
     }

     if((SMCnter % 1360) == 0) {
       bonusT1.tick();
       bonusT2.tick();
       possArrow.tick();        
     }
  //   BUZZER_TICK();
    SMCnter++;
  };
  
void myLoop() {
//void loop() {

  if (irrecv1.decode(&results1)){
        irrecv1.resume();
    if (results1.value == DEFAULT_VAL) {
        
    } else {
        remote_Val = results1.value;
    }

    if (remote_Val == CH_MINUS || remote_Val == 0xE318261B || remote_Val == 0xFFA25 || remote_Val == 0xFFA5D) {
      // increase score to Team 2
      team2SM.incScore();
//      if (remote_Val == 0xE318261B || remote_Val == 0xFFA25 || remote_Val == 0xFFA5D) {
//        Serial.print("Other Success");
//      } else {
      Serial.println("CH_MINUS");
//      }
    } 
    else if (remote_Val == VOL_DN || remote_Val == 0xFFE01 || remote_Val == 0xF076C13B || remote_Val == 0xF0A19552) {
        // lower score to Team 2
        team2SM.decScore();
        Serial.println(remote_Val, HEX);
    } 
    else if (remote_Val == CH_PLUS || remote_Val == 0xEE886D7F || remote_Val == 0xFFE21 || remote_Val == 0xFFE1D) {
        // add score to Team 1
        team1SM.incScore();
        Serial.println("CH_PLUS");
    } 
    else if (remote_Val == EQ || remote_Val == 0xE5CFBD7F || remote_Val == 0xFF906 || remote_Val == 0xFF907) {
        // lower score to Team 1
        team1SM.decScore();
        Serial.println("EQ");
    } 
    else if (remote_Val == CHANNEL || remote_Val == 0x511DBB || remote_Val == 0xFF629 || remote_Val == 0xFF14E) {
        // add time
        timerSM.IncGameClk();
        tone(A5, 300, 50);
        Serial.println("CHANNEL");
    } 
    else if (remote_Val == VOL_UP || remote_Val == 0xA3C8EDDB || remote_Val == 0xFFA85 || remote_Val == 0x38C8DAAD || remote_Val == 0xFF42B) {
        // lower time
        timerSM.DecGameClk();
        tone(A5, 250, 50);
        Serial.println("VOL_UP");
    } 
    else if (remote_Val == NEXT || remote_Val == 0xD7E84B1B || remote_Val == 0xFF02F || remote_Val == 0xFF03E) {
        // play/ pause
        timerSM.Play_Pause_Clk();
        Serial.println("Next");
    } 
    else if (remote_Val == PREV || remote_Val == 0x52A3D41F || remote_Val == 0xFF22D || remote_Val == 0xFF22E) {
        // add bonus Team 1
        bonusT1.IncBonus();
        tone(A5, 300, 50);
        Serial.println("PREV");
    } 
    else if (remote_Val == PLAY_PAUSE || remote_Val == 0x20FE4DBB || remote_Val == 0xFFC23 || remote_Val == 0xFFC3D) {
        // add bonus Team 2 
        bonusT2.IncBonus();
        tone(A5, 300, 50);
        Serial.println(remote_Val, HEX);
    } 
    else if (remote_Val == BTN_100 || remote_Val == 0xFF986 || remote_Val == 0x97483BFB || remote_Val == 0xC348C089) {
        // Buzzer
        timerSM.Pause_Buzzer();
        Serial.println("BTN_100");
    } 
    else if (remote_Val == BTN_0 || remote_Val == 0xFF989 || remote_Val == 0xFF44B || remote_Val == 0xC101E57B) {
        // Decrease QTR
        timerSM.DecQtr();
        tone(A5, 250, 50);
        Serial.println("BTN_0");
    } 
    else if (remote_Val == BTN_200 || remote_Val == 0xFFB04 || remote_Val == 0xF0C41643 || remote_Val == 0xFF827) {
        // advance QTR
        timerSM.IncQtr();
        tone(A5, 300, 50);
        Serial.println("BTN_200");
    } 
    else if (remote_Val == BTN_2 || remote_Val == 0xFF18E || remote_Val == 0x3D9AE3F7 || remote_Val == 0x6DF455A4) {
        // Switch possesion
        possArrow.ChangePoss();
        tone(A5, 350, 50);
        Serial.println("BTN_2");
    } 
    else if (remote_Val == BTN_7 || remote_Val == 0xFF42BD || remote_Val == 0x32C6FDF7 || remote_Val == 0xFF42B) {
        // Reset QTRS AND TIMER
        timerSM.resetClock();
        tone(A5, 300, 100);
        Serial.println("BTN_7");
    } 
    else if (remote_Val == BTN_8 || remote_Val == 0x1BC0157B || remote_Val == 0xFF4AB || remote_Val == 0xFC3632F9) {
        // Reset Scores
        team1SM.resetScore();
        team2SM.resetScore();
        tone(A5, 300, 100);
        Serial.println("BTN_8");
    } 
    else if (remote_Val == BTN_9 || remote_Val == 0xFF52A || remote_Val == 0x3EC3FC1B || remote_Val == 0x7D61C1A3) {
        // Reset all
        team1SM.resetScore();
        team2SM.resetScore();
        timerSM.resetClock();
        possArrow.ResetPoss();
        tone(A5, 300, 200);
        Serial.println("BTN_9");
    } 
    else if (remote_Val == DEFAULT_VAL) {
      
    } else {
      Serial.println(remote_Val, HEX);
    }
  }
}

void loop() {

}


