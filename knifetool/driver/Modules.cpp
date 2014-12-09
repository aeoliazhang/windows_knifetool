#include "Predef.h"
#include "Modules.h"

#define MM_TAG_MOD			'MOD'

#define EPROCESS_PEB_OFFSET							0x1B0
#define EPROCESS_PEB_OFFSET							0x1B0
#define PEB_LDR_OFFSET								0xC
#define INLOADORDERMODULELIST_OFFSET				0xC
#define INMEMORYORDERMODULELIST_OFFSET				0x14
#define ININITIALIZATIONORDERMODULELIST_OFFSET		0x1C

NTSTATUS EnumSystemModules(OUT LPVOID pModulesBuf)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG ulRetSize = 0;
	status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &ulRetSize);  
	if ( status == STATUS_INFO_LENGTH_MISMATCH && ulRetSize > 0)  
	{  
		LPVOID  pBuf = ExAllocatePoolWithTag(NonPagedPool, ulRetSize, MM_TAG_MOD);
		if ( pBuf !=NULL )  
		{
			status = ZwQuerySystemInformation(SystemModuleInformation, pBuf, ulRetSize, &ulRetSize); 
			if (NT_SUCCESS(status))
			{			
				PSYSTEM_PROCESS_MODULES spms = (PSYSTEM_PROCESS_MODULES)pBuf;
				PSYSTEM_MODULE_INFORMATION smi = spms->Modules;
				PSYS_MODULE_INFO_LIST sysModList = (PSYS_MODULE_INFO_LIST)pModulesBuf;
				sysModList->ulCount = spms->NumberOfModules;
				for (ULONG i = 0; i < spms->NumberOfModules; i++)
				{			
					sysModList->ModuleInfo[i].ImageBase = (ULONG)smi->ImageBase;
					sysModList->ModuleInfo[i].ImageSize = smi->ImageSize;
					sysModList->ModuleInfo[i].Flags = smi->Flags;
					sysModList->ModuleInfo[i].LoadOrderIndex = smi->LoadOrderIndex;
					RtlStringCchCopyA(sysModList->ModuleInfo[i].ModulePath, MAX_PATH, smi->FullPathName);
					smi++;
				}
			}
			
			ExFreePool(pBuf);
		} 
	}
    return status;
}

BOOL EnumProcessModulesByPeb(IN ULONG ulPid, OUT LPVOID pModulesBuf)
{
	BOOL bRet = FALSE;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ulEproc, ulPeb, ulLdr;
	PLDR_DATA_TABLE_ENTRY ldrEntry = NULL;
	PLIST_ENTRY ldrHeader=NULL, ldrNext = NULL;
	ULONG ulIndex;
	if(pModulesBuf != NULL)
	{		
		NTSTATUS status = PsLookupProcessByProcessId((HANDLE)ulPid, (PEPROCESS*)&ulEproc);
		if(NT_SUCCESS(status))
		{		
			KeAttachProcess((PRKPROCESS)ulEproc);	
			ulPeb = *(ULONG*)(ulEproc + EPROCESS_PEB_OFFSET);
			if(MmIsAddressValid((PVOID)ulPeb))
			{
				ulLdr  = *(ULONG*)(ulPeb + PEB_LDR_OFFSET);
				if(MmIsAddressValid((PVOID)ulLdr))
				{
					ldrHeader=(PLIST_ENTRY)(ulLdr+INLOADORDERMODULELIST_OFFSET);
					if(MmIsAddressValid(ldrHeader))
					{
						PPROCESS_MODULE_INFO_LIST  pPMI = (PPROCESS_MODULE_INFO_LIST)pModulesBuf;
						for (ldrNext = ldrHeader->Flink, ulIndex = 0; ldrNext != ldrHeader; ldrNext = ldrNext->Flink)
						{
							ldrEntry = CONTAINING_RECORD (ldrNext, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks); 
							if(MmIsAddressValid(ldrEntry))
							{							
								pPMI->ModuleInfo[ulIndex].DllBase = (ULONG)ldrEntry->DllBase;
								pPMI->ModuleInfo[ulIndex].SizeOfImage = (ULONG)ldrEntry->SizeOfImage;
								KdPrint(("base = 0x%08X", ldrEntry->DllBase));
								if(ldrEntry->FullDllName.Buffer != NULL)
								{
									RtlStringCchCopyW(pPMI->ModuleInfo[ulIndex].DllPath, MAX_PATH, ldrEntry->FullDllName.Buffer);
									KdPrint(("full name = %ws", ldrEntry->FullDllName.Buffer));	
								}							

								ulIndex++;							
								bRet = TRUE;
							}
						}
						pPMI->ulCount = ulIndex;
					}					
				}
			}
			KeDetachProcess();
			ObDereferenceObject((PVOID)ulEproc);	
		}
	}
	return bRet;
}


BOOL HideProcessModuleByPeb(IN ULONG ulPid, IN ULONG ulModBase)
{
	BOOL bRet = FALSE;
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ulEproc, ulPeb, ulLdr = NULL;
	PLDR_DATA_TABLE_ENTRY ldrEntry = NULL;
	PLIST_ENTRY ldrHeader = NULL, ldrCurrent = NULL;
	
	status = PsLookupProcessByProcessId((HANDLE)ulPid, (PEPROCESS*)&ulEproc);
	if(NT_SUCCESS(status))
	{		
		KeAttachProcess((PRKPROCESS)ulEproc);	
		ulPeb = *(ULONG*)(ulEproc + EPROCESS_PEB_OFFSET);
		if(MmIsAddressValid((PVOID)ulPeb))
		{
			ulLdr  = *(ULONG*)(ulPeb + PEB_LDR_OFFSET);
			if(MmIsAddressValid((PVOID)ulLdr))
			{	
				ldrHeader = ldrCurrent = (PLIST_ENTRY)(ulLdr + INLOADORDERMODULELIST_OFFSET);
				//InLoadOrderModuleList			
				if(MmIsAddressValid(ldrCurrent))
				{					
					do 
					{						
						LDR_DATA_TABLE_ENTRY *pEntry = CONTAINING_RECORD( ldrCurrent, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
						
						if(ulModBase == (ULONG)pEntry->DllBase)
						{
							
							pEntry->HashLinks.Flink->Blink = 0;
							pEntry->HashLinks.Blink->Flink = 0;
							
							pEntry->InLoadOrderLinks.Flink->Blink = pEntry->InLoadOrderLinks.Blink;
							pEntry->InLoadOrderLinks.Blink->Flink = pEntry->InLoadOrderLinks.Flink;						
							
							break;
						}
						
						ldrCurrent = ldrCurrent->Flink;
						
					} while (ldrCurrent != ldrHeader);
				}
				
				
				//InMemoryOrderModuleList
				ldrHeader = ldrCurrent = (PLIST_ENTRY)(ulLdr + INMEMORYORDERMODULELIST_OFFSET);
				
				if(MmIsAddressValid(ldrCurrent))
				{		
					
					do 
					{
						LDR_DATA_TABLE_ENTRY *pEntry = CONTAINING_RECORD( ldrCurrent, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
						
						if(ulModBase == (ULONG)pEntry->DllBase)
						{
							pEntry->InMemoryOrderLinks.Flink->Blink = pEntry->InMemoryOrderLinks.Blink;
							pEntry->InMemoryOrderLinks.Blink->Flink = pEntry->InMemoryOrderLinks.Flink;
							
							pEntry->HashLinks.Flink->Blink = 0;
							pEntry->HashLinks.Blink->Flink = 0;
							
							break;
						}
						
						ldrCurrent = ldrCurrent->Flink;
						
					} while (ldrCurrent != ldrHeader);
				}
				
				//InInitializationOrderModuleList
				ldrHeader = ldrCurrent = (PLIST_ENTRY)(ulLdr + ININITIALIZATIONORDERMODULELIST_OFFSET);
				if(MmIsAddressValid(ldrCurrent))
				{
					
					do 
					{
						LDR_DATA_TABLE_ENTRY *pEntry = CONTAINING_RECORD( ldrCurrent, LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);
						
						if(ulModBase == (ULONG)pEntry->DllBase)
						{
							pEntry->InInitializationOrderLinks.Flink->Blink = pEntry->InInitializationOrderLinks.Blink;
							pEntry->InInitializationOrderLinks.Blink->Flink = pEntry->InInitializationOrderLinks.Flink;
							
							pEntry->HashLinks.Flink->Blink = 0;
							pEntry->HashLinks.Blink->Flink = 0;
							
							break;
						}
						
						ldrCurrent = ldrCurrent->Flink;
						
					} while (ldrCurrent != ldrHeader);	
				}
			}
			
		}
		KeDetachProcess();
		ObDereferenceObject((PVOID)ulEproc);	
	}
	
	return bRet;
}


