//****************************************************************/
// main program for Simulator
//****************************************************************/

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "lxinterface.h"
#include "ebm.h"
#include "shmemdefs.h"
#include "defs.h"
#include "vars.h"
#include "measureTime.h"

#define ONLINE

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

//pthread_t threadTurnOnOutputs;

int handle_tick100;
int handle_idleloop;

void ebmSetGuiUpdating(void);
void ebmClearGuiUpdating(void);
void ebmSetGuiUpdateProgress(int progress);
void ebmUpdateVersionInfo(void);
void ebmInitHardware(int ebmId);
bool modbusTCPconnected=false;
bool init_drives(void);


int main(int argc, char *argv[])
{
   struct periodic_info info;
   int loopCnt = 0;
   
   shared_Mem = init_shm();

// set ebm callback routines 
   cbEbmSetGuiUpdating = ebmSetGuiUpdating;
   cbEbmClearGuiUpdating = ebmClearGuiUpdating;
   cbEbmSetGuiUpdateProgress = ebmSetGuiUpdateProgress;
   cbEbmUpdateVersionInfo = ebmUpdateVersionInfo;
   cbEbmInitHardware = ebmInitHardware;

   ebmInit(0);
   if (!ebmCommInit(0,(192UL << 24) | (168UL << 16) | (0UL << 8) | 10UL))
      return -1;
   

//  Init shm parameters
   shmWriteAsStringByTagname("sys_request","0");
   shmWriteAsStringByTagname("sys_request_param","0");
   shmWriteAsStringByTagname("sys_request_response","0");

   // init vars
   ok_to_start_tick = false;
   initial();
   // create threads
   systemLoopInit();

   //robotCon_Init();
   //pthread_create(&threadRobotCon, NULL, robotCon_Thread, NULL);
   //pthread_create(&threadTurnOnOutputs, NULL, turnOnOutputs, NULL);

   handle_tick100 = lxthread_Init(&tick100);
   handle_idleloop = lxthread_Init(&idleloop);

   mtimeInit(0,13000000);
   mtimeInit(1,1000000);
   mtimeInit(2,2000000);
   mtimeInit(3,10000000);
   
   make_periodic(10000, &info); // 10ms
   //debugPrint("Initial done\n");


   while (1) 
   { 
      mainLoop(loopCnt++);
      wait_period(&info);
//      mtimeMeasureTime(ID_0,MTIME_MEASURE,(loopCnt % 1000) == 0);
   }
   return 0;
}

// --- threads and interrupts-------------------------------------------

struct timespec tpMainLoopStart;   
int interruptCntInput1 = 0, interruptCntInput2 = 0;
int interruptCntInput3 = 0, interruptCntInput4 = 0;

void mainLoop(int loopCnt)
{
   InputInt_t InputInt;
   
   clock_gettime(CLOCK_MONOTONIC, &tpMainLoopStart);

// mtimeMeasureTime(ID_1, MTIME_START, WITHOUT_DEBUG);
   while (ebmGetInputInt(0,&InputInt))
   {
//    printf("--input:%d time:%llu inputs:%x encoder:%u\n",InputInt.input,InputInt.time,InputInt.inputs,InputInt.encoder);
      if (InputInt.input == 1)
      {
         ++interruptCntInput1;
      }
      else if (InputInt.input == 2) // outfeed motor pulse from counter wheel
      {
         input2_irq(InputInt);
         ++interruptCntInput2;
      }
      else if (InputInt.input == 3) // trig read of "counter wheel" position
      {
         ++interruptCntInput3;
      }
      else if (InputInt.input == 4) // 
         ++interruptCntInput4;
   }
// mtimeMeasureTime(ID_1, MTIME_MEASURE, (loopCnt % 1000) == 0);
// mtimeMeasureTime(ID_2, MTIME_START, WITHOUT_DEBUG);
   lxthread_Release(handle_tick100);
   lxthread_WaitFinished(handle_tick100);
// mtimeMeasureTime(ID_2, MTIME_MEASURE, (loopCnt % 1000) == 0);
// mtimeMeasureTime(ID_3,MTIME_START,WITHOUT_DEBUG);
   lxthread_Release(handle_idleloop);
   lxthread_WaitFinished(handle_idleloop);
// mtimeMeasureTime(ID_3,MTIME_MEASURE,(loopCnt % 1000) == 0);
   ebmSetSendFlag();
}

int getExecTimeLeft_us() // execution time left in current loop in microseconds
{
   struct timespec tp;   

   clock_gettime(CLOCK_MONOTONIC, &tp);
   return 10000 - (int)((((long long)tp.tv_sec*1000000000L+(long long)tp.tv_nsec) - ((long long)tpMainLoopStart.tv_sec*1000000000L+(long long)tpMainLoopStart.tv_nsec)) / 1000L);
}

void input2_irq(InputInt_t InputInt)
{
}


void initial(void)                        /* Setup machine at startup */
{
   shared_Mem->integer.Value[INFO_RUNNING] = VALUE_DISABLED;
   shared_Mem->integer.Value[INFO_LASTINT] = INT_NOINT;
   shared_Mem->integer.Value[ERROR_SOUND] = 2; // turn off error beep   
   shared_Mem->uinteger.Value[CMD] = CMD_NOCMD;
   shared_Mem->integer.Value[INFO_ERROR] = ERR_NOERROR;
   shared_Mem->integer.Value[INFO_WARNING] = WARN_NOWARN;
   shared_Mem->integer.Value[INFO_PROGRESS] = 0;
   guiStateSet(GUI_STATE_MAIN);
   set_defaults();
   
   shared_Mem->integer.Value[CFG_ENABLE_ROBOT] = 0;
   machine.state = MSTATE_DISABLED;
   machine.runState = RUN_STATE_UNDEFINED;
   error_Init(&machine.error);
   analog_Init();
}






void initStepper() {
   ebmStepInit(0, 1, 11, 12, true);          // EBM 0, stepper 1, STEP=output12, DIR=output13, positive direction
   usleep(50000);
   ebmStepSetAccDec(0, 1, 10000, 10000);     // Acc/Dec = 100 (example values)
   usleep(50000);
}

void turnOnStepper() {
   STEPPER_DIR_ON;
}

void turnOffStepper() {
   STEPPER_DIR_OFF;
}

void moveStepperSteps (int revolutions, int speed) {
   initStepper();
   ebmStepMoveRel(0, 1, revolutions * 400, speed * 55);          // Move xxx steps at speed yyy
   usleep(100000);    
   while (ebmStepStatus(0, 1) == 4)
   {
      printf("Status: %d, Pos: %d, Speed: %d\n", ebmStepStatus(0, 1), ebmStepCurPos(0, 1), ebmStepCurSpeed(0, 1));/* code */
      usleep(100000);                          // Wait
   }
   printf("stepper off (out 12)\n");
   ebmStepDeInit(0, 1);                      // Clean up after test
}

void moveStepperSpeed(int dir, int speed){
   initStepper();
   ebmStepMoveVel(0, 1, dir, speed * 55);
}

// Feed drive routines -------------------------------------

void drvFeed_Init()
{
   drvFeed.type = DRVTYPE_DIGITAX;
   drvFeed.download_done = false;
   drvFeed.requestedSpeed = 0;
   drvFeed.sentSpeed = -1;
   drvFeed.requestedTorque = 0;
   drvFeed.sentTorque = -1;
}

//void drvFeed_Cyclic()
//{
//   static unsigned int cycleCnt = 0;
//   
//   ++cycleCnt;
//   if ((cycleCnt % 50) == 0)
//      digitax_CheckHealthy(&digitaxFeed);
//        
//   // Test
//   
///*   drvFeed_SetTorque_percent(-shared_Mem->integer.Value[CFG_OUTCRASH_TIME]);
//   if (shared_Mem->integer.Value[CFG_OUTCRASH_TIME] <= 0)
//      drvFeed_SetSpeed_rpm(3000);
//   else
//      drvFeed_SetSpeed_rpm(0);
//*/
//   if (drvFeed.requestedSpeed != drvFeed.sentSpeed)
//   {
//      //debugPrint("inFeedspeed: %d\n",drvFeed.requestedSpeed);
//      digitax_SetSpeed(&digitaxFeed, drvFeed.requestedSpeed);
//      drvFeed.sentSpeed = drvFeed.requestedSpeed;
//   }
//   if (drvFeed.requestedTorque != drvFeed.sentTorque)
//   {
//      digitax_SetTorque(&digitaxFeed, drvFeed.requestedTorque);
//      drvFeed.sentTorque = drvFeed.requestedTorque;
//      //debugPrint("FeedTorque: %d\n",drvFeed.requestedTorque);
//   }
//      
//}

void drvFeed_SetSpeed_rpm(int speed)
{
   drvFeed.requestedSpeed = speed * 10; // 1/10 rpm
}

void drvFeed_SetSpeed_m_min(int speed) // m/min
{
   
   drvFeed.requestedSpeed = -(int)((float)speed * 10.75);  // webspeed = rpm / 2,5 * 0,074 * 3,14  =>  rpm = webspeed * 2,5 / 0,074 / 3,14 =>  rpm = webspeed * 10,75 
}

void drvFeed_SetTorque_percent(int torque) // %
{
   drvFeed.requestedTorque = torque * 100; // 1/100%
}
void feed_SetTorqueMode()
{
   if(config.infeed_motor_mode == IMM_PRINTER_SIMULATION || config.infeed_motor_mode == IMM_LOOSE_LOOP)
      digitax_SetTorqueMode(&digitaxFeed ,0);
   else
      digitax_SetTorqueMode(&digitaxFeed, 2);  // Torque mode
}

digitaxParameter_t digitaxParamsFeed[] =
{
   {3, 40, 17},     // P1 error Detection Level (00010001 = Disable trip encode 7, enable wire break detection)
   {1, 14, 3},      // Reference Selector (3 = Preset)
   {1, 15, 1},      // Preset Selector
   {2, 11, 100},    // Acceleration Rate 1 (0,1s)
   {2, 21, 100},    // Deceleration Rate 1 (0,1s)
   {3, 10, 50},     // Speed Controller Proportional Gain Kp1 (0,0050 s/rad)
   {3, 25, 3460},   // Position Feedback Phase Angle (xx degrees) // Todo check this value
   {3, 33, 16},     // P1 Rotary Turns Bits (default 16)
   {3, 34, 4096},   // P1 Rotary Lines Per Revolution (default 4096)
   {3, 35, 0},      // P1 Comms Bits (default 0)
   {3, 38, 3},      // P1 Device Type (default 3 = AB Servo)
   {3, 56, 0},      // P1 Feedback Reverse (0 = Off)
   {3, 57, 16},     // P1 Normalisation Turns (default 16)
   {3, 60, 5},      // P1 Calculation Time (default 5 µs)
   {3, 100, 1},     // F1 Freeze Trigger Source (1 = digital input 5)
   {3, 101, 2},     // F1 Freeze Mode (2 = rising all)
   {3, 118, 0},     // P1 Thermistor Type (default 0 = DIN44082)
   {3, 120, 3300},  // P1 Thermistor Trip Threshold (default 3300 Ω)
   {3, 121, 1800},  // P1 Thermistor Reset Threshold (default 1800 Ω)
   {4, 11, 0},      // Torque Mode Selector (default 0 = speed mode)
   {4, 13, 320},    // Current Controller Kp Gain   (autotune setting)
   {4, 14, 2297},   // Current Controller Ki Gain   (autotune setting)
   {4, 15, 850},    // Motor Thermal Time Constant 1 (85,0 s)
   {5, 7, 3200},    // Rated Current (3,200 A)
   {5, 9, 220},     // Rated Voltage (220 V)
   {5, 11, 5},      // Number Of Motor Poles (5 PolePairs)
   {5, 17, 2376875},// Stator Resistance (2,376875 Ω)   (autotune setting)
   {5, 18, 5},      // Maximum Switching Frequency (5 = 12 kHz)
   {5, 24, 14175},  // Ld (14,175 mH)          (autotune setting)
   {5, 32, 93},     // Torque Per Amp (0,93 Nm/A)
   {5, 33, 57},     // Volts Per 1000rpm (57 V)
   {5, 72, 16128},  // No-load Lq (16,128 mH)  (autotune setting)
   {6, 8, 0},       // Hold Zero Speed (0 = Disabled)
   {6, 65, 175},    // Standard Under Voltage Threshold
   {6, 66, 150},    // Low Under Voltage Threshold
   {8, 25, 0},      // Digital Input 05 Destination (0 = Unassigned). Used as F1 Freeze Trigger Source.
   {10, 30, 0},     // Braking Resistor Rated Power (0 kW)
   {10, 31, 0},     // Braking Resistor Thermal Time Constant (0 s)
   {10, 61, 0},     // Braking Resistor Resistance (0 Ω)
   {11, 44, 0},     // User Security Status (0 = Menu 0)
   {11, 79, 1181050212}, // Drive Name Characters 1-4 (1181050212 = Feed)
   {12, 41, 0},     // Brake Control: Enable (0 = Off)
   {0, 0, 0}        // End)      
};

bool init_drives(void)
{
   //bool mainRebootReq = false;
   //bool loopRebootReq = false;
   bool feedRebootReq = false;


   shared_Mem->integer.Value[INFO_PROGRESS] = 0;
   
   if (!ebm[0].ready)
   {
      printf("ebm not ready =S\n");
      return false;
   }

   else {printf("ebm ready (remove later)\n");}



   if (!digitaxFeed.connected)
   {
      if (!digitax_Connect(&digitaxFeed))
      {
         wait_np(2); // wait for tick100 to find the error
         return false;
      }
   }
   else
		modbusTCPconnected = true;
   

   shared_Mem->integer.Value[INFO_PROGRESS] = 80;
   if (!drvFeed.download_done)
   {
      printf("drvFeed not downloaded\n");
      digitax_DisableDrive(&digitaxFeed);
         
      if (!digitax_InitDrive(&digitaxFeed, DRIVEMODE_RFCS, digitaxParamsFeed, &feedRebootReq))
         return false;
      digitax_SetSpeed(&digitaxFeed, 0);
      digitax_SetTorque(&digitaxFeed, 0);
      
      //digitax_SetTorqueMode(&digitaxFeed, 2);  // Torque mode
      
      drvFeed.download_done = 1;
      //debugPrint("Init feed drive succeeded\n");      
   }
   shared_Mem->integer.Value[INFO_PROGRESS] = 90;   
   //idleloop_cyclic();
   digitax_EnableDrive(&digitaxFeed);
   //if (mainRebootReq || loopRebootReq || feedRebootReq)
   //{
   //   debugPrint("rebootreq\n");
   //   shared_Mem->uinteger.Value[STATE] = STATE_REBOOT;
   //   while (true)
   //   {
   //      wait_np(1);
   //      idleloop_cyclic();
   //   }
   //}
   //feed_SetTorqueMode();

   download_done = 1; 
   shared_Mem->integer.Value[INFO_PROGRESS] = 100;   
   wait_np(30);
   shared_Mem->integer.Value[INFO_PROGRESS] = 0;
   return true;
}







// standard tick (100 times per sec.)
void *tick100(void *t)
{
   static int ticker = 0;
   static int oldMachineRunState = -1;
   int timer = 0;

   while (!ok_to_start_tick)
      wait_np(1);
   
   while (1)
   {
      wait_np(1);
//debugPrint("tick100 time\n");

      


      analog_Tick100();

      //debugPrint("Height:%d\n",heightSensor_Value_mm());      
//      table.receiveNow = shared_Mem->integer.Value[CFG_RECEIVE_NOW];
      //shared_Mem->integer.Value[INFO_CURRENT_HEIGHT] = heightSensor_Value_mm();
      //shared_Mem->integer.Value[INFO_STACK_HEIGHT] = table.stackHeight;   
      //shared_Mem->integer.Value[INFO_LOWEST_PALLET_HEIGHT] = pallet[0].lowestHeight;   // lowest stack height on pallet
      //if (DIS_FULL_SENSOR_STARTED)
      //   shared_Mem->integer.Value[INFO_CONVEYOR_FULL] = CONV_READY_SIGNAL_STOPPED;
      //else
      //   shared_Mem->integer.Value[INFO_CONVEYOR_FULL] = CONV_FULL_SENSOR;

      if (machine.runState != oldMachineRunState)
         debugPrint("machine runstate:%d\n", machine.runState);
      oldMachineRunState = machine.runState;
      
      if (timer > 0)
         --timer;
        
      // error check
      //if (error_Check(&machine.error, machine.state))
      //   error_Handle(&machine.error);      
      //error_sound((guiStateGet() == GUI_STATE_ERROR) && (shared_Mem->integer.Value[CFG_MUTE] == 0)); // handle beeper

      
      ticker++;      
   }
}

// Text for debugPrint commands
char *cmdString[] = {
   "",
   "Start",
   "Stop",
   "3",
   "4",
   "Goto Setup",
   "Debug Off",
   "Back To Main Menu",
   "Goto pick",
   "Goto pallet",
   "Cancel",
   "Save",
   "Exit Service Mode",
   "Goto I/O-test",
   "Factory default",
   "Reset robot",
   "Calibrate Loop",
   "Clear Error",
   "Continue",
   "Abort",
   "Goto home",
   "Change pallet dialog",
   "Goto transport",
   "Reset Counters",
   "Goto Factory Reset",
   "Clear Debug Variables",
   "Goto Update Drive",
   "Update Drive",
   "Goto Motor Test",
   "Standby",
   "Soft Stop",
   "Soft Stop Off",
   "",
   "Temp. Ready",
   "PrnSim Start Feed",
   "PrnSim Stop Feed",
   "Rewind",
   "Stop Rewind",
   "Full Rewind",
   "Unwind",   
};

// idleloop (100 times per sec.)
void *idleloop(void *t)
{
   static unsigned int cycleCnt = 0;
   int err;

   wait_np(1); // first wait_np for syncronizing
   printf("Idleloop()......\n");

   
   // Set SaveId on sharedmem variables
   int handle;
   char tagname[20];
   for (int i = 0; i < CFGFILE_SIZE; ++i)
   {
      sprintf(tagname,"user_int_%03d",i);
      if (shmGetHandleByTagname(tagname, &handle,&err))
         shmSetSaveId(handle,1);    // set saveId=1 on config variables
   }
   for (int i = 0; i < CFGFILE_COMMONSIZE; ++i)
   {
      sprintf(tagname,"user_int_%03d",500 + i);
      if (shmGetHandleByTagname(tagname, &handle,&err))
         shmSetSaveId(handle,2);    // set saveId=2 on common config variables
   }
   for (int i = 0; i < CMOSFILE_SIZE; ++i)
   {
      sprintf(tagname,"cmos_int_%02d",i);
      if (shmGetHandleByTagname(tagname, &handle,&err))
         shmSetSaveId(handle,3);    // set saveId=3 on cmos variables
   }
   // load cmos
   cmos_Load();
   
   // If there's a problem with config 0 it must be created
   int returnValue;
   if ((returnValue = rtRequest(RTREQUEST_LOADCFG)) != 0)    
   {
      debugPrint("Problem with config 0: <%d>\n",shared_Mem->integer.Value[RT_REQUEST_RESPONSE]);
      set_defaults();
      
      for (int i = 0; i < CFGFILE_SIZE; i++)
         shared_Mem->integer.Value[i] = shared_Mem->integer.Default[i];
      for (int i = 500; i < 500+CFGFILE_COMMONSIZE; i++)
         shared_Mem->integer.Value[i] = shared_Mem->integer.Default[i];
      rtRequest(RTREQUEST_SAVECFG);
   }
   check_software_upgrade();

   // set ip on external network
   //shared_Mem->integer.Value[RT_REQUEST_IP] = shared_Mem->integer.Value[CFG_ETH1_IP];
   //shared_Mem->integer.Value[RT_REQUEST_NETMASK] = shared_Mem->integer.Value[CFG_ETH1_NETMASK];
   //shared_Mem->integer.Value[RT_REQUEST_GATEWAY] = shared_Mem->integer.Value[CFG_ETH1_GATEWAY];
   //rtRequestPar(RTREQUEST_SET_NETWORK,1);    
   //config.eth1_ip = shared_Mem->integer.Value[CFG_ETH1_IP];
   //config.eth1_netmask = shared_Mem->integer.Value[CFG_ETH1_NETMASK];
   //config.eth1_gateway = shared_Mem->integer.Value[CFG_ETH1_GATEWAY];
        
   char str = 0;
   int swVersion;
   char swVersionStr[30];
   
   swVersion = rtRequest(RTREQUEST_READ_MACHINE_VERSION);
   sprintf(swVersionStr,"%d-%02d-%02d", swVersion/10000, swVersion/100%100, swVersion%100);  
   shmSetTextByHandle(SHMEMTEXT_SW_ARTNO_MACHINE, shmReadAsStringByTagname("sys_request_string",&str), &err);            
   shmSetTextByHandle(SHMEMTEXT_SW_VER_MACHINE, swVersionStr, &err);            
   sw_machine_version = swVersion;
   
   swVersion = rtRequest(RTREQUEST_READ_SYSTEM_VERSION);
   sprintf(swVersionStr,"%d-%02d-%02d", swVersion/10000, swVersion/100%100, swVersion%100);  
   shmSetTextByHandle(SHMEMTEXT_SW_ARTNO_SYSTEM, shmReadAsStringByTagname("sys_request_string",&str), &err);            
   shmSetTextByHandle(SHMEMTEXT_SW_VER_SYSTEM, swVersionStr, &err);            
   sw_system_version = swVersion;

   swVersion = rtRequest(RTREQUEST_READ_BOOT_VERSION);
   sprintf(swVersionStr,"%d-%02d-%02d", swVersion/10000, swVersion/100%100, swVersion%100);  
   shmSetTextByHandle(SHMEMTEXT_SW_ARTNO_BOOT, shmReadAsStringByTagname("sys_request_string",&str), &err);            
   shmSetTextByHandle(SHMEMTEXT_SW_VER_BOOT, swVersionStr, &err);            
   sw_boot_version = swVersion;
         
   update_configs();
   wait_np(50);
   shared_Mem->uinteger.Value[CMD] = CMD_NOCMD;
   ok_to_start_tick = true;



   digitax_Init(&digitaxFeed,"192.168.1.16",1);  // DUBBELKOLLA IP OCH ID
   drvFeed_Init();

   




   static bool oldMotor = false;
   static bool oldServo = false;

   while (1)
   { 
      int cmdIndex;
      int command[2] = {CMD_NOCMD, CMD_NOCMD};
      error_t errorTest;
      static bool oldCfgEnableRobot = 0;
      static int oldMachineState = -1;
   
      wait_np(1);
      ++cycleCnt;
      ebmControl(0);
      heartbeat_cyclic();
//      robotCon_Cyclic();
      if (cycleCnt % 3000 == 0)
         cmos_Save();

      // handle disable
      if (shared_Mem->integer.Value[CFG_ENABLE_ROBOT] && !oldCfgEnableRobot)
      {
         machine.state = MSTATE_STOPPED;
         //conveyor_Enable();
      }
      else if (!shared_Mem->integer.Value[CFG_ENABLE_ROBOT] && oldCfgEnableRobot)
      {
         machine.state = MSTATE_DISABLED;
         //conveyor_Disable();
      }
      oldCfgEnableRobot = shared_Mem->integer.Value[CFG_ENABLE_ROBOT];

      // debug machine states
      if (machine.state != oldMachineState)
         debugPrint("machine state:%d\n", machine.state);
      oldMachineState = machine.state;
      
      lamp_Control();
      
      switch (machine.state)
      {
         case MSTATE_DISABLED:
            shared_Mem->integer.Value[INFO_RUNNING] = VALUE_DISABLED;
            break;        
         case MSTATE_STOPPED:
            shared_Mem->integer.Value[INFO_RUNNING] = VALUE_STOPPED;  
            break;
         case MSTATE_STARTED:
            shared_Mem->integer.Value[INFO_RUNNING] = VALUE_STARTED;
            break;
         case MSTATE_ERROR:
            break;
         case MSTATE_IOTEST:
            iotest();
            break;
      }
      
      // handle command buttons
      command[0] = shared_Mem->uinteger.Value[CMD] & 0xffff;
      command[1] = (shared_Mem->uinteger.Value[CMD] >> 16) & 0xffff;
      shared_Mem->uinteger.Value[CMD] = CMD_NOCMD;
      
      for (cmdIndex = 0; cmdIndex <= 1; cmdIndex++)
      {
         if ((command[cmdIndex] > CMD_NOCMD)
            && (command[cmdIndex] < (sizeof(cmdString) / sizeof(cmdString[0]))))
            //debugPrint("%s pressed\n", cmdString[command[cmdIndex]]);

         switch (shared_Mem->uinteger.Value[STATE])         
         {
            case GUI_STATE_MAIN:
               switch (command[cmdIndex])
               {
                  case CMD_START:
                     //if (error_Check(&errorTest, MSTATE_STARTED))
                     //{
                     //   error_Handle(&errorTest);
                     //   break;
                     //}
                     machine.state = MSTATE_STARTED;                        
                     break;
                  case CMD_STOP:
                     //if (!shared_Mem->integer.Value[CFG_ENABLE_ROBOT])
                     //   break;
                     machine.state = MSTATE_STOPPED;
                     printf("stepper off (out 12)\n");
                     ebmStepDeInit(0, 1);         // Clean up stepper
                     //table_Disable();
                     //conveyor_Disable();
                     break;
                  case CMD_CLEAR_PALLET_DIALOG:
                     guiStateSet(GUI_STATE_CLEAR_PALLET);
                     break;
                  case CMD_GOTO_HOME:
                     if (machine.state == MSTATE_STOPPED)
                     {
                        //robot_GotoHome();
                        //FINGER_RELEASE_ON;
                     }
                     break;
                  case CMD_GOTO_PICKING:
                     if (machine.state == MSTATE_STOPPED)
                     {
                        //robot_GotoPicking();
                        //FINGER_RELEASE_ON;
                     }
                     break;
                  case CMD_GOTO_PALLET:
                     if (machine.state == MSTATE_STOPPED)
                     {
                        //int xPos = 100;
                        //int yPos = 110;
                        //int zPos = 120;
                        //int xStack, yStack;
                        //int stackHeight = 100;
                        //pallet_GetLowestStack(0, &xStack, &yStack, &xPos, &yPos, &zPos);
                        //pallet_AddStack(0, xStack, yStack, 0, stackHeight);
                        //debugPrint("xStack:%d, yStack:%d, xPos:%d, yPos:%d, zPos:%d, height:%d\n",xStack, yStack,xPos, yPos, zPos, stackHeight);
                        //FINGER_RELEASE_OFF;
                        //robot_GotoPallet(0, xPos, yPos, zPos, 0);
                     }
                     break;
                  case CMD_RESET_ROBOT:
                     //robot_ResetCollision();
                     break;
                  case CMD_GOSETUP:
                     guiStateSet(GUI_STATE_SETUP);
                     break;
                  case CMD_MOTORS:
                     //pthread_create(&threadTurnOnOutputs, NULL, turnOnOutputs, NULL);
                     if (!oldMotor)
                     {
                        printf("Output 1 and 2 on \n");
                        M2_STEP_ON;
                        M2_DIR_ON;
                        oldMotor = true;
                     }
                     else if (oldMotor)
                     {
                        printf("Output 1 and 2 off \n");
                        M2_STEP_OFF;
                        M2_DIR_OFF;
                        oldMotor = false;
                     }
                     break;
                  case CMD_SERVO_RPM:
                     if (machine.state == MSTATE_STARTED){
                        if (!oldServo) {
                           printf("SERVO ON\n");
                           SERVO_ON;
                           drvFeed_SetSpeed_rpm(shared_Mem->integer.Value[IO_CMOS_VALUE]);
                           digitax_Run(&digitaxFeed);
                        }
                        else {
                           SERVO_OFF;
                           printf("SERVO OFF\n");
                        }
                        
                     }
                     else {
                        SERVO_OFF;
                        printf("Machine not started \n");
                     }
                     break;
                     //drvFeed_SetTorque_percent(shared_Mem->integer.Value[IO_TORQUE]);
                     
                     break;
                  case CMD_STEPPER_STEP:
                     if (machine.state == MSTATE_STARTED){
                        printf("stepper on (out 12)\n");
                        moveStepperSteps(shared_Mem->integer.Value[IO_REVOLUTIONS], shared_Mem->integer.Value[IO_SPEED]);  
                     }
                     else {
                        printf("Machine not started \n");
                     }
                     break;
                  case CMD_STEPPER_SPEED:
                     if (machine.state == MSTATE_STARTED){
                        printf("stepper on (out 12)\n");
                        //moveStepperSpeed(true, shared_Mem->integer.Value[IO_SPEED]);
                        init_drives();
                     }
                     else {
                        printf("Machine not started \n");
                     }
                     break;
               }
               break;
            case GUI_STATE_ERROR:
               switch (command[cmdIndex])
               {
                  case CMD_CLRERR_OK: case CMD_STOP: case CMD_START:
                     EMS_RESET_ON;
                     wait_np(40);
                     EMS_RESET_OFF;
                     ebm[0].connectEnable = true;
                     guiStateSet(GUI_STATE_MAIN);

                     machine.state = MSTATE_STOPPED;
                     if (command[cmdIndex] == CMD_START)
                     {
                        if (error_Check(&errorTest, MSTATE_STARTED))
                        {
                           error_Handle(&errorTest);
                           break;
                        }
                        machine.state = MSTATE_STARTED;                        
                     }
                     break;
               }
               break;
            case GUI_STATE_SETUP:
               switch (command[cmdIndex])
               {
                  case CMD_SETCFG:
                     guiStateSet(GUI_STATE_MAIN);
                     shared_Mem->integer.Value[INFO_RUNNING] = VALUE_SAVING;      
                     wait_np(50);                                         

                     rtRequest(RTREQUEST_SAVECFG);
/*                     if (config.country != shared_Mem->integer.Value[CFG_COUNTRY])//////////////////////////////////////////////////////////////////////////////////
                     {
                        debugPrint("changing language\n");
                        rtRequestPar(RTREQUEST_SETLANGUAGE, shared_Mem->integer.Value[CFG_COUNTRY]);
                        state_prev();
                        wait_np(20);
                     }*/
                     update_configs();
                     break;
                  case CMD_GOTO_TRANSPORT:
                     if ((machine.state == MSTATE_STOPPED) || (machine.state == MSTATE_IOTEST))
                     {
                        //robot_GotoTransport();
                        //FINGER_RELEASE_OFF;
                     }
                     break;                     
                  case CMD_CANCEL:
                     guiStateSet(GUI_STATE_MAIN);
                     rtRequest(RTREQUEST_LOADCFG);
                     break;                     
                  case CMD_GOTOIO:
                     machine.state = MSTATE_IOTEST;
                     guiStateSet(GUI_STATE_IO);
                     iotest_init();
                     break;
                  default:
                     break;
               }
               break;
            case GUI_STATE_IO:
               switch (command[cmdIndex])
               {
                  case CMD_GOSETUP:
                     machine.state = MSTATE_STOPPED;
                     guiStateSet(GUI_STATE_SETUP);
                     iotest_cleanup();                     
                     break;
                  case CMD_GOTO_TRANSPORT:
                     if ((machine.state == MSTATE_STOPPED) || (machine.state == MSTATE_IOTEST))
                     {
                        //robot_GotoTransport();
                        //FINGER_RELEASE_OFF;
                     }
                     break;                     
               }
               break;
            case GUI_STATE_CLEAR_PALLET:
               switch (command[cmdIndex])
               {
                  case CMD_CONTINUE:
                     //pallet_Clear(0);
                     guiStateSetPrev();
                     break;
                  case CMD_ABORT:
                     guiStateSetPrev();
                     break;
               }
               break;
            default:
               debugPrint("state unknown!!!!\n");
         }
      }
   }
}

void heartbeat_cyclic()
{
   static long long heartbeat_time = 0;
   
   if (gethrtime() > heartbeat_time)
   {
      if (HEARTBEAT_STARTED)
         HEARTBEAT_OFF;
      else
         HEARTBEAT_ON;
      shared_Mem->integer.Value[707] = HEARTBEAT_STARTED;
      heartbeat_time = gethrtime() + 1000000000ll; // toggle every 1s.
   }
}

void lamp_Control(void)
{
   static bool flashEnable = false;
   static int flashLength = 100; // 1s
   static int flashCnt = 0;
   
   if (flashEnable)
   {
      if (--flashCnt <= 0)
         flashCnt = flashLength;
   }
   else
      flashCnt = 0;

   switch (machine.state)
   {
      case MSTATE_DISABLED:
         flashEnable = false;
         RED_LAMP_OFF;       
         YELLOW_LAMP_OFF;       
         GREEN_LAMP_OFF;       
         break;        
      case MSTATE_STOPPED:
         if (SCANNER_STOP)
         {
            flashEnable = true;
            GREEN_LAMP_OFF;
            if (flashCnt > (flashLength / 2))
            {
               RED_LAMP_ON;
               YELLOW_LAMP_OFF;
            }
            else
            {
               RED_LAMP_OFF;
               YELLOW_LAMP_ON;
            }
         }
         else
         {
            flashEnable = false;
            RED_LAMP_ON;       
            YELLOW_LAMP_OFF;       
            GREEN_LAMP_OFF;
         }
         break;
      case MSTATE_STARTED:
         if (SCANNER_STOP)
         {
            flashEnable = true;
            RED_LAMP_OFF;
            if (flashCnt > (flashLength / 2))
            {
               GREEN_LAMP_ON;
               YELLOW_LAMP_OFF;
            }
            else
            {
               GREEN_LAMP_OFF;
               YELLOW_LAMP_ON;
            }
         }
         else
         {
            flashEnable = false;
            RED_LAMP_OFF;       
            YELLOW_LAMP_OFF;
            GREEN_LAMP_ON;
         }
         break;
      case MSTATE_ERROR:
         flashEnable = true;
         if (flashCnt > (flashLength / 2))
            RED_LAMP_ON;
         else
            RED_LAMP_OFF;
         YELLOW_LAMP_OFF;       
         GREEN_LAMP_OFF;       
         break;
      case MSTATE_IOTEST:
         flashEnable = false;
         break;
   }
}

//----Error handle -----------------------------------------------------

void error_Init(error_t *error)
{
   error->no = ERR_NOERROR;
   error->subNo = ERR_NOERROR;
}

bool error_Check(error_t *error, int machineState)
{
   if (machineState == MSTATE_ERROR)
      return false;

   if (ebm[0].connectionFailed ||
      ((machineState != MSTATE_DISABLED) && (machineState != MSTATE_STOPPED) && !ebm[0].ready))
   {
      ebm[0].connectionFailed = false;
      error->no = ERR_EBMCOMM;
      return true;
   }
   if (ebm[0].updateFailed)
   {
//    additional_error_info("Fault: %d",ebm[0].updateErrorCode);
//      shared_Mem->integer.Value[INFO_EBM_FWUPDATE_CODE] = ebm[0].updateErrorCode;
      error->subNo = ebm[0].updateErrorCode;
      ebm[0].updateFailed = false;
      error->no = ERR_EBMUPDATE;
      return true;
   }
   if (ebm[0].state == EBMSTATE_UPDATE_FAILED)
   {
      error->no = ERR_EBMFWVERSION;
      return true;
   }
   if (ebm[0].ready && ebmGetBit(0,EBMMEM_BIT_INPUT_FAULT))
   {
      error->no = ERR_EBMOUTFAULT;
      return true;
   }
   if (EMERGENCY_STOP)
   {
      error->no = ERR_EMERGENCYSTOP;
      return true;
   }
#ifdef ONLINE   
   if ((machineState == MSTATE_STARTED) && (shared_Mem->integer.Value[RB_MODE] != RB_MODE_RUN))
   {
      return true;
   }
#endif

   return false;
}

void error_Handle(error_t *error)
{
   debugPrint("error:%d, suberror:%d\n",error->no, error->subNo);
   shared_Mem->integer.Value[INFO_ERROR] = error->no;
   shared_Mem->integer.Value[INFO_SUBERROR] = error->subNo;
   
   machine.state = MSTATE_ERROR;
   guiStateSet(GUI_STATE_ERROR);

}

// ---------------------------------------------------------------------


// ---- common functions -----------------------------------------------
void debugPrint(char *fmt, ...)
{
   struct timespec tp;
   clock_gettime(CLOCK_MONOTONIC, &tp);
   va_list args;

   va_start(args,fmt);
   printf("%d.%09d ",tp.tv_sec,tp.tv_nsec);
   vprintf(fmt, args);
   va_end(args);
}

void logPrint(char *fmt, ...)
{
   static bool file_is_open = false;
   static FILE *f;
   static int cnt = 0;
   struct tm *lt;
   char timeStr[30];
   char fileName[30];
   struct timeval time;

   if (!file_is_open)
   {
      gettimeofday(&time,0);
      lt = localtime(&time.tv_sec);
      strftime(timeStr, sizeof timeStr, "%Y-%m-%d_%H-%M-%S", lt);
      sprintf(fileName,"/mainusb/log_%s_%d",timeStr,++cnt);
      
      if ((f = fopen(fileName,"wb")) == NULL)
      {
         printf("open file %s failed\n",fileName);
         return;
      }
      file_is_open = true;
   }
   va_list args;
   va_start(args,fmt);
   vfprintf(f, fmt, args);
   va_end(args);
   fflush(f);
}

void error_sound(bool play)
{
   static bool playing = false;
   if (play)
   {
      if (!playing)
      {
         shared_Mem->integer.Value[ERROR_SOUND] = 1;
         playing = true;
      }
   }
   else
   {
      if(playing)
      {
         shared_Mem->integer.Value[ERROR_SOUND] = 2;
         playing = false;
      }
   }
}

int rtRequestPar(int request, int par)
{
   while (shared_Mem->integer.Value[RT_REQUEST] != 0) // wait for ongoing command
      wait_np(1);

   shared_Mem->integer.Value[RT_REQUEST_RESPONSE] = 0;
   shared_Mem->integer.Value[RT_REQUEST_PARAM] = par;
   shared_Mem->integer.Value[RT_REQUEST] = request;
   
   // wait for request completion
   while (shared_Mem->integer.Value[RT_REQUEST])
      wait_np(1);
   return shared_Mem->integer.Value[RT_REQUEST_RESPONSE]; 
}

int rtRequest(int request)
{
   return rtRequestPar(request, 0);
}
//----------------------------------------------------------------------


// ---- gui state functions --------------------------------------------

void guiStateSet(int state)
{
   guiStatePrev = guiState;
   guiState = state;
   shared_Mem->uinteger.Value[STATE] = guiState;
   debugPrint("Gui state:%d\n", guiState);
}

void guiStateSetPrev(void)
{
   guiState = guiStatePrev;
   shared_Mem->uinteger.Value[STATE] = guiState;
   debugPrint("Gui state prev:%d\n", guiState);
}

int guiStateGet(void)
{
   return shared_Mem->uinteger.Value[STATE];
}

// ---------------------------------------------------------------------


// ---- cmos file parameters -------------------------------------------

void cmos_Save(void)
{
//   static int t_time, t_perc;
   // Save vars to NVRAM
   shared_Mem->integer.Value[CMOS_RUNSTAT] = runstat;
   shared_Mem->integer.Value[CMOS_ERRSTAT] = errstat;
   shared_Mem->integer.Value[CMOS_STOPSTAT] = stopstat;
   shared_Mem->integer.Value[CMOS_STANDSTAT] = standstat;
   //shared_Mem->integer.Value[CMOS_USERFEETS] = shared_Mem->integer.Value[INFO_USERFEETS];
   //shared_Mem->integer.Value[CMOS_USERTIME] = shared_Mem->integer.Value[INFO_USERHOURS] * 3600
   //   + shared_Mem->integer.Value[INFO_USERMINUTES] * 60;
   shared_Mem->integer.Value[CMOS_TOTALFEETS] = shared_Mem->integer.Value[INFO_TOTALFEETS];
   shared_Mem->integer.Value[CMOS_TOTALTIME] = shared_Mem->integer.Value[INFO_TOTALHOURS] * 3600
      + shared_Mem->integer.Value[INFO_TOTALMINUTES] * 60;
   //shared_Mem->integer.Value[CMOS_ROLLCHANGECOUNT] = roll_change_cnt;
   //shared_Mem->integer.Value[CMOS_ENTER_CODE_TIME] = speedLimit.enterCodeTime;
   shared_Mem->integer.Value[CMOS_MACHINE_TYPE] = machineType;
   
   

   shared_Mem->integer.Value[CMOS_NUMBER] = shared_Mem->integer.Value[IO_CMOS_VALUE];
   shared_Mem->integer.Value[CMOS_REVOLUTIONS] = shared_Mem->integer.Value[IO_REVOLUTIONS];
   shared_Mem->integer.Value[CMOS_SPEED] = shared_Mem->integer.Value[IO_SPEED];




   
   //t_time = runstat + errstat + stopstat + standstat;
   //if (t_time)
   //{
   //   shared_Mem->integer.Value[INFO_RUNSTAT_PERCENT] = (runstat * 100) / t_time;
   //   shared_Mem->integer.Value[INFO_ERRSTAT_PERCENT] = (errstat * 100) / t_time;
   //   shared_Mem->integer.Value[INFO_STOPSTAT_PERCENT] = (stopstat * 100) / t_time;
   //   shared_Mem->integer.Value[INFO_STANDSTAT_PERCENT] = (standstat * 100) / t_time;
//
   //   // some rounding errors might result in a total < 100%
   //   t_perc = shared_Mem->integer.Value[INFO_RUNSTAT_PERCENT]
   //      + shared_Mem->integer.Value[INFO_ERRSTAT_PERCENT]
   //      + shared_Mem->integer.Value[INFO_STOPSTAT_PERCENT]
   //      + shared_Mem->integer.Value[INFO_STANDSTAT_PERCENT];
   //   if (t_perc < 100)
   //      shared_Mem->integer.Value[INFO_STOPSTAT_PERCENT] += 100 - t_perc;
   //}
   //else
   //{
   //   shared_Mem->integer.Value[INFO_RUNSTAT_PERCENT] = 0;
   //   shared_Mem->integer.Value[INFO_ERRSTAT_PERCENT] = 0;
   //   shared_Mem->integer.Value[INFO_STOPSTAT_PERCENT] = 100;
   //   shared_Mem->integer.Value[INFO_STANDSTAT_PERCENT] = 0;
   //}
   if (rtRequest(RTREQUEST_SAVECMOS) != 0)    
      internalUsbCorrupt = true;
}

void cmos_Load(void)
{
   rtRequest(RTREQUEST_LOADCMOS);

   runstat = shared_Mem->integer.Value[CMOS_RUNSTAT];
   errstat = shared_Mem->integer.Value[CMOS_ERRSTAT];
   stopstat = shared_Mem->integer.Value[CMOS_STOPSTAT];
   standstat = shared_Mem->integer.Value[CMOS_STANDSTAT];
   //shared_Mem->integer.Value[INFO_USERFEETS] = shared_Mem->integer.Value[CMOS_USERFEETS];
   //shared_Mem->integer.Value[INFO_USERHOURS] = shared_Mem->integer.Value[CMOS_USERTIME] / 3600;
   //shared_Mem->integer.Value[INFO_USERMINUTES] = (shared_Mem->integer.Value[CMOS_USERTIME] % 3600) / 60;
   shared_Mem->integer.Value[INFO_TOTALFEETS] = shared_Mem->integer.Value[CMOS_TOTALFEETS];
   shared_Mem->integer.Value[INFO_TOTALHOURS] = shared_Mem->integer.Value[CMOS_TOTALTIME] / 3600;
   shared_Mem->integer.Value[INFO_TOTALMINUTES] = (shared_Mem->integer.Value[CMOS_TOTALTIME] % 3600) / 60;
   //roll_change_cnt = shared_Mem->integer.Value[CMOS_ROLLCHANGECOUNT];
   //speedLimit.enterCodeTime = shared_Mem->integer.Value[CMOS_ENTER_CODE_TIME];
   machineType = shared_Mem->integer.Value[CMOS_MACHINE_TYPE];
   //shared_Mem->integer.Value[INFO_MACHINE_TYPE] = machineType;


   shared_Mem->integer.Value[IO_CMOS_VALUE] = shared_Mem->integer.Value[CMOS_NUMBER];
   shared_Mem->integer.Value[IO_REVOLUTIONS] = shared_Mem->integer.Value[CMOS_REVOLUTIONS];
   shared_Mem->integer.Value[IO_SPEED] = shared_Mem->integer.Value[CMOS_SPEED];
}
// ---------------------------------------------------------------------

// ---- config parameters ----------------------------------------------
void set_defaults(void)
{
   int i;

   debugPrint("setting default params\n");   
   config.country = shared_Mem->integer.Default[CFG_COUNTRY] = shared_Mem->integer.Value[CFG_COUNTRY] = ENGLISH;

   shared_Mem->integer.Default[CFG_ETH0_IP] = shared_Mem->integer.Value[CFG_ETH0_IP] = (192UL << 24) | (168UL << 16) | (0UL << 8) | 50UL;
   shared_Mem->integer.Min[CFG_ETH0_IP] = 0;
   shared_Mem->integer.Max[CFG_ETH0_IP] = -1;                                  // MAXINT
   shared_Mem->integer.Default[CFG_ETH0_NETMASK] = shared_Mem->integer.Value[CFG_ETH0_NETMASK] = (255UL << 24) | (255UL << 16) | (255UL << 8) | 0UL;
   shared_Mem->integer.Min[CFG_ETH0_NETMASK] = 0;
   shared_Mem->integer.Max[CFG_ETH0_NETMASK] = -1;                                  // MAXINT
   shared_Mem->integer.Default[CFG_ETH0_GATEWAY] = shared_Mem->integer.Value[CFG_ETH0_GATEWAY] = (192UL << 24) | (168UL << 16) | (0UL << 8) | 1UL;
   shared_Mem->integer.Min[CFG_ETH0_GATEWAY] = 0;
   shared_Mem->integer.Max[CFG_ETH0_GATEWAY] = -1;                                  // MAXINT

   shared_Mem->integer.Default[CFG_USERPASSWD] = shared_Mem->integer.Value[CFG_USERPASSWD] = 4112;
   shared_Mem->integer.Min[CFG_USERPASSWD] = 0;
   shared_Mem->integer.Max[CFG_USERPASSWD] = 99999999;

   shared_Mem->integer.Value[CFG_SERVICECODE] = 0;
   shared_Mem->integer.Min[CFG_SERVICECODE] = 0;
   shared_Mem->integer.Max[CFG_SERVICECODE] = 99999999;
         
   for(i=0; i < 40; i++)
   {
     shared_Mem->integer.Default[IO_DIGOUT_START+i] = shared_Mem->integer.Value[IO_DIGOUT_START+i] = 0;
   }
   for(i=0; i < 4; i++)
   {
     shared_Mem->integer.Default[IO_ANAOUT_START+i] = shared_Mem->integer.Value[IO_ANAOUT_START+i] = 0;
     shared_Mem->integer.Min[IO_ANAOUT_START+i] = -100;
     shared_Mem->integer.Max[IO_ANAOUT_START+i] = 100;
     shared_Mem->integer.Default[IO_ANAOUT_RUN+i] = shared_Mem->integer.Value[IO_ANAOUT_RUN+i] = 0;
   }

   shared_Mem->integer.Value[CMOS_RUNSTAT] = 0;
   shared_Mem->integer.Value[CMOS_ERRSTAT] = 0;
   shared_Mem->integer.Value[CMOS_STOPSTAT] = 0;
   shared_Mem->integer.Value[CMOS_STANDSTAT] = 0;
   shared_Mem->integer.Value[CMOS_USERFEETS] = 0;
   shared_Mem->integer.Value[CMOS_USERTIME] = 0;
   shared_Mem->integer.Value[CMOS_TOTALFEETS] = 0;
   shared_Mem->integer.Value[CMOS_TOTALTIME] = 0;
   shared_Mem->integer.Value[CMOS_ROLLCHANGECOUNT] = 0;
   
   shared_Mem->integer.Value[CFG_SOFTWARE_VERSION] = shared_Mem->integer.Default[CFG_SOFTWARE_VERSION] = SOFTWARE_VERSION;

   shared_Mem->integer.Value[CFG_MUTE] = 0;
}

void update_configs(void)
{
   config.country = shared_Mem->integer.Value[CFG_COUNTRY];
   
   config.debug_mode = shared_Mem->integer.Value[CFG_DEBUG_MODE];
   user_password = shared_Mem->integer.Value[CFG_USERPASSWD];
   if(user_password != shared_Mem->integer.Value[CFG_SERVICECODE])
      shared_Mem->integer.Value[INFO_SERVICE] = OPERATOR_MODE;   
   
/* if ((shared_Mem->integer.Value[CFG_ETH0_IP] != config.eth0_ip) || (shared_Mem->integer.Value[CFG_ETH0_NETMASK] != config.eth0_netmask) || (shared_Mem->integer.Value[CFG_ETH0_GATEWAY] != config.eth0_gateway))
   {
      shared_Mem->integer.Value[RT_REQUEST_IP] = shared_Mem->integer.Value[CFG_ETH0_IP];
      shared_Mem->integer.Value[RT_REQUEST_NETMASK] = shared_Mem->integer.Value[CFG_ETH0_NETMASK];
      shared_Mem->integer.Value[RT_REQUEST_GATEWAY] = shared_Mem->integer.Value[CFG_ETH0_GATEWAY];
      rtRequestPar(RTREQUEST_SET_NETWORK,0);    
      config.eth0_ip = shared_Mem->integer.Value[CFG_ETH0_IP];
      config.eth0_netmask = shared_Mem->integer.Value[CFG_ETH0_NETMASK];
      config.eth0_gateway = shared_Mem->integer.Value[CFG_ETH0_GATEWAY];
   }*/
   //if ((shared_Mem->integer.Value[CFG_ETH1_IP] != config.eth1_ip) || (shared_Mem->integer.Value[CFG_ETH1_NETMASK] != config.eth1_netmask) || (shared_Mem->integer.Value[CFG_ETH1_GATEWAY] != config.eth1_gateway))
   //{
   //   shared_Mem->integer.Value[RT_REQUEST_IP] = shared_Mem->integer.Value[CFG_ETH1_IP];
   //   shared_Mem->integer.Value[RT_REQUEST_NETMASK] = shared_Mem->integer.Value[CFG_ETH1_NETMASK];
   //   shared_Mem->integer.Value[RT_REQUEST_GATEWAY] = shared_Mem->integer.Value[CFG_ETH1_GATEWAY];
   //   rtRequestPar(RTREQUEST_SET_NETWORK,1);    
   //   config.eth1_ip = shared_Mem->integer.Value[CFG_ETH1_IP];
   //   config.eth1_netmask = shared_Mem->integer.Value[CFG_ETH1_NETMASK];
   //   config.eth1_gateway = shared_Mem->integer.Value[CFG_ETH1_GATEWAY];
   //}
   //pallet_UpdateFromCFG();
}

void check_software_upgrade(void)
{
}
//----------------------------------------------------------------------

// ---- analog functions -----------------------------------------------

void analog_Init(void)
{
   int i, j;

   for(i = 0; i < 4; i++)
   {
      analog.value_idx[i] = 0;
      analog.in_calc[i] = 0;
      for( j = 0; j < 5; j++)
         analog.value[i][j] = 0;
   }

   for(i = 1; i <= 4; i++)
      ebmSetAnalogOut(0, i, 0);
}

void analog_Tick100(void)               
{
   int i, j;
   int maximum = 0, summa = 0, minimum = 0;
   int temp_adresult0=0;


   for( i = 0; i < 4; i++)
   {
      if( analog.in_calc[i] )
      {
         analog.value[i][analog.value_idx[i]] = GetAnalog(0,i);
         summa = 0;
         minimum = 1000;
         maximum = -1000;
         for(j=0; j<5; j++)
         {
            temp_adresult0 = analog.value[i][j];
            if(temp_adresult0 > maximum)
               maximum = temp_adresult0;
            if (temp_adresult0 < minimum)
               minimum = temp_adresult0;
            summa += temp_adresult0;
         }
         summa = summa - minimum - maximum;
         analog.in[i] = (summa/3);
         if( ++analog.value_idx[i] > 4)
            analog.value_idx[i] = 0;

      }
   }
}
//----------------------------------------------------------------------

// ---- iotest functions -----------------------------------------------
void iotest_init(void)
{
   int i;
   for (i = 0; i < 18; i++)
      shared_Mem->integer.Value[IO_DIGOUT_START + i]  = 0;
   shared_Mem->integer.Value[IO_DIGIN_LAST] = 0;

   for (i = 0; i < 4; i++)
   {
      shared_Mem->integer.Value[IO_ANAOUT_START + i]  = 0;
      ebmSetAnalogOut(0, i+1, 0);
      shared_Mem->integer.Value[IO_ANAOUT_RUN + i]  = 0;      
   }   
}

// iotest_cleanup() resets values to working condition after leaving io test
void iotest_cleanup(void)
{
   int i;

   for(i = 1; i <= 4; ++i)
      ebmSetAnalogOut(0, i, 0);
   for (i = 1; i <= 18; i++)
      ebmSetOutput(0, i, 0);      
}
 
void iotest(void)
{
   int i;
   
   // Set analog out
   for(i = 0; i < 4; ++i)
   {
      if (shared_Mem->integer.Value[IO_ANAOUT_RUN + i])
         ebmSetAnalogOut(0, i+1, (float)(shared_Mem->integer.Value[IO_ANAOUT_START + i]) / 10.0);            
      else
         ebmSetAnalogOut(0, i+1, 0);
   }
   // Get analog in
   for(i = 0; i < 3; ++i)
   {
      shared_Mem->integer.Value[IO_ANAIN_START + i*2] = GetAnalog(0,i);
      int tmpAnaIn = (int)(ebmGetAnalogIn(0, i+1) * 1000);
      shared_Mem->integer.Value[IO_ANAIN_START + i*2+1] = ((tmpAnaIn / 1000) << 16) + (tmpAnaIn % 1000) * 10;
   }
   // Set digital out
   for (i = 1; i <= 18; i++)
      ebmSetOutput(0, i, shared_Mem->integer.Value[IO_DIGOUT_START + i - 1]);      
   // Get digital in
   for (i = 1; i <= 28; i++)
   {
      bool input = ebmGetInput(0,i);
      if (input != shared_Mem->integer.Value[IO_DIGIN_START + i - 1])
      {
         shared_Mem->integer.Value[IO_DIGIN_LAST] = i;
         shared_Mem->integer.Value[IO_SOUND] = 1;
      }
      shared_Mem->integer.Value[IO_DIGIN_START + i - 1] = (int)input;
   }
}

//---------------------------------------------------------------
// ebm callback routines

void ebmSetGuiUpdating(void)
{
   guiStateSet(GUI_STATE_EBMUPDATE);
}

void ebmClearGuiUpdating(void)
{
   guiStateSetPrev();
}

void ebmSetGuiUpdateProgress(int progress)
{
   shared_Mem->integer.Value[INFO_PROGRESS] = progress;
}

void ebmUpdateVersionInfo(void)
{
   int err;
   char tmpstr[255];

   shared_Mem->integer.Value[INFO_VERSION1] = ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_VERSION);
   shared_Mem->integer.Value[INFO_VERSION2] = ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE); // 0 = EBM-CPU (196pin), 1 = EBM_CPU144 (144pin)
   shmSetTextByHandle(SHMEMTEXT_SW_ARTNO_EBM_APP, ebmGetString(0,EBMMEM_STRING_APP_NAME),&err);            
   sprintf(tmpstr,"%d-%02d-%02d",ebmGetUInt32(0,EBMMEM_UINT32_APP_VERSION)/10000,ebmGetUInt32(0,EBMMEM_UINT32_APP_VERSION)/100%100,ebmGetUInt32(0,EBMMEM_UINT32_APP_VERSION)%100);
   shmSetTextByHandle(SHMEMTEXT_SW_VER_EBM_APP, tmpstr, &err);            
   shmSetTextByHandle(SHMEMTEXT_SW_ARTNO_EBM_BOOT, ebmGetString(0,EBMMEM_STRING_BOOT_NAME), &err);            
   sprintf(tmpstr,"%d-%02d-%02d",ebmGetUInt32(0,EBMMEM_UINT32_BOOT_VERSION)/10000,ebmGetUInt32(0,EBMMEM_UINT32_BOOT_VERSION)/100%100,ebmGetUInt32(0,EBMMEM_UINT32_BOOT_VERSION)%100);
   shmSetTextByHandle(SHMEMTEXT_SW_VER_EBM_BOOT, tmpstr, &err);                  
   shared_Mem->integer.Value[INFO_EBM_CPU_SERIALNO] = ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_SERIALNO);
}

void ebmInitHardware(int ebmId)
{  
   printf("ebm%d Init hardware begin\n", ebmId);
   if (ebmId == 0)
   {
      ebmEnableWatchdog(0,200,0x0000);  // Enable hardstop;
//      ebmSetUInt8(0,EBMMEM_UINT8_CONFIG_INPUT_02,0x07);  // Enable interrupt on both edges and send interrupt                    
//      ebmSetUInt8(0,EBMMEM_UINT8_CONFIG_INPUT_03,0x05);  // Enable interrupt on rising edge and send interrupt
      EMS_RESET_ON;
      wait_np(40);
      EMS_RESET_OFF;
   }
   printf("ebm%d Init hardware end\n",ebmId);
}
// ---------------------------------------------------------------------

