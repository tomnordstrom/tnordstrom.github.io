#include <stdbool.h>

#define SHM_FUNC_VERSION							 12
#define PARAMETERS                                   8000
#define TAGNAMES                                     4000
#define MAXTAGNAMELEN                                32
#define REALS                                        1000
#define INTEGERS                                     3000
#define UINTEGERS                                    1000
#define BOOLEANS                                     2000                    
#define BYTES                                        8000                    
#define FIFOS                                        50
#define MAXFIFOLEN                                   4096
#define STRINGS                                      250
#define GROUPS                                       64
#define MAX_GROUP_ADDRESS                            1024
#define MAXSTRLEN                                    80
#define SHM_ID_KEY                                   333
#define INTEGER                                      0 //done
#define BOOLEAN                                      1 //done
#define REAL                                         2 //done
#define TEXT                                         3 //done
#define UINT                                         4 //done
#define FIFO                                         5 //done
#define BYTE                                         6 //done used in FIFOs 
#define DATA                                         7 //done used in FIFOs int[5]


#define NO_LIM                                      0x00
#define SET_MIN										0x01
#define SET_MAX                                     0x02
#define SET_DEF                                     0x04	
#define _Min                                        0
#define _Max                                        1
#define _Def                                        2
#define _Val                                        3

#define NO_SAVE                                     0x00	
#define SAVE_TYPE1                                  0x01
#define SAVE_TYPE2                                  0x02


#ifdef __cplusplus
extern "C" {
#endif

union dataType //160 bit
{
	int all[5];
	struct
	{
		signed i0:				32;
		signed i1:				32;
		signed i2:				32;
		signed i3:				32;
		signed i4:				32;
	}iPart;
	struct
	{
		unsigned s1:				16;
		unsigned s2:				16;
		unsigned long long ll1:		64;
		unsigned b1:				8;
		unsigned b2:				8;
		unsigned b3:				8;
		unsigned b4:				8;
		unsigned i1:				32;
	}comPart;
};



union Bit64tag
{	
	unsigned long long all;
	struct
	{
		unsigned type:				  	4;  //0->int 1->bool 2->real 3->string 4->uint 5->fifo
		unsigned reference:			  	12; //reference 0-4095
		
		unsigned constant:	      	  	1;
		unsigned store:	              	3; //0 = no store, 1 = yes SAVE_TYPE1, 2=yes SAVE_TYPE2
		
		unsigned initialized:	      	1;
		unsigned forcing:         		1;
		unsigned disableRangeCheck:	  	1; 
		unsigned changeBit:       	    1; //u

		unsigned bit24:                 1; 
		unsigned bit25:					1; //0x00000040
		unsigned bit26:					1; //0x00000020
		unsigned bit27:					1; //0x00000010
		
		unsigned bit28:					1; //0x00000008
		unsigned bit29:					1; //0x00000004
		unsigned bit30:					1; //0x00000002
		unsigned bit31:					1; //0x00000001
		
		unsigned group0:                6;
		unsigned group0Adr:             10;
		unsigned group1:                6;
		unsigned group1Adr:             10;
	}bits;
};
/*
union Bit80tag
{	
	unsigned short[5] all;
	struct
	{
		unsigned type:				  	4;  //0->int 1->bool 2->real 3->string 4->uint 5->fifo
		unsigned reference:			  	12; //reference 0-4095
		
		unsigned constant:	      	  	1;
		unsigned store:	              	3; //0 = no store, 1 = yes SAVE_TYPE1, 2=yes SAVE_TYPE2
		
		unsigned initialized:	      	1;
		unsigned forcing:         		1;
		unsigned disableRangeCheck:	  	1; 
		unsigned changeBit:       	    1; //u

		unsigned bit24:					1; //0x00000080
		unsigned bit25:					1; //0x00000040
		unsigned bit26:					1; //0x00000020
		unsigned bit27:					1; //0x00000010
		
		unsigned bit28:					1; //0x00000008
		unsigned bit29:					1; //0x00000004
		unsigned bit30:					1; //0x00000002
		unsigned bit31:					1; //0x00000001
		
		unsigned group0:                6;
		unsigned group0Adr:             10;
		unsigned group1:                6;
		unsigned group1Adr:             10;
		unsigned group2:                6;
		unsigned group2Adr:             10;
	}bits;
};
*/
union piStruct
{
    unsigned int all;
    struct
    {
	  unsigned connected:           1; //PPU
	  unsigned stopRequest:         1; //PPU
	  unsigned ready:               1; //PPU
	  unsigned slowRequest:         1; //PPU

	  unsigned eject:               1; //PPU
	  unsigned pause:               1; //PPU
	  unsigned backward:            1; //Printer
	  unsigned wakeUp:              1; //Printer

	  unsigned tensionControl:      1; //Printer
	  unsigned fastStop:            1; //PPU
	  unsigned splice:              1; //PPU
	  unsigned blowerOn:            1; //Printer

	  unsigned decurl:              1; //Printer
	  unsigned decurlExit:          1; //PPU
	  unsigned jobOffset:           1; //Printer-Internal
	  unsigned pagePulse:           1; //Printer-Internal

	  unsigned sixPPi:              1; //Printer-Internal
	  unsigned warning:             1; //PPU
	  unsigned loaded:              1; //PPU // not used

	  unsigned notUsed:             13;

   }status;
};

/*union fifo32tag
{
	unsigned int all;
	struct
	{
		unsigned typ:				  	4; //type
		unsigned head:			  		6; 
		unsigned tail:			  		6; 
		unsigned len:				  	6; //max 63 long (MAXFIFOLEN),, 255
		unsigned underFlow:				1;
		unsigned overFlow:				1;
		unsigned inQueue:				6;
		unsigned free:					2;
	}bits;
};*/


union fifo64tag
{
	unsigned long long all;
	struct
	{
		unsigned typ:				  	4; //type
		unsigned head:			  		12; 
		unsigned len:				  	12; //max 4095 long (MAXFIFOLEN)
		unsigned underFlow:				1;
		unsigned overFlow:				1;
		unsigned maxUsage:				12; 
		unsigned tail:			  		12; 
		unsigned free:					10;
	}bits;
};

typedef struct
{
	struct
	{
	    union 			Bit64tag config[PARAMETERS];  
	}parameter;
    struct 
    {
        char  			name[TAGNAMES][MAXTAGNAMELEN];
		int   			iRef[TAGNAMES];
		int   			status;
    }tag;
	struct 
    {
        float 			Value[REALS];
		float 			Max[REALS];
		float 			Min[REALS];
		float 			Default[REALS];
		float 			SaveValue[REALS];
		int				status;
    }real;
	struct 
    {
        int 		    Value[INTEGERS];
		int 			Max[INTEGERS];
		int 			Min[INTEGERS];
		int 			Default[INTEGERS];
		int 			SaveValue[INTEGERS];
		int				status;
    }integer;
	struct 
    {
        int 		    Value[BYTES];
		int				status;
    }byte;
	struct 
    {
        unsigned int 		    Value[UINTEGERS];
		unsigned int 			Max[UINTEGERS];
		unsigned int 			Min[UINTEGERS];
		unsigned int 			Default[UINTEGERS];
		unsigned int 			SaveValue[UINTEGERS];
		int				status;
    }uinteger;
	struct 
    {
        bool 			Value[BOOLEANS];
		bool 			Default[BOOLEANS];
		bool 			SaveValue[BOOLEANS];
		int				status;
    }boolean;
	struct 
	{
		//char          	noOfAttachedTexts;
		char          	txt[STRINGS][MAXSTRLEN];
		unsigned char 	length[STRINGS];
		short         	iRef[STRINGS];
		unsigned short 	saveCrC[STRINGS];
		int				status;
	}str;
	struct 
	{
		union           fifo64tag conf[FIFOS];
		short			ref[FIFOS][MAXFIFOLEN];
		//pthread_mutex_t fifoMutex[FIFOS];
		int				status;
	}fif;
    struct 
	{
       short            iRef[GROUPS][MAX_GROUP_ADDRESS];
       bool             inUse[GROUPS];
	   short            addressFailureRef0;
	   short            addressFailureRef1;
	}group;
	int status;
} shmem_t;

struct objectType
{
  int iValue;
  int iMax;
  int iMin;
  int iDefault;
  
  unsigned int uValue;
  unsigned int uMax;
  unsigned int uMin;
  unsigned int uDefault;
  
  float fValue;
  float fMax;
  float fMin;
  float fDefault;
  
  bool bValue;
  bool bDefault;
  
  char txt[80];
  unsigned short saveCrC;
   
  union Bit64tag config;  
  int handle;
  int type;
  int ref;
  
  union fifo64tag fifoconf;
 
  char *tag;
};
//FUNCTIONS
volatile shmem_t *init_shm(void); //                         ----------  1  ------------ 
void uninitialize_mem(void); //                     ----------  2  ------------  
//INIT FUNCTIONS                                    ----------  3  ------------  
//int shmInitInt   (int varNum, int          min ,int          max ,int          def     , char *tag, bool constant, unsigned char store);
int shmInitInt   (int varNum, int          min ,int          max ,int          def     , char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr);
int shmInitUInt  (int varNum, unsigned int min ,unsigned int max ,unsigned int def     , char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr);
int shmInitReal  (int varNum, float        min ,float        max ,float        def     , char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr);
int shmInitBool  (int varNum/*---------------------------------*/,bool         def     , char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr);
int shmInitString(int varNum/*---------------------------------*/, char str[MAXSTRLEN] , char *tag/*-----------*/, unsigned char store, char group0, short group0adr, char group1, short group1adr);
int shmInitFifo  (int varNum, unsigned char type, unsigned short length                 , char *tag, char group0, short group0adr, char group1, short group1adr);
int shmCreateVariable (struct objectType obj); //supports 4 types, BOOLEAN, INTEGER, UINT, REAL

//                                                  ---------------------------  
void sortTagnames(void); //                         ----------  4  ------------ 
//READ,WRITE,COPY BY HANDLE
void  shmSetChangeBit                   (int varNum);
void  shmResetChangeBit                 (int varNum);
bool  shmReadChangeBit                  (int varNum);
void  shmSetSaveId                      (int varNum, int value);
int   shmReadIntByHandle                (int varNum);
unsigned int shmReadUIntByHandle        (int varNum);
float shmReadRealByHandle	            (int varNum);
bool  shmReadBoolByHandle               (int varNum);
bool  shmCopyValueByHandles             (int varNumDestination,int varNumSource);
bool  shmCopyBoolsToUint                (int varNumDestination,int varNumSources[32]);
bool  shmWriteIntByHandle               (int varNum, int value);
bool  shmWriteRealByHandle              (int varNum, float value);
bool  shmWriteBoolByHandle              (int varNum, bool value);
bool  shmWriteUIntByHandle              (int varNum, unsigned int value);
//CHANGE LIMITS
bool  shmUIntLimitsByHandle             (int varNum, unsigned int limits[3], unsigned char function); 
bool  shmIntLimitsByHandle              (int varNum, int limits[3], unsigned char function);
bool  shmRealLimitsByHandle             (int varNum, float limits[3], unsigned char function);

bool  shmGetObjectStructByHandle        (int varNum, struct objectType *obj, int *errCode);
//READ LIMITS
unsigned int *shmUIntReadLimitsByHandle (int varNum); 
int *shmIntReadLimitsByHandle           (int varNum);
float *shmRealReadLimitsByHandle        (int varNum);
char *shmGetTagNameByHandle(int Handle);
//bool  shmGetTagnameByHandle(int varNum,char *str); //TODO
//GET,SET BY HANDLE
bool shmGetIntByHandle                  (int varNum,int *value , int *errCode);
bool shmGetUIntByHandle                 (int varNum,unsigned int *value , int *errCode);
bool shmGetRealByHandle                 (int varNum,float *value , int *errCode);
bool shmGetBoolByHandle                 (int varNum,bool *value, int *errCode);
bool shmGetTextByHandle                 (int varNum,char *str[MAXSTRLEN], int *errCode);
int  shmReadTypeByHandle                (int varNum);
bool shmSetIntByHandle                  (int varNum,int value , int *errCode);
bool shmSetUIntByHandle                 (int varNum,unsigned int value , int *errCode);
bool shmSetRealByHandle                 (int varNum,float value , int *errCode);
bool shmSetBoolByHandle                 (int varNum,bool value, int *errCode);
bool shmSetTextByHandle                 (int varNum,char str[MAXSTRLEN], int *errCode);
//ACCESS BY TAGNAMES
bool shmWriteAsStringByTagname          (char *tag, char *val); 
bool shmWriteForcedAsStringByTagname    (char *tag, char *val);
bool shmIsForcedByTagname               (char *tag);
char * shmReadAsStringByTagname         (char *tag, char *r);
char * shmReadInfoAsStringByTagname     (char *tag, char *r);
bool shmGetHandleByTagname              (char *s, int *iHandle , int *errCode);
bool shmGetTypeAndHandleByTagname       (char *s, int *iType, int *iHandle);
bool shmGetIntByTagname                 (char *s, int *iValue);
bool shmGetRealByTagname                (char *s, float *rValue);
bool shmGetBoolByTagname                (char *s, bool *bValue);
int  shmReadTypeByTagname               (char *s);
//FORCING FUNCTIONS
void shmUnForceAll                      (void);
bool shmForceSetIntByHandle             (int varNum,int value , int *errCode);
bool shmForceSetUIntByHandle            (int varNum,unsigned int value , int *errCode);
bool shmForceSetRealByHandle            (int varNum,float value , int *errCode);
bool shmForceSetBoolByHandle            (int varNum,bool value, int *errCode);
void shmForceResetByHandle              (int varNum);
bool shmIsForced                        (int varNum);
//FIFO FUNCTIONS, GENERAL
int   shmFifoInQueueByHandle 	        (int varNum);
bool  shmFifoRemoveLastByHandle         (int varNum);
int   shmReadFifoTypeByHandle           (int varNum);
int   shmReadFifoSizeByHandle           (int varNum);
bool  shmFifoIsEmpty                    (int varNum);
int   shmReadFifoHead                   (int varNum);
int   shmReadFifoTail                   (int varNum);
int   shmMaxUsage                       (int varNum);
//FIFO FUNCTIONS, INTEGER
bool  shmAddIntFifoByHandle             (int varNum,int value, int *errCode);
bool  shmGetIntFifoByHandle             (int varNum,int *value, int *errCode);
int   shmPeekIntFifoByHandle            (int varNum);
//FIFO FUNCTIONS, UINTEGER
bool  shmAddUIntFifoByHandle            (int varNum,unsigned int value, int *errCode);
bool  shmGetUIntFifoByHandle            (int varNum,unsigned int *value, int *errCode);
unsigned int  shmPeekUIntFifoByHandle   (int varNum);
//FIFO FUNCTIONS, REAL
bool  shmAddRealFifoByHandle            (int varNum,float value, int *errCode);
bool  shmGetRealFifoByHandle            (int varNum,float *value, int *errCode);
float shmPeekRealFifoByHandle           (int varNum);
//FIFO FUNCTIONS, DATA
bool  shmAddDataFifoByHandle            (int varNum,int values[5], int *errCode);
bool  shmGetDataFifoByHandle            (int varNum,int *values, int *errCode);
bool  shmPeekDataFifoByHandle           (int varNum,int *values, int *errCode);
//FIFO FUNCTIONS, UNION
bool  shmAddUnionFifoByHandle           (int varNum,union dataType val, int *errCode); //data fifo
bool  shmGetUnionFifoByHandle           (int varNum,union dataType *val, int *errCode); //data fifo
//FIFO FUNCTIONS, BYTE
bool  shmAddByteFifoByHandle            (int varNum,char val, int *errCode);
bool  shmGetByteFifoByHandle            (int varNum,char *val, int *errCode);
char  shmPeekByteFifoByHandle           (int varNum);
/*
 *s is a sting containing all bytes in fifo, 
 *varNum is the handle
 *n is the maximum no of characters to return to *s
 *Returns int no of characters, negative if fault 
*/
int shmCpynByteFromFifoByHandle         (char *s, int varNum, unsigned short n);
/*
 *s is a sting containing all bytes in fifo, until including the delimiter char
 *varNum is the handle
 *n is the maximum no of characters to return to *s
 *Returns int no of characters, negative if fault, (if no delimiter char is detected it returns no of characters * -1) 
*/
int shmCpyNdByteFromFifoByHandle        (char *s, int varNum, unsigned short n, char delimiter);
char * shmStrFromByteFifoByHandle       (char *s, int varNum); 
//search for byte in queue, returns the queue position if found, returns 0 if not found, returns negative on error
int shmByteSearchFifoByHandle           (int varNum, char val);
//source,destination handle,number of bytes
bool shmCpynByteToFifoByHandle          (char *s, int varNum, unsigned short n);
//SAVE FUNCTIONS
bool shmSetSavedByHandle                (int varNum);
bool shmRevertByHandle                  (int varNum); 
bool shmIsSaved                         (int varNum); //returns false if it should be saved and is not equal to the saved value
bool shmSetDefault                      (int varNum);
void  shmDefaultAll                     (void);
void  shmDefaultGrp                     (int saveId);  

int shmAllSaved                         (void); // returns #PARAMETERS if all is saved. else it returns the first unsaved parameter.
unsigned short calcSaveCrC              (int varNum); //used by TEXTs
//OTHER
int readReference                       (int varNum);
int readType                            (int varNum);
bool isInitialized                      (int varNum);
char * shmGetTypeTextByHandle           (int varNum);
//getHandleByGroupAddress returns -1 if no address is allocated
short shmGetHandleByGroupAddress        (char grp, short adr);
bool  shmGroupInUse                     (char grp);
short shmNextChangedAddressInGroup      (char grp, short adr, int *handle);
#ifdef __cplusplus
}
#endif

