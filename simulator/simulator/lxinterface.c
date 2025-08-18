#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <dirent.h>
#include "lxinterface.h"
#include "ebmMemDefs.h"

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

volatile shmem_t *shared_Mem;

bool IntEnvActive = false;
InputInt_t *IntEnvInputInt;

void IntEnv_Activate(InputInt_t *pInputInt)
{
	IntEnvInputInt = pInputInt;
	IntEnvActive = true;
}

void IntEnv_Deactivate(void)
{
	IntEnvActive = false;
	IntEnvInputInt = NULL;
}

hrtime_t gethrtime(void)
{
	struct timespec tp;
	
	if (IntEnvActive)
	{
//		printf("gethrtime %llu\n",IntEnvInputInt->time * 1000);
		return IntEnvInputInt->time * 1000;
	}
	else
	{
		clock_gettime(CLOCK_MONOTONIC, &tp);
//		printf("gethrtime: %ld %ld\n",tp.tv_sec,tp.tv_nsec);
		return((long long)tp.tv_sec*1000000000L+(long long)tp.tv_nsec);
	}
}

void wait_np(int ticks)
{
	lxthread_WaitNp(ticks);
}

void BeepBop(int xxx)
{
}

void SetAnalog(int EbmId, int Num, int Val)
{
   if (Val > 255)
      Val = 255;
   else if (Val < -256)
      Val = -256;
   ebmSetAnalogOut(EbmId, Num+1, (float)Val / 25.5);
}

void SetRawAnalog(int EbmId, int Num, int Val)
{
   ebmSetAnalogOut(EbmId, Num+1, (float)Val / 51.2 - 10.0);
}

int GetAnalog(int EbmId, int Num)
{
	return (short)(25.5 * ebmGetAnalogIn(EbmId, Num+1));   
}

int GetRawAnalog(int EbmId, int Num)
{
	return (short)(512.0 + 51.1 * ebmGetAnalogIn(EbmId, Num+1));
}

int modbusSend(unsigned int fp, void *buff, int cnt)
{
	return ebmUartSend(0, 1, buff, cnt);
}

int modbusReceive(unsigned int fp, void *buff, int cnt)
{
	return ebmUartReceive(0, 1, buff, cnt);
}

void modbusWait(unsigned int ms)
{
   if (ms < 10)
      ms = 10; // make sure that we wait atleast once

	wait_np(ms / 10);
}

// ---------------------------------------------------------------
// System tasks

int hShmInt_sys_request;
int hShmInt_sys_request_param;
int hShmInt_sys_request_response;
int hShmInt_sys_request_ip;
int hShmInt_sys_request_netmask;
int hShmInt_sys_request_gateway;


char *populateSysShmHandles(void)  // returns NULL if ok. returns tagname if not found in shm
{
	int errCode;
	if (!shmGetHandleByTagname("sys_request", &hShmInt_sys_request, &errCode)) return "sys_request";
	if (!shmGetHandleByTagname("sys_request_param", &hShmInt_sys_request_param, &errCode)) return "sys_request_param";
	if (!shmGetHandleByTagname("sys_request_response", &hShmInt_sys_request_response, &errCode)) return "sys_request_response";
	if (!shmGetHandleByTagname("sys_request_ip", &hShmInt_sys_request_ip, &errCode)) return "sys_request_ip";
	if (!shmGetHandleByTagname("sys_request_netmask", &hShmInt_sys_request_netmask, &errCode)) return "sys_request_netmask";
	if (!shmGetHandleByTagname("sys_request_gateway", &hShmInt_sys_request_gateway, &errCode)) return "sys_request_gateway";
	return NULL;
}

bool systemLoopInit(void)
{
	char *errTag;

	if ((errTag = populateSysShmHandles()) != NULL)
	{
		printf("Parameter %s is not found in shared memory\n",errTag);
		return false;
	}

	if (pthread_create(&threadSystemLoop, NULL, systemLoop, NULL) == 0)
		return true;
	else
		return false;
}

void *systemLoop(void *t)
{
	while (true)
	{
		usleep(1000);
		switch (shmReadIntByHandle(hShmInt_sys_request))
		{
			case 0:  // no request
			break;
			case RTREQUEST_LOADCFG:
				shmWriteIntByHandle(hShmInt_sys_request_response, loadConfig(shmReadIntByHandle(hShmInt_sys_request_param)));
				printf("loadcfg: %d\n",shmReadIntByHandle(hShmInt_sys_request_param));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_SAVECFG:
				saveConfig(shmReadIntByHandle(hShmInt_sys_request_param)); 
				printf("savecfg\n");
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_LOADSYS:
				loadSys(); 
				printf("loadsys\n");
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_SAVESYS:
				saveSys(); 
				printf("savesys\n");
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_LOADCMOS:
				shmWriteIntByHandle(hShmInt_sys_request_response, loadCmos());
				printf("loadcmos: %d\n",shmReadIntByHandle(hShmInt_sys_request_response));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_SAVECMOS:
				shmWriteIntByHandle(hShmInt_sys_request_response, saveCmos());
				printf("savecmos\n");
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_SETLANGUAGE:
				if (setLanguage(shmReadIntByHandle(hShmInt_sys_request_param)))
					shmWriteIntByHandle(hShmInt_sys_request_response, RTRESPONS_OK);
				else
					shmWriteIntByHandle(hShmInt_sys_request_response, RTRESPONS_LANGUAGE_ERROR);					
				printf("setlanguage to %d\n\n",shmReadIntByHandle(hShmInt_sys_request_param));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_GETFILEINFO_BOOTLOADER_196:
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwGetFileInfo(shmReadIntByHandle(hShmInt_sys_request_param),0,0));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_GETFILEINFO_APPLICATION_196:
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwGetFileInfo(shmReadIntByHandle(hShmInt_sys_request_param),1,0));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_GETFILEINFO_BOOTLOADER_144:
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwGetFileInfo(shmReadIntByHandle(hShmInt_sys_request_param),0,1));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_GETFILEINFO_APPLICATION_144:
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwGetFileInfo(shmReadIntByHandle(hShmInt_sys_request_param),1,1));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_DOWNLOAD_BOOTLOADER_196:
				printf("EBM bootloader download started\n\n");
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwDownload(shmReadIntByHandle(hShmInt_sys_request_param),0,0));
				printf("EBM bootloader download finished with code %d\n\n",shmReadIntByHandle(hShmInt_sys_request_response));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_DOWNLOAD_APPLICATION_196:
				printf("EBM application download started\n\n");
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwDownload(shmReadIntByHandle(hShmInt_sys_request_param),1,0));
				printf("EBM application download finished with code %d\n\n",shmReadIntByHandle(hShmInt_sys_request_response));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_DOWNLOAD_BOOTLOADER_144:
				printf("EBM bootloader download started\n\n");
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwDownload(shmReadIntByHandle(hShmInt_sys_request_param),0,1));
				printf("EBM bootloader download finished with code %d\n\n",shmReadIntByHandle(hShmInt_sys_request_response));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_FW_DOWNLOAD_APPLICATION_144:
				printf("EBM application download started\n\n");
				shmWriteIntByHandle(hShmInt_sys_request_response, ebmFwDownload(shmReadIntByHandle(hShmInt_sys_request_param),1,1));
				printf("EBM application download finished with code %d\n\n",shmReadIntByHandle(hShmInt_sys_request_response));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_READ_MACHINE_VERSION:
				shmWriteIntByHandle(hShmInt_sys_request_response, readVersionFile("machine_version"));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_READ_SYSTEM_VERSION:
				shmWriteIntByHandle(hShmInt_sys_request_response, readVersionFile("/startscripts/system_version"));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_READ_BOOT_VERSION:
				shmWriteIntByHandle(hShmInt_sys_request_response, readVersionFile("/startscripts/boot_version"));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			case RTREQUEST_SET_NETWORK:
				setNetwork(shmReadIntByHandle(hShmInt_sys_request_param),shmReadIntByHandle(hShmInt_sys_request_ip),shmReadIntByHandle(hShmInt_sys_request_netmask),shmReadIntByHandle(hShmInt_sys_request_gateway));
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
			default:
				printf("Non supported sys request: %d\n",shmReadIntByHandle(hShmInt_sys_request));
				shmWriteIntByHandle(hShmInt_sys_request_response,0);
				shmWriteIntByHandle(hShmInt_sys_request,0);
			break;
		}
	}
}

int loadConfig(int configNumber)
{
   char cmdstring[100];
   
   if(configNumber == 0)
      sprintf(cmdstring,"./shmUtil -load `cat /home/pi/machine/configfile`\n");
   else
      sprintf(cmdstring,"./shmUtil -load `cat /home/pi/machine/configfile`_%d\n", configNumber);
      
   printf(cmdstring);
   int returnValue = system(cmdstring);
	if (returnValue != 0)
		return returnValue;
   return system("./shmUtil -load `cat /home/pi/machine/configfile`_common");
}

void saveConfig(int configNumber)
{
   char cmdstring[100];
   
   if(configNumber == 0)
      sprintf(cmdstring,"./shmUtil -store -1 > `cat /home/pi/machine/configfile`\n");
   else
      sprintf(cmdstring,"./shmUtil -store -1 > `cat /home/pi/machine/configfile`_%d\n", configNumber);
      
   printf(cmdstring);
   system(cmdstring); 
   system("./shmUtil -store -2 > `cat /home/pi/machine/configfile`_common"); 
}
	
int loadCmos(void)
{
   return system("./shmUtil -load `cat /home/pi/machine/cmosfile`"); 
}

int saveCmos(void)
{
   return system("./shmUtil -store -3 > `cat /home/pi/machine/cmosfile`"); 
}

void loadSys(void)
{
   char buff[50];
   unsigned ipPart[4];
   exec_shell_command("ip -4 addr show eth0 | grep -oP \"(?<=inet ).*(?=/)\" | head -1",buff,50);
   if (sscanf(buff,"%d.%d.%d.%d",&ipPart[0],&ipPart[1],&ipPart[2],&ipPart[3]) != 4)
	   return;
}

void saveSys(void)
{
	;
}

void setNetwork(int net, int ip, int netmask, int gateway)
{
	char cmd[100];
	sprintf(cmd,"sudo ./chIp.sh eth%d %d.%d.%d.%d %d.%d.%d.%d %d.%d.%d.%d ",net, \
		(ip >> 24) & 0xff,(ip >> 16) & 0xff,(ip >> 8) & 0xff,ip & 0xff, \
		(netmask >> 24) & 0xff,(netmask >> 16) & 0xff,(netmask >> 8) & 0xff,netmask & 0xff, \
		(gateway >> 24) & 0xff,(gateway >> 16) & 0xff,(gateway >> 8) & 0xff,gateway & 0xff);
	printf("set network: %s\n",cmd);	
	system(cmd);
}

bool setLanguage(unsigned language)
{
	char cmd[300];
	char listItem[255];
	FILE *f;

	if ((f = fopen("languagelist","r")) == NULL)
	{
		printf("open file languagelist failed\n");
		return false;
	}
	for (int i = 0; i <= language; ++i)
	{
		if (!fgets(listItem, 255, f))
		{
			printf("Too less items in languagelist\n");
			fclose(f);
			return false;
		}
		printf("langlist %d %s\n",i,listItem);
	}
	char *pchr;	// get rid off cr, lf
	if ((pchr = strchr(listItem, '\r')) != NULL)
		*pchr = '\0';
	if ((pchr = strchr(listItem, '\n')) != NULL)
		*pchr = '\0';

	system("sudo mount -oremount -orw /home/pi/machine");
//	sprintf(cmd,"echo %s > language",listItem);
	sprintf(cmd,"cp %s index.cfg",listItem);
	system(cmd);
	system("sudo mount -oremount -oro /home/pi/machine");
	system("./restartgui.sh");

	fclose(f);
	return true;
}

int readVersionFile(char *fileName)	// set sys_request_string to artno and returns versionno
{
	FILE *f;
	char buffer[50] = {0};
	int swVersion;
	char *pchr;

	shmWriteAsStringByTagname("sys_request_string",buffer); // clear string
	if ((f = fopen(fileName,"r")) == NULL)
	{
		printf("open file %s failed\n",fileName);
		return 0;
	}
	if (!fgets(buffer, 50, f))
	{
		printf("Reading from file %s failed\n",fileName);
		return 0;
	}
	
	if ((pchr = strchr(buffer, '_')) != NULL)
		*pchr = '\0';
	else
		return 0;

	shmWriteAsStringByTagname("sys_request_string",buffer);
	swVersion = atoi(++pchr);

	printf("read version file: %s artno: %s version: %d\n",fileName, buffer, swVersion);
	return swVersion;
}

unsigned char calculate_crc(unsigned char *data, const size_t n,unsigned char crc)
{

	const unsigned char poly = 0b00011101;
	int datai, biti;

	crc = ~crc;
	for (datai = 0; datai < n; ++datai, ++data)
	{
		crc ^= *data;
		for (biti = 0; biti < 8; ++biti)
			crc = (crc << 1) ^ (crc & 0x80 ? poly : 0);
	}
	return ~crc;
}

bool waitForUpdatedShm(int ebmId)
{
	unsigned int nextSeqNo = ebmGetNextSeqNo(ebmId);
	int timeoutCnt = 5;	// timeout 50 ms
	while (true)
	{
		if (ebmGetSeqNo(ebmId) >= nextSeqNo)
			return true;
		if (--timeoutCnt == 0)
			return false;
		usleep(10000);
	}
}

#define CRC_KEY	123

int ebmFwDownload(int ebmId, int fwType, int hwType)	// fwType 0=bootloader, 1=firmware. hwType 0=196pin, 1=144pin
{
	FILE *f;
	char buffer[1100];
	char crcBuffer[2];
	int readCount, timeoutCnt;
	long fSize;
	char fileName[30];
	unsigned char checksum = 0;
	int fwUpdateFree;

	if (!ebmIsConnected(0))
		return -1;

	if (fwType == 0)
	{
		if (hwType == 0)
			sprintf(fileName, "ebm%c_boot_196.bin",ebmId+48);
		else if (hwType == 1)
			sprintf(fileName, "ebm%c_boot_144.bin",ebmId+48);
	}
	else
	{      
		if (hwType == 0)
			sprintf(fileName, "ebm%c_app_196.bin",ebmId+48);
		else if (hwType == 1)
			sprintf(fileName, "ebm%c_app_144.bin",ebmId+48);
	}
	
	if ((f = fopen(fileName,"rb")) == NULL)
	{
		printf("open file %s failed\n",fileName);
		return -2;
	}
	fseek (f,0,SEEK_END);
	fSize = ftell(f);
	rewind(f);
	printf("Filesize:%d\n",(int)fSize);
	
	ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
	timeoutCnt = 500;
	while (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) != FWUPDATE_RESPONSE_NONE)
	{
		if (--timeoutCnt == 0)
		{
			printf("Timeout waiting for ebm response\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			fclose(f);
			return -3;
		}
		usleep(1000);
	}

	if (fwType == 0)
		ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_RECEIVE_BOOTLOADER);
	else
		ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_RECEIVE_APPLICATION);
	timeoutCnt = 500;
	while (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) != FWUPDATE_RESPONSE_RECEIVING)
	{
		if (--timeoutCnt == 0)
		{
			printf("Timeout waiting for ebm response\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			fclose(f);
			return -4;
		}
		usleep(1000);
	}
	while (true)
	{
		if (!waitForUpdatedShm(ebmId))
		{
			printf("Timeout on waiting for ebm-card\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			fclose(f);
			return -5;
		}
		if (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) >= FWUPDATE_RESPONSE_ERROR_WRITE)
		{
			printf("Ebm error, receiving firmware. Response error:%d\n",ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE));
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			fclose(f);
			return -6;
		}
		timeoutCnt = 1000;
		while (((fwUpdateFree = ebmGetUInt16(ebmId,EBMMEM_UINT16_FIFO_FWUPDATE_FREE)) < 256) || (!shmFifoIsEmpty(ebmGetShmHandle(ebmId,MEM_TYPE_FIFO_UINT8,EBMMEM_FIFO_UINT8_FIRMWARE))))
		{
			if (--timeoutCnt == 0)
			{
				printf("Timeout waiting for ebm fifo\n");
				ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
				fclose(f);
				return -7;
			}
			usleep(1000);
		}
//		printf("fifo before len:%d\n",shmFifoInQueueByHandle(ebmGetShmHandle(ebmId,MEM_TYPE_FIFO_UINT8,EBMMEM_FIFO_UINT8_FIRMWARE)));

		readCount = fread(buffer,1,fwUpdateFree,f);
//		printf("buffer %s\n",buffer);
			
		printf("readCount:%d\n",readCount);
		
		if (ferror(f))
		{
			printf("Error reading file ebmapp.bin\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			fclose(f);
			return -8;
		}
//		printf("Copy to fifo\n");
		if (!shmCpynByteToFifoByHandle(buffer, ebmGetShmHandle(ebmId,MEM_TYPE_FIFO_UINT8,EBMMEM_FIFO_UINT8_FIRMWARE), readCount))
		{
			printf("Error, copy to fwupdate fifo failed\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			fclose(f);
			return -9;
		}
//		printf("fifo after len:%d\n",shmFifoInQueueByHandle(ebmGetShmHandle(ebmId,MEM_TYPE_FIFO_UINT8,EBMMEM_FIFO_UINT8_FIRMWARE)));
		checksum = calculate_crc((unsigned char *)buffer,readCount,checksum);
		if (feof(f) || (readCount < fwUpdateFree)) // means end of file
		{
			printf("End of file\n");
			crcBuffer[0] = CRC_KEY;
			crcBuffer[1] = checksum;
			if (!shmCpynByteToFifoByHandle(crcBuffer, ebmGetShmHandle(ebmId,MEM_TYPE_FIFO_UINT8,EBMMEM_FIFO_UINT8_FIRMWARE), 2))
			{
				printf("Error writing to fwupdate fifo\n");
				ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
				fclose(f);
				return -10;
			}
//			printf("fifo after len:%d\n",shmFifoInQueueByHandle(ebmGetShmHandle(ebmId,MEM_TYPE_FIFO_UINT8,EBMMEM_FIFO_UINT8_FIRMWARE)));
			break;
		}
	}
	usleep(50000); // wait to be sure all bytes have been sent
	ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_RECEIVE_FINISHED);
	printf("Checksum: %d\n",checksum);
	timeoutCnt = 2000;
	while (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) == FWUPDATE_RESPONSE_RECEIVING)
	{
		if (--timeoutCnt == 0)
		{
			printf("Timeout waiting for ebm response\n");
			ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
			fclose(f);
			return -11;
		}
		usleep(1000);
	}
	if (ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE) >= FWUPDATE_RESPONSE_ERROR_WRITE)  // errors starts from 10 
	{
		printf("Ebm error, received file integrity failed. Response error:%d\n",ebmGetUInt8(ebmId,EBMMEM_UINT8_FWUPDATE_RESPONSE));
		ebmSetUInt8(ebmId, EBMMEM_UINT8_FWUPDATE_CMD, FWUPDATE_CMD_NONE);
		fclose(f);
		return -12;
	}
	fclose(f);
	return 1;
}

#define VERSION_TAG "HERE_IS_EBM"
typedef struct 
{
	char           binRef[12];
	unsigned char	structVersion; // 1: First version, 2: Added hardwareType
	const char     *binEnd;
	char 		      appName[20];
	unsigned int   version;
	unsigned char  type;          // 1 = bootloader, 2 = application
	unsigned int   hwType;        // 0 = EBM-CPU 196pin, 1 = EBM-CPU144 144pin
} version_bin_t;

int ebmFwGetFileInfo(int ebmId, int fwType, int hwType)	// fwType 0=bootloader, 1=firmware.  hwType 0=196pin, 1=144pin
{
	FILE *f;
	char fileName[30];
	char buffer[1100];
	version_bin_t *version_bin;
	bool found = false;
	int readCount;
	
	if (fwType == 0)
	{
		if (hwType == 0)
			sprintf(fileName, "ebm%c_boot_196.bin",ebmId+48);
		else if (hwType == 1)
			sprintf(fileName, "ebm%c_boot_144.bin",ebmId+48);
   }
	else
	{
		if (hwType == 0)
			sprintf(fileName, "ebm%c_app_196.bin",ebmId+48);
		else if (hwType == 1)
			sprintf(fileName, "ebm%c_app_144.bin",ebmId+48);
	}
	
	if ((f = fopen(fileName,"rb")) == NULL)
	{
		printf("open file %s failed\n",fileName);
		return -1;
	}
	if ((readCount = fread(buffer,1,1000,f)) != 1000)
	{
		printf("Reading from file %s failed\n",fileName);
		return -2;
	}
	for (int i = 0; i < 900; i += 4)
	{
		if (!memcmp( (char *)(buffer + i), VERSION_TAG, 12))
		{
			printf("version found at pos:%d\n",i);
			version_bin = (version_bin_t *)(buffer + i);
			found = true;
			break;
		}
	}
	if (!found)
	{
		printf("Finding version info from file %s failed\n",fileName);
		return -3;
	}
	if (version_bin->hwType != hwType)
	{
		printf("Hardware type in file is %d, should have been %d\n",version_bin->hwType, hwType);
		return -4;
	}
	printf("%s appname:%s version:%d\n",fileName, version_bin->appName, version_bin->version);
	shmWriteAsStringByTagname("sys_request_string",version_bin->appName);

//	shmSetTextByHandle(shmHandle, version_bin->appName, &errCode);

	return version_bin->version;
}



int exec_shell_command(const char *pCommand, char *pData, int dataLen)
{
  FILE *pf;
  pf = popen(pCommand,"r");
  char * pCh;
  int returnValue;
 
  if(!pf){
    fprintf(stderr, "shell_command: Could not open pipe for output.\n");
    return -1;
  }
  // Grab data from process execution
  fgets(pData, dataLen , pf);
  if ((pCh = strchr(pData, '\n' )))
    *pCh = '\0'; // Remove any trailing new line
  if ((returnValue = pclose(pf)) == -1)
  {
    fprintf(stderr,"shell_command: Failed to close command stream \n");
    fprintf(stderr,pCommand);
  }
  return returnValue;
} 



