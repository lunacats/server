#include <Servo.h>

/*Arduino code for the 2017/18 Lunabot. Takes input
  from the RasPi serial commands and converts to actions.
  Makes use of Servo Library.

  hosted on the lunacats github at https://github.com/lunacats/server
  
  @authors Aldred Odierno, Bryant Volk, Garret Wedge
*/

//------------------------- Actions --------------------------------------//
// Change the Hex to match the controller actions (buttons etc..)
#define LEFT_FWD 0xD6
#define LEFT_REV 0xD8
#define RIGHT_FWD 0xDB
#define RIGHT_REV 0xDD
#define FRAME_UP 0xB8
#define FRAME_DOWN 0xB6
#define CBELT_ON 0xAA
#define CBELT_REV 0xAA
#define AUG_ON 0xB3
#define AUG_REV 0xB4
#define AUG_UP 0xB7
#define AUG_DOWN 0xB5
#define AUTO_MINE 0xB2
#define E_STOP 0xB1
//------------------------- Pins ----------------------------------------//
//Motors
#define LDRIVE_PIN 13
#define RDRIVE_PIN 12
#define LACT_PIN 11
#define RACT_PIN 10
#define CBELT_PIN 9
#define AUGPWR_PIN 8
#define AUGDIR_PIN 7
#define LBSCR_PIN 6
#define LBSDR_PIN 5
#define RBSCR_PIN 4
#define RBSDR_PIN 3
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
#define P_MOD 160 //Allows matching of actuator speeds.
#define N_MOD 20 
#define OFF LOW
#define ON HIGH
#define UP 1
#define DOWN -1
#define wait 1500
//------------------------- toggle bools ----------------------------------------//
int toggleFrameUp = 0;
int toggleFrameDown = 0;
int toggleAugOn = 0;
int toggleAugRev = 0;
int toggleAugUp = 0;
int toggleAugDown = 0;
//------------------------- function defs ---------------------------------------//
int leftForward();
int rightForward();
int rightReverse();
int leftReverse();
int frameUp();
int frameDown();
int augerOn();
int augerRev();
int augerUp();
int augerDown();
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
  
  //Initialize Lbscr
  pinMode(LBSCR_PIN, OUTPUT);
  digitalWrite(LBSCR_PIN, OFF);

  //Initialize LeftBscrDir
  pinMode(LBSDR_PIN, OUTPUT);
  digitalWrite(RBSDR_PIN, OFF);
  
  //Initialize Rbscr
  pinMode(RBSCR_PIN, OUTPUT);
  digitalWrite(RBSCR_PIN, OFF);
  
  //Initialize RightBscrDir
  pinMode(RBSDR_PIN, OUTPUT);
  digitalWrite(RBSDR_PIN, OFF);
  
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
    case (int) E_STOP:
      eStop();
      break;
  }
}

//====================== readCommand( Serial ser )==============
//Read a command from the given Serial connection 

/* function to move the left drive forward */
int leftForward() {
  Ldrive.write(POS);
  delay(wait);
  Ldrive.write(NEUTRAL);
  return 1;
}

/* function to move the right drive forward */
int rightForward() {
  Rdrive.write(POS);
  delay(wait);
  Rdrive.write(NEUTRAL);
  return 1;
}

/* function to move the right drive in reverse */
int rightReverse() {
  Rdrive.write(NEG);
  delay(wait);
  Rdrive.write(NEUTRAL);
  return 1;
}

/* function to move the left drive in reverse */
int leftReverse() {
  Ldrive.write(NEG);
  delay(wait);
  Ldrive.write(NEUTRAL);
  return 1;
}

/* 
  function to raise the frame
  this is a toggle. it returns the value to set the global toggle to
  @returns 1 if frame is raising, 0 if frame is stopped raising
*/
int frameUp() {
  if (!toggleFrameUp) {
    Lact.write(POS);
    Ract.write(P_MOD); //One of the actual actuators moves at a slower rate, so PWM is modified to match speeeds.       toggleFrameUp = 1;
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
    Lact.write(NEG);
    Ract.write(N_MOD); //One of the actual actuators moves at a slower rate, so PWM is modified to match speeeds.
    return 1;
  } else {
    Lact.write(NEUTRAL);  
    Ract.write(NEUTRAL);
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
    digitalWrite(AUGPWR_PIN, OFF);
    digitalWrite(AUGDIR_PIN,OFF);
    return 0;
  }
}

int augerUp() {
  digitalWrite(LBSDR_PIN, HIGH); // Enables left ball screw the motor to move in a particular direction
  digitalWrite(RBSDR_PIN, HIGH); // Enables the right ball screw motor to move in a particular direction
  if (!toggleAugUp) {
    digitalWrite(LBSCR_PIN, HIGH); 
    digitalWrite(RBSCR_PIN, HIGH); //If on of the ballscrews turn at a slower rate it will need its own delay
    return 1;
    //if (digitalRead(A_up)== HIGH)
    //break;
  } else {
    digitalWrite(LBSCR_PIN, LOW);
    digitalWrite(RBSCR_PIN, LOW);
    return 0;
  }
}

int augerDown() {
  digitalWrite(LBSDR_PIN, LOW); // Enables the left stepper motor to move in a particular direction
  digitalWrite(RBSDR_PIN, LOW); // Enables the right stepper motor to move in a particular direction
  if (!toggleAugDown) {
    digitalWrite(LBSCR_PIN, HIGH); 
    digitalWrite(RBSCR_PIN, HIGH); //If on of the ballscrews turn at a slower rate it will need its own delay
    return 1;
  } else {
    digitalWrite(LBSCR_PIN,LOW);
    digitalWrite(RBSCR_PIN, LOW);
    return 0;
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
  digitalWrite(LBSCR_PIN, OFF);
  digitalWrite(RBSCR_PIN, OFF);
  digitalWrite(LBSDR_PIN, OFF);
  digitalWrite(RBSDR_PIN, OFF);
}

void readCommand1(byte command)
{
  byte action = command;

  if( action == LEFT_FWD ) //See Actions section for assignments
  {
    //Serial.write("Reading Action LEFT_FWD");
    Ldrive.write(POS);
    delay(wait);
    Ldrive.write(NEUTRAL);
  }
  
  if( action == RIGHT_FWD ) //See Actions section for assignments
  {
    //Serial.write("Reading Action RIGHT_FWD");
    Rdrive.write(POS);
    delay(wait);
    Rdrive.write(NEUTRAL);    
  }
  
  if( action == LEFT_REV ) //See Actions section for assignments
  {
    //Serial.write("Reading Action LEFT_REV");
    Ldrive.write(NEG);
    delay(wait);
    Ldrive.write(NEUTRAL);    
  }

  if( action == RIGHT_REV ) //See Actions section for assignments
  {
    //Serial.write("Reading Action RIGHT_REV");
    Rdrive.write(NEG);
    delay(wait);
    Rdrive.write(NEUTRAL);    
  }  
  
  if( action == FRAME_UP ) //See Actions section for assignments
  {
    //while(digitalRead(F_up, LOW)){
    //Serial.write("Reading Action FRAME_UP");
    Lact.write(POS);
    Ract.write(P_MOD); //One of the actual actuators moves at a slower rate, so PWM is modified to match speeeds.
    delay(wait);
    Lact.write(NEUTRAL);  
    Ract.write(NEUTRAL); 
  }
  
  if( action == FRAME_DOWN ) //See Actions section for assignments
  {
    //while(digitalRead(F_down, LOW)){
    //Serial.write("Reading Action FRAME_DOWN");
    Lact.write(NEG);
    Ract.write(N_MOD); //One of the actual actuators moves at a slower rate, so PWM is modified to match speeeds.
    delay(wait);
    Lact.write(NEUTRAL);  
    Ract.write(NEUTRAL); 
  }
  
  if( action == CBELT_ON ) //See Actions section for assignments
  {
    //Serial.write("Reading Action CBELT_ON");
    Cbelt.write(POS);
    delay(wait);
    Rdrive.write(NEUTRAL);    
  }  
  
  if( action == CBELT_REV) //See Actions section for assignments
  {
    //Serial.write("Reading Action CBELT_REV");
    Cbelt.write(NEG);
    delay(wait);
    Rdrive.write(NEUTRAL);    
  }  
  
  if( action == AUG_ON ) //See Actions section for assignments
  {
    //Serial.write("Reading Action AUG_ON");
    digitalWrite(AUGPWR_PIN, ON);
    delay(wait);
    digitalWrite(AUGPWR_PIN, OFF); 
  }
  
  if( action == AUG_REV ) //See Actions section for assignments
  {
    //Serial.write("Reading Action AUG_REV");
    digitalWrite(AUGPWR_PIN, ON);
    digitalWrite(AUGDIR_PIN, ON);
    delay(wait);
    digitalWrite(AUGPWR_PIN, OFF);
    digitalWrite(AUGDIR_PIN,OFF);
  }
  
  if( action == AUG_UP ) //See Actions section for assignments
  {
    //Serial.write("Reading Action AUG_UP");
    //while(digitalRead(A_up, LOW)){
    digitalWrite(LBSDR_PIN, HIGH); // Enables left ball screw the motor to move in a particular direction
    digitalWrite(RBSDR_PIN, HIGH); // Enables the right ball screw motor to move in a particular direction
    for(int x = 0; x < 200; x++)   // Makes 200 pulses for making one full cycle rotation
    {
      if (digitalRead(A_up)== HIGH)
        break;
      digitalWrite(LBSCR_PIN, HIGH); 
      digitalWrite(RBSCR_PIN, HIGH); //If on of the ballscrews turn at a slower rate it will need its own delay
      delayMicroseconds(500); 
      digitalWrite(LBSCR_PIN,LOW);
      digitalWrite(RBSCR_PIN, LOW);
      delayMicroseconds(500); 
    }
  }
    
  if( action == AUG_DOWN ) //See Actions section for assignments
  {
    //Serial.write("Reading Action AUG_DOWN"); 
    //while(digitalRead(A_down, LOW)){
    digitalWrite(LBSDR_PIN, LOW); // Enables the left stepper motor to move in a particular direction
    digitalWrite(RBSDR_PIN, LOW); // Enables the right stepper motor to move in a particular direction
    for(int x = 0; x < 200; x++)   // Makes 200 pulses for making one full cycle rotation
    {
      digitalWrite(LBSCR_PIN, HIGH); 
      digitalWrite(RBSCR_PIN, HIGH); //If on of the ballscrews turn at a slower rate it will need its own delay
      delayMicroseconds(500); 
      digitalWrite(LBSCR_PIN,LOW);
      digitalWrite(RBSCR_PIN, LOW);
      delayMicroseconds(500); 
    }
  }  
  
/*if( action == AUTO_MINE ) //See Actions section for assignments
  {
    //Serial.write("Reading Action AUTO_MINE"); 
*/
  
  if( action == E_STOP ) //See Actions section for assignments
  {
    //Serial.write("Reading Action E_STOP"); 
    Ldrive.write(NEUTRAL);
    Rdrive.write(NEUTRAL);
    Lact.write(NEUTRAL);
    Ract.write(NEUTRAL);
    Cbelt.write(NEUTRAL);    
    digitalWrite(AUGPWR_PIN, OFF);
    digitalWrite(AUGDIR_PIN, OFF);
    digitalWrite(LBSCR_PIN, OFF);
    digitalWrite(RBSCR_PIN, OFF);
    digitalWrite(LBSDR_PIN, OFF);
    digitalWrite(RBSDR_PIN, OFF);
  }
}


