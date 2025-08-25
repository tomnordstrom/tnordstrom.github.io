/* vars.h */

#ifndef __VARS_H
#define __VARS_H

#include <stdbool.h>

#include "defs.h"
#include "lxinterface.h"
#include "ebm.h"
//#include "fs100.h"
//include "libmodbus/modbus.h"
#include "digitax.h"

extern int guiState;
extern int guiStatePrev;

extern int machineType;  // MACHINE_TYPE_U50 or MACHINE_TYPE_U50i
extern int sw_machine_version;
extern int sw_system_version;
extern int sw_boot_version;
extern unsigned int runstat,standstat,errstat,stopstat;
extern bool ok_to_start_tick;
extern int service_time;
extern int user_password;
extern bool internalUsbCorrupt;
extern bool download_done;		// used to know when to update freq. drive with var.

// ---- error ----------------------------------------------
typedef struct
{
   int no;
   int subNo;
   bool fullSignalConveyor;
} error_t;

void error_Init(error_t *error);
bool error_Check(error_t *error, int machineState);
void error_Handle(error_t *error);

// ---- machine ----------------------------------------------
#define MSTATE_DISABLED 0
#define MSTATE_STOPPED  1
#define MSTATE_STARTED  2
#define MSTATE_ERROR    3
#define MSTATE_IOTEST   4

#define RUN_STATE_UNDEFINED        0
#define RUN_STATE_GOTO_HOME        1
#define RUN_STATE_AT_HOME          2
#define RUN_STATE_GOTO_PICKING     3
#define RUN_STATE_AT_PICKING       4
#define RUN_STATE_GOTO_PALLET      5
#define RUN_STATE_AT_PALLET        6

typedef struct
{
   int state;
   int runState;
   error_t error;
} machine_t;
extern machine_t machine;

// ---- config parameters ----------------------------------------------
typedef struct
{
   int country;            // Languages
   int machine_version;
   int eth0_ip;
   int eth0_netmask;
   int eth0_gateway;
   int eth1_ip;
   int eth1_netmask;
   int eth1_gateway;
   int debug_mode;
   int infeed_motor_mode;
} cfg_t;
extern cfg_t config;



void turnOnOutputs(void);



// ---- analog ---------------------------------------------------------
#define ANALOG_OFFSET 512
typedef struct
{
   int in[4];
   int value[4][5];
   int value_idx[4];
   bool in_calc[4];
} analog_t;
extern analog_t analog;

void analog_Init(void);
void analog_Tick100(void);



//    Prototypes for functions in "main.c".
void mainLoop(int loopCnt);
void input2_irq(InputInt_t InputInt);
int getExecTimeLeft_us();                // execution time left in current loop in microseconds
void initial(void);
void *tick100(void *);
void *idleloop(void *);
void heartbeat_cyclic();
void lamp_Control(void);

void guiStateSet(int state);
void guiStateSetPrev(void);
int guiStateGet(void);

int heightSensor_Value_mm(void);
bool heightSensor_Active(void);


void debugPrint(char *fmt, ...);
void logPrint(char *fmt, ...);
void error_sound(bool play);
int rtRequestPar(int request, int par);
int rtRequest(int request);

void cmos_Save(void);
void cmos_Load(void);

void set_defaults(void);
void check_software_upgrade(void);
void update_configs(void);

void iotest_init(void);
void iotest_cleanup(void);
void iotest(void);
void motortest(void);






#define DRVTYPE_NONE    0
#define DRVTYPE_MX2     1
#define DRVTYPE_DIGITAX 2
#define DRVTYPE_NOTFOUND  99

#define DRVDIR_FORWARD 0
#define DRVDIR_REVERSE 1

typedef struct
{
   int type;
   bool download_done;   
   int requestedSpeed;
   int sentSpeed;
   int requestedTorque;
   int sentTorque;
} drvFeed_t;
extern drvFeed_t drvFeed;
void drvFeed_Init();
void drvFeed_Cyclic();
void drvFeed_SetSpeed_rpm(int speed);
void drvFeed_SetSpeed_m_min(int speed);
void drvFeed_SetTorque_percent(int torque);

void feed_SetTorqueMode();
extern digitax_t digitaxFeed;





#endif //__VARS_H
