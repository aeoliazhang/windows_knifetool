#include "Predef.h"
#include "SSDT.h"
#include "Pe.h"
#include "Modules.h"


#define MM_TAG_SSDT   'SSDT'

//declaration
BOOL   GetSysModuleName(ULONG ulFuncAddr, PSYS_MODULE_INFO_LIST pModules, PCHAR ModuleName);
BOOL   GetSysFuncName(ULONG ulFuncAddr, ULONG ulBase, PCHAR FuncName);
ULONG  GetKernelBaseAddress(char *szKernelPath);

NTSTATUS EnumSSDT(LPVOID pSsdtBuf)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;	
	char szKernelPath[256] ={0};
    ULONG ulKernelBase = GetKernelBaseAddress(szKernelPath);
	size_t dwSize = 0;
	if(pSsdtBuf != NULL)
	{	
		PSSDT_INFO_LIST pSsdtList = (PSSDT_INFO_LIST)pSsdtBuf;
		RtlStringCchLengthA(szKernelPath, 256, &dwSize);
		if(ulKernelBase != 0 && dwSize > 0)
		{
			PIMAGE_NT_HEADERS pNtHeader = GetNtHead(ulKernelBase);
			if(pNtHeader != NULL)
			{
				ULONG ulImageBase = pNtHeader->OptionalHeader.ImageBase;
				
				PKSERVICE_DESCRIPTOR_TABLE pKeServiceDescriptorTable = 
					(PKSERVICE_DESCRIPTOR_TABLE)GetFunctionAddr(L"KeServiceDescriptorTable");
				ULONG ulSsdtCnts = pKeServiceDescriptorTable->NumberOfServices;
				ULONG ulSsdtBase = (ULONG)pKeServiceDescriptorTable->ServiceTableBase;
				pSsdtList->ulCount = ulSsdtCnts;
				
				PULONG lpSsdtArray = (PULONG)ExAllocatePoolWithTag(PagedPool, ulSsdtCnts * sizeof(ULONG), MM_TAG_SSDT);
				if (lpSsdtArray != NULL) 
				{	
					ULONG ulSsdtRav = ulSsdtBase - ulKernelBase;				
					ULONG ulSsdtRaw = Rva2Raw(pNtHeader, ulSsdtRav);
					
					if (ulSsdtRaw != 0)
					{	
						ANSI_STRING azFilePath;
						RtlInitAnsiString(&azFilePath,szKernelPath);
						UNICODE_STRING uzFilePath;
						status = RtlAnsiStringToUnicodeString(&uzFilePath, &azFilePath, TRUE);
						if(NT_SUCCESS(status))
						{
							OBJECT_ATTRIBUTES ObjAttr = {0};
							InitializeObjectAttributes(&ObjAttr, &uzFilePath, OBJ_KERNEL_HANDLE |OBJ_CASE_INSENSITIVE, NULL, NULL); 
							
							HANDLE hFile = NULL;
							IO_STATUS_BLOCK IoStatusBlock;
							status=ZwCreateFile(
								&hFile,
								GENERIC_READ,
								&ObjAttr,
								&IoStatusBlock,
								NULL,
								FILE_ATTRIBUTE_NORMAL,
								FILE_SHARE_READ,
								FILE_OPEN,
								FILE_NON_DIRECTORY_FILE|FILE_RANDOM_ACCESS|FILE_SYNCHRONOUS_IO_NONALERT,
								NULL,
								0);
							
							if (NT_SUCCESS(status))
							{

								LARGE_INTEGER offset;
								offset.LowPart = ulSsdtRaw;
								offset.HighPart = 0;
								status=ZwReadFile(
									hFile,
									NULL,
									NULL,
									NULL,
									&IoStatusBlock,
									lpSsdtArray,
									ulSsdtCnts*sizeof(ULONG),
									&offset,
									NULL
									);
								if (NT_SUCCESS(status))
								{		
									PSYS_MODULE_INFO_LIST  pModules = (PSYS_MODULE_INFO_LIST)ExAllocatePoolWithTag(NonPagedPool, sizeof(SYS_MODULE_INFO_LIST), MM_TAG_SSDT);
									if (pModules != NULL)
									{
										RtlZeroMemory(pModules, sizeof(SYS_MODULE_INFO_LIST));
										EnumSystemModules(pModules);
									}
									
									//
									for (ULONG ul = 0; ul < ulSsdtCnts; ul++)
									{
										ULONG uOrgAddress = *(lpSsdtArray + ul) - ulImageBase + ulKernelBase;
										ULONG uCurrAddress = *((PULONG)ulSsdtBase + ul);
										pSsdtList->SsdtInfo[ul].ulIndex = ul;
										pSsdtList->SsdtInfo[ul].ulCurrAddress = uCurrAddress;
										pSsdtList->SsdtInfo[ul].ulOrgAddress = uOrgAddress;	

										if(pModules != NULL)
											GetSysModuleName(uCurrAddress, pModules, pSsdtList->SsdtInfo[ul].ModuleName);

									//	GetSysFuncName(uOrgAddress, ulKernelBase, pSsdtList->SsdtInfo[ul].FuncName);
									}

									if (pModules != NULL)
									{
										ExFreePool(pModules);
										pModules = NULL;
									}
								}
								ZwClose(hFile);
							}						
						}
					}
					
					ExFreePool(lpSsdtArray);
					lpSsdtArray = NULL;
				}
			}
		}
	}
	return status;
}

ULONG GetKernelBaseAddress(char *szKernelPath)
{
	ULONG ulKernelBase = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG ulRetSize = 0;
	status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &ulRetSize);  
	if ( status == STATUS_INFO_LENGTH_MISMATCH && ulRetSize > 0)  
	{  
		LPVOID  pBuf = ExAllocatePoolWithTag(NonPagedPool, ulRetSize, MM_TAG_SSDT);
		if ( pBuf !=NULL )  
		{
			status = ZwQuerySystemInformation(SystemModuleInformation, pBuf, ulRetSize, &ulRetSize); 
			if (NT_SUCCESS(status))
			{			
				PSYSTEM_PROCESS_MODULES spms = (PSYSTEM_PROCESS_MODULES)pBuf;			
				ulKernelBase = (ULONG)spms->Modules[0].ImageBase;
				RtlStringCchCopyA(szKernelPath, 256 ,"\\SystemRoot\\System32\\");
				RtlStringCchCatA(szKernelPath, 256, spms->Modules[0].OffsetToFileName+spms->Modules[0].FullPathName);			
				KdPrint(("szKernelPath == %s\r\n", szKernelPath) );
			}
			
			ExFreePool(pBuf);
		} 
	}
    return ulKernelBase;
}

BOOL GetSysModuleName(ULONG ulFuncAddr, PSYS_MODULE_INFO_LIST pModules, PCHAR ModuleName)
{
	BOOL bRet = FALSE;
	if(pModules != NULL && ModuleName != NULL)
	{
		RtlStringCchCopyA(ModuleName, MAX_PATH, "Unknown");
		for(ULONG nIndex = 0; nIndex < pModules->ulCount; nIndex++)
		{
			if(ulFuncAddr >= (ULONG)pModules->ModuleInfo[nIndex].ImageBase && 
				ulFuncAddr <= (ULONG)pModules->ModuleInfo[nIndex].ImageBase + (ULONG)pModules->ModuleInfo[nIndex].ImageSize)
			{
				RtlStringCchCopyA(ModuleName, MAX_PATH, pModules->ModuleInfo[nIndex].ModulePath);
				bRet = TRUE;
				break;
			}
		}			
	}

	return bRet;
}

/*
BOOL GetSysFuncName(ULONG ulFuncAddr, ULONG ulBase, PCHAR FuncName)
{
	BOOL bRet = FALSE;
	if(FuncName != NULL)
	{		
		RtlStringCchCopyA(FuncName, MAX_PATH, "Unknown");
		PIMAGE_NT_HEADERS pNtHeader = GetNtHead(ulBase);
		if(pNtHeader != NULL)
		{
			PIMAGE_EXPORT_DIRECTORY pExpDir = (PIMAGE_EXPORT_DIRECTORY)pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
			pExpDir = (PIMAGE_EXPORT_DIRECTORY)Rva2va((PIMAGE_DOS_HEADER)ulBase, pNtHeader, (ULONG)pExpDir);
			ULONG  *arrayOfFunctionNames = (ULONG *)Rva2va((PIMAGE_DOS_HEADER)ulBase, pNtHeader, pExpDir->AddressOfNames);
			ULONG  *arrayOfFunctionAddresses = (ULONG *)Rva2va((PIMAGE_DOS_HEADER)ulBase, pNtHeader, pExpDir->AddressOfFunctions);
			USHORT *arrayOfFunctionOrdinals = (USHORT *)Rva2va((PIMAGE_DOS_HEADER)ulBase, pNtHeader, pExpDir->AddressOfNameOrdinals);
			
			for(ULONG i  = 0; i < pExpDir->NumberOfNames; i++)
			{
				PCSTR pszName = (PCSTR)Rva2va((PIMAGE_DOS_HEADER)ulBase, pNtHeader, arrayOfFunctionNames[i]);	
				size_t ulLen = 0;
				if(pszName != NULL)
				{
					RtlStringCchLengthA(pszName, MAX_PATH, &ulLen);
					if ( ulLen> 2 && 'N' == pszName[0] && 't' == pszName[1] )
					{		
						ULONG  ulFunc = Rva2va((PIMAGE_DOS_HEADER)ulBase, pNtHeader, arrayOfFunctionAddresses[arrayOfFunctionOrdinals[i]]);
						if(ulFuncAddr == ulFunc)
						{
							RtlStringCchCopyA(FuncName, MAX_PATH, pszName);
							bRet = TRUE;
							break;
						}					
					}	
				}			
			}
		} 
	}
	
	return bRet;
}
*/