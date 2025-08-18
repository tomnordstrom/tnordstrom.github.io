#ifndef _ebm_H_
#define _ebm_H_

#include <stdbool.h>

#define MAX_NO_OF_EBM 10

typedef enum {EBMSTATE_NOT_CONNECTED, EBMSTATE_CONNECTING, EBMSTATE_UPDATING, EBMSTATE_UPDATE_FAILED, EBMSTATE_READY} ebmState_t;
typedef struct
{
   ebmState_t state;	
	bool connectEnable;     // set by application, reseted on connection failure
	bool updateFailed;		// trigged on update failure, reset must be done by application
	int updateErrorCode;
	bool connectionFailed;  // trigged on connection failure, reset must be done by application
	bool ready;             // ebm is updated and ready
} ebm_t;
extern ebm_t ebm[MAX_NO_OF_EBM];

extern void (*cbEbmSetGuiUpdating)(void);
extern void (*cbEbmClearGuiUpdating)(void);
extern void (*cbEbmSetGuiUpdateProgress)(int progress);
extern void (*cbEbmUpdateVersionInfo)(void);
extern void (*cbEbmInitHardware)(int ebmId);

void ebmInit(int ebmId);
void ebmControl(int ebmId);


#endif
