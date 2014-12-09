#pragma once

typedef enum  _OB_TYPE_INDEX
{
	OB_TYPE_INDEX_NULL,				 //0
	OB_TYPE_INDEX_TYPE,				 //1 // [ObjT] "Type"
	OB_TYPE_INDEX_DIRECTORY,         //2 // [Dire] "Directory"
	OB_TYPE_INDEX_SYMBOLIC_LINK,     //3 // [Symb] "SymbolicLink"
	OB_TYPE_INDEX_TOKEN,             //4 // [Toke] "Token"
	OB_TYPE_INDEX_PROCESS,           //5 // [Proc] "Process"
	OB_TYPE_INDEX_THREAD,            //6 // [Thre] "Thread"
	OB_TYPE_INDEX_JOB,               //7 // [Job ] "Job"
	OB_TYPE_INDEX_EVENT,             //8 // [Even] "Event"
	OB_TYPE_INDEX_EVENT_PAIR,        //9 // [Even] "EventPair"
	OB_TYPE_INDEX_MUTANT,            //10 // [Muta] "Mutant"
	OB_TYPE_INDEX_CALLBACK,          //11 // [Call] "Callback"
	OB_TYPE_INDEX_SEMAPHORE,         //12 // [Sema] "Semaphore"
	OB_TYPE_INDEX_TIMER,             //13 // [Time] "Timer"
	OB_TYPE_INDEX_PROFILE,           //14 // [Prof] "Profile"
	OB_TYPE_INDEX_WINDOW_STATION,    //15 // [Wind] "WindowStation"
	OB_TYPE_INDEX_DESKTOP,           //16 // [Desk] "Desktop"
	OB_TYPE_INDEX_SECTION,           //17 // [Sect] "Section"
	OB_TYPE_INDEX_KEY,               //18 // [Key ] "Key"
	OB_TYPE_INDEX_PORT,              //19 // [Port] "Port"
	OB_TYPE_INDEX_WAITABLE_PORT,     //20 // [Wait] "WaitablePort"
	OB_TYPE_INDEX_ADAPTER,           //21 // [Adap] "Adapter"
	OB_TYPE_INDEX_CONTROLLER,        //22 // [Cont] "Controller"
	OB_TYPE_INDEX_DEVICE,            //23 // [Devi] "Device"
	OB_TYPE_INDEX_DRIVER,            //24 // [Driv] "Driver"
	OB_TYPE_INDEX_IO_COMPLETION,     //25 // [IoCo] "IoCompletion"
	OB_TYPE_INDEX_FILE,              //26 // [File] "File"
	OB_TYPE_INDEX_WMI_GUID,          //27 // [WmiG] "WmiGuid" 
}OB_TYPE_INDEX;

// -----------------------
// system information
// -----------------------
typedef enum _SYSTEM_INFORMATION_CLASS     //    Q S
{
	SystemBasicInformation,                // 00 Y N
	SystemProcessorInformation,            // 01 Y N
	SystemPerformanceInformation,          // 02 Y N
	SystemTimeOfDayInformation,            // 03 Y N
	SystemNotImplemented1,                 // 04 Y N
	SystemProcessesAndThreadsInformation,  // 05 Y N
	SystemCallCounts,                      // 06 Y N
	SystemConfigurationInformation,        // 07 Y N
	SystemProcessorTimes,                  // 08 Y N
	SystemGlobalFlag,                      // 09 Y Y
	SystemNotImplemented2,                 // 10 Y N
	SystemModuleInformation,               // 11 Y N
	SystemLockInformation,                 // 12 Y N
	SystemNotImplemented3,                 // 13 Y N
	SystemNotImplemented4,                 // 14 Y N
	SystemNotImplemented5,                 // 15 Y N
	SystemHandleInformation,               // 16 Y N
	SystemObjectInformation,               // 17 Y N
	SystemPagefileInformation,             // 18 Y N
	SystemInstructionEmulationCounts,      // 19 Y N
	SystemInvalidInfoClass1,               // 20
	SystemCacheInformation,                // 21 Y Y
	SystemPoolTagInformation,              // 22 Y N
	SystemProcessorStatistics,             // 23 Y N
	SystemDpcInformation,                  // 24 Y Y
	SystemNotImplemented6,                 // 25 Y N
	SystemLoadImage,                       // 26 N Y
	SystemUnloadImage,                     // 27 N Y
	SystemTimeAdjustment,                  // 28 Y Y
	SystemNotImplemented7,                 // 29 Y N
	SystemNotImplemented8,                 // 30 Y N
	SystemNotImplemented9,                 // 31 Y N
	SystemCrashDumpInformation,            // 32 Y N
	SystemExceptionInformation,            // 33 Y N
	SystemCrashDumpStateInformation,       // 34 Y Y/N
	SystemKernelDebuggerInformation,       // 35 Y N
	SystemContextSwitchInformation,        // 36 Y N
	SystemRegistryQuotaInformation,        // 37 Y Y
	SystemLoadAndCallImage,                // 38 N Y
	SystemPrioritySeparation,              // 39 N Y
	SystemNotImplemented10,                // 40 Y N
	SystemNotImplemented11,                // 41 Y N
	SystemInvalidInfoClass2,               // 42
	SystemInvalidInfoClass3,               // 43
	SystemTimeZoneInformation,             // 44 Y N
	SystemLookasideInformation,            // 45 Y N
	SystemSetTimeSlipEvent,                // 46 N Y
	SystemCreateSession,                   // 47 N Y
	SystemDeleteSession,                   // 48 N Y
	SystemInvalidInfoClass4,               // 49
	SystemRangeStartInformation,           // 50 Y N
	SystemVerifierInformation,             // 51 Y Y
	SystemAddVerifier,                     // 52 N Y
	SystemSessionProcessesInformation      // 53 Y N
} SYSTEM_INFORMATION_CLASS;

// ---------------------------
// SystemProcessesAndThreadsInformation 5
// ---------------------------
typedef struct _SYSTEM_THREAD {    
	LARGE_INTEGER           KernelTime;    
	LARGE_INTEGER           UserTime;    
	LARGE_INTEGER           CreateTime;    
	ULONG                   WaitTime;    
	PVOID                   StartAddress;    
	CLIENT_ID               ClientId;    
	KPRIORITY               Priority;    
	LONG                    BasePriority;    
	ULONG                   ContextSwitchCount;    
	ULONG                   State;    
	KWAIT_REASON            WaitReason;    
} SYSTEM_THREAD, *PSYSTEM_THREAD;    

typedef struct _SYSTEM_PROCESS_INFORMATION {    
	ULONG                   NextEntryOffset;	
	ULONG                   NumberOfThreads;	
	LARGE_INTEGER           Reserved[3];    
	LARGE_INTEGER           CreateTime;				
	LARGE_INTEGER           UserTime;			
	LARGE_INTEGER           KernelTime;				
	UNICODE_STRING          ImageName;			
	KPRIORITY               BasePriority;			
	HANDLE                  ProcessId;				
	HANDLE                  InheritedFromProcessId;	
	ULONG                   HandleCount;			
	ULONG                   Reserved2[2];    
	ULONG                   PrivatePageCount;    
	VM_COUNTERS             VirtualMemoryCounters;  
	IO_COUNTERS             IoCounters;				
	SYSTEM_THREAD           Threads[1];				
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;   

// ---------------------------
// SystemModuleInformation 11
// ---------------------------
typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	CHAR FullPathName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;


typedef struct _SYSTEM_PROCESS_MODULES
{
	ULONG NumberOfModules;
	SYSTEM_MODULE_INFORMATION Modules[1];
} SYSTEM_PROCESS_MODULES, *PSYSTEM_PROCESS_MODULES;

// ---------------------------
// SystemHandleInformation 16
// ---------------------------
typedef struct _SYSTEM_HANDLE_INFORMATION 
{
	ULONG ProcessId;
	UCHAR ObjectTypeNumber;
	UCHAR Flags;						// 0x01 = PROTECT_FROM_CLOSE, 0x02 = INHERIT
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _SYSTEM_HANDLE_INFORMATION_LIST
{
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_INFORMATION Information[1];
} SYSTEM_HANDLE_INFORMATION_LIST,  *PSYSTEM_HANDLE_INFORMATION_LIST;

/*
typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllInformation,
	ObjectDataInformation
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;
*/

#define  ObjectNameInformation		1

//basic
typedef struct _OBJECT_BASIC_INFORMATION {
	ULONG                   Attributes;
	ACCESS_MASK             DesiredAccess;
	ULONG                   HandleCount;
	ULONG                   ReferenceCount;
	ULONG                   PagedPoolUsage;
	ULONG                   NonPagedPoolUsage;
	ULONG                   Reserved[3];
	ULONG                   NameInformationLength;
	ULONG                   TypeInformationLength;
	ULONG                   SecurityDescriptorLength;
	LARGE_INTEGER           CreationTime;
} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

//type
typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING          TypeName;
	ULONG                   TotalNumberOfHandles;
	ULONG                   TotalNumberOfObjects;
	WCHAR                   Unused1[8];
	ULONG                   HighWaterNumberOfHandles;
	ULONG                   HighWaterNumberOfObjects;
	WCHAR                   Unused2[8];
	ACCESS_MASK             InvalidAttributes;
	GENERIC_MAPPING         GenericMapping;
	ACCESS_MASK             ValidAttributes;
	BOOLEAN                 SecurityRequired;
	BOOLEAN                 MaintainHandleCount;
	USHORT                  MaintainTypeList;
	POOL_TYPE               PoolType;
	ULONG                   DefaultPagedPoolCharge;
	ULONG                   DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

//all
typedef struct _OBJECT_ALL_INFORMATION {
	ULONG                   NumberOfObjectsTypes;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
} OBJECT_ALL_INFORMATION, *POBJECT_ALL_INFORMATION;

//data
typedef struct _OBJECT_DATA_INFORMATION {
	BOOLEAN                 InheritHandle;
	BOOLEAN                 ProtectFromClose;
} OBJECT_DATA_INFORMATION, *POBJECT_DATA_INFORMATION;



// ---------------------------
// handle table 
// ---------------------------
typedef struct _HANDLE_TRACE_DB_ENTRY
{
	CLIENT_ID ClientId;
	PVOID Handle;
	ULONG Type;
	VOID * StackTrace[16];
} HANDLE_TRACE_DB_ENTRY, *PHANDLE_TRACE_DB_ENTRY;

typedef struct _HANDLE_TABLE_ENTRY_INFO
{
	ULONG AuditMask;
} HANDLE_TABLE_ENTRY_INFO, *PHANDLE_TABLE_ENTRY_INFO;

typedef struct _HANDLE_TABLE_ENTRY
{
	union
	{
		PVOID Object;
		ULONG ObAttributes;
		PHANDLE_TABLE_ENTRY_INFO InfoTable;
		ULONG Value;
	};
	union
	{
		ULONG GrantedAccess;
		struct
		{
			WORD GrantedAccessIndex;
			WORD CreatorBackTraceIndex;
		};
		LONG NextFreeTableEntry;
	};
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;


typedef struct _HANDLE_TRACE_DEBUG_INFO
{
	LONG RefCount;
	ULONG TableSize;
	ULONG BitMaskFlags;
	FAST_MUTEX CloseCompactionLock;
	ULONG CurrentStackIndex;
	HANDLE_TRACE_DB_ENTRY TraceDb[1];
} HANDLE_TRACE_DEBUG_INFO, *PHANDLE_TRACE_DEBUG_INFO;


typedef struct _HANDLE_TABLE
{
	ULONG   TableCode;
	PEPROCESS QuotaProcess;
	PVOID UniqueProcessId;
	EX_PUSH_LOCK HandleLock;
	LIST_ENTRY HandleTableList;
	EX_PUSH_LOCK HandleContentionEvent;
	PHANDLE_TRACE_DEBUG_INFO DebugInfo;
	LONG ExtraInfoPages;
	ULONG Flags;
	ULONG StrictFIFO: 1;
	LONG FirstFreeHandle;
	PHANDLE_TABLE_ENTRY LastFreeHandleEntry;
	LONG HandleCount;
	ULONG NextHandleNeedingPool;
} HANDLE_TABLE, *PHANDLE_TABLE;


//ped ldr table
typedef struct _PEB_LDR_DATA
{
	DWORD		dwLen;
	UCHAR		bInitialized;
	PVOID		Sshandle;
	LIST_ENTRY  InLoadOrderModuleList;
	LIST_ENTRY  InMemoryOrderModuleList;
	LIST_ENTRY  InInitializationOrderModuleList;
	PVOID	    EntryInProgress;
}PEB_LDR_DATA, *PPEB_LDR_DATA;


typedef struct _LDR_DATA_TABLE_ENTRY
{ 
	LIST_ENTRY     InLoadOrderLinks;
	LIST_ENTRY     InMemoryOrderLinks;
	LIST_ENTRY     InInitializationOrderLinks;
	PVOID          DllBase;
	PVOID          EntryPoint;
	ULONG          SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG          Flags;
	USHORT         LoadCount;
	USHORT         TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		struct
		{
			PVOID  SectionPointer;
			ULONG  CheckSum;
		};
	};
	union
	{
		ULONG      TimeDateStamp;
		PVOID      LoadedImports;
	};
	PVOID     EntryPointActivationContext;
	PVOID     PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

//ssdt
typedef struct _KSERVICE_DESCRIPTOR_TABLE
{
	PULONG ServiceTableBase;
	PULONG ServiceCounterTableBase;
	ULONG  NumberOfServices;
	PUCHAR ParamTableBase;
}KSERVICE_DESCRIPTOR_TABLE, *PKSERVICE_DESCRIPTOR_TABLE;