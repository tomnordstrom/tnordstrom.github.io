#include "digitax.h"
#include "stdio.h"
#include <errno.h>
#include "lxinterface.h"

extern void debugPrint(char *fmt, ...);
extern void heartbeat_cyclic();
extern int getExecTimeLeft_us(); // execution time left in current loop in microseconds
extern void logPrint(char *fmt, ...);

bool digitax_Init(digitax_t *digitax, char *ip, int id)
{
   digitax->id = id;
   digitax->connected = false;
   digitax->commErrNo = 0;
   digitax->addressingMode = 0;
   digitax->motor2Selected = 0;
   if ((digitax->mob = modbus_new_tcp(ip, 502)) == NULL)
   {
      debugPrint("modbus_new_tcp failed, error:%s\n",modbus_strerror(errno));
      return false;
   }
   printf("Digitax id:%d, ip:%s\n", digitax->id, ip);
   return true;   
}

bool digitax_Connect(digitax_t *digitax)
{
   debugPrint("Connecting to Digitax...\n");
   if (modbus_connect(digitax->mob) == -1)
   {
      digitax->connected = false;
      digitax->commErrNo = errno;      
      debugPrint("modbus_connect failed, id:%d, error:%s\n",digitax->id, modbus_strerror(errno));
      return false;
   }
   digitax->connected = true;
   digitax->commErrNo = 0;
   debugPrint("Digitax id:%d connected\n",digitax->id);   
//struct timeval response_timeout;  
//modbus_get_byte_timeout(digitax->mob,&response_timeout);
//debugPrint("responsetimeout s:%d %lld\n",response_timeout.tv_sec,response_timeout.tv_usec);
   debugPrint("Checking addressing mode, might causing a modbus_read_register failed\n");
   uint16_t mode;
   modbus_set_slave(digitax->mob, 3);
   digitax->addressingMode = 0;
   if (digitax_Read16(digitax, 15, 13, &mode)) // Modbus Adressing register Mode
   {
      modbus_set_slave(digitax->mob, 0);   
      debugPrint("mode:%d\n",digitax->addressingMode);
      return true;
   }
   digitax->commErrNo = 0; // reset error
   digitax->connected = true;
   digitax->addressingMode = 1;
   if (digitax_Read16(digitax, 15, 13, &mode))
   {
      modbus_set_slave(digitax->mob, 0);   
      debugPrint("mode:%d\n",digitax->addressingMode);
      return true;
   }
   digitax->addressingMode = 0;
   debugPrint("mode:unknown\n");
   modbus_set_slave(digitax->mob, 0);      
   return false;
}

bool digitax_Reconnect(digitax_t *digitax)
{
   debugPrint("Reconnecting to Digitax...\n");
   modbus_close(digitax->mob);
   return digitax_Connect(digitax);
}

int digitax_CalcAddress(int addressingMode, int menu, int parameter)
{
   if (addressingMode == 0)
      return menu * 100 + parameter - 1;
   else
      return menu * 256 + parameter - 1;
}

bool digitax_Write16(digitax_t *digitax, int menu, int parameter, uint16_t value)
{
   if (!digitax->connected)
      return false;
   if (getExecTimeLeft_us() < 2500)
      wait_np(1);
   if ((modbus_write_register(digitax->mob, digitax_CalcAddress(digitax->addressingMode, menu, parameter), value)) == -1)
   {
      digitax->commErrNo = errno;
      digitax->connected = false;
      debugPrint("modbus_write_register failed, id:%d, menu:%d, param:%d, address:%d, value:%d, error:%s\n",digitax->id,menu,parameter,digitax_CalcAddress(digitax->addressingMode, menu, parameter),value,modbus_strerror(errno));
      return false;
   }
//   logPrint("MB write16: %d.%d %d\n", menu, parameter, value);
   return true;
}

bool digitax_Write32(digitax_t *digitax, int menu, int parameter, uint32_t value)
{
   if (!digitax->connected)
      return false;
   if (getExecTimeLeft_us() < 2500)
      wait_np(1);
   uint32_t swapedvalue = ((value >> 16) & 0xffff) | (value << 16);
   if ((modbus_write_registers(digitax->mob, digitax_CalcAddress(digitax->addressingMode, menu, parameter) | 0x4000, 2, (uint16_t *)&swapedvalue)) == -1)
   {
      digitax->commErrNo = errno;
      digitax->connected = false;
      debugPrint("modbus_write_registers failed, id:%d, menu:%d, param:%d, address:%d, value:%u %u, error:%s\n",digitax->id,menu,parameter,digitax_CalcAddress(digitax->addressingMode, menu, parameter) | 0x4000,swapedvalue,value,modbus_strerror(errno));
      return false;
   }
//   logPrint("MB write32: %d.%d %d\n", menu, parameter, value);
   return true;
}

bool digitax_Read16(digitax_t *digitax, int menu, int parameter, uint16_t *value)
{
   if (!digitax->connected)
      return false;
   if (getExecTimeLeft_us() < 2500)
      wait_np(1);
   if ((modbus_read_registers(digitax->mob, digitax_CalcAddress(digitax->addressingMode, menu, parameter), 1, value)) == -1)
   {
      digitax->commErrNo = errno;
      digitax->connected = false;
      debugPrint("modbus_read_register failed, id:%d, menu:%d, param:%d, address:%d, error:%s\n",digitax->id,menu,parameter,digitax_CalcAddress(digitax->addressingMode, menu, parameter),modbus_strerror(errno));
      return false;
   }
//   logPrint("MB read16: %d.%d %d\n", menu, parameter, value);
   return true;
}

bool digitax_Read32(digitax_t *digitax, int menu, int parameter, uint32_t *value)
{
   uint32_t swapedvalue;

   if (!digitax->connected)
      return false;
   if (getExecTimeLeft_us() < 2500)
      wait_np(1);
   if ((modbus_read_registers(digitax->mob, digitax_CalcAddress(digitax->addressingMode, menu, parameter) | 0x4000, 2, (uint16_t *)&swapedvalue)) == -1)
   {
      digitax->commErrNo = errno;
      digitax->connected = false;
      debugPrint("modbus_read_register failed, id:%d, menu:%d, param:%d, address:%d, error:%s\n",digitax->id,menu,parameter,digitax_CalcAddress(digitax->addressingMode, menu, parameter) | 0x4000,modbus_strerror(errno));
      return false;
   }
   *value = ((swapedvalue >> 16) & 0xffff) | (swapedvalue << 16);
//   logPrint("MB read32: %d.%d %d\n", menu, parameter, value);
   return true;
}

bool digitax_SetAddressingMode(digitax_t *digitax, int mode)  // 0 = standard, 1 = modified
{
   if ((mode == 0) || (mode == 1))
   {
      modbus_set_slave(digitax->mob, 3);
      if (digitax_Write16(digitax, 15, 13, mode))
      {
         digitax->addressingMode = mode;
         modbus_set_slave(digitax->mob, 0);
         return true;
      }
   }
   modbus_set_slave(digitax->mob, 0);
   return false;
}

bool digitax_CompareParams(digitax_t *digitax, digitaxParameter_t *parameter, bool *isEqual)
{
   uint32_t readValue;
   
   *isEqual = false;
   while (!((parameter->menu == 0) && (parameter->parameter == 0) && (parameter->value == 0)))
   {
      if (!digitax_Read32(digitax, parameter->menu, parameter->parameter, &readValue))
         return false;
      if ((uint32_t)parameter->value != readValue)
      {
         debugPrint("Parameter %d.%d in drive (%d) is not equal to (%d)\n",parameter->menu,parameter->parameter,readValue,parameter->value);
         return true; // means no errors on reading the parameters
      }
      ++parameter;
//      wait_np(1);
   }
   *isEqual = true;
   return true;
}

bool digitax_WriteParams(digitax_t *digitax, digitaxParameter_t *parameter)
{
   while (!((parameter->menu == 0) && (parameter->parameter == 0) && (parameter->value == 0)))
   {
      debugPrint("Writing to drive:%d parameter: %d.%03d value:%d  ",digitax->id, parameter->menu, parameter->parameter, parameter->value);
      if (!digitax_Write32(digitax, parameter->menu, parameter->parameter, parameter->value))
      {
         debugPrint("Failed\n");
         return false;
      }
      debugPrint("Succeeded\n");
      ++parameter;
//      wait_np(1);
   }
   return true;   
}

bool digitax_SaveParams(digitax_t *digitax)
{
   if (!digitax_Write16(digitax, 11, 0, 1001))  // Save drive parameters to non-volatile memory
      return false;
   return digitax_ResetDrive(digitax);   
}

bool digitax_ResetDrive(digitax_t *digitax)
{
   if (!digitax_Write16(digitax, 10, 38, 100))
      return false;

   debugPrint("Drive reset id:%d  ", digitax->id);
   
   digitax->commErrNo = 0;
   uint16_t param;
//   int retVal = 0;
   long long timeoutTime = gethrtime() + 3000000000ll; // 3s
   while (gethrtime() < timeoutTime)
   {
//      debugPrint("time before read: %lld retVal: %d errno: %d\n",gethrtime() / 1000000ll, retVal, digitax->commErrNo);
      digitax_Read16(digitax, 11, 0, &param);
//      debugPrint("time after read: %lld retVal: %d errno: %d\n\n",gethrtime() / 1000000ll, retVal, digitax->commErrNo);
      digitax->commErrNo = 0;
      if (param == 0)
      {
         debugPrint("Succeeded\n");
         return true;
      }
      wait_np(1);
   }
   debugPrint("Failed\n");
   return false;      
}

bool digitax_SetIP(digitax_t *digitax, unsigned char ipPart1, unsigned char ipPart2, unsigned char ipPart3, unsigned char ipPart4)
{
   unsigned int ip = (ipPart1 << 24) | (ipPart2 << 16) | (ipPart3 << 8) | ipPart4;
   modbus_set_slave(digitax->mob, 3);

   if (!digitax_Write16(digitax, 2, 5, 0)) // disable DHCP
   {
      modbus_set_slave(digitax->mob, 0);      
      return false;
   }
   if (!digitax_Write32(digitax, 2, 6, ip)) // ip address
   {
      modbus_set_slave(digitax->mob, 0);      
      return false;
   }
   ip = (255 << 24) | (255 << 16) | (255 << 8);
   digitax_Write32(digitax, 2, 7, ip); // netmask
   ip = (ipPart1 << 24) | (ipPart2 << 16) | (ipPart3 << 8) | 1;
   digitax_Write32(digitax, 2, 8, ip); // gateway

   modbus_set_slave(digitax->mob, 0);      
   return true;
}

bool digitax_GetDriveMode(digitax_t *digitax, int *mode)
{
   *mode = 0; // reset high word;
   return digitax_Read16(digitax, 11, 84, (uint16_t *)mode);
}

bool digitax_SetDriveMode(digitax_t *digitax, int mode)
{
   if (!((mode >= 1) && (mode <= 4)))
      return false;
   if (!digitax_Write16(digitax, 11, 0, 1253))  // Change drive mode and load 50Hz defaults
      return false;
   if (!digitax_Write16(digitax, 11, 31, mode))
      return false;
   return digitax_ResetDrive(digitax);
}

bool digitax_AutoTune(digitax_t *digitax)
{
   int driveActive = 0;
   uint16_t param;
   
   debugPrint("Autotune started\n");
   if (!digitax_Read16(digitax,10,2,(uint16_t *)(&driveActive)))
      return false;
   if (driveActive == 0)
   {
      debugPrint("Autotune aborted, drive not active\n");      
      return false;
   }
   if (!digitax_DisableDrive(digitax))
      return false;
   wait_np(50);
   if (!digitax_Write16(digitax, 5, 12, 5)) // autotune 1=stationary
      return false;
   wait_np(50);
   if (!digitax_EnableDrive(digitax))
      return false;
   long long timeoutTime = gethrtime() + 60000000000ll; // 60s
   while (gethrtime() < timeoutTime)
   {
//      debugPrint("time before read: %lld retVal: %d errno: %d\n",gethrtime() / 1000000ll, retVal, digitax->commErrNo);
      if (!digitax_Read16(digitax, 5, 12, &param))
         return false;
//      debugPrint("time after read: %lld retVal: %d errno: %d\n\n",gethrtime() / 1000000ll, retVal, digitax->commErrNo);
      if (param == 0)
      {
         wait_np(50);
         digitax_DisableDrive(digitax);
         wait_np(50);
         heartbeat_cyclic();      
         digitax_EnableDrive(digitax);         
         wait_np(50);
         if (!digitax_CheckHealthy(digitax))
         {
            debugPrint("Autotune failed tripcode:%d\n",digitax->tripCode);
            return false;
         }
         debugPrint("Autotune succeeded\n");
         return true;
      }
      heartbeat_cyclic();      
      wait_np(1);
   }
   debugPrint("Autotune timeout\n");
   return false;
}

bool digitax_InitDrive(digitax_t *digitax, int driveMode,  digitaxParameter_t *parameter, bool *reboot_req)
{
   *reboot_req = false;
   if (!digitax_DisableDrive(digitax))
      return false;

   if (!digitax_SetAddressingMode(digitax, 1)) // Modified mode
      return false;
   int currentDriveMode;
   if (!digitax_GetDriveMode(digitax, &currentDriveMode))
      return false;
   debugPrint("current drive mode:%d\n",currentDriveMode);
   if (currentDriveMode != driveMode)
   {
      if (!digitax_SetDriveMode(digitax, driveMode))
         return false;
      debugPrint("drive mode programmed\n");
      *reboot_req = true;
      return true;
   }
   bool isEqual = false;
   if (!digitax_CompareParams(digitax, parameter, &isEqual))
      return false;
   if (!isEqual)
   {
      if (!digitax_WriteParams(digitax, parameter))
         return false;
//      if (!digitax_SetIP(&digitax, 192,168,1,16))
//         return false;
      if (!digitax_SaveParams(digitax))
         return false;
   }
   return true;
}

bool digitax_CheckHealthy(digitax_t *digitax)
{
   uint16_t driveHealthy;
   if (!digitax_Read16(digitax,10,1,&driveHealthy))
   {
      digitax->tripCode = 0;
      return false;
   }
   if (!driveHealthy)
   {
      digitax->tripCode = 0; // reset high word;      
      if (!digitax_Read16(digitax,10,20,(uint16_t *)(&digitax->tripCode)))
         return false;
//      debugPrint("TripCode:%d\n",digitax->tripCode);
   }
   else
      digitax->tripCode = 0;
   return true;
}

bool digitax_SetSpeed(digitax_t *digitax,int value) // 0,1 rpm
{
//      debugPrint("set speed:%d\n",value);
   return digitax_Write32(digitax, 1, 21, (uint32_t)value); // 0x4078
}

bool digitax_SetAcc(digitax_t *digitax,int value)   // 0,001 s
{
   if (digitax->motor2Selected)
      return digitax_Write32(digitax, 21, 04, (uint32_t)value);  // 0x4837
   else 
      return digitax_Write32(digitax, 2, 11, (uint32_t)value); // 0x40d2
}

bool digitax_SetDec(digitax_t *digitax, int value)   // 0,001 s
{
   if (digitax->motor2Selected)
      return digitax_Write32(digitax, 21, 05, (uint32_t)value);//0x4838
   else
      return digitax_Write32(digitax, 2, 21, (uint32_t)value);//0x40dc
}

bool digitax_EnableDrive(digitax_t *digitax)
{
   return digitax_Write16(digitax, 6, 15, 1); //0x266
}

bool digitax_DisableDrive(digitax_t *digitax)
{
   return digitax_Write16(digitax, 6, 15, 0);
}

bool digitax_SetDirection(digitax_t *digitax,bool reverse)
{
   return digitax_Write16(digitax, 6, 33, reverse); //0x278
}

bool digitax_Run(digitax_t *digitax)
{
   return digitax_Write16(digitax, 6, 34, 1);//0x279
}

bool digitax_Stop(digitax_t *digitax)
{
   return digitax_Write16(digitax, 6, 34, 0);
}

bool digitax_SetCurrentLimit(digitax_t *digitax, int value) // 0,1 %
{
//   debugPrint("set current limit:%d\n",value);
   return digitax_Write16(digitax, 4, 7, (uint16_t)value);//0x196
}

bool digitax_SetTorque(digitax_t *digitax, int value)        // 0,01 %
{
//   debugPrint("set torque ref: %d 1/10%%\n",value);
   return digitax_Write16(digitax, 4, 8, (uint16_t)value); //0x0197
}

bool digitax_SelectMotor(digitax_t *digitax, int value)     // 0 or 1
{
   if ((value == 0) || (value == 1))
   {
      digitax->motor2Selected = value;
      return digitax_Write16(digitax, 11, 45, (uint16_t)value);//0x0478
   }
   return false;
}

bool digitax_GetPosition(digitax_t *digitax, int *value)
{
   return digitax_Read32(digitax, 3, 58, (uint32_t *)value);//0x4165
}

bool digitax_GetFreezePosition(digitax_t *digitax, unsigned int *value)
{
   return digitax_Read32(digitax, 3, 103, value);
}

bool digitax_SetTorqueMode(digitax_t *digitax, int mode) // 0 = speed, 2 = Speed with torque limit, 3 = Torque with speed limit
{
   return digitax_Write16(digitax, 4, 11, (uint16_t)mode);
}

bool digitax_GetFinalCurrentRef(digitax_t *digitax, int *value)
{
   return digitax_Read16(digitax, 4, 4, (uint16_t *)value);
}

bool digitax_GetTorqueProducingCurrent(digitax_t *digitax, int *value)
{
   return digitax_Read32(digitax, 4, 2, (uint32_t *)value);
}

bool digitax_GetDCBusVoltage(digitax_t *digitax, int *value)
{
   return digitax_Read16(digitax, 5, 5, (uint16_t *)value);
}

bool digitax_SetRatedVoltage(digitax_t *digitax, int value)
{
   return digitax_Write16(digitax, 5, 9, (uint16_t)value);
}

bool digitax_SetKpKi(digitax_t *digitax, int Kp, int Ki)
{
   if (digitax_Write16(digitax, 4, 13, (uint16_t)Kp))
      if (digitax_Write16(digitax, 4, 14, (uint16_t)Ki))
         return true;
   return false;   
}

