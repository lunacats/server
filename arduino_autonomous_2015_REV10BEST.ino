


#include <Servo.h>

/*Arduino code for the 2015 Lunabot Autonomous. Takes input
  from the RasPi in the form of a header and then parameters 
  and interprets it to actually do what is required of it. This
  code is based on 2014 teleop code.
  Adapted by Bill Curtis March 2015
  Modified by Daniel Eltringham April 22, 2015
  Modified by Daniel Eltringham May 10, 2015
  Modified at RMC  May 18, 2015 (recalibrated hopper pot values)
  
  
The code was modified to use the servo library to send signals to all motor controllers.
This is going to be confusing. There are two instances of the Servo library's
Servo thing, called xpan and ytilt. These are necessary for actually making the real-world
servos do stuff. Later on in the code there are two instances of a Serv struct I made,
just called pan and tilt (because I made them first). These structs deal with the variables
necessary to know what the servos are supposed to be doing.*/
Servo xpan;
Servo ytilt;
Servo RightMotor;
Servo LeftMotor;
Servo HopperActuator;
Servo ScoopActuator;
   
#define MOTOR_CMD 0x00
#define SERVO_CMD 0x01 //I'm using 0x01 as the servo command header now.
#define EMERGENCY_STOP_CMD 0x05

//-------------------------MOTOR/PWM ID----------------------------------------
#define LEFT_ID 0
#define RIGHT_ID 1
#define SCOOP_ID 2
#define HOPPER_ID 3

//-------------------------SERVO ID-----------------------------------------
#define PAN_ID 4
#define TILT_ID 5

//-------------------------Pins-------------------------------------------- 
//Motors
#define LEFT_MOTOR_PIN  10
#define RIGHT_MOTOR_PIN 11
//Scoop
#define SCOOP_MOTOR_PIN 3 //ARM and BUCKET have become SCOOP and HOPPER, respectively
//Hopper
#define HOPPER_MOTOR_PIN  9

#define SCOOP_POT_PIN A0
#define HOPPER_POT_PIN A1

#define PAN_SERV_PIN 7
#define TILT_SERV_PIN 8

//-------------------------MISC--------------------------------------------
#define VICTOR_MAX 170 //FULL Forward
#define VICTOR_HALF_FORWARD 131 //half speed forward
#define VICTOR_STOP 92 //STOP on VICTOR
#define VICTOR_HALF_BACKWARD 51 //half speed backward
#define VICTOR_MIN 10 //FULL Reverse

#define INSPEED_MIN 0
#define INSPEED_HALF 127
#define INSPEED_MAX 255

#define FORWARD 0
#define REVERSE 1

#define L_MOTOR_REVERSED 0
#define R_MOTOR_REVERSED 0
#define SCOOP_REVERSED 0 //Set the actuator "reveresed" values to be the same
#define HOPPER_REVERSED 1

#define HOPPER_POT_MAX 652 //can not go any farther
#define HOPPER_POT_DRIVING 945
#define HOPPER_JIGGLE_POINT 983  //Moves between this point and hopper max to shake out simulant

#define SCOOP_FULL_DOWN 1022
#define SCOOP_FULL_UP 920
#define SCOOP_DRIVE 987

int lastLeftMotorSpeed = 0; //acceptable values: 1,0,-1
int lastRightMotorSpeed = 0; 
int curLeftMotorSpeed = 0;
int curRightMotorSpeed = 0;
boolean leftChanged = false;
boolean rightChanged = false;

//--------------Structs-----------------------------
typedef struct 
{
  int speedPin;//The pwm pin for the motor
  byte mSpeed; //The speed of the motor 0-255
  int reversed;
  int dir; //The direction of the motor
  int idNum;
} Motor;

typedef struct
{
  int angle;//the angle of the servo. No pin var is necessary because the Servo
            //library handles it.
} Serv;

Motor lMotor;
Motor rMotor;
Motor scoop; //'arm' became 'scoop' because that's what Daniel
             //called it in the layout file.
Motor hopper; //'bucket' became 'hopper' for the same reason.
              //It's also now an instance of Motor because we don't
              //use the actuator struct anymore. (I think).
Serv pan;
Serv tilt;

int scoopPotVal; //also changed from arm to scoop
int hopperPotVal; //also changed from bucket to hopper  `
boolean potsEnabled;

byte curData = 0x00;
byte prevData;

void setup( )
{
  Serial.begin( 115200 );
  
  //Initialize lMotor
  lMotor.reversed = L_MOTOR_REVERSED;
  lMotor.dir = 0;
  lMotor.idNum = LEFT_ID;
  lMotor.mSpeed = INSPEED_MIN;
  lMotor.speedPin = LEFT_MOTOR_PIN;

  LeftMotor.attach(LEFT_MOTOR_PIN); 
  LeftMotor.write(VICTOR_STOP);

  //Initialize rMotor
  rMotor.reversed = R_MOTOR_REVERSED;
  rMotor.dir = 0;
  rMotor.idNum = RIGHT_ID;
  rMotor.mSpeed = INSPEED_MIN;
  rMotor.speedPin = RIGHT_MOTOR_PIN;
  
  RightMotor.attach(RIGHT_MOTOR_PIN); 
  RightMotor.write(VICTOR_STOP);
  
  //Initialize scoop
  scoop.reversed = SCOOP_REVERSED; //It'll be interesting to see what, if anything, all  
  scoop.dir = 0;                   //this initializaton makes the new robot do right off
  scoop.idNum = SCOOP_ID;           //the bat. Some of this might have to be changed.
  scoop.mSpeed = INSPEED_MIN;
  scoop.speedPin = SCOOP_MOTOR_PIN;
  
  ScoopActuator.attach(SCOOP_MOTOR_PIN); 
  ScoopActuator.write(VICTOR_STOP);
  
  //Initialize hopper
  hopper.reversed = HOPPER_REVERSED;
  hopper.dir = 0;
  hopper.idNum = HOPPER_ID;
  hopper.mSpeed = INSPEED_MIN;
  hopper.speedPin = HOPPER_MOTOR_PIN;
  
  HopperActuator.attach(HOPPER_MOTOR_PIN); 
  HopperActuator.write(VICTOR_STOP);
  
  //Initialize pan and tilt servos
  pan.angle = 90;
  xpan.attach(PAN_SERV_PIN);//attach the servo to a pin
  xpan.write(pan.angle);
  
  tilt.angle = 0; //I felt it wasn't necessary to have a constant for the servo angle.
  ytilt.attach(TILT_SERV_PIN); 
  ytilt.write(tilt.angle);
  
  pinMode( SCOOP_POT_PIN, INPUT );
  scoopPotVal = analogRead(SCOOP_POT_PIN);
  
  pinMode( HOPPER_POT_PIN, INPUT );
  hopperPotVal = analogRead(HOPPER_POT_PIN);
 
}


void loop( )
{

 //     Serial.print("curData = ");
 //     Serial.println(curData);
      if(Serial.available() > 0)
      {
        prevData = curData;
  //    Serial.print("prevData = ");
  //    Serial.println(prevData);
      
        curData = Serial.read();
//      Serial.print("Byte read. Now curData = ");
//      Serial.println(curData);
      
      //Commands start with the header 0xFF 0xFE at the moment
        if( prevData == 0xFF && curData == 0xFE )
        {
          Serial.println( "VALID COMMAND Executing" );
          readCommand( );
        }
      }
      
  updateOutput( );
  int scoopPotVal = potRead(0);
  int hopperPotVal = potRead(1);
  
  if (hopperPotVal >= HOPPER_POT_MAX){
    
  }
}


/*====================== readCommand( Serial ser )==============
Read a command from the given Serial connection
*/

void readCommand( )
{
  byte commandType = Serial.read( );
//  Serial.print("Command read: ");
//  Serial.println(commandType);

  if( commandType == MOTOR_CMD ) //0x00 is a MOTOR Command
  {
    Serial.println("Reading Motor Command");
    readMotorCommand( );
  }
  //Check for other commands.
  
  /*The scoop and hopper are controlled via PWM now.*/
  
  if( commandType == SERVO_CMD ) //0x01 is a servo command now because I said so.
  {
    Serial.println("Reading Servo Command");
    readServoCommand( ); 
  }
  
  if( commandType == EMERGENCY_STOP_CMD )
  {
    Serial.println("EMERGENCY STOOOOOOOOOP!");
    emergencyStop( ); 
  }
}


/*====================== readMotorCommand( Serial ser ) =========
Run a motor command from the given serial connection
*/
void readMotorCommand( )
{
  byte motorID = Serial.read( ); //Which motor is to be run
  byte motorDir = Serial.read( );
  byte motorSpeed = Serial.read( );
  
  Serial.print( "motorID: " );
  Serial.println( motorID );
  Serial.print( " motorDir: " );
  Serial.println( motorDir );
  Serial.print( " motorSpeed: " );
  Serial.println( motorSpeed );
  
  if( motorID == LEFT_ID )       //Read motor ID and speed/dir, set accordingly
  {
    lMotor.dir = motorDir;
    if (motorSpeed <= 80)
    {
      lMotor.mSpeed = 0;
    }
    else 
    {
      lMotor.mSpeed = INSPEED_MAX;
    }
    
    if(lMotor.mSpeed == 0)
      curLeftMotorSpeed = 0;
    else
    {
      if(lMotor.dir == FORWARD)
      {
        curLeftMotorSpeed = 1;
      }
      else
      {
        curLeftMotorSpeed = -1;
      }
    }
    if (curLeftMotorSpeed != lastLeftMotorSpeed)
    {
      leftChanged = true;
      lastLeftMotorSpeed = curLeftMotorSpeed;
    }  
  }
  else if( motorID == RIGHT_ID )
  {
    rMotor.dir = motorDir;
    if (motorSpeed <= 80)
     { 
       rMotor.mSpeed = 0;
     }
    else
    {
      rMotor.mSpeed = INSPEED_MAX;
    }
    if(rMotor.mSpeed == 0)
      curRightMotorSpeed = 0;
    else
    {
      if(rMotor.dir == FORWARD)
      {
        curRightMotorSpeed = 1;
      }
      else
      {
        curRightMotorSpeed = -1;
      }
    }
    if (curRightMotorSpeed != lastRightMotorSpeed)
    {
      rightChanged = true;
      lastRightMotorSpeed = curRightMotorSpeed;
    }  
  }
  else if ( motorID == SCOOP_ID ) //These two are the parts I added for controlling the
  {                               //scoop and hopper. This code structure should be the
    scoop.dir = motorDir;         //the same as the drive motors, and as long as the dir
    scoop.mSpeed = motorSpeed;    //and speed are the right numbers it should work right.
  }                               //Just remember that they're actuators and not motors.
  else if ( motorID == HOPPER_ID )
  {
     hopper.dir = motorDir;
     hopper.mSpeed = motorSpeed;
  }
}


//============= Read Servo Commands ============
void readServoCommand( )
{
  byte servoID = Serial.read();  
  int ang = Serial.read();
  
  Serial.print("ServoID = ");
  Serial.println(servoID);
  Serial.print(" angle = ");
  Serial.println(ang);
  
  if( servoID == PAN_ID )       //read servo ID and angle, set accordingly
  {
    pan.angle = ang;
  }
  else if ( servoID == TILT_ID )
  {
    tilt.angle = ang;
  }
}  

//=============== E-Stop =========
void emergencyStop( )
{  
  //SHUTDOWN MOTORS
  
  lMotor.mSpeed = INSPEED_MIN;
  lMotor.dir = FORWARD;
  
  rMotor.mSpeed = INSPEED_MIN;
  rMotor.dir = FORWARD;
  
  //SHUT DOWN ACTUATORS
  
  scoop.mSpeed = INSPEED_MIN;  
  hopper.mSpeed = INSPEED_MIN; 
}                             

//============= Read Potentiometers ============

int potRead(int pData)
{
  if (pData == 0) {
    analogRead(SCOOP_POT_PIN);   //"primes" the pin
    return analogRead(SCOOP_POT_PIN);  //Scoop read
  }
  else if (pData == 1) {
    analogRead(HOPPER_POT_PIN);
    return analogRead(HOPPER_POT_PIN);  //Hopper read
  }
}

//--------THIS IS THE PART WHERE STUFF ACTUALLY STARTS HAPPENING-----------------------

void updateOutput( ) //Updated to reflect my changes.
{                    
  Serial.println("Updating Output");
  
  //LEFT MOTOR OUTPUT
  //ALSO handles corrected direction
  int actualLMDir = ( lMotor.dir + lMotor.reversed ) % 2;
  int actualLMSpeed = VICTOR_STOP;
  
  if( actualLMDir == FORWARD )
  {
    actualLMSpeed = map( lMotor.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MAX );
  }
  else 
  {
    actualLMSpeed = map( lMotor.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MIN );
  }
  
  //LeftMotor.write(actualLMSpeed); MOTOR WRITING IS DONE LATER ON, AFTER THE RIGHT SIDE MOTOR CALCULATIONS
  Serial.print("Actual Left Motor Speed = ");
  Serial.println(actualLMSpeed);
  
  //RIGHT MOTOR OUTPUT
  //ALSO handles corrected direction
  int actualRMDir = ( rMotor.dir + rMotor.reversed ) % 2;
  int actualRMSpeed = VICTOR_STOP;
  
  if( actualRMDir == FORWARD )
  {
    actualRMSpeed = map( rMotor.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MAX );
  }
  else 
  {
    actualRMSpeed = map( rMotor.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MIN );
  }
  
  if( leftChanged && rightChanged && (curLeftMotorSpeed + curRightMotorSpeed == 0 || abs(curLeftMotorSpeed + curRightMotorSpeed) == 2) )
  {
    LeftMotor.write(actualLMSpeed);
    RightMotor.write(actualRMSpeed);
    leftChanged = false;
    rightChanged = false;
  }
  
  Serial.print("Actual Right Motor Speed = ");
  Serial.println(actualRMSpeed);
  
  //SCOOP ACTUATOR OUTPUT
  
  int actualSCOOPDir = ( scoop.dir + scoop.reversed ) % 2;
  int actualSCOOPSpeed = VICTOR_STOP;
  
  if( actualSCOOPDir == FORWARD )
  {
    actualSCOOPSpeed = map( scoop.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MAX );
  }
  else 
  {
    actualSCOOPSpeed = map( scoop.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MIN );
  }
  
  ScoopActuator.write(actualSCOOPSpeed); 
  Serial.print("Actual Scoop Actuator Speed = ");
  Serial.println(actualSCOOPSpeed); 
  
  //HOPPER ACTUATOR OUTPUT
  int actualHOPPERDir = ( hopper.dir + hopper.reversed ) % 2;
  int actualHOPPERSpeed = VICTOR_STOP;
  
  if( actualHOPPERDir == FORWARD )
  {
    
      
      actualHOPPERSpeed = map( hopper.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MAX ); //This is when a normal forward
      
  }
  else 
  {
    int hopperPotVal = potRead(1);
      if (hopperPotVal >= HOPPER_POT_MAX){  //When the hopper is at max. extension and the motor is trying to move farther, stop it.
        hopper.mSpeed = INSPEED_MIN; 
        Serial.println('Hopper limit reached!');
      }
    actualHOPPERSpeed = map( hopper.mSpeed, INSPEED_MIN, INSPEED_MAX, VICTOR_STOP, VICTOR_MIN );
  }
  
  HopperActuator.write(actualHOPPERSpeed); 
  Serial.print("Actual Hopper Actuator Speed = ");
  Serial.println(actualHOPPERSpeed);
  
  //SERVO stuff
  //There shouldn't need to be any if statements for these.
  xpan.write(pan.angle);
  ytilt.write(tilt.angle);
//  Serial.print("Pan Servo Angle = ");
//  Serial.println(pan.angle);
//  Serial.print("Tilt Servo Angle = ");
//  Serial.println(tilt.angle);
  
  
  scoopPotVal = analogRead(SCOOP_POT_PIN);   //I'm assuming these will be put to use in some way,
  hopperPotVal = analogRead(HOPPER_POT_PIN); //but right now the code reads them but never uses them.

}
