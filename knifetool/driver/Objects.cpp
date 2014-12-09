#include "Predef.h"
#include "Modules.h"

#define MM_TAG_OBJ			'OBJ'


NTSTATUS EnumProcessObjects(IN ULONG ulPid, OUT LPVOID pObjectsBuf)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG ulRetSize = 0;
	ULONG ulSize = 0x8000;		
	LPVOID lpBuf = ExAllocatePoolWithTag(NonPagedPool, ulSize, MM_TAG_OBJ);
	while( lpBuf != NULL && 
		(status = ZwQuerySystemInformation(SystemHandleInformation, lpBuf, ulSize, NULL) == STATUS_INFO_LENGTH_MISMATCH ))
	{
		ExFreePool(lpBuf);
		lpBuf = NULL;
		ulSize *= 2;
		lpBuf = ExAllocatePoolWithTag(NonPagedPool, ulSize, MM_TAG_OBJ);		
	}
	
	if(NT_SUCCESS(status))
	{
		PPROCESS_OBJECT_INFO_LIST pObjList = (PPROCESS_OBJECT_INFO_LIST)pObjectsBuf;
		
		PSYSTEM_HANDLE_INFORMATION_LIST pSysHandleList = (PSYSTEM_HANDLE_INFORMATION_LIST)lpBuf;		
		PSYSTEM_HANDLE_INFORMATION pSysHandle = (PSYSTEM_HANDLE_INFORMATION)(pSysHandleList->Information);
		
		ULONG uIndex = 0;
		
		for(ULONG i = 0; i < pSysHandleList->NumberOfHandles; ++i)
		{
			if(pSysHandle[i].ProcessId != ulPid)
				continue;
			
			RtlStringCchCopyW(pObjList->ObjectInfo[uIndex].Type, OBJECT_MAX_TYPE,  L"<none>");
			RtlStringCchCopyW(pObjList->ObjectInfo[uIndex].Name, MAX_PATH, L"<none>");			
			pObjList->ObjectInfo[uIndex].Handle = pSysHandle[i].Handle;
			
			
			HANDLE hProc, hDup;
			OBJECT_ATTRIBUTES ObjAttrs;
			CLIENT_ID clientid;
			InitializeObjectAttributes(&ObjAttrs, 0 ,OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
			clientid.UniqueProcess = (HANDLE)ulPid;
			clientid.UniqueThread = 0;
			status = ZwOpenProcess(&hProc, PROCESS_ALL_ACCESS, &ObjAttrs, &clientid); 
			if(NT_SUCCESS(status))
			{
				__try
				{
					status = ZwDuplicateObject(hProc, (HANDLE)pSysHandle[i].Handle, (HANDLE)-1, &hDup, 0, 0,  DUPLICATE_SAME_ACCESS);
					if(NT_SUCCESS(status))
					{
						pObjList->ObjectInfo[uIndex].DupHandle = (ULONG)hDup;
					
						ULONG ulRetSize = 0;
						//get type info
						status = ZwQueryObject (hDup, ObjectTypeInformation, NULL, 0, &ulRetSize);
						if (status == STATUS_INFO_LENGTH_MISMATCH && ulRetSize > 0) 
						{
							LPVOID pTypeBuf = ExAllocatePoolWithTag(NonPagedPool, ulRetSize, MM_TAG_OBJ);
							if(pTypeBuf)
							{
								status = ZwQueryObject(hDup, ObjectTypeInformation, pTypeBuf, ulRetSize, &ulRetSize);
								if(NT_SUCCESS(status))
								{
									POBJECT_TYPE_INFORMATION pTypeInfo = (POBJECT_TYPE_INFORMATION)pTypeBuf;
									if(pTypeInfo->TypeName.Buffer && pTypeInfo->TypeName.Length > 0)
									{
										RtlStringCchCopyW(pObjList->ObjectInfo[uIndex].Type, OBJECT_MAX_TYPE, pTypeInfo->TypeName.Buffer);
									}				
								}
								ExFreePool(pTypeBuf);
								pTypeBuf = NULL;
							}					
						}
						
						//get name info					
						status = ZwQueryObject(hDup, (OBJECT_INFORMATION_CLASS)ObjectNameInformation, NULL, 0, &ulRetSize);
						if (status == STATUS_INFO_LENGTH_MISMATCH && ulRetSize > 0) 
						{
							LPVOID pNameBuf = ExAllocatePoolWithTag(NonPagedPool, ulRetSize, MM_TAG_OBJ);
							if(pNameBuf)
							{
								status = ZwQueryObject(hDup, (OBJECT_INFORMATION_CLASS)ObjectNameInformation, pNameBuf, ulRetSize, &ulRetSize);
								if(NT_SUCCESS(status))
								{
									POBJECT_NAME_INFORMATION pNameInfo = (POBJECT_NAME_INFORMATION)pNameBuf;
									if(pNameInfo->Name.Buffer != NULL  && pNameInfo->Name.Length > 0)
									{
										RtlStringCchCopyW(pObjList->ObjectInfo[uIndex].Name, MAX_PATH, pNameInfo->Name.Buffer);
									}				
								}
								ExFreePool(pNameBuf);
								pNameBuf = NULL;
							}					
						}		
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					KdPrint(("\r\nZwDuplicateObject error \r\n"));	
				}
			}
			
			uIndex++;	
			
		}
		pObjList->ulCount = uIndex;		
	}
	
	if(lpBuf)
	{
		ExFreePool(lpBuf);
		lpBuf = NULL;
	}  
	
	return status;
}


NTSTATUS CloseProcessObject(IN ULONG ulPid, ULONG handle)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	HANDLE hProc, hDup;
	OBJECT_ATTRIBUTES ObjAttrs;
	CLIENT_ID clientid;
	InitializeObjectAttributes(&ObjAttrs, 0 ,OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
	clientid.UniqueProcess = (HANDLE)ulPid;
	clientid.UniqueThread = 0;
	
	if(ulPid > 4)
	{
		status = ZwOpenProcess(&hProc, PROCESS_ALL_ACCESS, &ObjAttrs, &clientid); 
		if(NT_SUCCESS(status))
		{
			__try
			{
				status = ZwDuplicateObject(hProc, (HANDLE)handle, (HANDLE)-1, &hDup, 0, 0,  DUPLICATE_CLOSE_SOURCE);
				if(NT_SUCCESS(status))
				{
					status = NtClose(hDup);					
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				KdPrint(("ZwDuplicateObject error \r\n"));	
			}
		}
		else
		{
			KdPrint(("CloseProcessObject:: ZwOpenProcess error \r\n"));	
		}
	}	


	/*
	PEPROCESS pEproc = NULL;
	status = PsLookupProcessByProcessId((HANDLE)ulPid, &pEproc);
	if(NT_SUCCESS(status))
	{

		KeAttachProcess(pEproc);

		PVOID pObj = NULL;

		status = ObReferenceObjectByHandle((HANDLE)handle, 0, *ExEventObjectType, KernelMode, &pObj, NULL);
		if(NT_SUCCESS(status))
		{
			OBJECT_NAME_INFORMATION  objName;
			ULONG uRetSize = 0;
			status = ObQueryNameString(pObj, &objName,  1024 , &uRetSize);

			if(NT_SUCCESS(status))
			{

			}

			KdPrint(("test"));
		}


		KeDetachProcess();

		ObDereferenceObject(pEproc);
	}
	*/

	return status;
}


