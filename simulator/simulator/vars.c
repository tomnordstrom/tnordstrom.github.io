/* vars.h */

#include "vars.h"
#include "shmemdefs.h"

int guiState = GUI_STATE_MAIN;
int guiStatePrev = GUI_STATE_MAIN;


int machineType;  // MACHINE_TYPE_U50 or MACHINE_TYPE_U50i
int sw_machine_version;
int sw_system_version;
int sw_boot_version;
unsigned int runstat,standstat,errstat,stopstat;
bool ok_to_start_tick;
int service_time;
int user_password;
bool internalUsbCorrupt = false;
bool download_done;		// used to know when to update freq. drive with var.

machine_t machine;
cfg_t config;

analog_t analog;

digitax_t digitaxFeed;
drvFeed_t drvFeed;
