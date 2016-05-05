/* 
   -- ECE590 - Robot Design --
   David Gregory - Demo 2 - Robotic Arm
   OpenCM9.04 - Motor Control & USB Interface
   
   Controls 2 Dynamixel motors
*/
 /* Serial device defines for DXL1 bus */ 
#define DXL_BUS_ID 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04

#define ARM_MOTOR 1
#define SWVL_MOTOR 2
#define TILT_MOTOR 3

#define CLWSWVL_MOTOR 4
#define CLWGRP_MOTOR 5

#define ARM_SPEED 1023 //Slower turn speed
#define SWVL_SPEED  250
#define TILT_SPEED 1000 // Tilt Motor Speed
#define STOP_SPEED  0  //For wheelMode
#define CLWSWVL_SPEED 50
#define CLWGRP_SPEED 100

#define ARM_MIN 50
#define ARM_MAX 550
#define ARM_STEP 10

#define CLWSWVL_MIN 0
#define CLWSWVL_MAX 1020
#define CLWSWVL_MID 500

#define CLWGRP_MIN 200
#define CLWGRP_MAX 440
#define CLW_STEP 10

#define CW_BIT 0x400 //10th bit

#define MAXTRQ 1023
#define MINTRQ 1
#define HLFTRQ 511

#define TILT_TRQ 700
#define ARM_TRQ 1023
#define SWVL_TRQ 700
#define CLW_TRQ 700

//#define ARM_CW_LIMIT 0    //used for jointMode
//#define ARM_CCW_LIMIT 600

#define PRESENT_POS 37 //Current Position Register Address

#define LOOP_DELAY 250

Dynamixel Dxl(DXL_BUS_ID);

//int ARM_POSITION, ARM_POSITION_NEW;
int CLWSWVL_POSITION, CLWSWVL_POSITION_NEW;
int CLWGRP_POSITION, CLWGRP_POSITION_NEW;

byte STOP_NOW = 0;

void setup(){
  //You can attach your serialUSB interrupt
  //or, also detach the interrupt by detachInterrupt(void) method
  SerialUSB.attachInterrupt(usbInterrupt);
  
  pinMode(BOARD_LED_PIN, OUTPUT);  //toggleLED_Pin_Out; For FUN!!
  
  // Initialize the dynamixel bus:
  // Dynamixel 2.0 Baudrate -> 0: 9600, 1: 57600, 2: 115200, 3: 1Mbps 
  Dxl.begin(3);
  Dxl.wheelMode(ARM_MOTOR);   //jointMode() is to use position mode
  Dxl.wheelMode(SWVL_MOTOR);  //wheelMode() is to use wheel mode
  Dxl.wheelMode(TILT_MOTOR);  //wheelMode
  Dxl.jointMode(CLWSWVL_MOTOR);
  Dxl.jointMode(CLWGRP_MOTOR);
  
  Dxl.writeByte(ARM_MOTOR, 24, 0);  //Disable Torque Enable; NEEDED before changing CW/CCW Angles Limits
  Dxl.writeByte(CLWSWVL_MOTOR, 24, 0);  //Disable Torque Enable; NEEDED before changing CW/CCW Angles Limits
  Dxl.writeByte(CLWGRP_MOTOR, 24, 0);  //Disable Torque Enable; NEEDED before changing CW/CCW Angles Limits
  
  //Dxl.cwAngleLimit(ARM_MOTOR, ARM_CW_LIMIT);   //CW Angle Limit
  //Dxl.ccwAngleLimit(ARM_MOTOR, ARM_CCW_LIMIT);  //CCW Angle Limit -> minimum 700
  
  //Set TRQ limits
  Dxl.maxTorque(ARM_MOTOR, ARM_TRQ);
  Dxl.maxTorque(SWVL_MOTOR, SWVL_TRQ);
  Dxl.maxTorque(TILT_MOTOR, TILT_TRQ);
  Dxl.maxTorque(CLWSWVL_MOTOR, CLW_TRQ);
  Dxl.maxTorque(CLWGRP_MOTOR, CLW_TRQ);
  

  Dxl.goalSpeed(ARM_MOTOR, STOP_SPEED);     //wheelMode
  Dxl.goalSpeed(SWVL_MOTOR, STOP_SPEED);   //wheelMode
  Dxl.goalSpeed(TILT_MOTOR, STOP_SPEED);   //wheelMode
  Dxl.goalSpeed(CLWSWVL_MOTOR, CLWSWVL_SPEED);
  Dxl.goalSpeed(CLWGRP_MOTOR, CLWGRP_SPEED);
  
  Dxl.ledOff(ARM_MOTOR); //Turn off LED - ARM_MOTOR
  Dxl.ledOff(SWVL_MOTOR);
  Dxl.ledOff(TILT_MOTOR);
  Dxl.ledOff(CLWSWVL_MOTOR);
  Dxl.ledOff(CLWGRP_MOTOR);
  
  //ARM_POSITION = ARM_MIN;  //Start Postion of Arm
  //ARM_POSITION_NEW = ARM_POSITION;
  
  CLWSWVL_POSITION = CLWSWVL_MID;
  CLWSWVL_POSITION_NEW = CLWSWVL_POSITION;
  
  CLWGRP_POSITION = CLWGRP_MIN;
  CLWGRP_POSITION_NEW = CLWGRP_POSITION;
  
  //Dxl.writeByte(ARM_MOTOR, 24, 1);  //Enable Torque Enable???
  
  //Dxl.goalPosition(ARM_MOTOR, ARM_POSITION);  //Start Default position
  Dxl.goalPosition(CLWSWVL_MOTOR, CLWSWVL_POSITION);
  Dxl.goalPosition(CLWGRP_MOTOR, CLWGRP_POSITION);
  
  delay(3000); //give time for motor to move
 
}//End setup()

/** USB INTERRUPT **/
//USB max packet data is maximum 64byte, so nCount can not exceeds 64 bytes
void usbInterrupt(byte* buffer, byte nCount){
  
  //SerialUSB.print("nCount =");
  //SerialUSB.println(nCount);   //Prints byte count
  
  //RETURN Received CMD
  for(unsigned int i=0; i < nCount;i++){  //printf_SerialUSB_Buffer[N]_receive_Data
    SerialUSB.println((char)buffer[i]);
  }
    
  //Extend ARM
  if(buffer[0] == 119){         //ASCII w: EXTEND
    //SerialUSB.println("CMD_EXTEND RCVD");
    Dxl.ledOn(ARM_MOTOR,2);    //Turn LED on (GRN)
    Dxl.goalSpeed(ARM_MOTOR, ARM_SPEED); //CCW
    STOP_NOW = 0; //clear stop flag
    /* EXTEND Forward
    if(ARM_POSITION < ARM_MAX){
      ARM_POSITION_NEW = (ARM_POSITION + ARM_STEP);
      //ARM_POSITION += ARM_STEP;
      //Dxl.goalPosition(ARM_MOTOR, (ARM_POSITION + ARM_STEP));\
    }*/
  }
 
   //RETRACT ARM
  else if(buffer[0] == 120){       //ASCII x: RETRACT
    //SerialUSB.println("CMD_RTRCT RCVD");
    Dxl.ledOn(ARM_MOTOR,2);    //Turn LED on (GRN)
    Dxl.goalSpeed(ARM_MOTOR, ARM_SPEED | CW_BIT); //CW
    STOP_NOW = 0;   //clear stop flag
    /* ARM RETRACT
    if(ARM_POSITION > ARM_MIN){
      ARM_POSITION_NEW = (ARM_POSITION - ARM_STEP);
      //ARM_POSITION -= ARM_STEP;
      //Dxl.goalPosition(ARM_MOTOR, (ARM_POSITION - ARM_STEP));
    }*/ 
  }
  
  //LEFT  
  else if(buffer[0] == 97){       //ASCII a:LFT
    Dxl.ledOn(SWVL_MOTOR,3);    //Turn LED on (YLW)
    //SerialUSB.println("CMD_LFT RCVD");
    /* Turn LEFT */
    Dxl.goalSpeed(SWVL_MOTOR, SWVL_SPEED); //CCW
   STOP_NOW = 0; //clear stop flag 
  }
  
  //RIGHT  
  else if(buffer[0] == 100){       //ASCII d:RGT
    Dxl.ledOn(SWVL_MOTOR,3);    //Turn LED on (YLW)
    //SerialUSB.println("CMD_RGT RCVD");
    /* Turn RIGHT */
    Dxl.goalSpeed(SWVL_MOTOR, SWVL_SPEED | CW_BIT); //CW
    STOP_NOW = 0; //clear stop flag
  }
  
  //TILT FWD
  else if(buffer[0] == 101){
    Dxl.ledOn(TILT_MOTOR,5);    //Turn LED on (BLU)
    //SerialUSB.println("CMD_TILTFWD RCVD");
    Dxl.goalSpeed(TILT_MOTOR, TILT_SPEED | CW_BIT); //CW
  }
  
  //TILT BACK
  else if(buffer[0] == 99){
    Dxl.ledOn(TILT_MOTOR,5);    //Turn LED on (BLU)
    //SerialUSB.println("CMD_TILTBCK RCVD");
    Dxl.goalSpeed(TILT_MOTOR, TILT_SPEED); //CCW
  } 
  
  //STOP
  else if(buffer[0] == 115){       //ASCII s:STOP
    //SerialUSB.println("CMD_STOP RCVD");
   /*ALL STOP*/
    Dxl.goalSpeed(SWVL_MOTOR, STOP_SPEED);
    Dxl.goalSpeed(TILT_MOTOR, STOP_SPEED);   
  }
  
  //CLAW SWIVL CCW "j"
  else if(buffer[0] == 106){
    Dxl.ledOn(CLWSWVL_MOTOR,6);    //Turn LED on ()
    if(CLWSWVL_POSITION < CLWSWVL_MAX){
      CLWSWVL_POSITION_NEW = (CLWSWVL_POSITION + CLW_STEP);
    }
  }
  
  //CLAW SWIVL CW "l"
  else if(buffer[0] == 108){
    Dxl.ledOn(CLWSWVL_MOTOR,6);    //Turn LED on ()
    if(CLWSWVL_POSITION > CLWSWVL_MIN){
      CLWSWVL_POSITION_NEW = (CLWSWVL_POSITION - CLW_STEP);
    }
  }

  //CLAW GRIP CLOSE "k"
  else if(buffer[0] == 107){
    Dxl.ledOn(CLWGRP_MOTOR,7);    //Turn LED on ()
    if(CLWGRP_POSITION < CLWGRP_MAX){
      CLWGRP_POSITION_NEW = (CLWGRP_POSITION + CLW_STEP);
    }
  }
  
  //CLAW GRIP OPEN "i"
  else if(buffer[0] == 105){
    Dxl.ledOn(CLWGRP_MOTOR,7);    //Turn LED on ()
    if(CLWGRP_POSITION > CLWGRP_MIN){
      CLWGRP_POSITION_NEW = (CLWGRP_POSITION - CLW_STEP);
    }
  }  
 
  //INCORRECt COMMAND
  else    //CMD not received correctly!
    SerialUSB.println("ERROR: INCORRECT CMD RX");
}



/*************************/
/*********MAIN LOOP*******/
void loop(){
  
    //READ ARM Position
  //ARM_POSITION = Dxl.readWord(ARM_MOTOR, PRESENT_POS); // POLL Present ARM_POSITION
    //SerialUSB.print("Present Position: ");
    //SerialUSB.println(ARM_POSITION);
  
  //Flash the led
  toggleLED();
  delay(LOOP_DELAY);
  
  /*
  if(ARM_POSITION_NEW != ARM_POSITION){
    Dxl.goalPosition(ARM_MOTOR, ARM_POSITION_NEW); //Update ARM_POSITION
    ARM_POSITION = ARM_POSITION_NEW; //update ARM_POSITION
  }
  */
  
  if(CLWSWVL_POSITION_NEW != CLWSWVL_POSITION){
    Dxl.goalPosition(CLWSWVL_MOTOR, CLWSWVL_POSITION_NEW); //Update CLW_POSITION
    CLWSWVL_POSITION = CLWSWVL_POSITION_NEW; //update SWVL_POSITION
  }
 
  if(CLWGRP_POSITION_NEW != CLWGRP_POSITION){
    Dxl.goalPosition(CLWGRP_MOTOR, CLWGRP_POSITION_NEW); //Update CLW_POSITION
    CLWGRP_POSITION = CLWGRP_POSITION_NEW; //update GRP_POSITION
  } 
  
  
  if(STOP_NOW > 0){
    Dxl.goalSpeed(ARM_MOTOR, STOP_SPEED);
    Dxl.goalSpeed(SWVL_MOTOR, STOP_SPEED); //lets try this
    Dxl.goalSpeed(TILT_MOTOR, STOP_SPEED);
    Dxl.ledOff(ARM_MOTOR); //Turn off LED - ARM_MOTOR
    Dxl.ledOff(SWVL_MOTOR);
    Dxl.ledOff(TILT_MOTOR);
    Dxl.ledOff(CLWSWVL_MOTOR);
    Dxl.ledOff(CLWGRP_MOTOR);
  }
  else{
   STOP_NOW = 1; 
  }
}

