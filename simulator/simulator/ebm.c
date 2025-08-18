#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ebm.h"
#include "lxinterface.h"

ebm_t ebm[MAX_NO_OF_EBM];

void (*cbEbmSetGuiUpdating)(void) = NULL;
void (*cbEbmClearGuiUpdating)(void) = NULL;
void (*cbEbmSetGuiUpdateProgress)(int progress) = NULL;
void (*cbEbmUpdateVersionInfo)(void) = NULL;
void (*cbEbmInitHardware)(int ebmId) = NULL;

int ebmFwUpdate(int ebmId);
int ebmFwBurnBootloader(int ebmId);
int ebmFwBurnApplication(int ebmId);
int ebmJumpToApp(int ebmId);

int systemRequestPar(int request, int par);
int systemRequest(int request);

void ebmInit(int ebmId)
{
	ebm[ebmId].state = EBMSTATE_NOT_CONNECTED;
	ebm[ebmId].connectEnable = true;
	ebm[ebmId].updateFailed = false;
	ebm[ebmId].updateErrorCode = 0;
	ebm[ebmId].connectionFailed = false;
	ebm[ebmId].ready = false;
}
							
void ebmControl(int ebmId)
{
	static int updateResult;
	static bool firstConnection = true;
	static hrtime_t connectBeginTime;
	
	switch (ebm[ebmId].state)
	{
		case EBMSTATE_NOT_CONNECTED:
			if (ebm[ebmId].connectEnable)
			{	
				ebmUartClear(ebmId,1);         // clear uart1
				ebmUartClear(ebmId,2);         // clear uart2
				connectBeginTime = gethrtime();
				ebm[ebmId].state = EBMSTATE_CONNECTING;
				printf("ebm%d ebmState CONNECTING\n",ebmId);
			}
		break;
		case EBMSTATE_CONNECTING:
			if (ebmIsConnected(ebmId))
			{
				printf("ebm%d ebm connection established\n",ebmId);
				firstConnection = false;
				ebmSetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE,0); // Clear before connecting. Old ebm firmware will not set it.
				ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,0);	// toggle subscribe to force a change
				ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,1);
				wait_np(5);	// wait for shared mem to be updated
				printf("HW_Type: %d\n",ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE));
				ebm[ebmId].state = EBMSTATE_UPDATING;
			}
			else
			{
				if (firstConnection)
				{
					if (gethrtime() - connectBeginTime > 10000000000ll)		// try 10s before triggering an error
					{
						printf("ebm%d  set ebm connection failed\n",ebmId);
						ebm[ebmId].connectionFailed = true;
						ebm[ebmId].connectEnable = false;				
						ebm[ebmId].state = EBMSTATE_NOT_CONNECTED;
						printf("ebm%d ebmState NOT CONNECTED\n", ebmId);
						firstConnection = false;
					}
				}
				ebmConnect(ebmId);
			}
		break;
		case EBMSTATE_UPDATING:
			if (ebmIsConnected(ebmId))
			{
				printf("ebm%d ebmState: UPDATING\n",ebmId);
				cbEbmSetGuiUpdateProgress(0);
				cbEbmSetGuiUpdating();
				cbEbmUpdateVersionInfo();
				if ((updateResult = ebmFwUpdate(ebmId)) == 1)
				{
					cbEbmClearGuiUpdating();
					if (ebmGetBit(ebmId,EBMMEM_BIT_BOOTLOADER) && (ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION) > 0)) // if in bootloader and valid application, jump to it
					{
						if ((ebmJumpToApp(ebmId)) != 1)
						{
							ebm[ebmId].connectionFailed = true;
							ebm[ebmId].connectEnable = false;				
							ebm[ebmId].state = EBMSTATE_NOT_CONNECTED;
							printf("ebm%d ebmState NOT CONNECTED\n", ebmId);
							break;
						}
					}
					ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_ALL,0);	// toggle subscribe to force a change
					ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_ALL,1);
					wait_np(5);	// wait for shared mem to be updated
					cbEbmUpdateVersionInfo();
					cbEbmInitHardware(ebmId);
					ebm[ebmId].state = EBMSTATE_READY;
					ebm[ebmId].ready = true;					
					printf("ebm%d ebmState: READY\n",ebmId);
				}
				else
				{
					cbEbmClearGuiUpdating();
					ebm[ebmId].updateErrorCode = abs(updateResult);
					ebm[ebmId].updateFailed = true;
					ebm[ebmId].state = EBMSTATE_UPDATE_FAILED;
					printf("ebm%d ebmState: UPDATE READY\n",ebmId);
				}
			}
			else
			{
				ebm[ebmId].connectionFailed = true;
				ebm[ebmId].connectEnable = false;				
				ebm[ebmId].state = EBMSTATE_NOT_CONNECTED;
				printf("ebm%d ebmState NOT CONNECTED\n", ebmId);
			}
		break;
		case EBMSTATE_UPDATE_FAILED:
			if (!ebmIsConnected(ebmId))
			{
				ebm[ebmId].connectionFailed = true;
				ebm[ebmId].connectEnable = false;				
				ebm[ebmId].state = EBMSTATE_NOT_CONNECTED;
				printf("ebm%d ebmState NOT CONNECTED\n",ebmId);
			}
		break;
		case EBMSTATE_READY:
			if (!ebmIsConnected(ebmId))
			{
				ebm[ebmId].connectionFailed = true;
				ebm[ebmId].ready = false;
				ebm[ebmId].connectEnable = false;				
				ebm[ebmId].state = EBMSTATE_NOT_CONNECTED;
				printf("ebm%d ebmState NOT CONNECTED\n",ebmId);
			}
		break;		
	}
}

int ebmFwUpdate(int ebmId)
{
	static char fileAppName[20] = {0},fileBootName[20] = {0};
	static int fileAppVersion, fileBootVersion;
	bool updateBoot = false;
	bool updateApp = false;
	char str;
	int returnValue;
	int sysCmd;
	
	if (ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE) == 1)   // 0 = EBM-CPU 196pin, 1 = EBM-CPU144 144pin
	{
		printf("ebm%d Hardware Type:1 (EBM-CPU144 144pin)\n", ebmId);
		if ((fileBootVersion = systemRequestPar(RTREQUEST_FW_GETFILEINFO_BOOTLOADER_144,ebmId)) > 0)
			strcpy(fileBootName,shmReadAsStringByTagname("sys_request_string",&str));
		if ((fileAppVersion = systemRequestPar(RTREQUEST_FW_GETFILEINFO_APPLICATION_144,ebmId)) > 0)
			strcpy(fileAppName,shmReadAsStringByTagname("sys_request_string",&str));
	}
	else
	{
		printf("ebm%d Hardware Type:0 (EBM-CPU 196pin)\n", ebmId);
		if ((fileBootVersion = systemRequestPar(RTREQUEST_FW_GETFILEINFO_BOOTLOADER_196,ebmId)) > 0)
			strcpy(fileBootName,shmReadAsStringByTagname("sys_request_string",&str));
		if ((fileAppVersion = systemRequestPar(RTREQUEST_FW_GETFILEINFO_APPLICATION_196,ebmId)) > 0)
			strcpy(fileAppName,shmReadAsStringByTagname("sys_request_string",&str));
	}
	printf("ebm%d --file bootname:%s version:%d\n", ebmId, fileBootName, fileBootVersion);
	printf("ebm%d --file appname:%s version:%d\n", ebmId, fileAppName, fileAppVersion);
	
	printf("ebm%d --current bootname:%s version:%d\n", ebmId, ebmGetString(ebmId,EBMMEM_STRING_BOOT_NAME), ebmGetUInt32(ebmId,EBMMEM_UINT32_BOOT_VERSION));
	printf("ebm%d --current appname:%s version:%d\n", ebmId, ebmGetString(ebmId,EBMMEM_STRING_APP_NAME), ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION));
	if ((fileBootVersion > 0) &&  // if boot file exists
		 ((ebmGetUInt32(ebmId,EBMMEM_UINT32_BOOT_VERSION) != fileBootVersion) || (strncmp(ebmGetString(ebmId,EBMMEM_STRING_BOOT_NAME), fileBootName, 20) != 0)))  	// if different versions
		updateBoot = true;
	if ((fileAppVersion > 0) &&	// if app file exists
		((ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION) != fileAppVersion) || (strncmp(ebmGetString(ebmId,EBMMEM_STRING_APP_NAME), fileAppName, 20) != 0)))
		 updateApp = true;

	if ((ebmGetUInt32(ebmId,EBMMEM_UINT32_BOOT_VERSION) == (ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION)) && 			
		 (strncmp(ebmGetString(ebmId,EBMMEM_STRING_BOOT_NAME), ebmGetString(ebmId,EBMMEM_STRING_APP_NAME), 20) == 0)))		// if boot and app are the same means that bootloader is not present
	{
		printf("ebm%d Bootloader is not present in ebm\n", ebmId);
		return 1;
	}
	if (ebmGetBit(ebmId,EBMMEM_BIT_BOOTLOADER) && ((ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION) == 0)) && !updateApp)  // No application in ebm-card and no file to update with
	{
		printf("ebm%d No application in ebm card\n", ebmId);
		return -1;
	}

	if (ebmGetBit(ebmId,EBMMEM_BIT_BOOTLOADER))
	{
		if (updateApp)
		{
			if (ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE) == 1)
				sysCmd = RTREQUEST_FW_DOWNLOAD_APPLICATION_144;
			else
				sysCmd = RTREQUEST_FW_DOWNLOAD_APPLICATION_196;
			if ((returnValue = systemRequestPar(sysCmd,ebmId)) != 1)  // Download application
				return returnValue - 60;
			if ((returnValue = ebmFwBurnApplication(ebmId)) != 1)			// Burn application
				return returnValue - 80;
			if ((ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION) != fileAppVersion) || (strncmp(ebmGetString(ebmId,EBMMEM_STRING_APP_NAME), fileAppName, 20) != 0))
			{
				printf("ebm%d Updated application doesn't match file ???\n", ebmId);
				return -2;
			}
		}
		if (updateBoot && (ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION) > 0)) // if update req. and application exists in ebm
		{
			if (ebmGetBit(ebmId,EBMMEM_BIT_BOOTLOADER))  // if in bootloader, jump to application
			{
				if ((returnValue = ebmJumpToApp(ebmId)) != 1)
					return (10 + returnValue);
			}
			if (ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE) == 1)
				sysCmd = RTREQUEST_FW_DOWNLOAD_BOOTLOADER_144;
			else
				sysCmd = RTREQUEST_FW_DOWNLOAD_BOOTLOADER_196;
			if ((returnValue = systemRequestPar(sysCmd,ebmId)) != 1)  // Download bootloader
				return returnValue - 20;
			if ((returnValue = ebmFwBurnBootloader(ebmId)) != 1)			// Burn bootloader
				return returnValue - 40;
		}
	}
	else
	{
		if (updateBoot)
		{
			if (ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE) == 1)
				sysCmd = RTREQUEST_FW_DOWNLOAD_BOOTLOADER_144;
			else
				sysCmd = RTREQUEST_FW_DOWNLOAD_BOOTLOADER_196;         
			if ((returnValue = systemRequestPar(sysCmd,ebmId)) != 1)  // Download bootloader
				return returnValue - 20;
			if ((returnValue = ebmFwBurnBootloader(ebmId)) != 1)			// Burn bootloader
				return returnValue - 40;
		}
		if (updateApp)
		{
			if (ebmGetUInt32(0,EBMMEM_UINT32_HW_CPU_TYPE) == 1)
				sysCmd = RTREQUEST_FW_DOWNLOAD_APPLICATION_144;
			else
				sysCmd = RTREQUEST_FW_DOWNLOAD_APPLICATION_196;         
			if ((returnValue = systemRequestPar(sysCmd,ebmId)) != 1)  // Download application
				return returnValue - 60;
			if ((returnValue = ebmFwBurnApplication(ebmId)) != 1)			// Burn application
				return returnValue - 80;
			if ((ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION) != fileAppVersion) || (strncmp(ebmGetString(ebmId,EBMMEM_STRING_APP_NAME), fileAppName, 20) != 0))
			{
				printf("ebm%d Updated application doesn't match file ???\n",ebmId);
				return -2;
			}
		}
	}
	printf("ebm%d update finished\n",ebmId);
	return 1;
}

int ebmFwBurnBootloader(int ebmId)
{
	int timeoutCnt;

	printf("Start burning bootloader\n");
	ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_BURN_BOOTLOADER);
	timeoutCnt = 500;	//0.5s
	while (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) != FWUPDATE_RESPONSE_BURNING)	// wait for start burning response
	{
		if (--timeoutCnt == 0)
		{
			printf("Timeout waiting for ebm start burning\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			return -1;
		}
		wait_np(1);
	}
	printf("Burning response received\n");
	timeoutCnt = 30000;	// 30s
	while (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) == FWUPDATE_RESPONSE_BURNING)
	{
		if (!ebmIsConnected(ebmId))
		{
			int connectCnt = 10000;
			while (--connectCnt > 0)
			{
				ebmConnect(ebmId);
				wait_np(1);
				if (ebmIsConnected(ebmId))
				{
					printf("ebm conncted\n");
					ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,0);	// toggle subscribe to force a change
					ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,1);
					wait_np(5);	// wait for shared mem to be updated
					break;
				}
			}
			if (connectCnt == 0)
			{
				printf("Conncting to ebm failed\n");
				ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
				return -2;
			}					
		}
		if (--timeoutCnt == 0)
		{
			printf("Timeout waiting for ebm burning result\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			return -3;
		}
		wait_np(1);
	}
	if (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) != FWUPDATE_RESPONSE_BURN_OK)
	{
		printf("Burning bootloader failed, response=%d\n",ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE));
		ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
		return -4;
	}
	printf("Burning bootloader succeeded\n");
	return 1;
}

int ebmFwBurnApplication(int ebmId)
{
	int timeoutCnt;
	int returnValue;

	printf("Start burning application\n");
	ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_BURN_APPLICATION);
	timeoutCnt = 500;	//0.5s
	while (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) != FWUPDATE_RESPONSE_BURNING)	// wait for start burning response
	{
		if (--timeoutCnt == 0)
		{
			printf("Timeout waiting for ebm start burning\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			return -1;
		}
		wait_np(1);
	}
	printf("Burning response received\n");
	timeoutCnt = 30000;	// 30s
	while (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) == FWUPDATE_RESPONSE_BURNING)
	{
		if (!ebmIsConnected(ebmId))
		{
			int connectCnt = 10000;
			while (--connectCnt > 0)
			{
				ebmConnect(ebmId);
				wait_np(1);
				if (ebmIsConnected(ebmId))
				{
					printf("ebm conncted\n");
					ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,0);	// toggle subscribe to force a change
					ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,1);
					wait_np(5);	// wait for shared mem to be updated
					break;
				}
			}
			if (connectCnt == 0)
			{
				printf("Conncting to ebm failed\n");
				ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
				return -2;
			}					
		}
		if (--timeoutCnt == 0)
		{
			printf("Timeout waiting for ebm burning result\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			return -3;
		}
		cbEbmSetGuiUpdateProgress(ebmGetUInt8(0,EBMMEM_UINT8_FWUPDATE_PROGRESS));
		wait_np(1);
	}
	if (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) != FWUPDATE_RESPONSE_BURN_OK)
	{
		printf("Burning application failed, response=%d\n",ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE));
		ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
		return -4;
	}
	printf("Burning application succeeded\n");
	if ((returnValue = ebmJumpToApp(ebmId)) != 1)
		return -4 + returnValue;
	return 1;
}

int ebmJumpToApp(int ebmId)
{
	printf("Jump to app started\n");
	ebmSetUInt8(ebmId, EBMMEM_UINT8_BOOT_STATE, 1);	// boot directly application
	ebmSetBit(ebmId, EBMMEM_BIT_REBOOT, 1);
	wait_np(100);
	int connectCnt = 5000;
	while (--connectCnt > 0)
	{
		ebmConnect(ebmId);
		wait_np(1);
		if (ebmIsConnected(ebmId))
		{
			printf("ebm conncted\n");
			ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,0);	// toggle subscribe to force a change
			ebmSetBit(ebmId,EBMMEM_BIT_SUBSCRIBE_FWUPDATE,1);
			wait_np(5);	// wait for shared mem to be updated
			printf("bootloader:%d appver:%d\n",ebmGetBit(ebmId,EBMMEM_BIT_BOOTLOADER),ebmGetUInt32(ebmId,EBMMEM_UINT32_APP_VERSION));
			break;
		}
	}
	if (!ebmIsConnected(ebmId))
	{
		printf("Conncting to ebm failed\n");
		ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
		return -1;
	}
	if (ebmGetBit(ebmId,EBMMEM_BIT_BOOTLOADER))
	{
		printf("Jump to application failed, still connected to bootloader\n");
		return -2;
	}
	printf("Jump to app succeeded\n");

	return 1;
}

int systemRequestPar(int request, int par)
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

int systemRequest(int request)
{
   return systemRequestPar(request, 0);
}
