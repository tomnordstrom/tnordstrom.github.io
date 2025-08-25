#ifndef __DIGITAX_H
#define __DIGITAX_H

#include <stdbool.h>

#include "libmodbus/modbus.h"

typedef struct
{
   int id;
   modbus_t *mob;
   bool connected;
   bool commErrNo;
   int addressingMode;
   int tripCode;
   int motor2Selected;
} digitax_t;

typedef struct {
   int menu;
   int parameter;
   int value;
} digitaxParameter_t;

#define DRIVEMODE_OPENLOOP 1
#define DRIVEMODE_RFCA     2
#define DRIVEMODE_RFCS     3

bool digitax_Init(digitax_t *digitax, char *ip, int id);
bool digitax_Connect(digitax_t *digitax);
bool digitax_Reconnect(digitax_t *digitax);
int digitax_CalcAddress(int addressingMode, int menu, int parameter);
bool digitax_Write16(digitax_t *digitax, int menu, int parameter, uint16_t value);
bool digitax_Write32(digitax_t *digitax, int menu, int parameter, uint32_t value);
bool digitax_Read16(digitax_t *digitax, int menu, int parameter, uint16_t *value);
bool digitax_Read32(digitax_t *digitax, int menu, int parameter, uint32_t *value);
bool digitax_SetAddressingMode(digitax_t *digitax, int mode);  // 0 = standard, 1 = modified
bool digitax_CompareParams(digitax_t *digitax, digitaxParameter_t *parameter, bool *isEqual);
bool digitax_WriteParams(digitax_t *digitax, digitaxParameter_t *parameter);
bool digitax_SaveParams(digitax_t *digitax);
bool digitax_ResetDrive(digitax_t *digitax);
bool digitax_SetIP(digitax_t *digitax, unsigned char ipPart1, unsigned char ipPart2, unsigned char ipPart3, unsigned char ipPart4);
bool digitax_GetDriveMode(digitax_t *digitax, int *mode);
bool digitax_SetDriveMode(digitax_t *digitax, int mode);
bool digitax_AutoTune(digitax_t *digitax);
bool digitax_InitDrive(digitax_t *digitax, int driveMode,  digitaxParameter_t *parameter, bool *reboot_req);
bool digitax_CheckHealthy(digitax_t *digitax);
bool digitax_SetSpeed(digitax_t *digitax,int value);   // 0,1 rpm
bool digitax_SetAcc(digitax_t *digitax,int value);     // 0,001 s
bool digitax_SetDec(digitax_t *digitax,int value);     // 0,001 s
bool digitax_EnableDrive(digitax_t *digitax);
bool digitax_DisableDrive(digitax_t *digitax);
bool digitax_SetDirection(digitax_t *digitax,bool reverse);
bool digitax_Run(digitax_t *digitax);
bool digitax_Stop(digitax_t *digitax);
bool digitax_SetCurrentLimit(digitax_t *digitax, int value); // 0,1 %
bool digitax_SetTorque(digitax_t *digitax, int value);       // 0,01 %
bool digitax_SelectMotor(digitax_t *digitax, int value);     // 0 or 1
bool digitax_GetPosition(digitax_t *digitax, int *value);
bool digitax_GetFreezePosition(digitax_t *digitax, unsigned int *value);
bool digitax_SetTorqueMode(digitax_t *digitax, int mode); // 0 = speed, 2 = Speed with torque limit, 3 = Torque with speed limit
bool digitax_GetFinalCurrentRef(digitax_t *digitax, int *value);
bool digitax_GetTorqueProducingCurrent(digitax_t *digitax, int *value);
bool digitax_GetDCBusVoltage(digitax_t *digitax, int *value);
bool digitax_SetRatedVoltage(digitax_t *digitax, int value);
bool digitax_SetKpKi(digitax_t *digitax, int Kp, int Ki);

#endif
