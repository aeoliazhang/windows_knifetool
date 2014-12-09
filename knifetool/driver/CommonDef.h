#pragma once

// -----------------------
// process
// -----------------------
#define MAX_IAMGE_NAME			16
#define PROCESS_MAX_NUM			1024

//processes
typedef struct _PROCESS_INFO
{
	ULONG ulPID;
	WCHAR ImageName[MAX_IAMGE_NAME];
	WCHAR ImagePath[MAX_PATH];
	ULONG ulMemory;
	LONG  Priority; 
	ULONG ulThreads;
}PROCESS_INFO, *PPROCESS_INFO;

typedef struct _PROCESS_INFO_LIST
{
	ULONG        ulCount;
	PROCESS_INFO ProcInfo[PROCESS_MAX_NUM];
}PROCESS_INFO_LIST, *PPROCESS_INFO_LIST;


//Threads 
#define THREAD_MAX_NUM			1024

typedef struct _PROCESS_THREAD_INFO
{	
	ULONG			Tid;	
	ULONG			StartAddress;
	UCHAR			State;
	ULONG			ContextSwitches;
	CHAR			Priority;
	WCHAR			ModuleName[MAX_PATH];
}PROCESS_THREAD_INFO, *PPROCESS_THREAD_INFO;

typedef struct _PROCESS_THREAD_INFO_LIST
{
	ULONG				ulCount;
	PROCESS_THREAD_INFO ThreadInfo[THREAD_MAX_NUM];
}PROCESS_THREAD_INFO_LIST, *PPROCESS_THREAD_INFO_LIST;


#define MODULE_MAX_NUM			1024

//system modules
typedef struct _SYS_MODULE_INFO
{	
	ULONG  ImageBase;
	ULONG  ImageSize;
	ULONG  Flags;
	USHORT LoadOrderIndex;
	CHAR   ModulePath[MAX_PATH];
}SYS_MODULE_INFO, *PSYS_MODULE_INFO;

typedef struct _SYS_MODULE_INFO_LIST
{
	ULONG			ulCount;
	SYS_MODULE_INFO ModuleInfo[MODULE_MAX_NUM];
}SYS_MODULE_INFO_LIST, *PSYS_MODULE_INFO_LIST;

//process modules
typedef struct _PROCESS_MODULE_INFO
{	
	ULONG  DllBase;
	ULONG  SizeOfImage;
	WCHAR  DllPath[MAX_PATH];
}PROCESS_MODULE_INFO, *PPROCESS_MODULE_INFO;

typedef struct _PROCESS_MODULE_INFO_LIST
{
	ULONG				ulCount;
	PROCESS_MODULE_INFO ModuleInfo[MODULE_MAX_NUM];
}PROCESS_MODULE_INFO_LIST, *PPROCESS_MODULE_INFO_LIST;

typedef struct _PROCESS_MODULE_BASE
{
	ULONG Pid;
	ULONG ModBase;
}PROCESS_MODULE_BASE, *PPROCESS_MODULE_BASE;


//process objects
#define OBJECT_MAX_NUM			1024
#define OBJECT_MAX_TYPE			20


typedef struct _PROCESS_OBJECT_INFO
{	
	WCHAR  Type[OBJECT_MAX_TYPE];
	WCHAR  Name[MAX_PATH];
	ULONG  Handle;
	ULONG  DupHandle;
}PROCESS_OBJECT_INFO, *PPROCESS_OBJECT_INFO;

typedef struct _PROCESS_OBJECT_INFO_LIST
{
	ULONG				ulCount;
	PROCESS_OBJECT_INFO ObjectInfo[OBJECT_MAX_NUM];
}PROCESS_OBJECT_INFO_LIST, *PPROCESS_OBJECT_INFO_LIST;

typedef struct _PROCESS_OBJECT_HANDLE
{
	ULONG Pid;
	ULONG Handle;
}PROCESS_OBJECT_HANDLE, *PPROCESS_OBJECT_HANDLE;


// -----------------------
// SSDT
// -----------------------
#define SSDT_MAX_NUM			300

typedef struct _SSDT_INFO
{
	ULONG ulIndex;
	ULONG ulCurrAddress;
	ULONG ulOrgAddress;			
	CHAR  ModuleName[MAX_PATH];
	CHAR  FuncName[MAX_PATH];
}SSDT_INFO,*PSSDT_INFO;

typedef struct _SSDT_INFO_LIST
{
	ULONG ulCount;
	SSDT_INFO SsdtInfo[SSDT_MAX_NUM];
}SSDT_INFO_LIST,*PSSDT_INFO_LIST;


// -----------------------
// ShadowSSDT
// -----------------------

#define SHADOWSSDT_MAX_NUM		700

typedef struct _SHADOWSSDT_INFO
{
	ULONG ulIndex;
	ULONG ulCurrAddress;
	ULONG ulOrgAddress;			
	CHAR  ModuleName[MAX_PATH];
	CHAR  FuncName[MAX_PATH];
}SHADOWSSDT_INFO,*PSHADOWSSDT_INFO;

typedef struct _SHADOWSSDT_INFO_LIST
{
	ULONG ulCount;
	SHADOWSSDT_INFO ShadowSsdtInfo[SHADOWSSDT_MAX_NUM];
}SHADOWSSDT_INFO_LIST,*PSHADOWSSDT_INFO_LIST;