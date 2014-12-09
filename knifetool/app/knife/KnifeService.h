#pragma once



class CKnifeService
{
public:
	CKnifeService(void);
	~CKnifeService(void);
};

// ----------------------------
// install & uninstall service
// ----------------------------
extern BOOL KStartService();
extern BOOL KStopService();

// ----------------------------
// kernel functions
// ----------------------------
//
extern BOOL KEnumProcessesByNativeApi(PROCESS_INFO_LIST &ProcInfoList);
extern BOOL KTerminateProcessByAPC(DWORD dwPID);
extern BOOL KHideProcess(DWORD dwPID);

//
extern BOOL KEnumProcessThreads(DWORD dwPID, PROCESS_THREAD_INFO_LIST &ProcThreadList);

//
extern BOOL KEnumProcessModules(DWORD dwPID, PROCESS_MODULE_INFO_LIST &ProcModList);
extern BOOL KHideProcessModule(PROCESS_MODULE_BASE ProcModBase);

//
extern BOOL KEnumProcessObjects(DWORD dwPID,  PROCESS_OBJECT_INFO_LIST &ProcObjList);
extern BOOL KCloseHandle(PROCESS_OBJECT_HANDLE objHandle);

//
extern BOOL KEnumSystemModules(SYS_MODULE_INFO_LIST &SysModList);

//
extern BOOL KEnumSSDT(SSDT_INFO_LIST &ssdtList);




