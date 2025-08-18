#ifndef _ebmShmInterface_H_
#define _ebmShmInterface_H_
#include <stdbool.h>
#include "ebmMemDefs.h"

#define MAX_EBM_CONNECTIONS 3

extern int hShmInt_ebmComm_cmd;
extern int hShmInt_ebmComm_response;
extern int hShmInt_ebmComm_state;
extern int hShmInt_ebmComm_noOfEbm;
extern int hShmBool_ebmComm_sendFlag;

extern int hShmBool_ebm_connect[MAX_EBM_CONNECTIONS];
extern int hShmBool_ebm_connected[MAX_EBM_CONNECTIONS];
extern int hShmUInt_ebm_ipEbm[MAX_EBM_CONNECTIONS];
extern int hShmUInt_ebm_resendCnt[MAX_EBM_CONNECTIONS];
extern int hShmUInt_ebm_sendSeqNo[MAX_EBM_CONNECTIONS];
extern int hShmUInt_ebm_receivedSeqNo[MAX_EBM_CONNECTIONS];

#define EBMCOMM_CMD_NONE	0
#define EBMCOMM_CMD_INIT	1

#define EBMCOMM_RESPONSE_FAILED	0
#define EBMCOMM_RESPONSE_OK		1

#define EBMCOMM_STATE_NOT_INIT		0
#define EBMCOMM_STATE_INITIALIZED	1

typedef struct 
{
	unsigned char input;
	unsigned long long time;
	unsigned int inputs;
	unsigned int encoder;
} InputInt_t;

bool ebmCommCmd(int cmd);
bool ebmCommResult(int *result);

bool ebmCommInit(int ebmId, unsigned int ebmIP);
void ebmSetSendFlag(void);
void ebmClearSendFlag(void);
bool ebmGetSendFlag(void);

void ebmConnect(int ebmId);
void ebmDisconnect(int ebmId);
bool ebmIsConnected(int ebmId);

unsigned int ebmGetNextSeqNo(int ebmId);
unsigned int ebmGetSeqNo(int ebmId);

bool ebmGetInput(int ebmId, int inputNo);							// inputNo: 1-28
void ebmSetOutput(int ebmId, int outputNo, bool value);		// outputNo: 1-18
bool ebmGetOutput(int ebmId, int outputNo);						// outputNo: 1-18

float ebmGetAnalogIn(int ebmId, int inputNo);					// inputNo: 1-3
void ebmSetAnalogOut(int ebmId, int outputNo, float value);	// outputNo: 1-4
float ebmGetAnalogOut(int ebmId, int outputNo);					// outputNo: 1-4

bool ebmUartSetup(int ebmId, int uartNo, int baudrate, int databits, int stopbits, int parity); // parity:0=None,1=even,2=odd
int ebmUartSend(int ebmId, int uartNo, void *buff, int cnt);// uartNo: 1-2
int ebmUartReceive(int ebmId, int uartNo, void *buff, int cnt);	// uartNo: 1-2
void ebmUartClear(int ebmId, int uartNo);							// uartNo: 1-2

void ebmEnableWatchdog(int ebmId, unsigned time_ms, unsigned outputs);
void ebmDisableWatchdog(int ebmId);

bool ebmGetInputInt(int ebmId, InputInt_t *InputInt);

int ebmStepStatus(int ebmId, int stepNo);  // stepNo: 1-6
int ebmStepCurPos(int ebmId, int stepNo);  // stepNo: 1-6
int ebmStepCurSpeed(int ebmId, int stepNo);  // stepNo: 1-6

bool ebmStepInit(int ebmId, int stepNo, int outputNoStep, int outputNoDir, bool positiveDir);
bool ebmStepDeInit(int ebmId, int stepNo);
bool ebmStepSetupHome(int ebmId, int stepNo, int inputNoHome, bool homeActiveLevel, bool HomeDir);
bool ebmStepSetupLimit(int ebmId, int stepNo, int inputNoLimit, bool limitActiveLevel);
bool ebmStepHome(int ebmId, int stepNo, unsigned maxDist, unsigned homeSpeed);
bool ebmStepHomeBackoff(int ebmId, int stepNo, unsigned maxDist, unsigned homeSpeed, unsigned backoffSpeed);
bool ebmStepSetPos(int ebmId, int stepNo, int pos);
bool ebmStepSetAccDec(int ebmId, int stepNo, int acc, int dec);
bool ebmStepMoveAbs(int ebmId, int stepNo, int pos, unsigned speed);
bool ebmStepMoveRel(int ebmId, int stepNo, int dist, unsigned speed);
bool ebmStepMoveVel(int ebmId, int stepNo, bool dir, unsigned speed);
bool ebmStepStop(int ebmId, int stepNo);
bool ebmStepHalt(int ebmId, int stepNo);

bool ebmSendCommand(int ebmId, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4, unsigned int data5);


bool ebmGetBit(int ebmId, unsigned short handle);
void ebmSetBit(int ebmId, unsigned short handle, bool value);
unsigned char ebmGetUInt8(int ebmId, unsigned char handle);
void ebmSetUInt8(int ebmId, unsigned short handle, unsigned char value);
char ebmGetInt8(int ebmId, unsigned char handle);
void ebmSetInt8(int ebmId, unsigned short handle, char value);
unsigned short ebmGetUInt16(int ebmId, unsigned short handle);
void ebmSetUInt16(int ebmId, unsigned short handle, unsigned short value);
short ebmGetInt16(int ebmId, unsigned short handle);
void ebmSetInt16(int ebmId, unsigned short handle, short value);
unsigned int ebmGetUInt32(int ebmId, unsigned short handle);
void ebmSetUInt32(int ebmId, unsigned short handle, unsigned int value);
int ebmGetInt32(int ebmId, unsigned short handle);
void ebmSetInt32(int ebmId, unsigned short handle, int value);
unsigned long long ebmGetUInt64(int ebmId, unsigned short handle);
float ebmGetFloat(int ebmId, unsigned short handle);
void ebmSetFloat(int ebmId, unsigned short handle, float value);
void ebmSetUInt64(int ebmId, unsigned short handle, unsigned long long value);
char *ebmGetString(int ebmId, unsigned short handle);
int ebmGetShmHandle(int ebmId, unsigned short memType, unsigned short handle);

#endif
