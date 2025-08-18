#ifndef LXINTERFACE_H
#define LXINTERFACE_H
#include <stdbool.h>
#include <pthread.h>
#include "shmInterface.h"
#include "ebmShmInterface.h"
#include "lxthreads.h"

//------------------------------------------------------------
//shared mem
// index in shmem integers
#define USER_INT_START		0			// user_int 0-999, handle 0-999, integer 0-999
#define RT_INT_START 		1000		// rt_int 0-999, handle 1000-1999, integer 1000-1999
#define CMOS_START 			2000		// cmos 0-24, handle 2000-2024, integer 2000-2024
#define SHMEMTEXT_START		2025		// shmemtext 0-99, handle 2025-2124
#define RT_REQUEST 			2025		// integer 2025
#define RT_REQUEST_PARAM 	2026		// integer 2026
#define RT_REQUEST_RESPONSE 2027		// integer 2027		
#define RT_REQUEST_IP		2028		// integer 2028
#define RT_REQUEST_NETMASK	2029		// integer 2029
#define RT_REQUEST_GATEWAY	2030		// integer 2030
// index in shmem unsigned integers
#define STATE 				0		// handle 2125, uinteger 0
#define CMD 				1		// handle 2126, uinteger 1
#define LMAXGUI_VERSION		2		// handle 2127, uinteger 2
#define SIGN1				3		// handle 2128, uinteger 3
#define SIGN2				4		// handle 2129, uinteger 4

#define  INPUT1      ebmGetInput(0,1)   
#define  INPUT2      ebmGetInput(0,2)   
#define  INPUT3      ebmGetInput(0,3)   
#define  INPUT4      ebmGetInput(0,4)   
#define  INPUT5      ebmGetInput(0,5)   
#define  INPUT6      ebmGetInput(0,6)   
#define  INPUT7      ebmGetInput(0,7)   
#define  INPUT8      ebmGetInput(0,8)   
#define  INPUT9      ebmGetInput(0,9)   
#define  INPUT10      ebmGetInput(0,10)   
#define  INPUT11      ebmGetInput(0,11)   
#define  INPUT12      ebmGetInput(0,12)   
#define  INPUT13      ebmGetInput(0,13)   
#define  INPUT14      ebmGetInput(0,14)   
#define  INPUT15      ebmGetInput(0,15)   
#define  INPUT16      ebmGetInput(0,16)   
#define  INPUT17      ebmGetInput(0,17)   
#define  INPUT18      ebmGetInput(0,18)   
#define  INPUT19      ebmGetInput(0,19)   
#define  INPUT20      ebmGetInput(0,20)   
#define  INPUT21      ebmGetInput(0,21)   
#define  INPUT22      ebmGetInput(0,22)   
#define  INPUT23      ebmGetInput(0,23)   
#define  INPUT24      ebmGetInput(0,24)   
#define  INPUT25      ebmGetInput(0,25)   
#define  INPUT26      ebmGetInput(0,26)   
#define  INPUT27      ebmGetInput(0,27)   
#define  INPUT28      ebmGetInput(0,28)   

#define  OUTPUT1_ON    ebmSetOutput(0,1,1)
#define  OUTPUT1_OFF   ebmSetOutput(0,1,0)
#define  OUTPUT2_ON    ebmSetOutput(0,2,1)
#define  OUTPUT2_OFF   ebmSetOutput(0,2,0)
#define  OUTPUT3_ON    ebmSetOutput(0,3,1)
#define  OUTPUT3_OFF   ebmSetOutput(0,3,0)
#define  OUTPUT4_ON    ebmSetOutput(0,4,1)
#define  OUTPUT4_OFF   ebmSetOutput(0,4,0)
#define  OUTPUT5_ON    ebmSetOutput(0,5,1)
#define  OUTPUT5_OFF   ebmSetOutput(0,5,0)
#define  OUTPUT6_ON    ebmSetOutput(0,6,1)
#define  OUTPUT6_OFF   ebmSetOutput(0,6,0)
#define  OUTPUT7_ON    ebmSetOutput(0,7,1)
#define  OUTPUT7_OFF   ebmSetOutput(0,7,0)
#define  OUTPUT8_ON    ebmSetOutput(0,8,1)
#define  OUTPUT8_OFF   ebmSetOutput(0,8,0)
#define  OUTPUT9_ON    ebmSetOutput(0,9,1)
#define  OUTPUT9_OFF   ebmSetOutput(0,9,0)
#define  OUTPUT10_ON    ebmSetOutput(0,10,1)
#define  OUTPUT10_OFF   ebmSetOutput(0,10,0)
#define  OUTPUT11_ON    ebmSetOutput(0,11,1)
#define  OUTPUT11_OFF   ebmSetOutput(0,11,0)
#define  OUTPUT12_ON    ebmSetOutput(0,12,1)
#define  OUTPUT12_OFF   ebmSetOutput(0,12,0)
#define  OUTPUT13_ON    ebmSetOutput(0,13,1)
#define  OUTPUT13_OFF   ebmSetOutput(0,13,0)
#define  OUTPUT14_ON    ebmSetOutput(0,14,1)
#define  OUTPUT14_OFF   ebmSetOutput(0,14,0)
#define  OUTPUT15_ON    ebmSetOutput(0,15,1)
#define  OUTPUT15_OFF   ebmSetOutput(0,15,0)
#define  OUTPUT16_ON    ebmSetOutput(0,16,1)
#define  OUTPUT16_OFF   ebmSetOutput(0,16,0)
#define  OUTPUT17_ON    ebmSetOutput(0,17,1)
#define  OUTPUT17_OFF   ebmSetOutput(0,17,0)
#define  OUTPUT18_ON    ebmSetOutput(0,18,1)
#define  OUTPUT18_OFF   ebmSetOutput(0,18,0)

#define RTREQUEST_NOREQUEST            0
#define RTREQUEST_LOADCFG              1
#define RTREQUEST_SAVECFG              2
#define RTREQUEST_LOADSYS              3
#define RTREQUEST_SAVESYS              4
#define RTREQUEST_CLEARMEM             5
#define RTREQUEST_LOADCMOS             6
#define RTREQUEST_SAVECMOS             7
#define RTREQUEST_OPENFLASH            8
#define RTREQUEST_CLOSEFLASH           9
#define RTREQUEST_SETLANGUAGE          10
#define RTREQUEST_FW_GETFILEINFO_BOOTLOADER_196  11
#define RTREQUEST_FW_GETFILEINFO_APPLICATION_196 12
#define RTREQUEST_FW_GETFILEINFO_BOOTLOADER_144  13
#define RTREQUEST_FW_GETFILEINFO_APPLICATION_144 14
#define RTREQUEST_FW_DOWNLOAD_BOOTLOADER_196     15
#define RTREQUEST_FW_DOWNLOAD_APPLICATION_196    16
#define RTREQUEST_FW_DOWNLOAD_BOOTLOADER_144     17
#define RTREQUEST_FW_DOWNLOAD_APPLICATION_144    18
#define RTREQUEST_READ_MACHINE_VERSION           19
#define RTREQUEST_READ_SYSTEM_VERSION            20
#define RTREQUEST_READ_BOOT_VERSION              21
#define RTREQUEST_SET_NETWORK                    22

#define RTRESPONS_OK                   1
#define RTRESPONS_CFGNUMERR            2
#define RTRESPONS_NOCFGFILE            3
#define RTRESPONS_NOCFGWRITE           4
#define RTRESPONS_CFGFILESIZEERROR     5
#define RTRESPONS_CFGVERSIONERROR      6
#define RTRESPONS_MTD0_ERROR           7
#define RTRESPONS_BFLA_ERROR           8
#define RTRESPONS_LOCKMTD0_ERROR       9
#define RTRESPONS_CMOS_ACCESSERROR     10
#define RTRESPONS_CMOS_VERSIONERROR    11
#define RTRESPONS_LANGUAGE_ERROR       12

extern volatile shmem_t *shared_Mem;
pthread_t threadSystemLoop;

#define HRTICKS_PER_SEC 1000000000
void IntEnv_Activate(InputInt_t *pInputInt);
void IntEnv_Deactivate(void);
typedef long long hrtime_t;
hrtime_t gethrtime(void);
void wait_np(int ticks);
void BeepBop(int xxx);

void SetAnalog(int EbmId, int Num, int Val);
void SetRawAnalog(int EbmId, int Num, int Val);
int GetAnalog(int EbmId, int Num);
int GetRawAnalog(int EbmId, int Num);

int modbusSend(unsigned int fp, void *buff, int cnt);
int modbusReceive(unsigned int fp, void *buff, int cnt);
void modbusWait(unsigned int ms);

bool systemLoopInit(void);
void *systemLoop(void *t);
int loadConfig(int configNumber);
void saveConfig(int configNumber);
int loadCmos(void);
int saveCmos(void);
void loadSys(void);
void saveSys(void);
void setNetwork(int net, int ip, int netmask, int gateway);
bool setLanguage(unsigned language);
int readVersionFile(char *fileName);
int ebmFwDownload(int ebmId, int fwType, int hwType);		// fwType 0=bootloader, 1=firmware. hwType 0=196pin, 1=144pin
int ebmFwGetFileInfo(int ebmId, int fwType, int hwType);	// fwType 0=bootloader, 1=firmware. hwType 0=196pin, 1=144pin
int exec_shell_command(const char *pCommand, char *pData, int dataLen);


#endif


// fix Makefile, ta bort a libbar
// Makefile, lägg till CC=gcc, byt ut ld -r med gcc 
// Ta bort include iodefs.h och shmem.h i Makefile

// rem logger_init, logger_unint
// shmemdefs, lägg till RT_INT_START
// shmemdefs, lägg till CMOS_START
// replace shared_mem->rt_int  shared_Mem->integer.Value
// replace shared_mem->cmos.data  shared_Mem->integer.Value
// replace shared_mem->user_int_min  shared_Mem->integer.Min
// replace shared_mem->user_int_max  shared_Mem->integer.Max
// replace shared_mem->user_int_default  shared_Mem->integer.Default
// replace shared_mem->user_int  shared_Mem->integer.Value
// replace shared_mem->rt_request_response  shared_Mem->integer.Value[RT_REQUEST_RESPONSE]
// replace shared_mem->rt_request_param  shared_Mem->integer.Value[RT_REQUEST_PARAM]
// replace shared_mem->rt_request  shared_Mem->integer.Value[RT_REQUEST] 
// replace shared_mem->cmd  shared_Mem->uinteger.Value[CMD]   !!OBS uinteger
// replace shared_mem->state  shared_Mem->uinteger.Value[STATE]  !!OBS uinteger
// replace shared_mem->lmaxgui_version  shared_Mem->uinteger.Value[LMAXGUI_VERSION]  !!OBS uinteger
// replace shared_mem->kinfo.ipa  ny hantering av ip
// replace shared_mem->kinfo.ipgw  ny hantering av ip
// replace shared_mem->kinfo.ipmask  ny hantering av ip
// remark på shared_mem->sign2 och shared_mem->version
// Byt ut shared_mem->shmemtext till anrop av shmSetTextByHandle
// ta bort include shmem.h, alla rtlinux h-filer
// Lägg till lxinterface.h i alla filer
// Lägg till EbmId på GetAnalog,SetAnalog,GetRawAnalog,SetRawAnalog
// Ta bort läsning analog 3 (finns ej)
// Ta bort include fabhw och i Makefile
// Ta bort include fabio och i Makefile
// byt ut input,output defines i defs.h till anrop av ebm kommandon
// fixa io.c
// byt ut wait_np
// byt ut BeepBop 
// Lägg till lxthreads, shmInterface, ebmShmInterface filer och include.
// fixa iotest-rutinen, använd ebm kommandon.
// lägg till main() och mainLoop()
// Lägg till en wait_np(1) i början på varje thread för synkning
// Lägg till InputInt som parameter i interrupt-rutiner
// Ändra rutiner som väntar på att rt_request_response blir skilt 0 till att vänta på rt_request blir noll. 
// rt_response-svar är inte samma som förut. Gå igenom alla.
// Implementera ny hantering av ip.
// debugPrint, ändra till vprintf istället för vsprintf

// Lägg till ebm.h
// Lägg till ebm callback-rutiner och anpassa dem.
// Anropa ebmInit, ebmCommInit, ebmControl
// Lägg till felhanteringen av ebm-fel.
// Ge emergencystop-error bara när ebm[x].ready är true



// hur fungerar recept-sparning idag
// används sign2 ? i så fall ändra i main.c
// behövs version? och resten av shmem



