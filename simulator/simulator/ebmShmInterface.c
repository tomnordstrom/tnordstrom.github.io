#include <stdio.h>
#include "ebmShmInterface.h"
#include "shmInterface.h"

int ebmIdMax = 0;

int hShmInt_ebmComm_cmd;
int hShmInt_ebmComm_response;
int hShmInt_ebmComm_state;
int hShmInt_ebmComm_noOfEbm;
int hShmBool_ebmComm_sendFlag;

int hShmBool_ebm_connect[MAX_EBM_CONNECTIONS];
int hShmBool_ebm_connected[MAX_EBM_CONNECTIONS];
int hShmUInt_ebm_ipEbm[MAX_EBM_CONNECTIONS];
int hShmUInt_ebm_resendCnt[MAX_EBM_CONNECTIONS];
int hShmUInt_ebm_sendSeqNo[MAX_EBM_CONNECTIONS];
int hShmUInt_ebm_receivedSeqNo[MAX_EBM_CONNECTIONS];

char *ebmCommInitHandles(int ebmId);  // returns NULL if ok. returns tagname if not found in shm

bool ebmCommCmd(int cmd)
{
	printf("ebmCommCmd\n");

	if (shmReadIntByHandle(hShmInt_ebmComm_cmd) != 0)
		return false;
	printf("ebmCommCmd %d %d\n",hShmInt_ebmComm_cmd,cmd);
	shmWriteIntByHandle(hShmInt_ebmComm_cmd, cmd);
	return true;
}

bool ebmCommResponse(int *response)
{
	if (shmReadIntByHandle(hShmInt_ebmComm_cmd) != 0)
		return false;
	*response = shmReadIntByHandle(hShmInt_ebmComm_response);
	return true;
}

bool ebmCommInit(int ebmId, unsigned int ebmIP)
{
	char *errTag;

	if ((errTag = ebmCommInitHandles(ebmId)) != NULL)
	{
		printf("Parameter %s is not found in shared memory\n",errTag);
		return false;
	}
	if (ebmIP != 0)
	{
		shmWriteUIntByHandle(hShmUInt_ebm_ipEbm[ebmId], ebmIP);
		if (ebmIdMax < ebmId)
			ebmIdMax = ebmId;
		shmWriteIntByHandle(hShmInt_ebmComm_noOfEbm, ebmIdMax+1);
	}
	return true;
}

void ebmSetSendFlag(void)
{
	shmWriteBoolByHandle(hShmBool_ebmComm_sendFlag,1);
}

void ebmClearSendFlag(void)
{
	shmWriteBoolByHandle(hShmBool_ebmComm_sendFlag,0);
}

bool ebmGetSendFlag(void)
{
	return shmReadBoolByHandle(hShmBool_ebmComm_sendFlag);
}

void ebmConnect(int ebmId)
{
	printf("ebm%d ebmConnect\n", ebmId);
	if (shmReadIntByHandle(hShmInt_ebmComm_state) == EBMCOMM_STATE_NOT_INIT)
	{
		ebmCommCmd(EBMCOMM_CMD_INIT);
		return;
	}
	printf("ebm%d ebmConnect: %d\n",ebmId,hShmBool_ebm_connect[ebmId]);

	shmWriteBoolByHandle(hShmBool_ebm_connect[ebmId],1);
}

void ebmDisconnect(int ebmId)
{
	shmWriteBoolByHandle(hShmBool_ebm_connect[ebmId],0);
}

bool ebmIsConnected(int ebmId)
{
	return shmReadBoolByHandle(hShmBool_ebm_connected[ebmId]);
}

unsigned int ebmGetNextSeqNo(int ebmId)
{
	if (shmReadUIntByHandle(hShmUInt_ebm_sendSeqNo[ebmId]) != shmReadUIntByHandle(hShmUInt_ebm_receivedSeqNo[ebmId]))
		return shmReadUIntByHandle(hShmUInt_ebm_receivedSeqNo[ebmId]) + 2;
	else
		return shmReadUIntByHandle(hShmUInt_ebm_receivedSeqNo[ebmId]) + 1;
}

unsigned int ebmGetSeqNo(int ebmId)
{
	return shmReadUIntByHandle(hShmUInt_ebm_receivedSeqNo[ebmId]);
}

bool ebmGetInput(int ebmId, int inputNo)						// inputNo: 1-28
{
	if ((inputNo < 1) || (inputNo > 28))
		return false;
	return ebmGetBit(ebmId, EBMMEM_BIT_INPUT_START+inputNo-1);
}

void ebmSetOutput(int ebmId, int outputNo, bool value)						// outputNo: 1-18
{
	if ((outputNo < 1) || (outputNo > 18))
		return;
	ebmSetBit(ebmId, EBMMEM_BIT_OUTPUT_START+outputNo-1,value);
}

bool ebmGetOutput(int ebmId, int outputNo)						// outputNo: 1-18
{
	if ((outputNo < 1) || (outputNo > 18))
		return false;
	return ebmGetBit(ebmId, EBMMEM_BIT_OUTPUT_START+outputNo-1);
}

float ebmGetAnalogIn(int ebmId, int inputNo)					// inputNo: 1-3
{
	if ((inputNo < 1) || (inputNo > 3))
		return 0;
	return ebmGetFloat(ebmId, EBMMEM_FLOAT_ANALOG_IN1+inputNo-1);
}

void ebmSetAnalogOut(int ebmId, int outputNo, float value)	// outputNo: 1-4
{
	if ((outputNo < 1) || (outputNo > 4))
		return;
	ebmSetFloat(ebmId, EBMMEM_FLOAT_ANALOG_OUT1+outputNo-1, value);
}

float ebmGetAnalogOut(int ebmId, int outputNo)					// outputNo: 1-4
{
	if ((outputNo < 1) || (outputNo > 4))
		return 0;
	return ebmGetFloat(ebmId, EBMMEM_FLOAT_ANALOG_OUT1+outputNo-1);
}

bool ebmUartSetup(int ebmId, int uartNo, int baudrate, int databits, int stopbits, int parity) // parity:0=None,1=even,2=odd
{
   if (!ebmIsConnected(0))
      return 0;
   if (uartNo == 1)
      uartNo = EBMMEM_UINT32_CONFIG_UART_1;
   else if (uartNo == 2)
      uartNo = EBMMEM_UINT32_CONFIG_UART_2;
   else 
      return false;
   if ((databits < 5) || (databits > 8))
      return false;
   if ((stopbits < 1) || (stopbits > 2))
      return false;
   if ((parity < 0) || (parity > 2))
      return false;
   ebmSetUInt32(ebmId, uartNo, (baudrate & 0xffffff) | ((databits-5) << 24) | (stopbits << 26) | (parity << 28));
   return true;
}

int ebmUartSend(int ebmId, int uartNo, void *buff, int cnt)	// uartNo: 1-2
{
	short shmHandle;
	if (!ebmIsConnected(0))
		return 0;
	if (uartNo == 1)
		uartNo = EBMMEM_FIFO_UINT8_UART1_TX;
	else if (uartNo == 2)
		uartNo = EBMMEM_FIFO_UINT8_UART2_TX;
	else 
		return -1;
	
	// ToDo, check if there is room for all bytes in buffer
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FIFO_UINT8, uartNo)) != -1)
	{
		if (shmCpynByteToFifoByHandle(buff, shmHandle, cnt))
			return cnt;
		else
			return -1;
	}
	else
		return -1;
}

int ebmUartReceive(int ebmId, int uartNo, void *buff, int cnt)	// uartNo: 1-2
{
	short shmHandle;
	if (uartNo == 1)
		uartNo = EBMMEM_FIFO_UINT8_UART1_RX;
	else if (uartNo == 2)
		uartNo = EBMMEM_FIFO_UINT8_UART2_RX;

	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FIFO_UINT8, uartNo)) != -1)
		return (shmCpynByteFromFifoByHandle(buff, shmHandle, cnt));
	else
		return 0;
}

void ebmUartClear(int ebmId, int uartNo)	// uartNo: 1-2
{
	char c;
	short shmHandle;
	unsigned short address;

	if (uartNo == 1)
		address = EBMMEM_FIFO_UINT8_UART1_RX;
	else if (uartNo == 2)
		address = EBMMEM_FIFO_UINT8_UART2_RX;
	else return;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FIFO_UINT8, address)) != -1)
		while (shmCpynByteFromFifoByHandle(&c, shmHandle, 1));

	if (uartNo == 1)
		address = EBMMEM_FIFO_UINT8_UART1_TX;
	else if (uartNo == 2)
		address = EBMMEM_FIFO_UINT8_UART2_TX;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FIFO_UINT8, address)) != -1)
		while (shmCpynByteFromFifoByHandle(&c, shmHandle, 1));
}

void ebmEnableWatchdog(int ebmId, unsigned time_ms, unsigned outputs)
{	
	ebmSetUInt32(ebmId, EBMMEM_UINT32_WATCHDOG_TIME, time_ms);
	ebmSetUInt32(ebmId, EBMMEM_UINT32_WATCHDOG_OUTPUTS, outputs);
}

void ebmDisableWatchdog(int ebmId)
{
	ebmSetUInt32(ebmId, EBMMEM_UINT32_WATCHDOG_OUTPUTS, 0);
}

bool ebmGetInputInt(int ebmId, InputInt_t *InputInt)
{
	unsigned int fifoData[5];
	int errCode;
	short shmHandle;

	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FIFO_UINTx5, EBMMEM_FIFO_UINTx5_INPUT_INT)) != -1)
	{
		if (shmGetDataFifoByHandle(shmHandle, (int *)fifoData, &errCode))
		{
			InputInt->input = fifoData[0];
			InputInt->time = (((unsigned long long)fifoData[2]) << 32) | (unsigned long long)fifoData[1];
			InputInt->inputs = fifoData[3];
			InputInt->encoder = fifoData[4];
			return true;
		}
	}
	return false;
}

int ebmStepStatus(int ebmId, int stepNo)  // stepNo: 1-6
{
	if ((stepNo < 1) || (stepNo > 6))
		return 0;
	return ebmGetInt32(ebmId, EBMMEM_INT32_STEP1_STATUS+stepNo-1);
}

int ebmStepCurPos(int ebmId, int stepNo)  // stepNo: 1-6
{
	if ((stepNo < 1) || (stepNo > 6))
		return 0;
	return ebmGetInt32(ebmId, EBMMEM_INT32_STEP1_CURPOS+stepNo-1);
}

int ebmStepCurSpeed(int ebmId, int stepNo)  // stepNo: 1-6
{
	if ((stepNo < 1) || (stepNo > 6))
		return 0;
	return ebmGetInt32(ebmId, EBMMEM_INT32_STEP1_CURSPEED+stepNo-1);
}

bool ebmStepInit(int ebmId, int stepNo, int outputNoStep, int outputNoDir, bool positiveDir)
{
	return ebmSendCommand(ebmId, 100, (unsigned)stepNo, (unsigned)outputNoStep, (unsigned)outputNoDir, (unsigned)positiveDir);
}

bool ebmStepDeInit(int ebmId, int stepNo)
{
	return ebmSendCommand(ebmId, 101, (unsigned)stepNo, 0, 0, 0);
}

bool ebmStepSetupHome(int ebmId, int stepNo, int inputNoHome, bool homeActiveLevel, bool HomeDir)
{
	return ebmSendCommand(ebmId, 102, (unsigned)stepNo, (unsigned)inputNoHome, (unsigned)homeActiveLevel, (unsigned)HomeDir);
}

bool ebmStepSetupLimit(int ebmId, int stepNo, int inputNoLimit, bool limitActiveLevel)
{
	return ebmSendCommand(ebmId, 103, (unsigned)stepNo, (unsigned)inputNoLimit, (unsigned)limitActiveLevel, 0);
}

bool ebmStepHome(int ebmId, int stepNo, unsigned maxDist, unsigned homeSpeed)
{
	return ebmSendCommand(ebmId, 105, (unsigned)stepNo, maxDist, homeSpeed, 0);
}

bool ebmStepHomeBackoff(int ebmId, int stepNo, unsigned maxDist, unsigned homeSpeed, unsigned backoffSpeed)
{
	return ebmSendCommand(ebmId, 106, (unsigned)stepNo, maxDist, homeSpeed, backoffSpeed);
}

bool ebmStepSetPos(int ebmId, int stepNo, int pos)
{
	return ebmSendCommand(ebmId, 107, (unsigned)stepNo, (unsigned)pos, 0, 0);
}

bool ebmStepSetAccDec(int ebmId, int stepNo, int acc, int dec)
{
	return ebmSendCommand(ebmId, 108, (unsigned)stepNo, (unsigned)acc, (unsigned)dec, 0);
}

bool ebmStepMoveAbs(int ebmId, int stepNo, int pos, unsigned speed)
{
	return ebmSendCommand(ebmId, 109, (unsigned)stepNo, (unsigned)pos, speed, 0);
}

bool ebmStepMoveRel(int ebmId, int stepNo, int dist, unsigned speed)
{
	return ebmSendCommand(ebmId, 110, (unsigned)stepNo, (unsigned)dist, speed, 0);
}

bool ebmStepMoveVel(int ebmId, int stepNo, bool dir, unsigned speed)
{
	return ebmSendCommand(ebmId, 111, (unsigned)stepNo, (unsigned)dir, (unsigned)speed, 0);
}

bool ebmStepStop(int ebmId, int stepNo)
{
	return ebmSendCommand(ebmId, 112, (unsigned)stepNo, 0, 0, 0);
}

bool ebmStepHalt(int ebmId, int stepNo)
{
	return ebmSendCommand(ebmId, 113, (unsigned)stepNo, 0, 0, 0);
}

bool ebmSendCommand(int ebmId, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4, unsigned int data5)
{
	unsigned int fifoData[5];
	int errCode;
	short shmHandle;

	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FIFO_UINTx5, EBMMEM_FIFO_UINTx5_COMMAND)) != -1)
	{
		fifoData[0] = data1;
		fifoData[1] = data2;
		fifoData[2] = data3;
		fifoData[3] = data4;
		fifoData[4] = data5;
		if (shmAddDataFifoByHandle(shmHandle, (int *)fifoData, &errCode))
			return true;
	}
	return false;
}

//----------------------------------------------------------------------
bool ebmGetBit(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_BIT, handle)) != -1)
		return shmReadBoolByHandle(shmHandle);	
	return 0;
}

void ebmSetBit(int ebmId, unsigned short handle, bool value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_BIT, handle)) != -1)
	{
//		printf("handle %d -> shmHandle %d\n",handle, shmHandle);
		shmWriteBoolByHandle(shmHandle, value);	
	}
}

unsigned char ebmGetUInt8(int ebmId, unsigned char handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT8, handle)) != -1)
		return (unsigned char)(shmReadUIntByHandle(shmHandle));	
	return 0;
}

void ebmSetUInt8(int ebmId, unsigned short handle, unsigned char value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT8, handle)) != -1)
		shmWriteUIntByHandle(shmHandle, (unsigned int)value);	
}

char ebmGetInt8(int ebmId, unsigned char handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_INT8, handle)) != -1)
		return (char)(shmReadIntByHandle(shmHandle));	
	return 0;
}

void ebmSetInt8(int ebmId, unsigned short handle, char value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_INT8, handle)) != -1)
		shmWriteIntByHandle(shmHandle, (int)value);	
}

unsigned short ebmGetUInt16(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT16, handle)) != -1)
		return (unsigned short)shmReadUIntByHandle(shmHandle);	
	return 0;
}

void ebmSetUInt16(int ebmId, unsigned short handle, unsigned short value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT16, handle)) != -1)
		shmWriteUIntByHandle(shmHandle, value);	
}

short ebmGetInt16(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_INT16, handle)) != -1)
		return (short)shmReadUIntByHandle(shmHandle);	
	return 0;
}

void ebmSetInt16(int ebmId, unsigned short handle, short value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_INT16, handle)) != -1)
		shmWriteIntByHandle(shmHandle, (int)value);	
}

unsigned int ebmGetUInt32(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT32, handle)) != -1)
		return shmReadUIntByHandle(shmHandle);	
	return 0;
}

void ebmSetUInt32(int ebmId, unsigned short handle, unsigned int value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT32, handle)) != -1)
		shmWriteUIntByHandle(shmHandle, value);	
}

int ebmGetInt32(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_INT32, handle)) != -1)
		return shmReadIntByHandle(shmHandle);	
	return 0;
}

void ebmSetInt32(int ebmId, unsigned short handle, int value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_INT32, handle)) != -1)
		shmWriteIntByHandle(shmHandle, value);	
}

float ebmGetFloat(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FLOAT, handle)) != -1)
		return shmReadRealByHandle(shmHandle);	
	return 0;
}

void ebmSetFloat(int ebmId, unsigned short handle, float value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_FLOAT, handle)) != -1)
		shmWriteRealByHandle(shmHandle, value);	
}
/*
double ebmGetDouble(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_DOUBLE, handle)) != -1)
		return shmReadDoubleByHandle(shmHandle);	
	return 0;
}

void ebmSetDouble(int ebmId, double handle, float value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_DOUBLE, handle)) != -1)
		shmWriteDoubleByHandle(shmHandle, value);	
}
*/
unsigned long long ebmGetUInt64(int ebmId, unsigned short handle)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT64, handle)) != -1)
		return ((((unsigned long long)shmReadUIntByHandle(shmHandle+1)) << 32) | (unsigned long long)shmReadUIntByHandle(shmHandle));
	return 0;
}

void ebmSetUInt64(int ebmId, unsigned short handle, unsigned long long value)
{
	short shmHandle;
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_UINT64, handle)) != -1)
	{
		shmWriteUIntByHandle(shmHandle+1, (unsigned int)(value >> 32)); 
		shmWriteUIntByHandle(shmHandle, (unsigned int)(value & 0xffffffff)); 
	}
}

char *ebmGetString(int ebmId, unsigned short handle)
{
	short shmHandle;
	int errCode;
	char *str;
	
	if ((shmHandle = ebmGetShmHandle(ebmId, MEM_TYPE_STRING, handle)) != -1)
	{
		shmGetTextByHandle(shmHandle, &str, &errCode);
		return str;
	}
	else
		return NULL;
}

int ebmGetShmHandle(int ebmId, unsigned short memType, unsigned short handle)
{	
	if (memType >= 32) // FIFO
		memType = (memType - 16) + (ebmId * 32);
	else
		memType = memType + (ebmId * 32);

	return shmGetHandleByGroupAddress(memType, handle);
}

char *ebmTagName(int ebmId,char *tagName)
{
	static char s[30];
	sprintf(s,"ebm%d_%s",ebmId,tagName);
	return s;
}

char *ebmCommInitHandles(int ebmId)  // returns NULL if ok. returns tagname if not found in shm
{
	int errCode;
	if (!shmGetHandleByTagname("ebmcomm_cmd", &hShmInt_ebmComm_cmd, &errCode)) return "ebmcomm_cmd";
	if (!shmGetHandleByTagname("ebmcomm_response", &hShmInt_ebmComm_response, &errCode)) return "ebmcomm_response";
	if (!shmGetHandleByTagname("ebmcomm_state", &hShmInt_ebmComm_state, &errCode)) return "ebmcomm_state";
	if (!shmGetHandleByTagname("ebmcomm_noofebm", &hShmInt_ebmComm_noOfEbm, &errCode)) return "ebmcomm_noofebm";
	if (!shmGetHandleByTagname("ebmcomm_sendflag", &hShmBool_ebmComm_sendFlag, &errCode)) return "ebmcomm_sendflag";

	if (!shmGetHandleByTagname(ebmTagName(ebmId,"connect"), &hShmBool_ebm_connect[ebmId], &errCode)) return ebmTagName(ebmId,"connect");
	if (!shmGetHandleByTagname(ebmTagName(ebmId,"connected"), &hShmBool_ebm_connected[ebmId], &errCode)) return ebmTagName(ebmId,"connected");
	if (!shmGetHandleByTagname(ebmTagName(ebmId,"ip_ebm"), &hShmUInt_ebm_ipEbm[ebmId], &errCode)) return ebmTagName(ebmId,"ip_ebm");
	if (!shmGetHandleByTagname(ebmTagName(ebmId,"resend_cnt"), &hShmUInt_ebm_resendCnt[ebmId], &errCode)) return ebmTagName(ebmId,"resend_cnt");
	if (!shmGetHandleByTagname(ebmTagName(ebmId,"send_seqno"), &hShmUInt_ebm_sendSeqNo[ebmId], &errCode)) return ebmTagName(ebmId,"send_seqno");
	if (!shmGetHandleByTagname(ebmTagName(ebmId,"received_seqno"), &hShmUInt_ebm_receivedSeqNo[ebmId], &errCode)) return ebmTagName(ebmId,"received_seqno");
	return NULL;
}

/*
char *populateEbmShmHandles(int id)  // returns NULL if ok. returns tagname if not found in shm
{
	int errCode;
//	int id;
	if (!shmGetHandleByTagname("ebmcomm_cmd", &(ebmShmHandles[EBMCOMM_CMD]), &errCode)) return "ebmcomm_cmd";
	if (!shmGetHandleByTagname("ebmcomm_response", &(ebmShmHandles[EBMCOMM_RESPONSE]), &errCode)) return "ebmcomm_response";
	if (!shmGetHandleByTagname("ebmcomm_state", &(ebmShmHandles[EBMCOMM_STATE]), &errCode)) return "ebmcomm_state";
	if (!shmGetHandleByTagname("ebmcomm_noofebm", &(ebmShmHandles[EBMCOMM_NO_OF_EBM]), &errCode)) return "ebmcomm_noofebm";
	if (!shmGetHandleByTagname("ebmcomm_sendflag", &(ebmShmHandles[EBMCOMM_SENDFLAG]), &errCode)) return "ebmcomm_sendflag";

//	for (id = 0; id < 1; ++id)
//	{
		if (!shmGetHandleByTagname(ebmTagName(id,"app_version"), &(ebmShmHandles[EBM_SHM(id,VERSION)]), &errCode)) return ebmTagName(id,"app_version");
		if (!shmGetHandleByTagname(ebmTagName(id,"connect"), &(ebmShmHandles[EBM_SHM(id,CONNECT)]), &errCode)) return ebmTagName(id,"connect");
		if (!shmGetHandleByTagname(ebmTagName(id,"connected"), &(ebmShmHandles[EBM_SHM(id,CONNECTED)]), &errCode)) return ebmTagName(id,"connected");
		if (!shmGetHandleByTagname(ebmTagName(id,"input01"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT01)]), &errCode)) return ebmTagName(id,"input01");
		if (!shmGetHandleByTagname(ebmTagName(id,"input02"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT02)]), &errCode)) return ebmTagName(id,"input02");
		if (!shmGetHandleByTagname(ebmTagName(id,"input03"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT03)]), &errCode)) return ebmTagName(id,"input03");
		if (!shmGetHandleByTagname(ebmTagName(id,"input04"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT04)]), &errCode)) return ebmTagName(id,"input04");
		if (!shmGetHandleByTagname(ebmTagName(id,"input05"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT05)]), &errCode)) return ebmTagName(id,"input05");
		if (!shmGetHandleByTagname(ebmTagName(id,"input06"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT06)]), &errCode)) return ebmTagName(id,"input06");
		if (!shmGetHandleByTagname(ebmTagName(id,"input07"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT07)]), &errCode)) return ebmTagName(id,"input07");
		if (!shmGetHandleByTagname(ebmTagName(id,"input08"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT08)]), &errCode)) return ebmTagName(id,"input08");
		if (!shmGetHandleByTagname(ebmTagName(id,"input09"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT09)]), &errCode)) return ebmTagName(id,"input09");
		if (!shmGetHandleByTagname(ebmTagName(id,"input10"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT10)]), &errCode)) return ebmTagName(id,"input10");
		if (!shmGetHandleByTagname(ebmTagName(id,"input11"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT11)]), &errCode)) return ebmTagName(id,"input11");
		if (!shmGetHandleByTagname(ebmTagName(id,"input12"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT12)]), &errCode)) return ebmTagName(id,"input12");
		if (!shmGetHandleByTagname(ebmTagName(id,"input13"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT13)]), &errCode)) return ebmTagName(id,"input13");
		if (!shmGetHandleByTagname(ebmTagName(id,"input14"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT14)]), &errCode)) return ebmTagName(id,"input14");
		if (!shmGetHandleByTagname(ebmTagName(id,"input15"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT15)]), &errCode)) return ebmTagName(id,"input15");
		if (!shmGetHandleByTagname(ebmTagName(id,"input16"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT16)]), &errCode)) return ebmTagName(id,"input16");
		if (!shmGetHandleByTagname(ebmTagName(id,"input17"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT17)]), &errCode)) return ebmTagName(id,"input17");
		if (!shmGetHandleByTagname(ebmTagName(id,"input18"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT18)]), &errCode)) return ebmTagName(id,"input18");
		if (!shmGetHandleByTagname(ebmTagName(id,"input19"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT19)]), &errCode)) return ebmTagName(id,"input19");
		if (!shmGetHandleByTagname(ebmTagName(id,"input20"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT20)]), &errCode)) return ebmTagName(id,"input20");
		if (!shmGetHandleByTagname(ebmTagName(id,"input21"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT21)]), &errCode)) return ebmTagName(id,"input21");
		if (!shmGetHandleByTagname(ebmTagName(id,"input22"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT22)]), &errCode)) return ebmTagName(id,"input22");
		if (!shmGetHandleByTagname(ebmTagName(id,"input23"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT23)]), &errCode)) return ebmTagName(id,"input23");
		if (!shmGetHandleByTagname(ebmTagName(id,"input24"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT24)]), &errCode)) return ebmTagName(id,"input24");
		if (!shmGetHandleByTagname(ebmTagName(id,"input25"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT25)]), &errCode)) return ebmTagName(id,"input25");
		if (!shmGetHandleByTagname(ebmTagName(id,"input26"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT26)]), &errCode)) return ebmTagName(id,"input26");
		if (!shmGetHandleByTagname(ebmTagName(id,"input27"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT27)]), &errCode)) return ebmTagName(id,"input27");
		if (!shmGetHandleByTagname(ebmTagName(id,"input28"), &(ebmShmHandles[EBM_SHM(id,EBM_INPUT28)]), &errCode)) return ebmTagName(id,"input28");

		if (!shmGetHandleByTagname(ebmTagName(id,"output01"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT01)]), &errCode)) return ebmTagName(id,"output01");
		if (!shmGetHandleByTagname(ebmTagName(id,"output02"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT02)]), &errCode)) return ebmTagName(id,"output02");
		if (!shmGetHandleByTagname(ebmTagName(id,"output03"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT03)]), &errCode)) return ebmTagName(id,"output03");
		if (!shmGetHandleByTagname(ebmTagName(id,"output04"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT04)]), &errCode)) return ebmTagName(id,"output04");
		if (!shmGetHandleByTagname(ebmTagName(id,"output05"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT05)]), &errCode)) return ebmTagName(id,"output05");
		if (!shmGetHandleByTagname(ebmTagName(id,"output06"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT06)]), &errCode)) return ebmTagName(id,"output06");
		if (!shmGetHandleByTagname(ebmTagName(id,"output07"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT07)]), &errCode)) return ebmTagName(id,"output07");
		if (!shmGetHandleByTagname(ebmTagName(id,"output08"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT08)]), &errCode)) return ebmTagName(id,"output08");
		if (!shmGetHandleByTagname(ebmTagName(id,"output09"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT09)]), &errCode)) return ebmTagName(id,"output09");
		if (!shmGetHandleByTagname(ebmTagName(id,"output10"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT10)]), &errCode)) return ebmTagName(id,"output10");
		if (!shmGetHandleByTagname(ebmTagName(id,"output11"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT11)]), &errCode)) return ebmTagName(id,"output11");
		if (!shmGetHandleByTagname(ebmTagName(id,"output12"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT12)]), &errCode)) return ebmTagName(id,"output12");
		if (!shmGetHandleByTagname(ebmTagName(id,"output13"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT13)]), &errCode)) return ebmTagName(id,"output13");
		if (!shmGetHandleByTagname(ebmTagName(id,"output14"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT14)]), &errCode)) return ebmTagName(id,"output14");
		if (!shmGetHandleByTagname(ebmTagName(id,"output15"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT15)]), &errCode)) return ebmTagName(id,"output15");
		if (!shmGetHandleByTagname(ebmTagName(id,"output16"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT16)]), &errCode)) return ebmTagName(id,"output16");
		if (!shmGetHandleByTagname(ebmTagName(id,"output17"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT17)]), &errCode)) return ebmTagName(id,"output17");
		if (!shmGetHandleByTagname(ebmTagName(id,"output18"), &(ebmShmHandles[EBM_SHM(id,EBM_OUTPUT18)]), &errCode)) return ebmTagName(id,"output18");

		if (!shmGetHandleByTagname(ebmTagName(id,"ain01"), &(ebmShmHandles[EBM_SHM(id,AIN01)]), &errCode)) return ebmTagName(id,"ain01");
		if (!shmGetHandleByTagname(ebmTagName(id,"ain02"), &(ebmShmHandles[EBM_SHM(id,AIN02)]), &errCode)) return ebmTagName(id,"ain02");
		if (!shmGetHandleByTagname(ebmTagName(id,"ain03"), &(ebmShmHandles[EBM_SHM(id,AIN03)]), &errCode)) return ebmTagName(id,"ain03");

		if (!shmGetHandleByTagname(ebmTagName(id,"aout01"), &(ebmShmHandles[EBM_SHM(id,AOUT01)]), &errCode)) return ebmTagName(id,"aout01");
		if (!shmGetHandleByTagname(ebmTagName(id,"aout02"), &(ebmShmHandles[EBM_SHM(id,AOUT02)]), &errCode)) return ebmTagName(id,"aout02");
		if (!shmGetHandleByTagname(ebmTagName(id,"aout03"), &(ebmShmHandles[EBM_SHM(id,AOUT03)]), &errCode)) return ebmTagName(id,"aout03");
		if (!shmGetHandleByTagname(ebmTagName(id,"aout04"), &(ebmShmHandles[EBM_SHM(id,AOUT04)]), &errCode)) return ebmTagName(id,"aout04");

		if (!shmGetHandleByTagname(ebmTagName(id,"encoder1"), &(ebmShmHandles[EBM_SHM(id,ENCODER)]), &errCode)) return ebmTagName(id,"encoder1");
		if (!shmGetHandleByTagname(ebmTagName(id,"ip_ebm"), &(ebmShmHandles[EBM_SHM(id,IP_EBM)]), &errCode)) return ebmTagName(id,"ip_ebm");
		if (!shmGetHandleByTagname(ebmTagName(id,"interruptFifo"), &(ebmShmHandles[EBM_SHM(id,INTERRUPT_FIFO)]), &errCode)) return ebmTagName(id,"interruptFifo");
		if (!shmGetHandleByTagname(ebmTagName(id,"watchdog_time"), &(ebmShmHandles[EBM_SHM(id,WATCHDOG_TIME)]), &errCode)) return ebmTagName(id,"watchdog_time");
		if (!shmGetHandleByTagname(ebmTagName(id,"watchdog_outputs"), &(ebmShmHandles[EBM_SHM(id,WATCHDOG_OUTPUTS)]), &errCode)) return ebmTagName(id,"watchdog_outputs");

		if (!shmGetHandleByTagname(ebmTagName(id,"uart1_tx"), &(ebmShmHandles[EBM_SHM(id,UART1_TX)]), &errCode)) return ebmTagName(id,"uart1_tx");
		if (!shmGetHandleByTagname(ebmTagName(id,"uart1_rx"), &(ebmShmHandles[EBM_SHM(id,UART1_RX)]), &errCode)) return ebmTagName(id,"uart1_rx");
		if (!shmGetHandleByTagname(ebmTagName(id,"uart2_tx"), &(ebmShmHandles[EBM_SHM(id,UART2_TX)]), &errCode)) return ebmTagName(id,"uart2_tx");
		if (!shmGetHandleByTagname(ebmTagName(id,"uart2_rx"), &(ebmShmHandles[EBM_SHM(id,UART2_RX)]), &errCode)) return ebmTagName(id,"uart2_rx");
		if (!shmGetHandleByTagname(ebmTagName(id,"fwupdate_fifo"), &(ebmShmHandles[EBM_SHM(id,FWUPDATE_FIFO)]), &errCode)) return ebmTagName(id,"fwupdate_fifo");
		if (!shmGetHandleByTagname(ebmTagName(id,"fwupdate_cmd"), &(ebmShmHandles[EBM_SHM(id,FWUPDATE_CMD)]), &errCode)) return ebmTagName(id,"fwupdate_cmd");
//		if (!shmGetHandleByTagname(ebmTagName(id,"fwupdate_handshake"), &(ebmShmHandles[EBM_SHM(id,FWUPDATE_HANDSHAKE)]), &errCode)) return ebmTagName(id,"fwupdate_handshake");
		if (!shmGetHandleByTagname(ebmTagName(id,"resend_cnt"), &(ebmShmHandles[EBM_SHM(id,EBMCOMM_RESEND_CNT)]), &errCode)) return ebmTagName(id,"resend_cnt");
		if (!shmGetHandleByTagname(ebmTagName(id,"send_seqno"), &(ebmShmHandles[EBM_SHM(id,EBMCOMM_SEND_SEQNO)]), &errCode)) return ebmTagName(id,"send_seqno");
		if (!shmGetHandleByTagname(ebmTagName(id,"received_seqno"), &(ebmShmHandles[EBM_SHM(id,EBMCOMM_RECEIVED_SEQNO)]), &errCode)) return ebmTagName(id,"received_seqno");
//	}
	return NULL;
}

*/
