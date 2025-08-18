/* defs.h */

#define SOFTWARE_VERSION 1                // Saved in config file, used for detecting software upgrades
#define MACHINE_VERSION_UNKNOWN 0
#define MACHINE_VERSION_1       1         // First version
#define FORWARD 0
#define REVERSE 1

#define SERVICETIME 600                   // 10 min
#define M_PI 3.14159265359

//*****************************************
//*   language                            *
//*****************************************
#define ENGLISH  0
#define GERMAN   1
#define FRENCH   2
#define ITALIAN  3
#define SPANISH  4
#define POLISH   5

//******************************************
//*   I/O Digital Inputs (inputs1-28)      *
//******************************************
#define STEPM_HOME        ebmGetInput(0,1)    // 
#define STEPM_ROTATION    ebmGetInput(0,2)    //  
#define STOP_PLATE        ebmGetInput(0,5)    // Stop plate sensor in picking station
#define PALLET            ebmGetInput(0,6)    // Pallet in place
#define POS_SENSOR_L      ebmGetInput(0,7)    // Position sensor left
#define POS_SENSOR_R      ebmGetInput(0,8)    // Position sensor right
#define SCANNER_SLOW      (!ebmGetInput(0,9))    // Warning from laser scanner, run slow
#define SCANNER_STOP      (!ebmGetInput(0,10))   // Warning from laser scanner, stop
#define EMERGENCY_STOP    (!ebmGetInput(0,11))// Emergency stop signal
#define EMERGENCY_FAIL    ebmGetInput(0,12)   // Failure found in emergency stop curcuit
#define CONV_FULL_SENSOR  (!ebmGetInput(0,13))   // Lift in top position
//#define CONV_FULL_SENSOR  (ebmGetInput(0,13))   // Lift in top position
#define CONV_START_SENSOR ebmGetInput(0,14)   // Lift in lowest position

//**************************************************
//*   I/O Digital Outputs (outputs1-18) ON/OFF     *
//**************************************************
#define M2_STEP_ON            ebmSetOutput(0,1,1)  // 
#define M2_STEP_OFF           ebmSetOutput(0,1,0)  // 
#define M2_DIR_ON             ebmSetOutput(0,2,1)  // 
#define M2_DIR_OFF            ebmSetOutput(0,2,0)  // 
#define M1_ON                 ebmSetOutput(0,3,1)  // 
#define M1_OFF                ebmSetOutput(0,3,0)  // 
#define TEACH_HEIGHT_ON       ebmSetOutput(0,4,1)  // 
#define TEACH_HEIGHT_OFF      ebmSetOutput(0,4,0)  //
#define FINGER_RELEASE_ON     ebmSetOutput(0,5,1)  // 
#define FINGER_RELEASE_OFF    ebmSetOutput(0,5,0)  // 
#define EMS_RESET_ON          ebmSetOutput(0,6,1)  // 
#define EMS_RESET_OFF         ebmSetOutput(0,6,0)  //
#define HEARTBEAT_ON          ebmSetOutput(0,7,1)  // 
#define HEARTBEAT_OFF         ebmSetOutput(0,7,0)  // 
#define GREEN_LAMP_ON         ebmSetOutput(0,8,1)  // 
#define GREEN_LAMP_OFF        ebmSetOutput(0,8,0)  // 
#define YELLOW_LAMP_ON        ebmSetOutput(0,9,1)  // 
#define YELLOW_LAMP_OFF       ebmSetOutput(0,9,0)  //
#define RED_LAMP_ON           ebmSetOutput(0,10,1) // 
#define RED_LAMP_OFF          ebmSetOutput(0,10,0) // 
//#define DIS_FULL_SENSOR_ON    ebmSetOutput(0,11,1) // Disconnect fullsensor to conveyor
//#define DIS_FULL_SENSOR_OFF   ebmSetOutput(0,11,0) // 
#define STEPPER_PWM_ON        ebmSetOutput(0,11,1) //
#define STEPPER_PWM_OFF       ebmSetOutput(0,11,0) //
//#define CONV_MOTOR_ON         ebmSetOutput(0,12,1) // 
//#define CONV_MOTOR_OFF        ebmSetOutput(0,12,0) // 
#define STEPPER_DIR_ON        ebmSetOutput(0,12,1) // 
#define STEPPER_DIR_OFF       ebmSetOutput(0,12,0) // 
#define CONV_READY_SIGNAL_ON  ebmSetOutput(0,13,1) // Conveyor ready (active)/ full (inactive) signal to stacker
#define CONV_READY_SIGNAL_OFF ebmSetOutput(0,13,0) // 
#define RESET_ROBOT_ON        ebmSetOutput(0,14,1) // 
#define RESET_ROBOT_OFF       ebmSetOutput(0,14,0) // 
#define O15_ON                ebmSetOutput(0,15,1) // 
#define O15_OFF               ebmSetOutput(0,15,0) //
#define SLOW_REQ_ON           ebmSetOutput(0,16,1) // Slow request to stacker
#define SLOW_REQ_OFF          ebmSetOutput(0,16,0) //
#define O17_ON                ebmSetOutput(0,17,1) //
#define O17_OFF               ebmSetOutput(0,17,0) //
#define O18_ON                ebmSetOutput(0,18,1) //
#define O18_OFF               ebmSetOutput(0,18,0) //

//***********************************************************
//*   I/O Digital Outputs (outputs1-18) STARTED/STOPPED   *
//***********************************************************

#define MAIN_STARTED          ebmGetOutput(0,1)      // Main motor STARTED
#define MAIN_STOPPED          (!ebmGetOutput(0,1))   // Main motor STOPPED 
#define LIFT_STARTED          ebmGetOutput(0,2)      // Lift motor STARTED
#define LIFT_STOPPED          (!ebmGetOutput(0,2))   // Lift motor STOPPED
#define LOOP_STARTED          ebmGetOutput(0,3)      // Loop lift motor STARTED
#define LOOP_STOPPED          (!ebmGetOutput(0,3))   // Loop lift motor STOPPED
#define RESET_SAFETY_RELAY_STARTED  ebmGetOutput(0,4)// Reset signal to emergency stop time relay STARTED
#define RESET_SAFETY_RELAY_STOPPED  (!ebmGetOutput(0,4))// Reset signal to emergency stop time relay STOPPED
#define FREQ1_STARTED         ebmGetOutput(0,5)      // Main motor STARTED
#define FREQ1_STOPPED         (!ebmGetOutput(0,5))   // Main motor STOPPED
#define FREQ2_STARTED         ebmGetOutput(0,6)      // Outfeed motor STARTED
#define FREQ2_STOPPED         (!ebmGetOutput(0,6))   // Outfeed motor STOPPED
#define HEARTBEAT_STARTED      ebmGetOutput(0,7)      // Softstop printer STARTED
#define HEARTBEAT_STOPPED      (!ebmGetOutput(0,7))   // Softstop printer STOPPED
#define STP_PRN_STARTED       ebmGetOutput(0,8)      // Stop printer STARTED
#define STP_PRN_STOPPED       (!ebmGetOutput(0,8))   // Stop printer STOPPED
#define COMP_STARTED          ebmGetOutput(0,10)     // Compressor STARTED 
#define COMP_STOPPED          (!ebmGetOutput(0,10))  // Compressor STOPPED
//#define DIS_FULL_SENSOR_STARTED   ebmGetOutput(0,11)     // Safety ready STARTED
//#define DIS_FULL_SENSOR_STOPPED   (!ebmGetOutput(0,11))  // Safety ready STOPPED
#define STEPPER_PWM_STARTED   ebmGetOutput(0,11)     
#define STEPPER_PWM_STOPPED   (!ebmGetOutput(0,11))  
//#define CONV_MOTOR_STARTED        ebmGetOutput(0,12)
//#define CONV_MOTOR_STOPPED        (!ebmGetOutput(0,12))
#define STEPPER_DIR_STARTED    ebmGetOutput(0,12)
#define STEPPER_DIR_STOPPED    (!ebmGetOutput(0,12))
#define CONV_READY_SIGNAL_STARTED ebmGetOutput(0,13)     // Green lamp STARTED
#define CONV_READY_SIGNAL_STOPPED (!ebmGetOutput(0,13))  // Green lamp STOPPED
#define YELLOW_LAMP_STARTED   ebmGetOutput(0,14)     // Yellow lamp STARTED
#define YELLOW_LAMP_STOPPED   (!ebmGetOutput(0,14))  // Yellow lamp STOPPED
#define RED_LAMP_STARTED      ebmGetOutput(0,15)     // Red lamp STARTED
#define RED_LAMP_STOPPED      (!ebmGetOutput(0,15))  // Red lamp STOPPED
#define OUT_OF_DECURL_STARTED ebmGetOutput(0,17)
#define OUT_OF_DECURL_STOPPED (!ebmGetOutput(0,17))
#define O18_STARTED           ebmGetOutput(0,18)
#define O18_STOPPED           (!ebmGetOutput(0,18))

// Analog outputs used as digital outputs
#define START_REQ_STARTED     (ebmGetAnalogOut(0,2) == 10.0)
#define START_REQ_STOPPED     (ebmGetAnalogOut(0,2) == 0)
#define STOP_REQ_STARTED      (ebmGetAnalogOut(0,3) == 10.0)
#define STOP_REQ_STOPPED      (ebmGetAnalogOut(0,3) == 0)
#define SUSPEND_REQ_STARTED   (ebmGetAnalogOut(0,4) == 10.0)
#define SUSPEND_REQ_STOPPED   (ebmGetAnalogOut(0,4) == 0)
