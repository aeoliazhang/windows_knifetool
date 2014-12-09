#include "Predef.h"
#include "Threads.h"
#include "Modules.h"

#define MM_TAG_THREAD     'thrd'

#define EPROCESS_THREADLISTHEAD_OFFSET					0x190
#define ETHREAD_THREADLISTENTRY_OFFSET					0x22C
#define ETHREAD_CID_OFFSET								0x1EC  
#define ETHREAD_CREATETIME_OFFSET						0x1C0
#define ETHREAD_STATE_OFFSET							0x02D
#define ETHREAD_KERNELTIME_OFFSET						0x144
#define ETHREAD_USERTIME_OFFSET							0x148
#define ETHREAD_CONTEXTSWITCHES_OFFSET					0x04C
#define ETHREAD_PRIORITY_OFFSET							0x033
#define ETHREAD_BASEPRIORITY_OFFSET						0x06C
#define ETHREAD_STARTADDRESS_OFFSET						0x224
#define ETHREAD_LPCRMID_OFFSET							0x228

// -----------------------
// declaration
// -----------------------
BOOL GetModuleName(IN ULONG ulAddr, PPROCESS_MODULE_INFO_LIST pModules, OUT PWCHAR pszModuleName);

// -----------------------
// define
// -----------------------
BOOL EnumProcessThreads(IN ULONG ulPid, OUT LPVOID pThreadsBuf)
{
	BOOL bRet = FALSE;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ulEproc, ulEThread;
	PLIST_ENTRY TListHead = NULL, TListCur = NULL;
	PCLIENT_ID pClientId;
	PPROCESS_THREAD_INFO_LIST pPTI;
	ULONG ulIndex = 0;
	PPROCESS_MODULE_INFO_LIST pModules = (PPROCESS_MODULE_INFO_LIST)ExAllocatePoolWithTag(NonPagedPool, sizeof(PROCESS_MODULE_INFO_LIST),MM_TAG_THREAD);
	if(pModules != NULL)
	{
		EnumProcessModulesByPeb(ulPid, pModules);	
		if(pThreadsBuf != NULL)
		{		
			NTSTATUS status = PsLookupProcessByProcessId((HANDLE)ulPid, (PEPROCESS*)&ulEproc);
			if(NT_SUCCESS(status))
			{		
				TListHead = (PLIST_ENTRY)(ulEproc + EPROCESS_THREADLISTHEAD_OFFSET);
				if(MmIsAddressValid(TListHead))
				{
					pPTI = (PPROCESS_THREAD_INFO_LIST)pThreadsBuf;
					
					for (TListCur = TListHead->Flink, ulIndex = 0; TListCur != TListHead; TListCur = TListCur->Flink)
					{				
						ulEThread= (ULONG)TListCur - ETHREAD_THREADLISTENTRY_OFFSET;	
						if(MmIsAddressValid((PVOID)ulEThread))
						{
							pClientId=(PCLIENT_ID)(ulEThread + ETHREAD_CID_OFFSET);
							pPTI->ThreadInfo[ulIndex].Tid = (ULONG)pClientId->UniqueThread;						
							pPTI->ThreadInfo[ulIndex].State = *(UCHAR *)(ulEThread + ETHREAD_STATE_OFFSET);					
							pPTI->ThreadInfo[ulIndex].ContextSwitches = *(ULONG *)(ulEThread + ETHREAD_CONTEXTSWITCHES_OFFSET);
							pPTI->ThreadInfo[ulIndex].Priority = *(CHAR *)(ulEThread + ETHREAD_PRIORITY_OFFSET);					
							
							ULONG ulStartAddr = *(ULONG*)(ulEThread + ETHREAD_LPCRMID_OFFSET);
							if (ulStartAddr == 0)
							{
								pPTI->ThreadInfo[ulIndex].StartAddress = *(ULONG *)(ulEThread+ETHREAD_STARTADDRESS_OFFSET);
							}
							else
							{
								pPTI->ThreadInfo[ulIndex].StartAddress = ulStartAddr;
							}
							
							RtlStringCchCopyW(pPTI->ThreadInfo[ulIndex].ModuleName, MAX_PATH, L"unknown");
							GetModuleName(pPTI->ThreadInfo[ulIndex].StartAddress , pModules, pPTI->ThreadInfo[ulIndex].ModuleName);

							KdPrint(("module name  = %ws", pPTI->ThreadInfo[ulIndex].ModuleName));							
							
							ulIndex++;					
							bRet = TRUE;
						}				
					}
					pPTI->ulCount = ulIndex;
				}			
				ObDereferenceObject((PVOID)ulEproc);	
			}
		}
		
		ExFreePool(pModules);
	}	
	
	return bRet;
}


BOOL GetModuleName(IN ULONG ulAddr, PPROCESS_MODULE_INFO_LIST pModules, OUT PWCHAR pszModuleName)
{
	BOOL bRet = FALSE;
	ULONG ulIndex = 0; 
	if( pModules &&  pszModuleName )
	{
		for(ulIndex = 0; ulIndex < pModules->ulCount; ulIndex++)
		{
			if(ulAddr >= pModules->ModuleInfo[ulIndex].DllBase && 
			   ulAddr <= pModules->ModuleInfo[ulIndex].SizeOfImage + pModules->ModuleInfo[ulIndex].DllBase)
			{
				RtlStringCchCopyW(pszModuleName, MAX_PATH, pModules->ModuleInfo[ulIndex].DllPath);	
				bRet = TRUE;
				break;
			}
		}
		if (ulIndex == pModules->ulCount)
		{
			RtlStringCchCopyW(pszModuleName, MAX_PATH,L"Unknown");
		}
	}
	return bRet;
}

