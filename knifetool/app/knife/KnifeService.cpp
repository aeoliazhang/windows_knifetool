#include "StdAfx.h"
#include "KnifeService.h"
#include <winsvc.h>  
#include "IOCtrlCode.h"


#define KNIFE_SYMBOL_LINK					_T("\\\\.\\knife")
#define KNIFE_SERVICE_NAME					_T("knife")
#define KNIFE_DRIVER_FILE					_T("knife.sys")


CKnifeService::CKnifeService(void)
{
}

CKnifeService::~CKnifeService(void)
{
}

BOOL KStartService()
{
	BOOL bRet = FALSE;

	TCHAR strAppPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, strAppPath, MAX_PATH);
	*_tcsrchr(strAppPath,'\\') = 0;	
	TCHAR strDriverPath[MAX_PATH] = {0};
	_stprintf_s(strDriverPath, _T("%s\\%s"), strAppPath, KNIFE_DRIVER_FILE);
	
	SC_HANDLE hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if(hServiceMgr != NULL)
	{		
		SC_HANDLE hService = CreateService( hServiceMgr,
			KNIFE_SERVICE_NAME, 
			KNIFE_SERVICE_NAME,
			SERVICE_ALL_ACCESS, 
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START, 
			SERVICE_ERROR_IGNORE,
			strDriverPath,
			NULL,  
			NULL,  
			NULL,  
			NULL,
			NULL);

		if( (hService != NULL) ||
			(GetLastError() == ERROR_SERVICE_EXISTS &&
			(hService = OpenService( hServiceMgr, KNIFE_SERVICE_NAME, SERVICE_ALL_ACCESS )) != NULL))
		{
			if( (StartService( hService, NULL, NULL) ) ||
				(GetLastError() == ERROR_SERVICE_ALREADY_RUNNING))
			{
				bRet = TRUE;
			}
			//start Service error
			else
			{
				AfxMessageBox(GetLastErrorString());
			}	
			CloseServiceHandle(hService);	
			CloseServiceHandle(hServiceMgr);	
		}
		//CreateService || OpenService error
		else
		{		
			AfxMessageBox(GetLastErrorString());
			CloseServiceHandle(hServiceMgr);		
		}
	}
	//OpenSCManager error 
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KStopService()
{
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );  
	if( hServiceMgr != NULL )  
	{
		SC_HANDLE hService = OpenService( hServiceMgr, KNIFE_SERVICE_NAME, SERVICE_ALL_ACCESS ); 
		if(hService != NULL)
		{
			SERVICE_STATUS svrStatus;			
			if( ControlService( hService, SERVICE_CONTROL_STOP , &svrStatus ) && DeleteService(hService))  
			{  
				bRet = TRUE;	
			} 
			else
			{
				AfxMessageBox(GetLastErrorString());
			}
			CloseServiceHandle(hService);	
			CloseServiceHandle(hServiceMgr);	
		}
		//OpenService error
		else
		{		
			AfxMessageBox(GetLastErrorString());
			CloseServiceHandle(hServiceMgr);		
		}	
	}  
	//OpenSCManager error 
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KEnumProcessesByNativeApi(PROCESS_INFO_LIST &ProcInfoList)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_ENUM_PROCESSES_NATIVEAPI, NULL, 0, &ProcInfoList, sizeof(ProcInfoList), &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KTerminateProcessByAPC(DWORD dwPID)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_TERMINATE_PROCESS_BYAPC, &dwPID, sizeof(dwPID), NULL, 0, &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KHideProcess(DWORD dwPID)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_HIDE_PROCESS_BYACTIVELIST, &dwPID, sizeof(dwPID), NULL, 0, &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KEnumProcessThreads(DWORD dwPID, PROCESS_THREAD_INFO_LIST &ProcThreadList)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{		
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_ENUM_PROCESS_THREADS, &dwPID, sizeof(dwPID), &ProcThreadList, sizeof(ProcThreadList), &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KEnumProcessModules(DWORD dwPID, PROCESS_MODULE_INFO_LIST &ProcModList)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{		
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_ENUM_PROCESS_MODULES, &dwPID, sizeof(dwPID), &ProcModList, sizeof(ProcModList), &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KHideProcessModule(PROCESS_MODULE_BASE ProcModBase)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_HIDE_MODULE_BYPEB, &ProcModBase, sizeof(ProcModBase), NULL, 0, &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KEnumProcessObjects(DWORD dwPID,  PROCESS_OBJECT_INFO_LIST &ProcObjList)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{		
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_ENUM_PROCESS_OBJECTS, &dwPID, sizeof(dwPID), &ProcObjList, sizeof(ProcObjList), &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KCloseHandle(PROCESS_OBJECT_HANDLE objHandle)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_CLOSE_OBJECT, &objHandle, sizeof(objHandle), NULL, 0, &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KEnumSystemModules(SYS_MODULE_INFO_LIST &SysModList)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_ENUM_SYSMODULES, NULL, 0, &SysModList, sizeof(SYS_MODULE_INFO_LIST), &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}

BOOL KEnumSSDT(SSDT_INFO_LIST &ssdtList)
{
	BOOL bRet = FALSE;
	HANDLE hDevice = CreateFile(KNIFE_SYMBOL_LINK, GENERIC_READ|GENERIC_WRITE, 0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{		
		DWORD dwRet = 0;
		bRet = DeviceIoControl(hDevice, IOCTL_KNIFE_ENUM_SSDT, NULL, 0, &ssdtList, sizeof(ssdtList), &dwRet, NULL);
		CloseHandle(hDevice);
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}

	return bRet;
}