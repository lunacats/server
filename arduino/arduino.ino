#include <Servo.h>

/*Arduino code for the 2017/18 Lunabot. Takes input
  from the RasPi serial commands and converts to actions.
  Makes use of Servo Library.

  hosted on the lunacats github at https://github.com/lunacats/server
  
  @authors Aldred Odierno, Bryant Volk, Garrett Wedge
*/

//------------------------- Actions --------------------------------------//
// Change the Hex to match the controller actions (buttons etc..)
#define LEFT_FWD 0xD6
#define LEFT_NEUTRAL 0xD0
#define LEFT_REV 0xD8
#define RIGHT_FWD 0xDB
#define RIGHT_NEUTRAL 0xD5
#define RIGHT_REV 0xDD
#define FRAME_UP 0xB8
#define FRAME_DOWN 0xB6
#define CBELT_ON 0xD1
#define CBELT_REV 0xD3
#define AUG_ON 0xB3
#define AUG_REV 0xB4
#define AUG_UP 0xB5
#define AUG_DOWN 0xB7
#define AUTO_MINE 0xB2
#define E_STOP 0xB1
#define WHEEL_TOGGLE 0xD4
//------------------------- Pins ----------------------------------------//
//Motors
#define LDRIVE_PIN 12
#define RDRIVE_PIN 13
#define LACT_PIN 11
#define RACT_PIN 10
#define CBELT_PIN 9
#define AUGPWR_PIN 8
#define AUGDIR_PIN 7
#define BSCRUP_PIN 5 //had to change. broke a pin off in 6
#define BSCRDN_PIN 4
#define LIMSWUP_PIN 3
#define LIMSWDN_PIN 2
//Limit Switches     I hope we have have enough pins left for these. 
#define A_up 0
#define A_down 0
             //In reality, I don't think we'll need these...
#define F_up 0
#define F_down 
//-----------------------Servos-----------------
Servo Ldrive;
Servo Rdrive;
Servo Lact;
Servo Ract;
Servo Cbelt;
//-------------------------MISC---------------------------------------
// The numbers are based on servo angles 0'-180'. For our case, 90' is NEUTRAL. Some buffer is given on either side.  
#define POS 170 //FULL Forward
#define NEUTRAL 92 //STOP on VICTOR this number may have to be calibrated...
#define NEG 10 //FULL Reverse
#define P_MOD 140 //Allows matching of actuator speeds.
#define N_MOD 45
#define OFF LOW
#define ON HIGH
#define UP 1
#define DOWN -1
#define wait 1000
#define WHEEL_MOD_POS 110 // slower than 170
#define WHEEL_MOD_NEG 70 // slower than 45
//------------------------- toggle bools ----------------------------------------//
int toggleFrameUp = 0;
int toggleFrameDown = 0;
int toggleAugOn = 0;
int toggleAugRev = 0;
int toggleAugUp = 0;
int toggleAugDown = 0;
int toggleConveyerOn = 0;
int toggleConveyerRev = 0;
int toggleWheelSpeed = 0;
//------------------------- function defs ---------------------------------------//
void leftForward();
void rightForward();
void rightReverse();
void leftReverse();
void leftNeutral();
void rightNeutral();
int toggleWheelMod();
int frameUp();
int frameDown();
int augerOn();
int augerRev();
int augerUp();
int augerDown();
int conveyerOn();
int converyerRev();
void eStop();
//------------------------------------------------------------------------------//

void setup() {
  //Initialize Ldrive
  Ldrive.attach(LDRIVE_PIN);
  Ldrive.write(NEUTRAL);
    
  //Initialize Rdrive
  Rdrive.attach(RDRIVE_PIN);
  Rdrive.write(NEUTRAL);
  
  //Initialize Lact;
  Lact.attach(LACT_PIN);
  Lact.write(NEUTRAL);
  
  //Initialize Ract;
  Ract.attach(RACT_PIN);
  Ract.write(NEUTRAL);
  
  //Initialize Cbelt
  Cbelt.attach(CBELT_PIN);
  Cbelt.write(NEUTRAL);    
  
  //Initialize AugPwr
  pinMode(AUGPWR_PIN, OUTPUT);
  digitalWrite(AUGPWR_PIN, OFF);
  
   //Initialize AugDir
  pinMode(AUGDIR_PIN, OUTPUT);
  digitalWrite(AUGDIR_PIN, OFF);
  
  //Initialize RightBscrDir
  pinMode(BSCRUP_PIN, OUTPUT);
  digitalWrite(BSCRUP_PIN, OFF);

  //Initialize RightBscrDir
  pinMode(BSCRDN_PIN, OUTPUT);
  digitalWrite(BSCRDN_PIN, OFF);

  //Initialize RightBscrDir
  pinMode(LIMSWUP_PIN, INPUT_PULLUP);

  //Initialize RightBscrDir
  pinMode(LIMSWDN_PIN, INPUT_PULLUP);

  Serial.begin( 9600 );
}

void loop() {
    if(Serial.available() > 0) {
      byte data = Serial.read();
      Serial.write(data);
      readCommand(data);
    }
}

void readCommand(byte command) {
  int c = (int) command;

  // need to add cbelt
  switch (c) {
    case (int) LEFT_FWD:
      leftForward();
      break;
    case (int) RIGHT_FWD:
      rightForward();
      break;
    case (int) RIGHT_REV:
      rightReverse(); 
      break;
    case (int) LEFT_REV:
      leftReverse();
      break;
    case (int) RIGHT_NEUTRAL:
      rightNeutral();
      break;
    case (int) LEFT_NEUTRAL:
      leftNeutral();
      break;
    case (int) CBELT_ON:
      toggleConveyerOn = conveyerOn();
      break;
    case (int) CBELT_REV:
      toggleConveyerRev = conveyerRev();
      break;
    case (int) FRAME_UP:
      toggleFrameUp = frameUp();
      break;
    case (int) FRAME_DOWN:
      toggleFrameDown = frameDown();
      break;
    case (int) AUG_ON:
      toggleAugOn = augerOn();
      break;
    case (int) AUG_REV:
      toggleAugRev = augerRev();
      break;
    case (int) AUG_UP:
      toggleAugUp = augerUp();
      break;
    case (int) AUG_DOWN:
      toggleAugDown = augerDown();
      break;
    case (int) WHEEL_TOGGLE:
      toggleWheelSpeed = toggleWheelMod();
    case (int) E_STOP:
      eStop();
      break;
  }
}

//====================== readCommand( Serial ser )==============
//Read a command from the given Serial connection 

/* function to move the left drive forward */
void leftForward() {
  if (digitalRead(LIMSWUP_PIN) == LOW) { // only allow the wheels to move if the auger is tripping the limit switches
    if (toggleWheelSpeed) {
      Ldrive.write(WHEEL_MOD_POS);
    } else {
      Ldrive.write(POS);
    }
  }
}

void leftNeutral() {
  Ldrive.write(NEUTRAL);
}

/* function to move the right drive forward */
void rightForward() {
  if (digitalRead(LIMSWUP_PIN) == LOW) { // only allow the wheels to move if the auger is tripping the limit switches
    if (toggleWheelSpeed) {
      Rdrive.write(WHEEL_MOD_POS);
    } else {
      Rdrive.write(POS);
    }
  }
}

/* function to move the right drive in reverse */
void rightReverse() {
  if (digitalRead(LIMSWUP_PIN) == LOW) {
    if (toggleWheelSpeed) {
      Rdrive.write(WHEEL_MOD_NEG);
    } else {
      Rdrive.write(NEG);
    }
  }
}

void rightNeutral() {
  Rdrive.write(NEUTRAL);
}

/* function to move the left drive in reverse */
void leftReverse() {
  if (digitalRead(LIMSWUP_PIN) == LOW) { // only allow the wheels to move if the auger is tripping the limit switches
    if (toggleWheelSpeed) {
      Ldrive.write(WHEEL_MOD_NEG);
    } else {
      Ldrive.write(NEG);
    }
  }
}

/* toggle wheel speed */
int toggleWheelMod() {
  if (!toggleWheelSpeed) {
    return 1;
  } else {
    return 0;
  }
}

/* 
  function to raise the frame
  this is a toggle. it returns the value to set the global toggle to
  @returns 1 if frame is raising, 0 if frame is stopped raising
*/
int frameUp() {
  if (!toggleFrameUp) {
    Lact.write(NEG);
    Ract.write(N_MOD); //One of the actual actuators moves at a slower rate, so PWM is modified to match speeeds.       toggleFrameUp = 1;
    return 1;
  } else {
    Lact.write(NEUTRAL);
    Ract.write(NEUTRAL);
    return 0;
  }
}

/*
  function to lower the frame
  this is a toggle
  @returns 1 if frame is lowering, 0 if frame is stopped lowering
*/
int frameDown() {
  if (!toggleFrameDown) {
    Lact.write(POS);
    Ract.write(P_MOD); //One of the actual actuators moves at a slower rate, so PWM is modified to match speeeds.
    return 1;
  } else {
    Lact.write(NEUTRAL);  
    Ract.write(NEUTRAL);
    return 0;
  }
}


int conveyerOn() {
  if (!toggleConveyerOn) {
    Cbelt.write(POS);
    return 1;
  } else {
    Cbelt.write(NEUTRAL);
    return 0;
  }
}

int conveyerRev() {
  if (!toggleConveyerRev) {
    Cbelt.write(NEG);
    return 1;
  } else {
    Cbelt.write(NEUTRAL);
    return 0;
  }
}

int augerOn() {
  if (!toggleAugOn) {
    digitalWrite(AUGPWR_PIN, ON);
    return 1;
  } else {
    digitalWrite(AUGPWR_PIN, OFF);
    return 0;
  }
}

int augerRev() {
  if (!toggleAugRev) {
    digitalWrite(AUGPWR_PIN, ON);
    digitalWrite(AUGDIR_PIN, ON);
    return 1;
  } else {
    digitalWrite(AUGDIR_PIN, OFF);
    digitalWrite(AUGPWR_PIN, OFF);
    toggleAugOn = 0;
    return 0;
  }
}

int augerUp() {
  if (!digitalRead(LIMSWUP_PIN) == LOW) {//only allows the action if it's not at the top already
      digitalWrite(BSCRUP_PIN, HIGH); // Enables the left stepper motor to move in a particular direction
      delay(wait);
      digitalWrite(BSCRUP_PIN, LOW); // Enables the left stepper motor to move in a particular direction
  }
}

int augerDown() {
//  status_down = digitalRead(LIMSWDN_PIN);
 if (!digitalRead(LIMSWDN_PIN) == LOW) {// only allows the action if it's not at the bottom already
      digitalWrite(BSCRDN_PIN, HIGH); // Enables the left stepper motor to move in a particular direction
      delay(wait);
      digitalWrite(BSCRDN_PIN, LOW); // Enables the left stepper motor to move in a particular direction
  } 
}


void eStop() {
  Ldrive.write(NEUTRAL);
  Rdrive.write(NEUTRAL);
  Lact.write(NEUTRAL);
  Ract.write(NEUTRAL);
  Cbelt.write(NEUTRAL);    
  digitalWrite(AUGPWR_PIN, OFF);
  digitalWrite(AUGDIR_PIN, OFF);
  
  digitalWrite(BSCRUP_PIN, OFF);
  digitalWrite(BSCRDN_PIN, OFF);
}

