//BUTTON BOX 
//USE w ProMicro
//Tested in WIN10 + Assetto Corsa
//AMSTUDIO
//20.8.17
//Modified by Dave Pike on Wed 19 June 2019 to add support for Xbox / PS4 using keyboard.h
//Tested on Xbox One X with Project Cars 2

#include <Keypad.h>
#include <Keyboard.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 4
#define NUMBUTTONS 24
#define NUMROWS 5
#define NUMCOLS 5

char keys[NUMROWS][NUMCOLS] = {
  {'1','2','3','4','5'},
  {'6','7','8','9','0'},
  {'a','b','c','d','e'},
  {'f','g','h','i','j'},
  {'k','l','m','n','o'},
};

struct rotarykeysdef {
  byte pin1;
  byte pin2;
  char ccwchar;
  char cwchar;
  volatile unsigned char state;
};

rotarykeysdef rotarykeys[NUMROTARIES] {
  {0,1,'p','q',0},
  {2,3,'r','s',0},
  {4,5,'t','u',0},
  {6,7,'v','w',0},
};

#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0

#ifdef HALF_STEP
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

byte rowPins[NUMROWS] = {21,20,19,18,15}; 
byte colPins[NUMCOLS] = {14,16,10,9,8}; 

Keypad buttbx = Keypad( makeKeymap(keys), rowPins, colPins, NUMROWS, NUMCOLS); 

void setup() {
  rotary_init();
}

void loop() { 
  CheckAllEncoders();
  CheckAllButtons();
}

void CheckAllButtons(void) {
    if (buttbx.getKeys())
    {
       for (int i=0; i<LIST_MAX; i++)   
        {
           if ( buttbx.key[i].stateChanged )   
            {
            switch (buttbx.key[i].kstate) {  
                    case PRESSED:
                              Keyboard.press(buttbx.key[i].kchar);
                              break;
                    case HOLD:
                              break;
                    case RELEASED:
                              Keyboard.release(buttbx.key[i].kchar);
                              break;
                    case IDLE:
                              break;
            }
           }   
         }
     }
}

void rotary_init() {
  for (int i=0;i<NUMROTARIES;i++) {
    pinMode(rotarykeys[i].pin1, INPUT);
    pinMode(rotarykeys[i].pin2, INPUT);
    #ifdef ENABLE_PULLUPS
      digitalWrite(rotarykeys[i].pin1, HIGH);
      digitalWrite(rotarykeys[i].pin2, HIGH);
    #endif
  }
}

unsigned char rotary_process(int _i) {
   unsigned char pinstate = (digitalRead(rotarykeys[_i].pin2) << 1) | digitalRead(rotarykeys[_i].pin1);
  rotarykeys[_i].state = ttable[rotarykeys[_i].state & 0xf][pinstate];
  return (rotarykeys[_i].state & 0x30);
}

void CheckAllEncoders(void) {
  for (int i=0;i<NUMROTARIES;i++) {
    unsigned char result = rotary_process(i);
    if (result == DIR_CCW) {
      Keyboard.press(rotarykeys[i].ccwchar); delay(50); Keyboard.release(rotarykeys[i].ccwchar);
    };
    if (result == DIR_CW) {
      Keyboard.press(rotarykeys[i].cwchar); delay(50); Keyboard.release(rotarykeys[i].cwchar);
    };
  }
}
