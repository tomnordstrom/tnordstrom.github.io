#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
//#include <stdarg.h> ////debugPrint
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <pthread.h>
#include "shmInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

volatile shmem_t *shared_mem;

volatile shmem_t *init_shm(void) {
  key_t key = SHM_ID_KEY;
  int shmid;
  shmid = shmget(key, sizeof(shmem_t), IPC_CREAT | 0666);
  shared_mem = shmat(shmid, NULL, 0);
  return shared_mem;
}

void uninitialize_mem(void)
{
	int i,j;
	for (i=0;i<PARAMETERS;i++) shared_mem->parameter.config[i].all=0;
	for (i=0;i<TAGNAMES;i++) {
		shared_mem->tag.name[i][0] = '\0';
		shared_mem->tag.iRef[i] = -1;
	}
    for (i=0;i<GROUPS;i++)
    {
       shared_mem->group.inUse[i] = false;
       for (j=0;j<MAX_GROUP_ADDRESS;j++)
       {
           shared_mem->group.iRef[i][j]=-1;
           
       }
    }
	shared_mem->tag.status = 0;
	shared_mem->real.status = 0;
	shared_mem->integer.status = 0;
	shared_mem->boolean.status = 0;
	shared_mem->uinteger.status = 0;
	shared_mem->str.status = 0;
	shared_mem->fif.status = 0;
	shared_mem->byte.status = 0;
	shared_mem->status = 0;
	shared_mem->group.addressFailureRef0 = 0;
	shared_mem->group.addressFailureRef1 = 0;
}

int attachInteger(int min, int max, int def)
{
	shared_mem->integer.Value[shared_mem->integer.status] = def;
	shared_mem->integer.Default[shared_mem->integer.status] = def;
	shared_mem->integer.Max[shared_mem->integer.status] = max;
	shared_mem->integer.Min[shared_mem->integer.status] = min;
	shared_mem->integer.SaveValue[shared_mem->integer.status] = def;
	return shared_mem->integer.status++;
}

int attachUInteger(int min, int max, int def)
{
	shared_mem->uinteger.Value[shared_mem->uinteger.status] = def;
	shared_mem->uinteger.Default[shared_mem->uinteger.status] = def;
	shared_mem->uinteger.Max[shared_mem->uinteger.status] = max;
	shared_mem->uinteger.Min[shared_mem->uinteger.status] = min;
	shared_mem->uinteger.SaveValue[shared_mem->uinteger.status] = def;
	return shared_mem->uinteger.status++;	
}

int attachData(void)
{
	shared_mem->integer.Value[shared_mem->integer.status] = 0; //int[0]
	shared_mem->integer.Default[shared_mem->integer.status] = 0; //int[1]
	shared_mem->integer.Min[shared_mem->integer.status] = 0; //int[2]
	shared_mem->integer.Max[shared_mem->integer.status] = 0; //int[3]
	shared_mem->integer.SaveValue[shared_mem->integer.status] = 0;
	return shared_mem->integer.status++;	
}

int attachReal(float min, float max, float def)
{
	shared_mem->real.Value[shared_mem->real.status] = def;
	shared_mem->real.Default[shared_mem->real.status] = def;
	shared_mem->real.Max[shared_mem->real.status] = max;
	shared_mem->real.Min[shared_mem->real.status] = min;
	shared_mem->real.SaveValue[shared_mem->real.status] = def;
	return shared_mem->real.status++;
}

int attachBoolean(int def)
{
	shared_mem->boolean.Value[shared_mem->boolean.status] = def;
	shared_mem->boolean.Default[shared_mem->boolean.status] = def;
	shared_mem->boolean.SaveValue[shared_mem->boolean.status] = def;
	return shared_mem->boolean.status++;
}

int attachByte()
{
	shared_mem->boolean.Value[shared_mem->byte.status] = 0;
	return shared_mem->byte.status++;
}

char * shmGetTypeTextByHandle (int varNum)
{
   if (shared_mem->parameter.config[varNum].bits.type == 0) return "integer";
   if (shared_mem->parameter.config[varNum].bits.type == 1) return "boolean";
   if (shared_mem->parameter.config[varNum].bits.type == 2) return "real";
   if (shared_mem->parameter.config[varNum].bits.type == 3) return "string";
   if (shared_mem->parameter.config[varNum].bits.type == 4) return "unsigned integer";
   if (shared_mem->parameter.config[varNum].bits.type == 5) return "fifo";
   return "";
}


char * shmGetTagNameByHandle(int Handle)
{
  int i;
  
  for (i=0;i<TAGNAMES;i++)
  {
    if (shared_mem->tag.iRef[i]==Handle)
    {
      //*t = (char *)(shared_mem->tag.name[i]);
	  //sprintf(t,"%s",shared_mem->tag.name[i]);
	  return (char *)shared_mem->tag.name[i];
    } 
    if (shared_mem->tag.iRef[i]==-1) break;
  }
  //char tmp[MAXTAGNAMELEN];
  //tmp[0] = '\0';
  return "";
}

/*bool shmGetTagNameByHandle(int Handle,char *t[],  int *errCode)
{
   int i,j;
  *errCode = 1;
  for (i=0;i<TAGNAMES;i++)
  {
    if (shared_mem->tag.iRef[i]==Handle)
    {
      // *t = (char *)(shared_mem->tag.name[i]);
	  //sprintf(t,"%s",shared_mem->tag.name[i]);
	  strcpy(t,(const char *)shared_mem->tag.name[i]);
	  for (i=0;i<32;i++) {
		  t[j]=shared_mem->tag.name[i][j];
		  t[j+1]=0;
		  if (shared_mem->tag.name[i][j] == 0) break;
	  }
      *errCode = 0;
      return 1;
    } 
    if (shared_mem->tag.iRef[i]==-1)
    {
	   //strcpy(t,"bajskorv");	
       printf("not found %i\n",i);
       // *t = "\0";
	    t = "\0";
	   //sprintf(t,"notFound");
       return 0;
    }
  }
  t = "\0";
  return 0;
}*/

/*bool  shmSetObjectStructByHandle (int varNum, struct objectType obj, int *errCode);
{
	
	return 1;
}
bool  shmChangeTagName (int varNum, char *tag, bool sort, int *errCode);
{
	
	return 1;
}*/
int shmCreateVariable (struct objectType obj)
{
	switch (obj.type)
	{
		case INTEGER:
			return shmInitInt (obj.handle, obj.iMin, obj.iMax , obj.iDefault, obj.tag , obj.config.bits.constant, obj.config.bits.store,0,0,0,0);
		break;
		case UINT:
			return shmInitUInt (obj.handle, obj.uMin, obj.uMax , obj.uDefault, obj.tag, obj.config.bits.constant, obj.config.bits.store,0,0,0,0);
		break;
		case BOOLEAN:
			return shmInitBool (obj.handle, obj.bDefault, obj.tag, obj.config.bits.constant, obj.config.bits.store,0,0,0,0);
		break;
		case REAL:
			return shmInitReal (obj.handle, obj.fMin, obj.fMax , obj.fDefault, obj.tag, obj.config.bits.constant, obj.config.bits.store,0,0,0,0);
		break;
		case TEXT:
			//int shmInitString(int varNum, char str[MAXSTRLEN] , char *tag, unsigned char store);
		break;
		case FIFO:
			//int shmInitFifo  (int varNum, unsigned char type, unsigned char length , char *tag);
		break;
		default:
		break;
	}
	return 1;
}

bool  shmGetObjectStructByHandle(int varNum, struct objectType *obj, int *errCode)
{
	struct objectType tmp;
//	char *x;
	int i;
//	int err;
	*errCode = 0;
	if (varNum<0 || varNum>=PARAMETERS) 
    {
      *errCode = 1; //accessing parmeter outside shared memory
      return 0;
    }
    if (shared_mem->parameter.config[varNum].bits.initialized == 0)
    {
      *errCode = 2; //accessing non initialized parameter
      return 0;
    }
	tmp.handle = varNum;
	tmp.type = readType(varNum);
	tmp.ref = readReference(varNum);
	tmp.config = shared_mem->parameter.config[varNum];
	tmp.saveCrC = 0;
	
	switch (tmp.type)
	{
		case INTEGER:
		  tmp.iValue = shared_mem->integer.Value[tmp.ref];
		  tmp.iMax = shared_mem->integer.Max[tmp.ref];
		  tmp.iMin = shared_mem->integer.Min[tmp.ref];
		  tmp.iDefault = shared_mem->integer.Default[tmp.ref];
		break;
		case UINT:
		  tmp.uValue = shared_mem->uinteger.Value[tmp.ref];
		  tmp.uMax = shared_mem->uinteger.Max[tmp.ref];
		  tmp.uMin = shared_mem->uinteger.Min[tmp.ref];
		  tmp.uDefault = shared_mem->uinteger.Default[tmp.ref];
		break;
		case BOOLEAN:
		  tmp.bValue = shared_mem->boolean.Value[tmp.ref];
		  tmp.bDefault = shared_mem->boolean.Default[tmp.ref];
		break;
		case REAL:
		  tmp.fValue = shared_mem->real.Value[tmp.ref];
		  tmp.fMax = shared_mem->real.Max[tmp.ref];
		  tmp.fMin = shared_mem->real.Min[tmp.ref];
		  tmp.fDefault = shared_mem->real.Default[tmp.ref];
		break;
		case TEXT:
		  for (i=0;i<80;i++) tmp.txt[i] = shared_mem->str.txt[shared_mem->parameter.config[varNum].bits.reference][i]; 
		  tmp.saveCrC = shared_mem->str.saveCrC[shared_mem->parameter.config[varNum].bits.reference];
		  //shmGetTextByHandle(varNum,&x,&err); //works
		  //for (i=0;i<80;i++) tmp.txt[i] = x[i]; 
		break;
		case FIFO:
		  tmp.fifoconf = shared_mem->fif.conf[tmp.ref]; 
		break;
		default:
		break;
	}
	tmp.tag = "";
	for (i=0;i<TAGNAMES;i++) //does a liniear search,, don't poll this function to often..
	{
		if (shared_mem->tag.iRef[i]==tmp.handle)
		{
		   tmp.tag = (char *)&shared_mem->tag.name[i][0];
		   break;
		}
		if (-1 == shared_mem->tag.iRef[i]) break; //not found
	}
    *obj = tmp;
	return 1;
}


bool attachTag(char *t,int varNum)
{
  int i,tagRef;
  if (shared_mem->tag.status<0) 
  {
    printf("attachTagAndUnit--Trying to add to sorted name array \n");
    return 0;
  }
  //strcpy(shared_mem->tag.name[varNum],str);
  for(i = 0; i < TAGNAMES; i++)
  {
    //see if the variable is already tagged, otherwise take the last free
    if (shared_mem->tag.iRef[i]==varNum || shared_mem->tag.iRef[i]==-1) 
    {
      //printf("variable already taged %d \n",i);
      break;
    }
  }
  tagRef = i;
  //printf("tagRef %d \n",i);
  for (i=0;i<MAXTAGNAMELEN;i++) 
  {
    if (t[i]=='\0' || (MAXTAGNAMELEN-1)==i) 
    {
      shared_mem->tag.name[tagRef][i]='\0';
      break;
    }
    else 
    {
      shared_mem->tag.name[tagRef][i] = t[i];
    }
    shared_mem->tag.iRef[tagRef]=varNum;
  }
  shared_mem->tag.name[tagRef][MAXTAGNAMELEN-1]='\0'; //just to be sure
  //printf("no %d\n",shared_mem->tag.status);
  shared_mem->tag.status++;
  //printf("%s %d",shared_mem->tag.name[tagRef],shared_mem->tag.status);
  return 1;
}

int attachText(char str[MAXSTRLEN])
{
   int i;
   //printf ("in attachText %s  %d\n",str,shared_mem->str.status);
   for (i=0;i<MAXSTRLEN;i++) shared_mem->str.txt[shared_mem->str.status][i]=0;
   for (i=0;str[i]!=0;i++) {
	   shared_mem->str.txt[shared_mem->str.status][i]=str[i];
	   if (i==(MAXSTRLEN-1)) {
		   shared_mem->str.txt[shared_mem->str.status][i]='\0';
		   break;
	   }
   }
   //shared_mem->str.iRef = varNum;
   shared_mem->str.saveCrC[shared_mem->str.status] = 0;
   //printf("%s \n",shared_mem->str.txt[shared_mem->str.status]);
   return shared_mem->str.status++;
}

unsigned short calcSaveCrC(int varNum)
{
  if (varNum<0 || varNum>=PARAMETERS) return 0;
  if (!isInitialized(varNum)) return 0;
  if (readType(varNum) != TEXT) return 0;
  int i;
  unsigned short crc=0;
  for (i=0;i<MAXSTRLEN;i++) {
	  if (shared_mem->str.txt[shared_mem->parameter.config[varNum].bits.reference][i] != '\0') crc += shared_mem->str.txt[shared_mem->parameter.config[varNum].bits.reference][i];
	  else break;
  } 
  return crc + (i<<9);   
}

bool shmSetTextByHandle(int varNum,char str[MAXSTRLEN], int *errCode)
{
  int i;
  bool x=false;
  *errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != TEXT) {
    *errCode = 3; //accessing non text
    return false;
  }
  if (shared_mem->parameter.config[varNum].bits.constant)
  {
	*errCode = 5; //setting constant
    return false; 
  }
  for (i=0;i<MAXSTRLEN;i++) {
	   if (str[i]==0) x = true;
	   if (x) shared_mem->str.txt[shared_mem->parameter.config[varNum].bits.reference][i]='\0';
	   else shared_mem->str.txt[shared_mem->parameter.config[varNum].bits.reference][i]=str[i];
	   if (i==(MAXSTRLEN-1)) {
		   shared_mem->str.txt[shared_mem->str.status][i]='\0';
		   break;
	   }
   }
   shared_mem->parameter.config[varNum].bits.changeBit = 1;
   /*shared_mem->str.saveCrC[shared_mem->parameter.config[varNum].bits.reference] = */
   //printf ("CrC:%d \n",calcSaveCrC(varNum));
   return true;
}



bool shmAddRealFifoByHandle(int varNum,float value, int *errCode)
{
	int _fifo;
	int _intRef;
	int iQ;
	
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.head ];
	
	//pthread_mutex_lock(shared_mem->fif.fifoMutex[_fifo]);
	
	if (shared_mem->fif.conf[_fifo].bits.typ != REAL)
	{
	  *errCode = 33; //wrong fifo type
	  ////pthread_mutex_unlock(shared_mem->fif.fifoMutex[_fifo]);
      return false;
	}
	iQ = shmFifoInQueueByHandle(varNum);
	if ((iQ+1)>=shared_mem->fif.conf[_fifo].bits.len/*shared_mem->fif.conf[_fifo].bits.inQueue >= shared_mem->fif.conf[_fifo].bits.len*/) {
		shared_mem->fif.conf[_fifo].bits.overFlow = 1;
		*errCode = 60;
		////pthread_mutex_unlock(shared_mem->fif.fifoMutex[_fifo]);
		return false;
	}
	shared_mem->real.Value[_intRef] = value;
	if ((shared_mem->fif.conf[_fifo].bits.head+1) >= shared_mem->fif.conf[_fifo].bits.len)
		shared_mem->fif.conf[_fifo].bits.head = 0;
	else
		shared_mem->fif.conf[_fifo].bits.head++;
	
	//shared_mem->fif.conf[_fifo].bits.inQueue++;
	//shared_mem->fif.conf[_fifo].bits.inQueue++;
//	if (shared_mem->fif.conf[_fifo].bits.head >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.head = 0;
	
	
	if ( (iQ+1) > shared_mem->fif.conf[_fifo].bits.maxUsage ) shared_mem->fif.conf[_fifo].bits.maxUsage = iQ+1;
	
	
	//pthread_mutex_unlock(shared_mem->fif.fifoMutex[_fifo]);
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
	
	return true;
	
	
}
bool shmGetRealFifoByHandle(int varNum,float *value, int *errCode)
{
	int _fifo;
	int _intRef;
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	
	//pthread_mutex_lock(&shared_mem->fif.fifoMutex[_fifo]);
	
	if (shared_mem->fif.conf[_fifo].bits.typ != REAL)
	{
	  *errCode = 33; //wrong fifo type
	  //pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
      return false;
	}
	if (shmFifoIsEmpty(varNum)/*  shared_mem->fif.conf[_fifo].bits.inQueue==0*/) {
		*errCode = 63;
		shared_mem->fif.conf[_fifo].bits.underFlow = 1;
		*value = 0;
		//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
		return false;
	}
	//shared_mem->fif.conf[_fifo].bits.inQueue--;
	*value = shared_mem->real.Value[_intRef];
	shared_mem->fif.conf[_fifo].bits.tail++;
	
	if (shared_mem->fif.conf[_fifo].bits.tail >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.tail = 0;
	//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
	return true;
}


float shmPeekRealFifoByHandle (int varNum)
{
	int _fifo;
	int _intRef;
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	return shared_mem->real.Value[_intRef];
}

bool shmAddIntFifoByHandle (int varNum,int value, int *errCode)
{
	int _fifo;
	int _intRef;
	int iQ;
	
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.head ];
	
	//pthread_mutex_lock(&shared_mem->fif.fifoMutex[_fifo]);
	
	if (shared_mem->fif.conf[_fifo].bits.typ != INTEGER)
	{
	  *errCode = 34; //wrong fifo type
	  //pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
      return false;
	}
	iQ = shmFifoInQueueByHandle(varNum);
	if ((iQ+1)>=shared_mem->fif.conf[_fifo].bits.len/*shared_mem->fif.conf[_fifo].bits.inQueue >= shared_mem->fif.conf[_fifo].bits.len*/) {
		shared_mem->fif.conf[_fifo].bits.overFlow = 1;
		*errCode = 60;
		//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
		return false;
	}
	shared_mem->integer.Value[_intRef] = value;
	if ((shared_mem->fif.conf[_fifo].bits.head+1) >= shared_mem->fif.conf[_fifo].bits.len)
		shared_mem->fif.conf[_fifo].bits.head = 0;
	else
		shared_mem->fif.conf[_fifo].bits.head++;
	//shared_mem->fif.conf[_fifo].bits.inQueue++;
//	if (shared_mem->fif.conf[_fifo].bits.head >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.head = 0;
	
	if ( (iQ+1) > shared_mem->fif.conf[_fifo].bits.maxUsage ) shared_mem->fif.conf[_fifo].bits.maxUsage = iQ+1;
	
	//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
	
	return true;
}

bool shmAddByteFifoByHandle (int varNum,char value, int *errCode)
{
	int _fifo;
	int _intRef;
	int iQ;
	
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.head ];
	
	//pthread_mutex_lock(&shared_mem->fif.fifoMutex[_fifo]);
	
	if (shared_mem->fif.conf[_fifo].bits.typ != BYTE)
	{
	  *errCode = 34; //wrong fifo type
	  //pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
      return false;
	}
	iQ = shmFifoInQueueByHandle(varNum);
	if ((iQ+1)>=shared_mem->fif.conf[_fifo].bits.len/*shared_mem->fif.conf[_fifo].bits.inQueue >= shared_mem->fif.conf[_fifo].bits.len*/) {
		shared_mem->fif.conf[_fifo].bits.overFlow = 1;
		*errCode = 60;
		//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
		return false;
	}
	shared_mem->byte.Value[_intRef] = value;
	if ((shared_mem->fif.conf[_fifo].bits.head+1) >= shared_mem->fif.conf[_fifo].bits.len)
		shared_mem->fif.conf[_fifo].bits.head = 0;
	else
		shared_mem->fif.conf[_fifo].bits.head++;
	//shared_mem->fif.conf[_fifo].bits.inQueue++;
//	if (shared_mem->fif.conf[_fifo].bits.head >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.head = 0;
	if ( (iQ+1) > shared_mem->fif.conf[_fifo].bits.maxUsage ) shared_mem->fif.conf[_fifo].bits.maxUsage = iQ+1;
	//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
    shared_mem->parameter.config[varNum].bits.changeBit = 1;
	
	return true;
}

//destination,source handle,number of bytes

char * shmStrFromByteFifoByHandle (char *s, int varNum)
{
	char t[255];
	int i=0;
	int err;
//	char x;
	if (varNum<0 || varNum>=PARAMETERS) return "Error:shmStrFromByteFifoByHandle:1";
    if (!isInitialized(varNum)) return "Error:shmStrFromByteFifoByHandle:2";
    if (readType(varNum) != FIFO) return "Error:shmStrFromByteFifoByHandle:3";
	if (shmReadFifoTypeByHandle(varNum)!=BYTE) return "Error:shmStrFromByteFifoByHandle:4";
	for (i=0;i<255;i++)
	{
		if (shmFifoIsEmpty(varNum)) {
			t[i] = '\0';
			break;
		}
		else if (!shmGetByteFifoByHandle(varNum,&t[i],&err)) {
			return "Error:shmStrFromByteFifoByHandle:5";
		}
		if (t[i] == '\0') break;
	}
	t[i+1] = '\0';
	strncpy(s,t,strlen(t)+1);
	return s;
}

int shmCpyNdByteFromFifoByHandle (char *s, int varNum, unsigned short n, char delimiter)
{
	char t;
	int i=0;
	int err;
//	char x;
	int found = -1;
	if (varNum<0 || varNum>=PARAMETERS) return -1;
    if (!isInitialized(varNum)) return -1;
    if (readType(varNum) != FIFO) return -1;
	if (shmReadFifoTypeByHandle(varNum)!=BYTE) return -1;
	for (i=0;i<n;i++)
	{
		if (shmFifoIsEmpty(varNum)) break;
		if (shmGetByteFifoByHandle(varNum,&t,&err)) {
			*(s+i) = t;
			if (t == delimiter) {
				found = 1;
				break;
			}
		}
		else
			return -1;
	}
	if (found>0) return i+1;
	else return i*-1;
}

int shmByteSearchFifoByHandle (int varNum, char val)
{
	int _fifo;
	int _Ref;
	int i;
	int len;
	int size;
	int tail;
//	int head;
	bool found = false;
    if (varNum<0 || varNum>=PARAMETERS) {
      return -1;
    }
    if (!isInitialized(varNum)) {
      return -1;
    }
    if (readType(varNum) != FIFO) {
      return -1;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	len = shmFifoInQueueByHandle(varNum);
	size = shmReadFifoSizeByHandle(varNum);
	//head = shmReadFifoHead(varNum);
	tail = shmReadFifoTail(varNum);
	//printf("%d %d %d %d %d \n",_fifo,len,size,head,tail);
	
	for (i=0;i<(len);i++)
	{
		_Ref = shared_mem->fif.ref[ _fifo ][ (tail+i)%size ];
		if (shared_mem->byte.Value[_Ref] == val) {
			found = true;
			break;
		}
		//printf("%c %d %d\n",shared_mem->byte.Value[_Ref],_Ref,(tail+i)%size);
	}
	if (found) return i+1;
	return 0;
}

int shmCpynByteFromFifoByHandle (char *s, int varNum, unsigned short n)
{
	char t;
	int i=0;
	int err;
//	char x;
	if (varNum<0 || varNum>=PARAMETERS) return -1;
    if (!isInitialized(varNum)) return -1;
    if (readType(varNum) != FIFO) return -1;
	if (shmReadFifoTypeByHandle(varNum)!=BYTE) return -1;
	for (i=0;i<n;i++)
	{
		if (shmFifoIsEmpty(varNum))
			break;
		if (shmGetByteFifoByHandle(varNum,&t,&err)) {
			*(s+i) = t;
		}
		else
			return -1;
	}
	return i;
}
//source,destination handle,number of bytes
bool shmCpynByteToFifoByHandle (char *s, int varNum, unsigned short n)
{
	int i,err;
	for (i=0;i<n;i++)
	{
		if (shmAddByteFifoByHandle(varNum,*(s+i),&err)==false) return false;
	}
//	printf("@@fifolen:%d\n",shmFifoInQueueByHandle(varNum));

	return true;
}

char shmPeekByteFifoByHandle (int varNum)
{
	int _fifo;
	int _intRef;
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	return shared_mem->byte.Value[_intRef];
}


int shmMaxUsage(int varNum)
{
	int _fifo;
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	return shared_mem->fif.conf[_fifo].bits.maxUsage;
}

bool shmGetByteFifoByHandle(int varNum,char *value, int *errCode)
{
	int _fifo;
	int _intRef;
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	if (shared_mem->fif.conf[_fifo].bits.typ != BYTE)
	{
	  *errCode = 33; //wrong fifo type
      return false;
	}
	if (shmFifoIsEmpty(varNum)/*shared_mem->fif.conf[_fifo].bits.inQueue==0*/) {
		*errCode = 63;
		shared_mem->fif.conf[_fifo].bits.underFlow = 1;
		*value = 0;
		return false;
	}
	//shared_mem->fif.conf[_fifo].bits.inQueue--;
	*value = shared_mem->byte.Value[_intRef];
	if ((shared_mem->fif.conf[_fifo].bits.tail+1) >= shared_mem->fif.conf[_fifo].bits.len)
		shared_mem->fif.conf[_fifo].bits.tail = 0;
	else
		shared_mem->fif.conf[_fifo].bits.tail++;
//	if (shared_mem->fif.conf[_fifo].bits.tail >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.tail = 0;
	
	return true;
}

bool shmAddUnionFifoByHandle (int varNum,union dataType val, int *errCode)
{
	int _fifo;
	int _intRef;
	int iQ;
	
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.head ];
	
	//pthread_mutex_lock(&shared_mem->fif.fifoMutex[_fifo]);
	
	if (shared_mem->fif.conf[_fifo].bits.typ != DATA)
	{
	  *errCode = 34; //wrong fifo type
	  //pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
      return false;
	}
	iQ = shmFifoInQueueByHandle(varNum);
	if ((iQ+1)>=shared_mem->fif.conf[_fifo].bits.len /*  shared_mem->fif.conf[_fifo].bits.inQueue >= shared_mem->fif.conf[_fifo].bits.len*/) {
		shared_mem->fif.conf[_fifo].bits.overFlow = 1;
		//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
		*errCode = 60;
		return false;
	}
	
	shared_mem->integer.Value[_intRef] 		= (int)(val.iPart.i0);
	shared_mem->integer.Max[_intRef] 		= (int)(val.iPart.i1);
	shared_mem->integer.Min[_intRef] 		= (int)(val.iPart.i2);
	shared_mem->integer.Default[_intRef] 	= (int)(val.iPart.i3);
	shared_mem->integer.SaveValue[_intRef] 	= (int)(val.iPart.i4);
	
	if ((shared_mem->fif.conf[_fifo].bits.head+1) >= shared_mem->fif.conf[_fifo].bits.len)
		shared_mem->fif.conf[_fifo].bits.head = 0;
	else
		shared_mem->fif.conf[_fifo].bits.head++;
	
	//shared_mem->fif.conf[_fifo].bits.inQueue++;
//	if (shared_mem->fif.conf[_fifo].bits.head >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.head = 0;
	if ( (iQ+1) > shared_mem->fif.conf[_fifo].bits.maxUsage ) shared_mem->fif.conf[_fifo].bits.maxUsage = iQ+1;
	//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
	return true;
}

bool shmAddDataFifoByHandle (int varNum,int values[5], int *errCode)
{
	int _fifo;
	int _intRef;
	int iQ;
	
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.head ];
	
	//pthread_mutex_lock(&shared_mem->fif.fifoMutex[_fifo]);
	
	if (shared_mem->fif.conf[_fifo].bits.typ != DATA)
	{
	  *errCode = 34; //wrong fifo type
	  //pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
      return false;
	}
	iQ = shmFifoInQueueByHandle(varNum);
	if ((iQ+1)>=shared_mem->fif.conf[_fifo].bits.len /*shared_mem->fif.conf[_fifo].bits.inQueue >= shared_mem->fif.conf[_fifo].bits.len*/) {
		shared_mem->fif.conf[_fifo].bits.overFlow = 1;
		*errCode = 60;
		//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
		return false;
	}
	
	shared_mem->integer.Value[_intRef] = values[0];
	shared_mem->integer.Max[_intRef] = values[1];
	shared_mem->integer.Min[_intRef] = values[2];
	shared_mem->integer.Default[_intRef] = values[3];
	shared_mem->integer.SaveValue[_intRef] = values[4];

	if ((shared_mem->fif.conf[_fifo].bits.head+1) >= shared_mem->fif.conf[_fifo].bits.len)
		shared_mem->fif.conf[_fifo].bits.head = 0;
	else
		shared_mem->fif.conf[_fifo].bits.head++;
	
	//shared_mem->fif.conf[_fifo].bits.inQueue++;
//	if (shared_mem->fif.conf[_fifo].bits.head >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.head = 0;
	if ( (iQ+1) > shared_mem->fif.conf[_fifo].bits.maxUsage ) shared_mem->fif.conf[_fifo].bits.maxUsage = iQ+1;
	//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
	return true;
}

bool shmAddUIntFifoByHandle(int varNum,unsigned int value, int *errCode)
{
	int _fifo;
	int _intRef;
	int iQ;
	
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.head ];
	
	//pthread_mutex_lock(&shared_mem->fif.fifoMutex[_fifo]);
	
	if (shared_mem->fif.conf[_fifo].bits.typ != UINT)
	{
	  *errCode = 35; //wrong fifo type
	  //pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
      return false;
	}
	iQ = shmFifoInQueueByHandle(varNum);
	if ((iQ+1)>=shared_mem->fif.conf[_fifo].bits.len /*shared_mem->fif.conf[_fifo].bits.inQueue >= shared_mem->fif.conf[_fifo].bits.len*/) {
		shared_mem->fif.conf[_fifo].bits.overFlow = 1;
		*errCode = 60;
		//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
		return false;
	}

	shared_mem->uinteger.Value[_intRef] = value;
	if ((shared_mem->fif.conf[_fifo].bits.head+1) >= shared_mem->fif.conf[_fifo].bits.len)
		shared_mem->fif.conf[_fifo].bits.head = 0;
	else
		shared_mem->fif.conf[_fifo].bits.head++;

	//shared_mem->fif.conf[_fifo].bits.inQueue++;
//	if (shared_mem->fif.conf[_fifo].bits.head >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.head = 0;
	if ( (iQ+1) > shared_mem->fif.conf[_fifo].bits.maxUsage ) shared_mem->fif.conf[_fifo].bits.maxUsage = iQ+1;
	//pthread_mutex_unlock(&shared_mem->fif.fifoMutex[_fifo]);
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
	return true;
}

bool shmGetUIntFifoByHandle(int varNum,unsigned int *value, int *errCode)
{
	int _fifo;
	int _intRef;
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	if (shared_mem->fif.conf[_fifo].bits.typ != UINT)
	{
	  *errCode = 32; //wrong fifo type
      return false;
	}
	if (shmFifoIsEmpty(varNum)/*   shared_mem->fif.conf[_fifo].bits.inQueue==0*/) {
		*errCode = 62;
		shared_mem->fif.conf[_fifo].bits.underFlow = 1;
		*value = 0;
		return false;
	}
	//shared_mem->fif.conf[_fifo].bits.inQueue--;
	*value = shared_mem->uinteger.Value[_intRef];
	shared_mem->fif.conf[_fifo].bits.tail++;
	if (shared_mem->fif.conf[_fifo].bits.tail >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.tail = 0;
	return true;
}


bool shmGetIntFifoByHandle (int varNum,int *value, int *errCode)
{
    int _fifo;
	int _intRef;
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	if (shared_mem->fif.conf[_fifo].bits.typ != INTEGER)
	{
	  *errCode = 31; //wrong fifo type
      return false;
	}
	if (shmFifoIsEmpty(varNum)/*shared_mem->fif.conf[_fifo].bits.inQueue==0*/) {
		*errCode = 61;
		shared_mem->fif.conf[_fifo].bits.underFlow = 1;
		*value = 0;
		return false;
	}
	//shared_mem->fif.conf[_fifo].bits.inQueue--;
	*value = shared_mem->integer.Value[_intRef];
	shared_mem->fif.conf[_fifo].bits.tail++;
	if (shared_mem->fif.conf[_fifo].bits.tail >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.tail = 0;
	return true;
}

bool shmPeekDataFifoByHandle (int varNum,int *values, int *errCode)
{
	int _fifo;
	int _intRef;
	//int v[4];
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	if (shared_mem->fif.conf[_fifo].bits.typ != DATA)
	{
	  *errCode = 31; //wrong fifo type
      return false;
	}
	if (shmFifoIsEmpty(varNum)/*  shared_mem->fif.conf[_fifo].bits.inQueue==0*/) {
		*errCode = 61;
		*(values) = 0;
		*(values+1) = 0;
		*(values+2) = 0;
		*(values+3) = 0;
		*(values+4) = 0;
		return false;
	}
	*(values) = shared_mem->integer.Value[_intRef];
	*(values+1) = shared_mem->integer.Max[_intRef];
	*(values+2) = shared_mem->integer.Min[_intRef];
	*(values+3) = shared_mem->integer.Default[_intRef];
	*(values+4) = shared_mem->integer.SaveValue[_intRef];
	return true;	
}

bool shmGetUnionFifoByHandle (int varNum,union dataType *val, int *errCode)
{
	int _fifo;
	int _intRef;
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	if (shared_mem->fif.conf[_fifo].bits.typ != DATA)
	{
	  *errCode = 31; //wrong fifo type
      return false;
	}
	if (shmFifoIsEmpty(varNum) /*  shared_mem->fif.conf[_fifo].bits.inQueue==0 */) {
		*errCode = 61;
		shared_mem->fif.conf[_fifo].bits.underFlow = 1;
		val->iPart.i0 = 0;
		val->iPart.i1 = 0;
		val->iPart.i2 = 0;
		val->iPart.i3 = 0;
		val->iPart.i4 = 0;
		return false;
	}
	//shared_mem->fif.conf[_fifo].bits.inQueue--;
	
	val->iPart.i0 = shared_mem->integer.Value[_intRef];
	val->iPart.i1 = shared_mem->integer.Max[_intRef];
	val->iPart.i2 = shared_mem->integer.Min[_intRef];
	val->iPart.i3 = shared_mem->integer.Default[_intRef];
	val->iPart.i4 = shared_mem->integer.SaveValue[_intRef];
	
	shared_mem->fif.conf[_fifo].bits.tail++;
	if (shared_mem->fif.conf[_fifo].bits.tail >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.tail = 0;
	return true;
}

bool shmGetDataFifoByHandle (int varNum, int * values, int *errCode)
{
	int _fifo;
	int _intRef;
	*errCode = 0;
    if (varNum<0 || varNum>=PARAMETERS) {
      *errCode = 1; //accessing parmeter outside shared memory
      return false;
    }
    if (!isInitialized(varNum)) {
      *errCode = 2; //accessing non initialized parameter
      return false;
    }
    if (readType(varNum) != FIFO) {
      *errCode = 3; //accessing non fifo
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	if (shared_mem->fif.conf[_fifo].bits.typ != DATA)
	{
	  *errCode = 31; //wrong fifo type
      return false;
	}
	if (shmFifoIsEmpty(varNum)  /*shared_mem->fif.conf[_fifo].bits.inQueue==0*/) {
		*errCode = 61;
		shared_mem->fif.conf[_fifo].bits.underFlow = 1;
		*(values) = 0;
		*(values+1) = 0;
		*(values+2) = 0;
		*(values+3) = 0;
		*(values+4) = 0;
		return false;
	}
	//shared_mem->fif.conf[_fifo].bits.inQueue--;
	
	*(values) = shared_mem->integer.Value[_intRef];
	*(values+1) = shared_mem->integer.Max[_intRef];
	*(values+2) = shared_mem->integer.Min[_intRef];
	*(values+3) = shared_mem->integer.Default[_intRef];
	*(values+4) = shared_mem->integer.SaveValue[_intRef];
	
	shared_mem->fif.conf[_fifo].bits.tail++;
	if (shared_mem->fif.conf[_fifo].bits.tail >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.tail = 0;
	return true;
}

bool  shmFifoRemoveLastByHandle (int varNum)
{
	int _fifo;
//	int _intRef;
    if (varNum<0 || varNum>=PARAMETERS) {
      return false;
    }
    if (!isInitialized(varNum)) {
      return false;
    }
    if (readType(varNum) != FIFO) {
      return false;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
//	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	
	
	
	
	if (shmFifoIsEmpty(varNum)  /*shared_mem->fif.conf[_fifo].bits.inQueue==0*/) {
		shared_mem->fif.conf[_fifo].bits.underFlow = 1;
		return false;
	}
	//shared_mem->fif.conf[_fifo].bits.inQueue--;
	shared_mem->fif.conf[_fifo].bits.tail++;
	if (shared_mem->fif.conf[_fifo].bits.tail >= shared_mem->fif.conf[_fifo].bits.len) shared_mem->fif.conf[_fifo].bits.tail = 0;
	return true;
}

int   shmReadFifoTypeByHandle   (int varNum)
{
	int _fifo;
    if (varNum<0 || varNum>=PARAMETERS) {
      return -1;
    }
    if (!isInitialized(varNum)) {
      return -1;
    }
    if (readType(varNum) != FIFO) {
      return -1;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	return shared_mem->fif.conf[ _fifo ].bits.typ;
}

int    shmReadFifoSizeByHandle (int varNum)
{
	int _fifo;
    if (varNum<0 || varNum>=PARAMETERS) {
      return -1;
    }
    if (!isInitialized(varNum)) {
      return -1;
    }
    if (readType(varNum) != FIFO) {
      return -1;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	
	return shared_mem->fif.conf[ _fifo ].bits.len;
}

/*bool shmFifoIsEmpty (int varNum)
{
	if (shmFifoInQueueByHandle(varNum) == 0) return 1;
	return 0;	
}*/


bool shmFifoIsEmpty (int varNum)
{
	//if (shmFifoInQueueByHandle(varNum) == 0) return 1;
    if (shmReadFifoHead(varNum) == shmReadFifoTail(varNum)) return 1;
	return 0;	
}

int shmFifoInQueueByHandle (int varNum)
{
	int size;	
	size = shmReadFifoSizeByHandle(varNum);
	return ((shmReadFifoHead(varNum)-shmReadFifoTail(varNum)+size)%size);
}


int   shmReadFifoHead			(int varNum)
{
	int _fifo;
	if (varNum<0 || varNum>=PARAMETERS) {
      return -1;
    }
    if (!isInitialized(varNum)) {
      return -1;
    }
    if (readType(varNum) != FIFO) {
      return -1;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	return shared_mem->fif.conf[ _fifo ].bits.head;
}
int   shmReadFifoTail			(int varNum)
{
	int _fifo;
	if (varNum<0 || varNum>=PARAMETERS) {
      return -1;
    }
    if (!isInitialized(varNum)) {
      return -1;
    }
    if (readType(varNum) != FIFO) {
      return -1;
    }
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	return shared_mem->fif.conf[ _fifo ].bits.tail;
}

int shmPeekIntFifoByHandle (int varNum)
{
	int _fifo;
	int _intRef;
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	return shared_mem->integer.Value[_intRef];
}

unsigned int shmPeekUIntFifoByHandle (int varNum)
{
	int _fifo;
	int _intRef;
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	_intRef = shared_mem->fif.ref[ _fifo ][ shared_mem->fif.conf[ _fifo ].bits.tail ];
	return shared_mem->uinteger.Value[_intRef];
}

/*int shmFifoInQueueByHandle (int varNum)
{
	int _fifo;
	_fifo = shared_mem->parameter.config[varNum].bits.reference;
	return shared_mem->fif.conf[ _fifo ].bits.inQueue;
}*/

int attachFifo(unsigned char type,unsigned short length)
{
	int i;
	shared_mem->fif.conf[shared_mem->fif.status].all = 0;
	shared_mem->fif.conf[shared_mem->fif.status].bits.len = length;
	shared_mem->fif.conf[shared_mem->fif.status].bits.typ = type;
	if (type == INTEGER)
	{
		for (i=0;i<length;i++) shared_mem->fif.ref[shared_mem->fif.status][i]=attachInteger(0,0,0);
	}
	else if (type == UINT)
	{
		for (i=0;i<length;i++) shared_mem->fif.ref[shared_mem->fif.status][i]=attachUInteger(0,0,0);
	}
	else if (type == BOOLEAN)
	{
		for (i=0;i<length;i++) shared_mem->fif.ref[shared_mem->fif.status][i]=attachBoolean(0);
	}
	else if (type == REAL)
	{
		for (i=0;i<length;i++) shared_mem->fif.ref[shared_mem->fif.status][i]=attachReal(0,0,0);
	}
	else if (type == DATA)
	{
		for (i=0;i<length;i++) shared_mem->fif.ref[shared_mem->fif.status][i]=attachData();
	}
	else if (type == BYTE)
	{
		for (i=0;i<length;i++) shared_mem->fif.ref[shared_mem->fif.status][i]=attachByte();
	}
	else return -1;
	return shared_mem->fif.status++;
}

int attachGroup(char grp ,short adr,int varNum)
{
   if (shared_mem->group.iRef[(short)grp][adr] == -1)
   {
      shared_mem->group.iRef[(short)grp][adr] = varNum;
	  shared_mem->group.inUse[(short)grp] = true;
	  return 1;
   }
   else
   {
      shared_mem->group.addressFailureRef0 = shared_mem->group.iRef[(short)grp][adr];
	  shared_mem->group.addressFailureRef1 = varNum;
	  printf("shm attachGroup FAIL!!!! group %d address occupied %d handleA %d handleB %d\n",grp,adr,shared_mem->group.iRef[(short)grp][adr],varNum);
	  return 0;
    }
	return 0;
}

bool shmGroupInUse(char grp)
{
   return shared_mem->group.inUse[(short)grp];
}

short shmGetHandleByGroupAddress  (char grp, short adr)
{
   return shared_mem->group.iRef[(short)grp][adr];
}

short shmNextChangedAddressInGroup(char grp, short adr, int * handle)
{
   short i,varNum;
   if (adr == MAX_GROUP_ADDRESS) return -2;
   for (i=(adr+1);i<MAX_GROUP_ADDRESS;i++) {
      varNum = shared_mem->group.iRef[(short)grp][i];
      if (varNum != -1) {
         if (readType(varNum) == FIFO) {
            if (!shmFifoIsEmpty(varNum)) {
               *handle = varNum;
               return i;
            }
         }
         else if (shmReadChangeBit(varNum)) {
           *handle = varNum;
           return i;
         }
      }
   }
   return -1;
}

int shmInitFifo(int varNum,unsigned char type,unsigned short length,char *tag, char group0, short group0adr, char group1, short group1adr)
{
  if (isInitialized(varNum)) {
	  printf("shmFifo: FAIL!!!! Handle %d alreadey initialized!!! \n",varNum);
	  return 0;
  }
  shared_mem->parameter.config[varNum].all=0;
  //if (min == 0 && max == 0) shared_mem->parameter.config[varNum].bits.disableRangeCheck=1;
  //else if (min>=max || def<min || def>max) return -1;
  shared_mem->parameter.config[varNum].bits.type=FIFO;
  shared_mem->parameter.config[varNum].bits.reference = attachFifo(type,length);
  if (shared_mem->parameter.config[varNum].bits.reference == -1) {
	return ++shared_mem->status;
  }
  shared_mem->parameter.config[varNum].bits.initialized = true;
  shared_mem->parameter.config[varNum].bits.forcing = false;
  if (group0 > 0 && group0 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group0 = group0;
    shared_mem->parameter.config[varNum].bits.group0Adr = group0adr;
	attachGroup(group0,group0adr,varNum);
  }
  if (group1 > 0 && group1 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group1 = group1;
    shared_mem->parameter.config[varNum].bits.group1Adr = group1adr;
	attachGroup(group1,group1adr,varNum);
  }
  if (tag != NULL){
    //printf("attachTagName %d %s\n", varNum, tag);
    if (!attachTag(tag,varNum))
    {
      shared_mem->parameter.config[varNum].bits.initialized = false;
      return 0;
    }
  }
  return ++shared_mem->status;
}

int shmInitUInt(int varNum, unsigned int min ,unsigned int max ,unsigned int def, char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr)
{
  if (isInitialized(varNum)) {
	  printf("shmInitUInt: FAIL!!!! Handle %d alreadey initialized!!! \n",varNum);
	  return 0;
  }
  shared_mem->parameter.config[varNum].all=0;
  if (min == 0 && max == 0) shared_mem->parameter.config[varNum].bits.disableRangeCheck=1;
  else if (min>=max || def<min || def>max) return -1;
  shared_mem->parameter.config[varNum].bits.type=UINT;
  shared_mem->parameter.config[varNum].bits.constant=constant;
  shared_mem->parameter.config[varNum].bits.store=store;
  shared_mem->parameter.config[varNum].bits.reference = attachUInteger(min,max,def);
  shared_mem->parameter.config[varNum].bits.initialized = true;
  shared_mem->parameter.config[varNum].bits.forcing = false;
  if (group0 > 0 && group0 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group0 = group0;
    shared_mem->parameter.config[varNum].bits.group0Adr = group0adr;
	attachGroup(group0,group0adr,varNum);
  }
  if (group1 > 0 && group1 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group1 = group1;
    shared_mem->parameter.config[varNum].bits.group1Adr = group1adr;
	attachGroup(group1,group1adr,varNum);
  }
  if (tag != NULL){
    //printf("attachTagName %d %s\n", varNum, tag);
    if (!attachTag(tag,varNum))
    {
      shared_mem->parameter.config[varNum].bits.initialized = false;
      return -3;
    }
  }
  else shared_mem->parameter.config[varNum].bits.store=0; // value without tag name should not be stored
  return ++shared_mem->status;
}


//int shmInitInt(int varNum, int min ,int max ,int def, char *tag, bool constant, unsigned char store)
int shmInitInt (int varNum, int min ,int max ,int def, char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr)
{
  if (isInitialized(varNum)) {
	  printf("shmInitInt: FAIL!!!! Handle %d alreadey initialized!!! \n",varNum);
	  return 0;
  }
  shared_mem->parameter.config[varNum].all=0;
  if (min == 0 && max == 0) shared_mem->parameter.config[varNum].bits.disableRangeCheck=1;
  else if (min>=max || def<min || def>max) return -1;
  shared_mem->parameter.config[varNum].bits.type=INTEGER;
  shared_mem->parameter.config[varNum].bits.constant=constant;
  shared_mem->parameter.config[varNum].bits.store=store;
  shared_mem->parameter.config[varNum].bits.reference = attachInteger(min,max,def);
  shared_mem->parameter.config[varNum].bits.initialized = true;
  shared_mem->parameter.config[varNum].bits.forcing = false;
  if (group0 > 0 && group0 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group0 = group0;
    shared_mem->parameter.config[varNum].bits.group0Adr = group0adr;
	attachGroup(group0,group0adr,varNum);
  }
  if (group1 > 0 && group1 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group1 = group1;
    shared_mem->parameter.config[varNum].bits.group1Adr = group1adr;
	attachGroup(group1,group1adr,varNum);
  }
  if (tag != NULL){
    //printf("attachTagName %d %s\n", varNum, tag);
    if (!attachTag(tag,varNum))
    {
      shared_mem->parameter.config[varNum].bits.initialized = false;
      return -3;
    }
  }
  else shared_mem->parameter.config[varNum].bits.store=0; // value without tag name should not be stored
  return ++shared_mem->status;
}

int shmInitBool(int varNum, bool def ,char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr)
{
	if (isInitialized(varNum)) {
	  printf("shmInitBool: FAIL!!!! Handle %d alreadey initialized!!! \n",varNum);
	  return 0;
    }
	shared_mem->parameter.config[varNum].all=0;
//	if (def>1 || def<0) return -2;
	shared_mem->parameter.config[varNum].bits.type=BOOLEAN;
	shared_mem->parameter.config[varNum].bits.constant=constant;
	shared_mem->parameter.config[varNum].bits.reference = attachBoolean(def);
	shared_mem->parameter.config[varNum].bits.initialized = true;
    shared_mem->parameter.config[varNum].bits.forcing = false;
	shared_mem->parameter.config[varNum].bits.store=store;
	if (group0 > 0 && group0 < GROUPS)
    {
       shared_mem->parameter.config[varNum].bits.group0 = group0;
       shared_mem->parameter.config[varNum].bits.group0Adr = group0adr;
	   attachGroup(group0,group0adr,varNum);
    }
    if (group1 > 0 && group1 < GROUPS)
    {
       shared_mem->parameter.config[varNum].bits.group1 = group1;
       shared_mem->parameter.config[varNum].bits.group1Adr = group1adr;
	   attachGroup(group1,group1adr,varNum);
    } 
	if (tag != NULL){
		//printf("attachTagName %d %s\n", varNum, tag);
		if (!attachTag(tag,varNum))
		{
		  shared_mem->parameter.config[varNum].bits.initialized = false;
		  return -3;
		}
    } 
	else shared_mem->parameter.config[varNum].bits.store=0; // value without tag name should not be stored
	return ++shared_mem->status;
}
int shmInitReal(int varNum, float min ,float max ,float def, char *tag, bool constant, unsigned char store, char group0, short group0adr, char group1, short group1adr)
{
  if (isInitialized(varNum)) {
	  printf("shmInitReal: FAIL!!!! Handle %d alreadey initialized!!! \n",varNum);
	  return 0;
  }
  shared_mem->parameter.config[varNum].all=0;
  if (min == 0.0 && max == 0.0) shared_mem->parameter.config[varNum].bits.disableRangeCheck=1;
  else if (min>=max || def<min || def>max) return -1;
  shared_mem->parameter.config[varNum].bits.type=REAL;
  shared_mem->parameter.config[varNum].bits.constant=constant;
  shared_mem->parameter.config[varNum].bits.store=store;
  shared_mem->parameter.config[varNum].bits.reference = attachReal(min,max,def);
  shared_mem->parameter.config[varNum].bits.initialized = true;
  shared_mem->parameter.config[varNum].bits.forcing = false;
  if (group0 > 0 && group0 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group0 = group0;
    shared_mem->parameter.config[varNum].bits.group0Adr = group0adr;
	attachGroup(group0,group0adr,varNum);
  }
  if (group1 > 0 && group1 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group1 = group1;
    shared_mem->parameter.config[varNum].bits.group1Adr = group1adr;
	attachGroup(group1,group1adr,varNum);
  }
  if (tag != NULL){
		//printf("attachTagName %d %s\n", varNum, tag);
		if (!attachTag(tag,varNum))
		{
		  shared_mem->parameter.config[varNum].bits.initialized = false;
		  return -3;
		}
  } 
  else shared_mem->parameter.config[varNum].bits.store=0; // value without tag name should not be stored
  return ++shared_mem->status;
}
int shmInitString(int varNum, char str[MAXSTRLEN] , char *tag, unsigned char store, char group0, short group0adr, char group1, short group1adr)
{
  if (isInitialized(varNum)) {
	  printf("shmInitString: FAIL!!!! Handle %d alreadey initialized!!! \n",varNum);
	  return 0;
  }
  shared_mem->parameter.config[varNum].all=0;
  shared_mem->parameter.config[varNum].bits.type=TEXT;
  shared_mem->parameter.config[varNum].bits.store=store;
  shared_mem->parameter.config[varNum].bits.initialized = true;
  shared_mem->parameter.config[varNum].bits.reference = attachText(str);
  //shared_mem->str.saveCrC[shared_mem->str.status] = calcSaveCrC(varNum);
  shared_mem->parameter.config[varNum].bits.forcing = false;
  if (group0 > 0 && group0 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group0 = group0;
    shared_mem->parameter.config[varNum].bits.group0Adr = group0adr;
	attachGroup(group0,group0adr,varNum);
  }
  if (group1 > 0 && group1 < GROUPS)
  {
    shared_mem->parameter.config[varNum].bits.group1 = group1;
    shared_mem->parameter.config[varNum].bits.group1Adr = group1adr;
	attachGroup(group1,group1adr,varNum);
  }
  if (tag != NULL){
		//printf("attachTagName %d %s\n", varNum, tag);
		if (!attachTag(tag,varNum))
		{
		  shared_mem->parameter.config[varNum].bits.initialized = false;
		  return -3;
		}
  } 
  //shared_mem->str.saveCrC[shared_mem->str.status] = calcSaveCrC(varNum);
  shmSetSavedByHandle(varNum);
  return ++shared_mem->status;
}

int readReference(int varNum)
{
	return shared_mem->parameter.config[varNum].bits.reference;
}

int readType(int varNum)
{
	return shared_mem->parameter.config[varNum].bits.type;
}

bool isInitialized(int varNum)
{
	return shared_mem->parameter.config[varNum].bits.initialized;
}

void shmUnForceAll (void)
{
	int i;
	for (i=0;i<PARAMETERS;i++) shared_mem->parameter.config[i].bits.forcing=false;
}

bool shmIsSaved(int varNum)
{
	if (varNum<0 || varNum>=PARAMETERS) {
		return true;
	}
	if (!isInitialized(varNum)) {
		return true;
	}
	if (!shared_mem->parameter.config[varNum].bits.store) { //not a value that should be stored
		return true;
	}
	int t = readType(varNum);
	if (t==INTEGER) {
		if (shared_mem->integer.Value[readReference(varNum)] == shared_mem->integer.SaveValue[readReference(varNum)]) return true;
		else return false;
	}
	else if (t==REAL) {
		if (shared_mem->real.Value[readReference(varNum)] == shared_mem->real.SaveValue[readReference(varNum)]) return true;
		else return false;
	}
	else if (t==BOOLEAN) {
		if (shared_mem->boolean.Value[readReference(varNum)] == shared_mem->boolean.SaveValue[readReference(varNum)]) return true;
		else return false;
	}
	else if (t==UINT) {
		if (shared_mem->uinteger.Value[readReference(varNum)] == shared_mem->uinteger.SaveValue[readReference(varNum)]) return true;
		else return false;
	}
	else if (t==TEXT) {
		if (calcSaveCrC(varNum) == shared_mem->str.saveCrC[readReference(varNum)]) return true;
		else return false;
	}
	return true;
}

int shmAllSaved(void)
{
	int i;
	for (i=0;i<PARAMETERS;i++)
	{
		if (!shmIsSaved(i)) return i;
	}
	return PARAMETERS;
}

bool shmRevertByHandle(int varNum)
{
	if (varNum<0 || varNum>=PARAMETERS) {
		return false;
	}
	if (!isInitialized(varNum)) {
		return false;
	}
	if (!shared_mem->parameter.config[varNum].bits.store) { //not a value that should be stored
		return false;
	}
	int t = readType(varNum);
	if (t==INTEGER) {
		return shmWriteIntByHandle(varNum,shared_mem->integer.SaveValue[readReference(varNum)]);
	}
	if (t==REAL) {
		return shmWriteRealByHandle(varNum,shared_mem->real.SaveValue[readReference(varNum)]);
	}
	if (t==BOOLEAN) {
		return shmWriteBoolByHandle(varNum,shared_mem->boolean.SaveValue[readReference(varNum)]);
	}
	if (t==UINT) {
		return shmWriteUIntByHandle(varNum,shared_mem->uinteger.SaveValue[readReference(varNum)]);
	}
	return false;
}

bool shmCopyBoolsToUint(int varNumDestination,int varNumSources[32])
{
	if (varNumDestination<0 || varNumDestination>=PARAMETERS) {
		return false;
	}
	if (!isInitialized(varNumDestination)) {
		return false;
	}
	int x = readType(varNumDestination);
	int tmp=0;
	if (x!=UINT) {
		return false;
	}
	for (x=0;x<32;x++) {
		if (varNumSources[x]>=0) tmp += shmReadBoolByHandle(varNumSources[x])<<x;
	}
	shmWriteUIntByHandle(varNumDestination,tmp);
	return true;
}

bool shmSetDefault(int varNum)
{
	if (varNum<0 || varNum>=PARAMETERS) {
		return false;
	}
	if (!isInitialized(varNum)) {
		return false;
	}
	int t = readType(varNum);
	if (t==INTEGER) {
		return shmWriteIntByHandle(varNum,shared_mem->integer.Default[readReference(varNum)]);
	}
	if (t==REAL) {
		return shmWriteRealByHandle(varNum,shared_mem->real.Default[readReference(varNum)]);
	}
	if (t==BOOLEAN) {
		return shmWriteBoolByHandle(varNum,shared_mem->boolean.Default[readReference(varNum)]);
	}
	if (t==UINT) {
		return shmWriteUIntByHandle(varNum,shared_mem->uinteger.Default[readReference(varNum)]);
	}
	return false;
}

void shmDefaultAll(void)
{
	int i;
	for (i=0;i<PARAMETERS;i++) shmSetDefault(i);
}

void shmDefaultGrp(int saveId)
{
	int i;
	for (i=0;i<PARAMETERS;i++) {
		if (saveId == shared_mem->parameter.config[i].bits.store) shmSetDefault(i);
	}
}

bool shmSetSavedByHandle(int varNum)
{
	if (varNum<0 || varNum>=PARAMETERS) {
		return false;
	}
	if (!isInitialized(varNum)) {
		return false;
	}
	if (!shared_mem->parameter.config[varNum].bits.store) { //not a value that should be stored
		return false;
	}
	int t = readType(varNum);
	if (t==INTEGER) {
		shared_mem->integer.SaveValue[readReference(varNum)] = shared_mem->integer.Value[readReference(varNum)];
		return true;
	}
	if (t==REAL) {
		shared_mem->real.SaveValue[readReference(varNum)] = shared_mem->real.Value[readReference(varNum)];
		return true;
	}
	if (t==BOOLEAN) {
		shared_mem->boolean.SaveValue[readReference(varNum)] = shared_mem->boolean.Value[readReference(varNum)];
		return true;
	}
	if (t==UINT) {
		shared_mem->uinteger.SaveValue[readReference(varNum)] = shared_mem->uinteger.Value[readReference(varNum)];
		return true;
	}
	if (t==TEXT) {
		shared_mem->str.saveCrC[readReference(varNum)] = calcSaveCrC(varNum);
		return true;
	}
	return false;
}

void shmForceResetByHandle(int varNum)
{
	shared_mem->parameter.config[varNum].bits.forcing = false;
}

bool shmIsForced(int varNum)
{
	return shared_mem->parameter.config[varNum].bits.forcing;
}

bool shmForceSetIntByHandle(int varNum,int value , int *errCode)
{
	shared_mem->parameter.config[varNum].bits.forcing = false;
	if (shmSetIntByHandle(varNum,value,errCode)) {
		shared_mem->parameter.config[varNum].bits.forcing = true;
		return true;
	}
	return false;
}

bool shmForceSetUIntByHandle(int varNum,unsigned int value , int *errCode)
{
//	bool r;
	shared_mem->parameter.config[varNum].bits.forcing = false;
	if (shmSetUIntByHandle(varNum,value,errCode)) {
		shared_mem->parameter.config[varNum].bits.forcing = true;
		return true;
	}
	return false;
}

bool shmForceSetRealByHandle(int varNum,float value , int *errCode)
{
//	bool r;
	shared_mem->parameter.config[varNum].bits.forcing = false;
	if (shmSetRealByHandle(varNum,value,errCode)) {
		shared_mem->parameter.config[varNum].bits.forcing = true;
		return true;
	}
	return false;
}

bool shmForceSetBoolByHandle(int varNum,bool value, int *errCode)
{
//	bool r;
	shared_mem->parameter.config[varNum].bits.forcing = false;
	if (shmSetBoolByHandle(varNum,value,errCode)) {
		shared_mem->parameter.config[varNum].bits.forcing = true;
		return true;
	}
	return false;
}

bool shmSetUIntByHandle(int varNum,unsigned int value , int *errCode)
{
  *errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != UINT) {
    *errCode = 3; //accessing non unsigned integer
    return false;
  }
  if ((value > shared_mem->uinteger.Max[readReference(varNum)] || value < shared_mem->uinteger.Min[readReference(varNum)] ) && shared_mem->parameter.config[varNum].bits.disableRangeCheck==false)
  {
	*errCode = 4; //value error
    return false; 
  }
  if (shared_mem->parameter.config[varNum].bits.constant)
  {
	*errCode = 5; //setting constant
    return false; 
  }
  if (shared_mem->parameter.config[varNum].bits.forcing) //  can't set value beeing forced
  {
	*errCode = -1; //trying to set forced
    return true;  //dont set
  }
  if (shared_mem->uinteger.Value[readReference(varNum)] != value)
  {
	  shared_mem->uinteger.Value[readReference(varNum)] = value;
	  shared_mem->parameter.config[varNum].bits.changeBit = 1;
  }
  //shared_mem->uinteger.Value[readReference(varNum)] = value;
  return true;
}

bool shmSetIntByHandle(int varNum,int value , int *errCode)
{
  *errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != INTEGER) {
    *errCode = 3; //accessing non integer
    return false;
  }
  if ((value > shared_mem->integer.Max[readReference(varNum)] || value < shared_mem->integer.Min[readReference(varNum)] ) && shared_mem->parameter.config[varNum].bits.disableRangeCheck==false)
  {
	
	*errCode = 4; //value error
    return false; 
  }
  if (shared_mem->parameter.config[varNum].bits.constant)
  {
	*errCode = 5; //setting constant
    return false; 
  }
  if (shared_mem->parameter.config[varNum].bits.forcing) //  can't set value beeing forced
  {
	*errCode = -1; //trying to set forced
    return true;  //dont set
  }
  if (shared_mem->integer.Value[readReference(varNum)] != value)
  {
	shared_mem->integer.Value[readReference(varNum)] = value;
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
  }
  
  return true;
}


bool  shmCopyValueByHandles   (int varNumDestination,int varNumSource)
{
	int type,iVal;
	float fVal;
	bool bVal;
	type = readType(varNumDestination);
	if (type ==  readType(varNumSource))
	{
		switch (type)
		{
			case INTEGER:
			   iVal = shmReadIntByHandle(varNumSource);
			   return shmWriteIntByHandle(varNumDestination,iVal);
			break;
			case REAL:
			   fVal = shmReadRealByHandle(varNumSource);
			   return shmWriteRealByHandle(varNumDestination,fVal);
			break;
			case BOOLEAN:
			   bVal = shmReadBoolByHandle(varNumSource);
			   return shmWriteBoolByHandle(varNumDestination,bVal);
			break;
			default:
				printf("shmCopyValueByHandles:unknown type\n");
			break;
		}
	}
//	else
	return false;
}

bool shmWriteUIntByHandle(int varNum, unsigned int value) {
	int err;
	if (shmSetUIntByHandle(varNum,value,&err)) return true;
	return false;	
}

bool  shmWriteIntByHandle (int varNum, int value)
{
	int err;
	if (shmSetIntByHandle(varNum,value,&err)) return true;
	return false;
}
bool  shmWriteRealByHandle(int varNum, float value)
{
	int err;
	if (shmSetRealByHandle(varNum,value,&err)) return true;
	return false;
}
bool  shmWriteBoolByHandle(int varNum, bool value)
{
	int err;
	if (shmSetBoolByHandle(varNum,value,&err)) return true;
	return false;
}

bool shmSetRealByHandle(int varNum,float value , int *errCode)
{
  *errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != REAL) {
    *errCode = 3; //accessing non Real
    return false;
  }
  if ((value > shared_mem->real.Max[readReference(varNum)] || value < shared_mem->real.Min[readReference(varNum)]) && shared_mem->parameter.config[varNum].bits.disableRangeCheck==false)
  {
	*errCode = 4; //value error
    return false; 
  }
  if (shared_mem->parameter.config[varNum].bits.constant)
  {
	*errCode = 5; //setting constant
    return false; 
  }
  if (shared_mem->parameter.config[varNum].bits.forcing) //  can't set value beeing forced
  {
	*errCode = -1; //trying to set forced
    return true;  //dont set
  }
  if (shared_mem->real.Value[readReference(varNum)] != value)
  {
	shared_mem->real.Value[readReference(varNum)] = value;
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
  }
  
  return true;
}

bool shmSetBoolByHandle(int varNum,bool value, int *errCode)
{
	*errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != BOOLEAN) {
    *errCode = 3; //accessing non bool
    return false;
  }
/*  if (value > 1 || value < 0)
  {
	*errCode = 4; //value error
    return false; 
  }*/
  if (shared_mem->parameter.config[varNum].bits.constant)
  {
	*errCode = 5; //setting constant
    return false; 
  }
  if (shared_mem->parameter.config[varNum].bits.forcing) //  can't set value beeing forced
  {
	*errCode = -1; //trying to set forced
    return true;  //dont set
  }
  if (shared_mem->boolean.Value[readReference(varNum)] != value)
  {
	shared_mem->boolean.Value[readReference(varNum)] = value;
	shared_mem->parameter.config[varNum].bits.changeBit = 1;
  }
  return true;
}

void  shmSetChangeBit(int varNum)
{
	shared_mem->parameter.config[varNum].bits.changeBit=1;
}

void  shmResetChangeBit(int varNum)
{
	shared_mem->parameter.config[varNum].bits.changeBit=0;
}

bool  shmReadChangeBit(int varNum)
{	
   return shared_mem->parameter.config[varNum].bits.changeBit;
}

void  shmSetSaveId(int varNum, int value)
{
	if (value > 7) // 3 bits
		return;
	shared_mem->parameter.config[varNum].bits.store=value;
}

int shmReadIntByHandle(int varNum)
{
	int err,retVal;
	if (shmGetIntByHandle(varNum, &retVal, &err))
		return retVal;
    return 0;
}

unsigned int shmReadUIntByHandle(int varNum)
{
	int err;
	unsigned int retVal;
	if (shmGetUIntByHandle(varNum, &retVal, &err))
		return retVal;
    return 0;
}

float shmReadRealByHandle(int varNum)
{
	int err;
	float retVal;
	if (shmGetRealByHandle(varNum, &retVal, &err))
		return retVal;
    return 0;
}
bool shmReadBoolByHandle(int varNum)
{
	int err;
	bool retVal;
	if (shmGetBoolByHandle(varNum, &retVal, &err))
	  return retVal;
    return 0;
}

bool shmGetTextByHandle(int varNum, char **str, int *errCode)
{
//  int i;
  *errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
	*str = "\0";
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
	*str = "\0";
    return false;
  }
  if (readType(varNum) != TEXT) {
    *errCode = 3; //accessing non integer
	*str = "\0";
    return false;
  }

  *str = (char *)(shared_mem->str.txt[shared_mem->parameter.config[varNum].bits.reference]);
  
  //sprintf(*str,"%s",shared_mem->str.txt[shared_mem->parameter.config[varNum].bits.reference]);

  return true;
}

bool shmGetIntByHandle(int varNum,int *value , int *errCode)
{ 
  *errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != INTEGER) {
    *errCode = 3; //accessing non integer
    return false;
  }
  *value = shared_mem->integer.Value[readReference(varNum)];
  return true;
}

bool shmGetUIntByHandle(int varNum,unsigned int *value , int *errCode)
{ 
  *errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != UINT) {
    *errCode = 3; //accessing non integer
    return false;
  }
  *value = shared_mem->uinteger.Value[readReference(varNum)];
  return true;
}


bool shmGetRealByHandle(int varNum,float *value , int *errCode)
{
	*errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = 1; //accessing parmeter outside shared memory
    return false;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != REAL) {
    *errCode = 3; //accessing non integer
    return false;
  }
  *value = shared_mem->real.Value[readReference(varNum)];
  return true;
	
}
bool shmGetBoolByHandle(int varNum,bool *value, int *errCode)
{
	*errCode = 0;
  if (varNum<0 || varNum>=PARAMETERS) {
    *errCode = false; //accessing parmeter outside shared memory
    return 0;
  }
  if (!isInitialized(varNum)) {
    *errCode = 2; //accessing non initialized parameter
    return false;
  }
  if (readType(varNum) != BOOLEAN) {
    *errCode = 3; //accessing non integer
    return false;
  }
  *value = shared_mem->boolean.Value[readReference(varNum)];
  return true;	
}

int shmReadTypeByTagname(char *s)
{
	int varNum,err;
	if (shmGetHandleByTagname(s, &varNum,&err)) {
		return readType(varNum);
	}
	else
		return -1;	
}

int shmReadTypeByHandle(int varNum)
{
	return readType(varNum);
}

bool shmIsForcedByTagname(char *tag)
{
	int varNum,err;
	if (shmGetHandleByTagname(tag, &varNum,&err))
	{
		return shmIsForced(varNum);
	}
	return false;
}

bool shmWriteForcedAsStringByTagname(char *tag, char *val)
{
		int varNum,err,len,i;
	if (shmGetHandleByTagname(tag, &varNum,&err))
	{
		len = strlen(val);
		if (0==len) return false;
		else if (1==len)
		{
			if (val[0]=='0') shared_mem->parameter.config[varNum].bits.forcing = 0;
			else shared_mem->parameter.config[varNum].bits.forcing = 1;
		}
		else {
			for (i=0;i<len;i++)
			{
				if (val[i]=='0') {
					shared_mem->parameter.config[varNum].bits.forcing = 0;
					return true;
				}
				if (val[i]=='1') {
					shared_mem->parameter.config[varNum].bits.forcing = 1;
					return true;
				}
			}
		}
	}
	return false;
	
}


char * shmReadInfoAsStringByTagname(char *tag, char *r)
{
	int varNum,err,type,i;
	//static char *t;
	char t[80];
	//char *r;
	for (i=0;i<80;i++) t[i]=0;
	//t="";
	if (shmGetHandleByTagname(tag, &varNum,&err))
	{
		type = readType(varNum);
		//printf("type %d varNum %d\n",type,varNum);
		if (BOOLEAN == type)
		{
			sprintf(t,"[B%c]%d",shmIsForced(varNum)==true?'F':'f',shmReadBoolByHandle(varNum));
			strncpy(r,t,strlen(t)+1);
		}
		else if (INTEGER == type)
		{
			i = shmReadIntByHandle(varNum);
			sprintf(t,"[I%c]%d",shmIsForced(varNum)==true?'F':'f',i);
			strncpy(r,t,strlen(t)+1);
		}
		else if (UINT == type)
		{
			int u = shmReadUIntByHandle(varNum);
			sprintf(t,"[U%c]%d",shmIsForced(varNum)==true?'F':'f',u);
			strncpy(r,t,strlen(t)+1);
		}
		else if (REAL == type)
		{
			sprintf(t,"[R%c]%f",shmIsForced(varNum)==true?'F':'f',shmReadRealByHandle(varNum));
			strncpy(r,t,strlen(t)+1);
		}
		else if (TEXT == type)
		{
			char *x;
			shmGetTextByHandle(varNum, &x, &err);
			sprintf(t,"[Tf]%s",x);
			strncpy(r,t,strlen(t)+1);
		}
	}
	return r;
}

char * shmReadAsStringByTagname(char *tag, char *r)
{
	int varNum,err,type,i;
	//static char *t;
	char t[80];
	//char *r;
	for (i=0;i<80;i++) t[i]=0;
	//t="";
	if (shmGetHandleByTagname(tag, &varNum,&err))
	{
		type = readType(varNum);
		//printf("type %d varNum %d\n",type,varNum);
		if (BOOLEAN == type)
		{
			sprintf(t,"%d",shmReadBoolByHandle(varNum));
			strncpy(r,t,strlen(t)+1);
		}
		else if (INTEGER == type)
		{
			i = shmReadIntByHandle(varNum);
			sprintf(t,"%d\0",i);
			strncpy(r,t,strlen(t)+1);
		}
		else if (UINT == type)
		{
			int u = shmReadUIntByHandle(varNum);
			sprintf(t,"%d\0",u);
			strncpy(r,t,strlen(t)+1);
		}
		else if (REAL == type)
		{
			sprintf(t,"%f",shmReadRealByHandle(varNum));
			strncpy(r,t,strlen(t)+1);
		}
		else if (TEXT == type)
		{
			shmGetTextByHandle(varNum, &r, &err);
		}
	}
	return r;
}

bool  shmWriteAsStringByTagname(char *tag, char *val)
{
	int varNum,err,type,len,i;
	if (shmGetHandleByTagname(tag, &varNum,&err))
	{
		type = readType(varNum);
		len = strlen(val);
//		printf("len %d \n",len);
		if (BOOLEAN == type)
		{
			if (0==len) return false;
			else if (1==len)
			{
				if (val[0]=='0') shmWriteBoolByHandle(varNum,0);
				else shmWriteBoolByHandle(varNum,1);
			}
			else {
				for (i=0;i<len;i++)
				{
					if (val[i]=='0') {
						shmWriteBoolByHandle(varNum,0);
						return true;
					}
					if (val[i]=='1') {
						shmWriteBoolByHandle(varNum,1);
						return true;
					}
				}
			}
		}
		else if (INTEGER == type || UINT == type)
		{
			if (0==len) return false;
			i = atoi(val);
			//printf("type %d varNum %d val %d \n",type,varNum,i);
			if (UINT == type) shmWriteUIntByHandle(varNum,(unsigned int )(i<0?abs(i):i));
			else shmWriteIntByHandle(varNum,i);
		}
		else if (REAL == type)
		{
			float f;
			if (0==len) return false;
			f = atof(val);
			shmWriteRealByHandle(varNum,f);
		}
		else if (TEXT == type)
		{
			char tmpstr[80];
			sprintf(tmpstr,"");
			sprintf(tmpstr,"%s",val);
			tmpstr[79]='\0';
			shmSetTextByHandle(varNum,tmpstr,&err);
		}
		else return false;
		return true;
	}
	else return false;
}



bool shmGetTypeAndHandleByTagname(char *s, int *iType, int *iHandle)
{
	int varNum,err;
	if (shmGetHandleByTagname(s, &varNum,&err)) {
		*iType = readType(varNum);
		*iHandle = varNum;
		return true;
	}
	else
		return false;	
}

bool shmGetIntByTagname(char *s, int *iValue)
{
	int type,varNum;
	if (shmGetTypeAndHandleByTagname(s,&type,&varNum))
	{
		if (type == INTEGER)
		{
			*iValue = shmReadIntByHandle (varNum);
         return true;
		}
		else
			return false;
	}
//	else
	return false;
	
}
bool shmGetRealByTagname(char *s, float *rValue)
{
	int type,varNum;
	if (shmGetTypeAndHandleByTagname(s,&type,&varNum))
	{
		if (type == REAL)
		{
			*rValue = shmReadRealByHandle (varNum);
         return true;
		}
		else
			return false;
	}
//	else
	return false;
}
bool shmGetBoolByTagname(char *s, bool *bValue)
{
	int type,varNum;
	if (shmGetTypeAndHandleByTagname(s,&type,&varNum))
	{
		if (type == BOOLEAN)
		{
			*bValue = shmReadBoolByHandle (varNum);
         return true;
		}
		else
			return false;
	}
//	else
	return false;
}


bool shmGetHandleByTagname(char *s, int *iHandle , int *errCode)
{
  int i,j,len;
  if (shared_mem->tag.status == 0) //no tagnames
  {
    *errCode = 3; //no tagnames
    return 0;
  }
  else if (shared_mem->tag.status>0) //not sorted do linear search
  {
    for (i=0;i<TAGNAMES;i++)
    {
      if (s[i]=='\0') break; //find length of s
    }
    if (i>29)
    {
      *iHandle = -1;
      *errCode = 1; //too long or no \0
      return 0;
    }
      len = i;
    for (i=0;i<TAGNAMES;i++)
    {
      for(j=0;j<len;j++)
        { 
          if (shared_mem->tag.name[i][j] != s[j]) break;
        }
      if (j==len && shared_mem->tag.name[i][j+1]=='\0') //match stop looking 
      {
        *iHandle = shared_mem->tag.iRef[i];
        *errCode = 0;
        return 1;
      }
      if (shared_mem->tag.name[i][0]=='\0' || shared_mem->tag.iRef[i]==-1) 
      {
        *iHandle = -1;
        *errCode = 2; //not found
        return 0; 
      }
    }
    *iHandle = -1;
    *errCode = 2; //not found
    return 0;
  }
  else // sorted name tag list do binary search
  {
    int first=0;
    int last=(-shared_mem->tag.status)-1;
	
	//printf("first %d last %d\n",first,last);
	
    while (first<=last)
    {
      int middle = (first+last)/2;
      int comp = strcmp((const char *)s,(const char *)(shared_mem->tag.name[middle]));
      if (comp == 0) 
      {
        *iHandle = shared_mem->tag.iRef[middle];
        *errCode = 0;
        return 1;
      }//string found
      else if (comp > 0)
      {
        first = middle + 1;
		//printf("Down first %d last %d\n",first,last);
      }
      else
      {
        last = middle - 1;
		//printf("Up first %d last %d\n",first,last);
      }
    }
    *iHandle = -1;
    *errCode = 2; //not found
    return 0;
  }
}

bool  shmUIntLimitsByHandle	  		(int varNum, unsigned int limits[3], unsigned char function)
{
	int tmp;
	unsigned int tmpLim[3];
	if (varNum<0 || varNum>=PARAMETERS) return 0;
	if (!isInitialized(varNum)) return 0;
	if (readType(varNum) != UINT) return 0;
	tmp = readReference(varNum);

	if (function & SET_MIN) tmpLim[_Min] = limits[_Min];
	else tmpLim[_Min] = shared_mem->uinteger.Min[tmp];
	
	if (function & SET_MAX) tmpLim[_Max] = limits[_Max];
	else tmpLim[_Max] = shared_mem->uinteger.Max[tmp];
	
	if (function & SET_DEF) tmpLim[_Def] = limits[_Def];
	else tmpLim[_Def] = shared_mem->uinteger.Default[tmp];
	
	if (tmpLim[_Min] > tmpLim[_Max]) return 0;
	if (tmpLim[_Min] > tmpLim[_Def]) return 0;
	if (tmpLim[_Max] < tmpLim[_Def]) return 0;
	
	if (shared_mem->uinteger.Value[tmp] > tmpLim[_Max]) shared_mem->uinteger.Value[tmp] = tmpLim[_Max];
	if (shared_mem->uinteger.Value[tmp] < tmpLim[_Min]) shared_mem->uinteger.Value[tmp] = tmpLim[_Min];

	shared_mem->uinteger.Min[tmp] = tmpLim[_Min];
	shared_mem->uinteger.Max[tmp] = tmpLim[_Max];
	shared_mem->uinteger.Default[tmp] = tmpLim[_Def];
	
	return 1;
}
bool  shmIntLimitsByHandle	  		(int varNum, int limits[3], unsigned char function)
{
	int tmp;
	int tmpLim[3];
	if (varNum<0 || varNum>=PARAMETERS) return 0;
	if (!isInitialized(varNum)) return 0;
	if (readType(varNum) != INTEGER) return 0;
	tmp = readReference(varNum);

	if (function & SET_MIN) tmpLim[_Min] = limits[_Min];
	else tmpLim[_Min] = shared_mem->integer.Min[tmp];
	
	if (function & SET_MAX) tmpLim[_Max] = limits[_Max];
	else tmpLim[_Max] = shared_mem->integer.Max[tmp];
	
	if (function & SET_DEF) tmpLim[_Def] = limits[_Def];
	else tmpLim[_Def] = shared_mem->integer.Default[tmp];
	
	if (tmpLim[_Min] > tmpLim[_Max]) return 0;
	if (tmpLim[_Min] > tmpLim[_Def]) return 0;
	if (tmpLim[_Max] < tmpLim[_Def]) return 0;
	
	if (shared_mem->integer.Value[tmp] > tmpLim[_Max]) shared_mem->integer.Value[tmp] = tmpLim[_Max];
	if (shared_mem->integer.Value[tmp] < tmpLim[_Min]) shared_mem->integer.Value[tmp] = tmpLim[_Min];

	shared_mem->integer.Min[tmp] = tmpLim[_Min];
	shared_mem->integer.Max[tmp] = tmpLim[_Max];
	shared_mem->integer.Default[tmp] = tmpLim[_Def];
	
	return 1;
}
bool  shmRealLimitsByHandle	  		(int varNum, float limits[3], unsigned char function)
{
	int tmp;
	float tmpLim[3];
	if (varNum<0 || varNum>=PARAMETERS) return 0;
	if (!isInitialized(varNum)) return 0;
	if (readType(varNum) != REAL) return 0;
	tmp = readReference(varNum);

	if (function & SET_MIN) tmpLim[_Min] = limits[_Min];
	else tmpLim[_Min] = shared_mem->real.Min[tmp];
	
	if (function & SET_MAX) tmpLim[_Max] = limits[_Max];
	else tmpLim[_Max] = shared_mem->real.Max[tmp];
	
	if (function & SET_DEF) tmpLim[_Def] = limits[_Def];
	else tmpLim[_Def] = shared_mem->real.Default[tmp];
	
	if (tmpLim[_Min] > tmpLim[_Max]) return 0;
	if (tmpLim[_Min] > tmpLim[_Def]) return 0;
	if (tmpLim[_Max] < tmpLim[_Def]) return 0;
	
	if (shared_mem->real.Value[tmp] > tmpLim[_Max]) shared_mem->real.Value[tmp] = tmpLim[_Max];
	if (shared_mem->real.Value[tmp] < tmpLim[_Min]) shared_mem->real.Value[tmp] = tmpLim[_Min];

	shared_mem->real.Min[tmp] = tmpLim[_Min];
	shared_mem->real.Max[tmp] = tmpLim[_Max];
	shared_mem->real.Default[tmp] = tmpLim[_Def];
	
	return 1;
}

unsigned int *shmUIntReadLimitsByHandle  (int varNum)
{
	int tmp;
	if (varNum<0 || varNum>=PARAMETERS) return 0;
	if (!isInitialized(varNum)) return 0;
	if (readType(varNum) != UINT) return 0;
	tmp = readReference(varNum);
	static unsigned int r[4];
	if (shared_mem->parameter.config[varNum].bits.disableRangeCheck){
		r[_Min] = 0;
		r[_Max] = 0xffffffff;
		r[_Def] = shared_mem->real.Default[tmp];
		r[_Val] = shared_mem->real.Value[tmp];
		return r;	
	}
	r[_Min] = shared_mem->real.Min[tmp];
	r[_Max] = shared_mem->real.Max[tmp];
	r[_Def] = shared_mem->real.Default[tmp];
	r[_Val] = shared_mem->real.Value[tmp];
	return r;
} 
int *shmIntReadLimitsByHandle            (int varNum)
{
	int tmp;
	if (varNum<0 || varNum>=PARAMETERS) return 0;
	if (!isInitialized(varNum)) return 0;
	if (readType(varNum) != INTEGER) return 0;
	tmp = readReference(varNum);
	static int r[4];
	if (shared_mem->parameter.config[varNum].bits.disableRangeCheck){
		r[_Min] = 0x10000000;
		r[_Max] = 0x7fffffff;
		r[_Def] = shared_mem->real.Default[tmp];
		r[_Val] = shared_mem->real.Value[tmp];
		return r;	
	}
	r[_Min] = shared_mem->real.Min[tmp];
	r[_Max] = shared_mem->real.Max[tmp];
	r[_Def] = shared_mem->real.Default[tmp];
	r[_Val] = shared_mem->real.Value[tmp];
	return r;
}
float *shmRealReadLimitsByHandle         (int varNum)
{
	int tmp;
	if (varNum<0 || varNum>=PARAMETERS) return 0;
	if (!isInitialized(varNum)) return 0;
	if (readType(varNum) != REAL) return 0;
	tmp = readReference(varNum);
		static float r[4];
	if (shared_mem->parameter.config[varNum].bits.disableRangeCheck){
		r[_Min] = 0.0;
		r[_Max] = 0.0;
		r[_Def] = shared_mem->real.Default[tmp];
		r[_Val] = shared_mem->real.Value[tmp];
		return r;	
	}
	r[_Min] = shared_mem->real.Min[tmp];
	r[_Max] = shared_mem->real.Max[tmp];
	r[_Def] = shared_mem->real.Default[tmp];
	r[_Val] = shared_mem->real.Value[tmp];
	return r;
}

void sortTagnames(void)
{
int i,x,y,n;
char temp[MAXTAGNAMELEN];
int tempRef;
n=shared_mem->tag.status; // number of names
/*Bubble Sort*/
for(x=0; x<n; x++)
  {
    for(y=0; y<n-1; y++)
    {
      if((strcmp((const char *)(shared_mem->tag.name[y]),(const char *)shared_mem->tag.name[y+1]))>0)
      {
		
        //temp = shared_mem->tag.name[y+1];
        for (i=0;i<MAXTAGNAMELEN;i++) temp[i]=shared_mem->tag.name[y+1][i];
        tempRef = shared_mem->tag.iRef[y+1];
        //shared_mem->tag.name[y+1] = shared_mem->tag.name[y];
        for (i=0;i<MAXTAGNAMELEN;i++) shared_mem->tag.name[y+1][i]=shared_mem->tag.name[y][i];
        shared_mem->tag.iRef[y+1] = shared_mem->tag.iRef[y];
        //shared_mem->tag.name[y] = temp;
        for (i=0;i<MAXTAGNAMELEN;i++) shared_mem->tag.name[y][i]=temp[i];
        shared_mem->tag.iRef[y]=tempRef;
		if (tempRef == -1) printf("tempRef %d %s %s\n",tempRef,shared_mem->tag.name[y],shared_mem->tag.name[y+1]);
      }
    }
  }
shared_mem->tag.status = -shared_mem->tag.status; //mark sorted
};


void sortTagnames2(void)
{
int i,x,y,n;
char temp[MAXTAGNAMELEN];
int tempRef;
n=shared_mem->tag.status; // number of names

for(x=0; x<n; x++)
  {
    for(y=0; y<n-1; y++)
    {
      if((strcmp((const char *)(shared_mem->tag.name[y]),(const char *)shared_mem->tag.name[y+1]))>0)
      {
        //temp = shared_mem->tag.name[y+1];
        for (i=0;i<MAXTAGNAMELEN;i++) temp[i]=shared_mem->tag.name[y+1][i];
        tempRef = shared_mem->tag.iRef[y+1];
        //shared_mem->tag.name[y+1] = shared_mem->tag.name[y];
        /*for (i=0;i<MAXTAGNAMELEN;i++) shared_mem->tag.name[y+1][i]=shared_mem->tag.name[y][i];
        shared_mem->tag.iRef[y+1] = shared_mem->tag.iRef[y];
        //shared_mem->tag.name[y] = temp;
        for (i=0;i<MAXTAGNAMELEN;i++) shared_mem->tag.name[y][i]=temp[i];
        shared_mem->tag.iRef[y]=tempRef;*/
		for (i=0;i<MAXTAGNAMELEN;i++) {
			shared_mem->tag.name[y+1][i]=shared_mem->tag.name[y][i];
			shared_mem->tag.name[y][i]=temp[i];
		}
        shared_mem->tag.iRef[y+1] = shared_mem->tag.iRef[y];
		shared_mem->tag.iRef[y]=tempRef;
      }
    }
  }
shared_mem->tag.status = -shared_mem->tag.status; //mark sorted
};

void swapTags(int a, int b)
{
	int i;
	char temp[MAXTAGNAMELEN];
	int tempRef;
	for (i=0;i<MAXTAGNAMELEN;i++) temp[i]=shared_mem->tag.name[b][i];
    tempRef = shared_mem->tag.iRef[b];
	for (i=0;i<MAXTAGNAMELEN;i++) {
		shared_mem->tag.name[b][i]=shared_mem->tag.name[a][i];
		shared_mem->tag.name[a][i]=temp[i];
	}
    shared_mem->tag.iRef[b] = shared_mem->tag.iRef[a];
	shared_mem->tag.iRef[a]=tempRef;
}

/*void sortTagnames(void)
{
	int x;
	for (x=0;x<shared_mem->tag.status;x+=2)
	{
		if((strcmp((const char *)(shared_mem->tag.name[x]),(const char *)shared_mem->tag.name[x+1]))>0) swapTags(x,x+1);
	}
	for (x=0;x<shared_mem->tag.status;x+=4)
	{
		if((strcmp((const char *)(shared_mem->tag.name[x]),(const char *)shared_mem->tag.name[x+1]))>0) {
			swapTags(x,x+1);
			if((strcmp((const char *)(shared_mem->tag.name[x+1]),(const char *)shared_mem->tag.name[x+2]))>0) {
				swapTags(x+1,x+2);
				if((strcmp((const char *)(shared_mem->tag.name[x+2]),(const char *)shared_mem->tag.name[x+3]))>0) {
					swapTags(x+2,x+3);
				}
			}
		}
	}
}*/

//#define MAX 10  // This is the default size of every string 

 //MergeSort(arr,0,size-1);

void Merge(char* arr[],int low,int mid,int high) //Merging the Array Function
{
    int nL= mid-low+1;
    int nR= high-mid;

    char** L=malloc(sizeof(char *)*nL);
    char** R=malloc(sizeof(char *)*nR);
    int i;
    for(i=0;i<nL;i++)
    {
        L[i]=malloc(sizeof(arr[low+i]));
        strcpy(L[i],arr[low+i]);
    }
    for(i=0;i<nR;i++)
    {
        R[i]=malloc(sizeof(arr[mid+i+1]));
        strcpy(R[i],arr[mid+i+1]);
    }
    int j=0,k;
    i=0;
    k=low;
    while(i<nL&&j<nR)
    {
        if(strcmp(L[i],R[j])<0)strcpy(arr[k++],L[i++]);
        else strcpy(arr[k++],R[j++]);
    }
    while(i<nL)strcpy(arr[k++],L[i++]);
    while(j<nR)strcpy(arr[k++],R[j++]);

}


void MergeSort(char* arr[],int low,int high) //Main MergeSort function
{
    /*if(low<high)
    {
        int mid=(low+high)/2;
        MergeSort(arr,low,mid);
        MergeSort(arr,mid+1,high);
        Merge(arr,low,mid,high);
    }*/
	
	if(low<high)
    {
        int mid=(low+high)/2;
        MergeSort(arr,low,mid);
        MergeSort(arr,mid+1,high);
        Merge(arr,low,mid,high);
    }
	
}

#ifdef __cplusplus
}
#endif


