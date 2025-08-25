// Shared memory interface

// A command is executed when the calling process sets the cmd parameter.
// When the command has been handled the real time kernel process sets
// the parameter back to 0. If an error occurs the info_error
// parameter will contain the specific error code.
// CMD            Command Buttons (shmem.state)
// STATES         (shmem.state)
// INPUTS         IO=inputs (shmem.rt_int[0-49])
// INFO           INFO vars (shmem.rt_int[150-249])
// DEBUG          DB vars (shmem.rt_int[250-299])
// MB             MODBUS vars (shmem.rt_int[400-599]) 
// CONFIG         CFG vars (shmem.user_int[0-499])
// CONFIG         COMMON CFG vars (shmem.user_int[500-550])

// COMMANDS
//*****************************************************
//*   CMD=Command Buttons (shmem.state)               *
//*****************************************************
#define CMD_NOCMD                   0     // Nothing to do
#define CMD_START                   1     // Start machine: ready.cfg
#define CMD_STOP                    2     // Stop machine:  ready.cfg
#define CMD_GOSETUP                 5     // Enter service mode: setup.cfg
#define CMD_DEBUG_OFF               6     // Debug mode mode: ready.cfg
#define CMD_MAIN_MENU               7     // Back to main menu: loadmenu.cfg
#define CMD_GOTO_PICKING            8     // Robot goto picking position
#define CMD_GOTO_PALLET             9     // Robot goto pallet position
#define CMD_CANCEL                  10    // Cancel: setup.cfg,clearmemconfirm.cfg
#define CMD_SETCFG                  11    // Writes all cfg_ parameters (save and exit config setup.cfg
#define CMD_EXITSERVICEMODE         12    // Exit service mode: setup.cfg
#define CMD_GOTOIO                  13    // Goto i/o-menu: setup.cfg
#define CMD_RESETCFG                14    // Reset all variables to defaults: clearmemconfirm.cfg
#define CMD_RESET_ROBOT             15    // Reset robot
#define CMD_CLRERR_OK               17    // Clear all errors and return to prev.state: error.cfg
#define CMD_CONTINUE                18
#define CMD_ABORT                   19
#define CMD_GOTO_HOME               20     // Robot goto home position
#define CMD_CLEAR_PALLET_DIALOG     21
#define CMD_GOTO_TRANSPORT          22

#define CMD_MOTORS                  23     // 
#define CMD_SERVO_RPM               24     // 
#define CMD_STEPPER_STEP            25
#define CMD_STEPPER_SPEED           26

//*****************************************************
//*   STATES   (shmem.state)                          *
//*****************************************************
#define GUI_STATE_MAIN                  0    // 
#define GUI_STATE_ERROR                 1    // error.cfg
#define GUI_STATE_SETUP                 2    // Setup mode setup.cfg
#define GUI_STATE_IO                    3    // in i/o-menu. iotest.cfg
#define GUI_STATE_MOTORTEST             4    // motors test
#define GUI_STATE_EBMUPDATE             5    // ebm firmware update
#define GUI_STATE_COLLISION             6    
#define GUI_STATE_CLEAR_PALLET          7    


#define STATE_REBOOT                   10    // Reboot menu

//shmem.rt_int[0-999]
//*****************************************************
//*   IO=inputs (shmem.rt_int[0-49])                  *
//*****************************************************
#define IO_DIGIN_LAST               RT_INT_START + 0  // Last_digital inputs (shmem.rt_int[0])
#define IO_DIGIN_START              RT_INT_START + 1  // digital inputs start (shmem.rt_int[1])
#define IO_ANAIN_START              RT_INT_START + 43 // analog inputs start (shmem.rt_int[43-50])

//*****************************************************
//*   INFO=INFO vars (shmem.rt_int[150-249])          *
//*****************************************************
#define INFO_RUNNING                RT_INT_START + 150   // Running information
   #define VALUE_DISABLED             0  
   #define VALUE_STOPPED              1
   #define VALUE_STARTED              2
   #define VALUE_SAVING               3

#define INFO_LASTINT                RT_INT_START + 151   // Last Interrupt information
   #define INT_NOINT                      0     // No Int.
#define INFO_ERROR                 RT_INT_START + 152   // Error information
   #define ERR_NOERROR                    0     // Everything's fine
   #define ERR_EMERGENCYSTOP              1     // Emergency stop
   #define ERR_EBMCOMM                    2    // EBM communication failed
   #define ERR_EBMUPDATE                  3    // EBM firmware update failed
   #define ERR_EBMFWVERSION               4    // Wrong version of EBM firmware
   #define ERR_EBMOUTFAULT                5    // EBM digital output overload
   #define ERR_INTERNAL_USB_CORRUPT       6    // Internal USB stick in the display is corrupt
   #define ERR_ROBOT_NOT_IN_RUNMODE       7
   #define ERR_ROBOT_ALARM                8
   #define ERR_FULL_PALLET                9    //

#define INFO_SUBERROR              RT_INT_START + 153   // Error information

#define INFO_WARNING               RT_INT_START + 154   // Warning information
   #define WARN_NOWARN                    0
#define INFO_CURRENT_HEIGHT        RT_INT_START + 155
#define INFO_STACK_HEIGHT          RT_INT_START + 156
#define INFO_LOWEST_PALLET_HEIGHT  RT_INT_START + 157
                                                    
#define INFO_DEBUG                 RT_INT_START + 158   // Debug information
#define INFO_CONVEYOR_FULL         RT_INT_START + 159
#define INFO_SERVICE                RT_INT_START + 164   // Service mode infor.
   #define  OPERATOR_MODE                 0     // Show Operator mode
   #define  SERVICE_MODE                  1     // Show Service mode
   #define  DEVELOP_MODE                  3     // Show Develop mode

#define INFO_VERSION1               RT_INT_START + 165
#define INFO_VERSION2               RT_INT_START + 166
#define INFO_VERSION3               RT_INT_START + 167
#define INFO_VERSION4               RT_INT_START + 168
#define INFO_VERSION5               RT_INT_START + 169
#define INFO_VERSION6               RT_INT_START + 170
#define INFO_VERSION7               RT_INT_START + 171
#define INFO_VERSION8               RT_INT_START + 172
#define INFO_VERSION9               RT_INT_START + 173
#define INFO_VERSION10              RT_INT_START + 174
#define INFO_MEMCLEARING            RT_INT_START + 175   // show clear memory menu
#define INFO_RUNSTAT_PERCENT        RT_INT_START + 186   // Info running time in %
#define INFO_ERRSTAT_PERCENT        RT_INT_START + 187   // Info error time in %
#define INFO_STOPSTAT_PERCENT       RT_INT_START + 188   // Info stopped time in %
#define INFO_STANDSTAT_PERCENT      RT_INT_START + 189   // Info waiting time in %
#define INFO_TOTALHOURS             RT_INT_START + 190   // Info total hours
#define INFO_TOTALMINUTES           RT_INT_START + 191   // Info total min.
#define INFO_TOTALFEETS             RT_INT_START + 192   // Info total feets in x*100feet


#define INFO_REMEMBER               RT_INT_START + 193   // Remember number in "Remember"


#define INFO_PROGRESS               RT_INT_START + 195   // progress of current operation (%)
#define INFO_LIFE_TIME_COUNTER      RT_INT_START + 208
#define INFO_USER_TIME_COUNTER      RT_INT_START + 209
#define INFO_MACHINE_VERSION        RT_INT_START + 210
#define INFO_EBM_CPU_SERIALNO       RT_INT_START + 225

//*****************************************************
//*   On/Off mode:                                    *
//*****************************************************
#define OFF                         0
#define ON                          1

//*****************************************************
//*   DB=DEBUG vars (shmem.rt_int[250-299])           *
//*****************************************************
#define  DB_FIRST_DIAM              RT_INT_START + 250   // Starting diam.
#define  DB_DIAM                    RT_INT_START + 251   // Measured diam.
#define  DB_NOT_USED                RT_INT_START + 252   // 
#define  DB_MAIN_SPEED              RT_INT_START + 253   // Speed of main motor in rpm
#define  DB_OUTFEED_SPEED           RT_INT_START + 254   // Requested speed (m/min)
#define  DB_OUTFEED_TORQUE          RT_INT_START + 255   // Requested torque (%)
#define  DB_OUTFEED_LOOP_POS        RT_INT_START + 256   // Position of outfeed loop (0 in top, 255 in bottom)
#define  DB_MAIN_POS                RT_INT_START + 257   // Pulses from cog wheel
#define  DB_NOT_USED1               RT_INT_START + 258   //
#define  DB_MAIN_ACC                RT_INT_START + 259   // Value of acceleration
#define  DB_MAIN_DEC                RT_INT_START + 260   // Value of deceleration
#define  DB_ADRESULT0               RT_INT_START + 261   // Analog0 value loop1
#define  DB_OUTFEED_LOOP_POS_TOP    RT_INT_START + 262   // Top position of outfeed loop
#define  DB_PAPER_THICKNESS         RT_INT_START + 263   // Paper thickness in µm
#define  DB_NOT_USED2               RT_INT_START + 264   //
#define  DB_NOT_USED3               RT_INT_START + 265   //
#define  DB_NOT_USED4               RT_INT_START + 266   //
#define  DB_NOT_USED5               RT_INT_START + 267   //
#define  DB_ADRESULT0_MAX           RT_INT_START + 268   // When running Maximum Adresult0
#define  DB_LOOP_TORQUE             RT_INT_START + 269   // 
#define  DB_NOT_USED6               RT_INT_START + 270   // 
#define  DB_NOT_USED7               RT_INT_START + 271   // 
#define  DB_NOT_USED8               RT_INT_START + 272   // 
#define  DB_NOT_USED9               RT_INT_START + 273

#define  DB_COUNTER_SPEED           RT_INT_START + 274
#define  DB_COUNTER_PULSES          RT_INT_START + 275   // Pulses from counter wheel
#define  DB_NOT_USED10              RT_INT_START + 276
#define  DB_NOT_USED11              RT_INT_START + 277
#define  DB_MOTOR_PARAM_1           RT_INT_START + 278
#define  DB_MOTOR_PARAM_2           RT_INT_START + 279
#define  DB_MOTOR_PARAM_3           RT_INT_START + 280
#define  DB_MOTOR_PARAM_4           RT_INT_START + 281
#define  DB_MOTOR_PARAM_5           RT_INT_START + 282
#define  DB_MOTOR_PARAM_6           RT_INT_START + 283

//*****************************************************
//*   RB = ROBOT vars (shmem.rt_int[300-399])        *
//*****************************************************

#define RB_MODE                      RT_INT_START + 300
  #define RB_MODE_TEACH 0
  #define RB_MODE_RUN   1
#define RB_ALARM                     RT_INT_START + 301
#define RB_COLLISION                 RT_INT_START + 302
#define RB_HEARTBEAT                 RT_INT_START + 303
#define RB_EXECUTE_STATE             RT_INT_START + 304
#define RB_COMMAND_RESPONSE          RT_INT_START + 310
  #define RB_RESPONSE_NONE      0
  #define RB_RESPONSE_EXECUTING 1
  #define RB_RESPONSE_FINISHED  2
  #define RB_RESPONSE_FAILED    3
#define RB_FINGER_RELEASE            RT_INT_START + 311

#define RB_COMMAND                   RT_INT_START + 350
  #define RB_COMMAND_NONE           0
  #define RB_COMMAND_GOTO_HOME      1
  #define RB_COMMAND_GOTO_PICKING   2
  #define RB_COMMAND_GOTO_PALLET    3
  #define RB_COMMAND_CONTINUE       4
  #define RB_COMMAND_RESET_ALARM    5
  #define RB_COMMAND_GOTO_TRANSPORT 6
#define RB_TOOL                      RT_INT_START + 351
#define RB_XPOS                      RT_INT_START + 352
#define RB_YPOS                      RT_INT_START + 353
#define RB_ZPOS                      RT_INT_START + 354
#define RB_DIRECTION                 RT_INT_START + 355
#define RB_HOLD                      RT_INT_START + 356


//*****************************************************
//*   MB = MODBUS vars (shmem.rt_int[400-599])        *
//*****************************************************

#define MB_REGISTER_VERSION          RT_INT_START + 401
#define MB_COMPATIBLE_VERSION        RT_INT_START + 402
#define MB_RESERVED_1                RT_INT_START + 403
#define MB_RESERVED_2                RT_INT_START + 404
#define MB_MACHINE_NAME_0            RT_INT_START + 405
#define MB_MACHINE_NAME_1            RT_INT_START + 406
#define MB_MACHINE_NAME_2            RT_INT_START + 407
#define MB_MACHINE_NAME_3            RT_INT_START + 408
#define MB_MACHINE_NAME_4            RT_INT_START + 409
#define MB_MACHINE_NAME_5            RT_INT_START + 410
#define MB_MACHINE_NAME_6            RT_INT_START + 411
#define MB_MACHINE_NAME_7            RT_INT_START + 412
#define MB_MACHINE_NAME_8            RT_INT_START + 413
#define MB_MACHINE_NAME_9            RT_INT_START + 414
#define MB_MACHINE_VERSION           RT_INT_START + 415

#define MB_SW_ARTNO_L                RT_INT_START + 416
#define MB_SW_ARTNO_H                RT_INT_START + 417
#define MB_SW_VERSION_L              RT_INT_START + 418
#define MB_SW_VERSION_H              RT_INT_START + 419
#define MB_MACHINE_STATUS            RT_INT_START + 420
   #define MB_STATUS_STOPPED   0
   #define MB_STATUS_ERROR     1
   #define MB_STATUS_WARNING   2
   #define MB_STATUS_READY     3
   #define MB_STATUS_RUNNING   4
   #define MB_STATUS_LOADING   5

#define MB_ERROR                     RT_INT_START + 421
#define MB_WARNING                   RT_INT_START + 422
#define MB_WEB_SPEED_M_MIN           RT_INT_START + 423
#define MB_ROLL_DIAM_MM              RT_INT_START + 424
#define MB_REMAIN_M                  RT_INT_START + 425
#define MB_REMAIN_TIME_MINUTES       RT_INT_START + 426
#define MB_ROLLED_OFF_LENGTH_M       RT_INT_START + 427
#define MB_PAPER_THICKNESS_UM        RT_INT_START + 428
 
#define MB_COMMAND_RESPONSE          RT_INT_START + 430
  #define MB_RESPONSE_NONE      0
  #define MB_RESPONSE_EXECUTING 1
  #define MB_RESPONSE_FINISHED  2
#define MB_COMMAND_RESULT            RT_INT_START + 432
  #define MB_RESULT_NONE        0
  #define MB_RESULT_OK          1
  #define MB_RESULT_FAILED      2
  
#define MB_UPDATE_TIME_L             RT_INT_START + 440
#define MB_UPDATE_TIME_H             RT_INT_START + 441
#define MB_DIGITAL_INPUTS1_L         RT_INT_START + 442
#define MB_DIGITAL_INPUTS1_H         RT_INT_START + 443
#define MB_DIGITAL_INPUTS2_L         RT_INT_START + 444
#define MB_DIGITAL_INPUTS2_H         RT_INT_START + 445
#define MB_DIGITAL_OUTPUTS1_L        RT_INT_START + 446
#define MB_DIGITAL_OUTPUTS1_H        RT_INT_START + 447
#define MB_DIGITAL_OUTPUTS2_L        RT_INT_START + 448
#define MB_DIGITAL_OUTPUTS2_H        RT_INT_START + 449
#define MB_ANALOG_INPUT1             RT_INT_START + 450
#define MB_ANALOG_INPUT2             RT_INT_START + 451
#define MB_ANALOG_INPUT3             RT_INT_START + 452
#define MB_ANALOG_INPUT4             RT_INT_START + 453
#define MB_ANALOG_OUTPUT1            RT_INT_START + 454
#define MB_ANALOG_OUTPUT2            RT_INT_START + 455
#define MB_ANALOG_OUTPUT3            RT_INT_START + 456
#define MB_ANALOG_OUTPUT4            RT_INT_START + 457
#define MB_SIGNALS_OUT               RT_INT_START + 460
#define MB_FEED_MOTOR_STATUS         RT_INT_START + 461
#define MB_ROLL_STATUS               RT_INT_START + 462
#define MB_ADDITIONAL_STATUS         RT_INT_START + 463

#define MB_ROLL_DIRECTION            RT_INT_START + 470
#define MB_ACTIVE_STOP_DIAM_MM       RT_INT_START + 471
#define MB_REMAIN_DIAM1_M            RT_INT_START + 472
#define MB_REMAIN_DIAM2_M            RT_INT_START + 473

#define MB_ROLL_CHANGE_COUNT         RT_INT_START + 480

#define MB_COMMAND                   RT_INT_START + 500
  #define MB_COMMAND_NONE    0
  #define MB_COMMAND_START   3
  #define MB_COMMAND_STOP    4
  #define MB_COMMAND_RUN     7
  #define MB_COMMAND_STOPRUN 8
#define MB_COMMAND_PARAMETER         RT_INT_START + 502

#define MB_CLIENT_DEVICE_ID          RT_INT_START + 510
  #define MB_CID_C50     50
  #define MB_CID_SPLICER 40
#define MB_HEARTBEAT                 RT_INT_START + 511
#define MB_CYCLE_UP                  RT_INT_START + 512
#define MB_REVERSE                   RT_INT_START + 513
#define MB_DECURL_REQUEST            RT_INT_START + 514
#define MB_EXT_DEV_READY             RT_INT_START + 515
//#define MB_ENABLE_FEED_MOTOR         RT_INT_START + 516
#define MB_RUN                       RT_INT_START + 516
#define MB_EJECT                     RT_INT_START + 517

#define MB_SPEED_REFERENCE_M_MIN     RT_INT_START + 520
#define MB_ACC_M_MIN_S               RT_INT_START + 521
#define MB_DEC_M_MIN_S               RT_INT_START + 522
#define MB_STOP_DIAMETER_MM          RT_INT_START + 523
#define MB_DIAMETER1_MM              RT_INT_START + 524
#define MB_DIAMETER2_MM              RT_INT_START + 525
#define MB_PAPER_THICKNESS_REF_UM    RT_INT_START + 530
 
//*****************************************************
//*   Sound vars (shmem.rt_int[612-613])           *
//*****************************************************

#define ERROR_SOUND                    RT_INT_START + 612
#define IO_SOUND                       RT_INT_START + 613

//*****************************************************
//*   CFG=CONFIG vars (shmem.user_int[1-xxx])         *
//*****************************************************
#define CFG_STACK_HEIGHT_PALLET_1         1        // CFG 1-50, 5 * 10
#define CFG_STACK_HEIGHT_PALLET_2         51       // CFG 51-100, 5 * 10
#define CFG_STACK_LAST_DIR_PALLET_1       101      // CFG 1-50, 5 * 10




#define CFG_INFEED_MOTOR_MODE             135
   #define IMM_NOT_INSTALLED           0
   #define IMM_FREE_ROLL               1
   #define IMM_CONTROLLED_TENSION      2
   #define IMM_PRINTER_SIMULATION      3
   #define IMM_LOOSE_LOOP              4




#define CFG_STACK_LAST_DIR_PALLET_2       151      // CFG 51-100, 5 * 10
#define CFG_COUNTRY                       201      // Languages
#define CFG_SOFTWARE_VERSION              202      // Used for triggering actions after software upgrade

//********************************************************
//*   CFGFILE=CONFIG FILES vars                          *
//********************************************************
#define CFGFILE_SIZE                203     // How many config variables
#define CFGFILE_MACHINETYPE         550   // Machine type
#define CFGFILE_VERSION             1     // cfg-file version

//********************************************************
//*   CFGFILE=COMMON CONFIG FILE vars (shmem.user_int[500-550])*
//********************************************************
#define CFG_USERPASSWD              501   // User password
#define CFG_ETH0_IP                 502
#define CFG_ETH0_NETMASK            503
#define CFG_ETH0_GATEWAY            504
#define CFG_ETH1_IP                 505
#define CFG_ETH1_NETMASK            506
#define CFG_ETH1_GATEWAY            507
#define CFGFILE_COMMONSIZE          8    // Size on the common file
//********************************************************
//*   CFGFILE=CONFIG FILES vars (shmem.user_int[600-699])*
//********************************************************
#define CFG_DEBUG_MODE              601   // Debug On/Off
#define CFG_SERVICECODE             602   // Service Code
#define CFG_MUTE                    606   // mute error beep
#define CFG_RECEIVE_NOW             607
#define CFG_ENABLE_ROBOT            608
//#define CFG_DETONATE_BOMB           609
// Sound error 612
// Sound iotest 613

//********************************************************
//*   IO outputs (shmem.user_int[701-746])               *
//*   IO Analog outputs (shmem.user_int[747-750])        *
//********************************************************
#define IO_DIGOUT_START             701   // shmem.user_int[701-746]


#define IO_ANAOUT_START             747   // shmem.user_int[747-750]

#define IO_REVOLUTIONS              748   // shmem.user_int[748)
#define IO_SPEED                    749   // shmem.user_int[749)
#define IO_CMOS_VALUE               750   // shmem.user_int[750)

#define IO_ANAOUT_RUN               751   // analog outputs run button (shmem.user_int[751-754])

//********************************************************
//*     other stuff [800-899]                                     *
//********************************************************
#define MT_OUTFEED_MOTOR            800
#define MT_LOOP_MOTOR               801
#define MT_ROLL_MOTOR               802
#define MT_LIFT_MOTOR               803
#define MT_OUTFEED_MOTOR_RUN        804
#define MT_LOOP_MOTOR_RUN           805
#define MT_ROLL_MOTOR_RUN           806
#define MT_LIFT_MOTOR_RUN           807

//********************************************************
//*   IO outputs dummis (shmem.user_int[950-1000])       *
//********************************************************
#define IO_DIGOUT_DUMMIS_START      950   // IO "start" dummys
                                          // shmem.user_int[950-1000]

//********************************************************
//*                     CMOS defs                        *
//********************************************************
#define CMOSFILE_SIZE               15                 // How many cmos variables
#define CMOS_RUNSTAT                CMOS_START + 0     // run time
#define CMOS_ERRSTAT                CMOS_START + 1     // error time
#define CMOS_STOPSTAT               CMOS_START + 2     // stop time
#define CMOS_STANDSTAT              CMOS_START + 3     // waiting time
#define CMOS_USERFEETS              CMOS_START + 4     // user feets unit 100 feet
#define CMOS_USERTIME               CMOS_START + 5     // user time "houre, min."
#define CMOS_CRC_NEVERUSETHIS       CMOS_START + 6     // Reserved position, do not use
#define CMOS_TOTALFEETS             CMOS_START + 7     // total feets unit 100 feet
#define CMOS_TOTALTIME              CMOS_START + 8     // total time "hour, min."
#define CMOS_ROLLCHANGECOUNT        CMOS_START + 9     // Total no of roll changes
#define CMOS_ENTER_CODE_TIME        CMOS_START + 10    // Time left to enter speed license code
#define CMOS_MACHINE_TYPE           CMOS_START + 11    // machine type 
   #define MACHINE_TYPE_U50  0
   #define MACHINE_TYPE_U50i 1
#define CMOS_NUMBER                 CMOS_START + 12    // Remember number set in "Remember"
#define CMOS_REVOLUTIONS            CMOS_START + 13    // Remember number set in "Revolutions"
#define CMOS_SPEED                  CMOS_START + 14    // Remember number set in "Speed"

//********************************************************
//*                     SHMEMTEXT defs                   *
//********************************************************
#define SHMEMTEXT_SW_ARTNO_MACHINE  SHMEMTEXT_START + 0
#define SHMEMTEXT_SW_VER_MACHINE    SHMEMTEXT_START + 1
#define SHMEMTEXT_ERROR_ROW2        SHMEMTEXT_START + 2
#define SHMEMTEXT_3                 SHMEMTEXT_START + 3
#define SHMEMTEXT_NET_DEV_CONNECTED SHMEMTEXT_START + 4
#define SHMEMTEXT_5                 SHMEMTEXT_START + 5
#define SHMEMTEXT_6                 SHMEMTEXT_START + 6
#define SHMEMTEXT_MACHINE_OPTIONS   SHMEMTEXT_START + 7
#define SHMEMTEXT_SW_ARTNO_SYSTEM   SHMEMTEXT_START + 8
#define SHMEMTEXT_SW_VER_SYSTEM     SHMEMTEXT_START + 9
#define SHMEMTEXT_SW_ARTNO_BOOT     SHMEMTEXT_START + 10
#define SHMEMTEXT_SW_VER_BOOT       SHMEMTEXT_START + 11
#define SHMEMTEXT_SW_ARTNO_EBM_APP  SHMEMTEXT_START + 12
#define SHMEMTEXT_SW_VER_EBM_APP    SHMEMTEXT_START + 13
#define SHMEMTEXT_SW_ARTNO_EBM_BOOT SHMEMTEXT_START + 14
#define SHMEMTEXT_SW_VER_EBM_BOOT   SHMEMTEXT_START + 15

