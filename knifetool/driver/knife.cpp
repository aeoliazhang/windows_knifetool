
#include "Predef.h"
#include "knife.h"
#include "Pe.h"
#include "ProcessKill.h"
#include "SSDT.h"
#include "Processes.h"
#include "Modules.h"
#include "Threads.h"
#include "Objects.h"


#define MM_TAG_KNIFE	'KNIF'

extern "C"
NTSTATUS DriverEntry (
			IN PDRIVER_OBJECT pDriverObject,
			IN PUNICODE_STRING pRegistryPath	) 
{

	NTSTATUS status;	

	//register routine
	pDriverObject->DriverUnload=DriverUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]=DispatchIoControl;
	
	//
	status = CreateDevice(pDriverObject);

	return status;
}

NTSTATUS CreateDevice (
		IN PDRIVER_OBJECT	pDriverObject) 
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;
	

	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName,L"\\Device\\knife");	

	status = IoCreateDevice( pDriverObject,
						sizeof(DEVICE_EXTENSION),
						&(UNICODE_STRING)devName,
						FILE_DEVICE_UNKNOWN,
						0, TRUE,
						&pDevObj );
	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO;
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;
	pDevExt->ustrDeviceName = devName;
	
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName,L"\\??\\knife");
	pDevExt->ustrSymLinkName = symLinkName;
	status = IoCreateSymbolicLink( &symLinkName,&devName );
	if (!NT_SUCCESS(status)) 
	{
		IoDeleteDevice( pDevObj );
		return status;
	}
	return STATUS_SUCCESS;
}

VOID DriverUnload (IN PDRIVER_OBJECT pDriverObject) 
{
	PDEVICE_OBJECT	pNextObj;
	pNextObj = pDriverObject->DeviceObject;
	while (pNextObj != NULL) 
	{
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)
			pNextObj->DeviceExtension;

		UNICODE_STRING pLinkName = pDevExt->ustrSymLinkName;
		IoDeleteSymbolicLink(&pLinkName);
		pNextObj = pNextObj->NextDevice;
		IoDeleteDevice( pDevExt->pDevice );
	}
}

NTSTATUS DispatchCreate(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
    pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	
    return status;
}

NTSTATUS DispatchClose(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	pIrp->IoStatus.Status = status;
    pIrp->IoStatus.Information = 0;	
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);	
    return status;
}

NTSTATUS DispatchIoControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp) 
{
	NTSTATUS status=STATUS_SUCCESS;
	ULONG ulCtlCode;              
	PIO_STACK_LOCATION pIrpStack;   
	pIrpStack=IoGetCurrentIrpStackLocation(pIrp); 
	
	ulCtlCode=pIrpStack->Parameters.DeviceIoControl.IoControlCode; 
	
	switch (ulCtlCode)
	{ 
	case IOCTL_KNIFE_ENUM_PROCESSES_NATIVEAPI:
		{
			status =  IrpEnumProcessesByNativeApi(pDevObj,pIrp);
		}
		break;

	case IOCTL_KNIFE_TERMINATE_PROCESS_BYAPC:
		{
			status = IrpTerminateProcessByAPC(pDevObj,pIrp);
		}
		break;

	case IOCTL_KNIFE_HIDE_PROCESS_BYACTIVELIST:
		{
			status = IrpHideProcessByActiveList(pDevObj, pIrp);
		}
		break;

	case IOCTL_KNIFE_ENUM_PROCESS_THREADS:
		{
			status =  IrpEnumProcessThreads(pDevObj,pIrp);	
		}
		break;

	case IOCTL_KNIFE_ENUM_PROCESS_MODULES:
		{
			status =  IrpEnumProcessModules(pDevObj,pIrp);
		}
		break;

	case IOCTL_KNIFE_HIDE_MODULE_BYPEB:
		{
			status = IrpHideProcessModuleByPeb(pDevObj,pIrp);				
		}
		break;

	case IOCTL_KNIFE_ENUM_PROCESS_OBJECTS:
		{
			status = IrpEnumProcessObjects(pDevObj, pIrp);		
		}
		break;
		
	case IOCTL_KNIFE_CLOSE_OBJECT:
		{
			status = IrpCloseObject(pDevObj, pIrp);		
		}

	case IOCTL_KNIFE_ENUM_SYSMODULES:
		{
			status = IrpEnumSystemModules(pDevObj, pIrp);
		}
		break;

	case IOCTL_KNIFE_ENUM_SSDT:
		{
			status = IrpEnumSSDT(pDevObj, pIrp);
		}
		break;

	default:
		{
			status = STATUS_INVALID_DEVICE_REQUEST;  
		}	 
		break;      
		
	}
	pIrp->IoStatus.Status = status;
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );      
    return status;
}

NTSTATUS IrpEnumProcessesByNativeApi(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PVOID pProcessesBuf;
	ULONG ulOutputBufferSize;
	PIO_STACK_LOCATION pIoStackLocation;
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	ulOutputBufferSize = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	if (ulOutputBufferSize < sizeof(PROCESS_INFO_LIST))
	{
		return STATUS_INFO_LENGTH_MISMATCH;
	}
	pProcessesBuf = ExAllocatePoolWithTag(NonPagedPool, sizeof(PROCESS_INFO_LIST), MM_TAG_KNIFE);
	if (!pProcessesBuf)
	{	
		return STATUS_UNSUCCESSFUL;
	}
	RtlZeroMemory(pProcessesBuf,sizeof(PROCESS_INFO_LIST));
	status = EnumProcessesByNativeApi(pProcessesBuf);
	if (!NT_SUCCESS(status))
	{
		ExFreePool(pProcessesBuf);
		return status;
	}
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pProcessesBuf, sizeof(PROCESS_INFO_LIST));
	pIrp->IoStatus.Information = sizeof(PROCESS_INFO_LIST);
	ExFreePool(pProcessesBuf);
	return status;
}

NTSTATUS IrpTerminateProcessByAPC(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ulPID;
	ulPID =*(ULONG*)pIrp->AssociatedIrp.SystemBuffer;
	status=TerminateProcessByApc(ulPID);
	pIrp->IoStatus.Information=0;
    return status;
}

NTSTATUS IrpHideProcessByActiveList(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ulPID;
	ulPID = *(ULONG*)pIrp->AssociatedIrp.SystemBuffer;
	status = HideProcessByActiveList(ulPID);
	pIrp->IoStatus.Information=0;
    return status;
}

NTSTATUS IrpEnumProcessThreads(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PVOID pThreadsBuf;
	ULONG ulOutputBufferSize;
	PIO_STACK_LOCATION pIoStackLocation;
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	ulOutputBufferSize = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG ulPID;
	ulPID =*(ULONG*)pIrp->AssociatedIrp.SystemBuffer;
	if (ulOutputBufferSize < sizeof(PROCESS_THREAD_INFO_LIST))
	{
		return STATUS_INFO_LENGTH_MISMATCH;
	}
	pThreadsBuf = ExAllocatePoolWithTag(NonPagedPool, sizeof(PROCESS_THREAD_INFO_LIST), MM_TAG_KNIFE);
	if (!pThreadsBuf)
	{	
		return STATUS_UNSUCCESSFUL;
	}
	RtlZeroMemory(pThreadsBuf,sizeof(PROCESS_THREAD_INFO_LIST));
	EnumProcessThreads(ulPID, pThreadsBuf);
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pThreadsBuf, sizeof(PROCESS_THREAD_INFO_LIST));
	pIrp->IoStatus.Information = sizeof(PROCESS_THREAD_INFO_LIST);
	ExFreePool(pThreadsBuf);
	return status;
}

NTSTATUS IrpEnumProcessModules(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PVOID pModulesBuf;
	ULONG ulOutputBufferSize;
	PIO_STACK_LOCATION pIoStackLocation;
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	ulOutputBufferSize = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG ulPID;
	ulPID =*(ULONG*)pIrp->AssociatedIrp.SystemBuffer;
	if (ulOutputBufferSize < sizeof(PROCESS_MODULE_INFO_LIST))
	{
		return STATUS_INFO_LENGTH_MISMATCH;
	}
	pModulesBuf = ExAllocatePoolWithTag(NonPagedPool, sizeof(PROCESS_MODULE_INFO_LIST), MM_TAG_KNIFE);
	if (!pModulesBuf)
	{	
		return STATUS_UNSUCCESSFUL;
	}
	RtlZeroMemory(pModulesBuf,sizeof(PROCESS_MODULE_INFO_LIST));
	EnumProcessModulesByPeb(ulPID, pModulesBuf);
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pModulesBuf, sizeof(PROCESS_MODULE_INFO_LIST));
	pIrp->IoStatus.Information = sizeof(PROCESS_MODULE_INFO_LIST);
	ExFreePool(pModulesBuf);
	return status;
}

NTSTATUS IrpHideProcessModuleByPeb(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PROCESS_MODULE_BASE PMB = {0};
	RtlCopyMemory(&PMB, pIrp->AssociatedIrp.SystemBuffer, sizeof(PROCESS_MODULE_BASE));
	status = HideProcessModuleByPeb(PMB.Pid, PMB.ModBase);
	pIrp->IoStatus.Information=0;
    return status;
}

NTSTATUS IrpEnumProcessObjects(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PVOID pObjectsBuf;
	ULONG ulOutputBufferSize;
	PIO_STACK_LOCATION pIoStackLocation;
	ULONG ulPID;
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	ulOutputBufferSize = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	if (ulOutputBufferSize < sizeof(PROCESS_OBJECT_INFO_LIST))
	{
		return STATUS_INFO_LENGTH_MISMATCH;
	}

	pObjectsBuf = ExAllocatePoolWithTag(NonPagedPool, sizeof(PROCESS_OBJECT_INFO_LIST), MM_TAG_KNIFE);
	if (!pObjectsBuf)
	{	
		return STATUS_UNSUCCESSFUL;
	}
	
	ulPID =*(ULONG*)pIrp->AssociatedIrp.SystemBuffer;
	RtlZeroMemory(pObjectsBuf, sizeof(PROCESS_OBJECT_INFO_LIST));
	status = EnumProcessObjects(ulPID, pObjectsBuf);
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pObjectsBuf, sizeof(PROCESS_OBJECT_INFO_LIST));
	pIrp->IoStatus.Information = sizeof(PROCESS_OBJECT_INFO_LIST);
	ExFreePool(pObjectsBuf);
	
	return status;
}

NTSTATUS IrpCloseObject(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PROCESS_OBJECT_HANDLE objHandle =*(PROCESS_OBJECT_HANDLE*)pIrp->AssociatedIrp.SystemBuffer;
	status = CloseProcessObject(objHandle.Pid, objHandle.Handle);
	pIrp->IoStatus.Information=0;
    return status;
}

NTSTATUS IrpEnumSystemModules(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PVOID pModulesBuf;
	ULONG ulOutputBufferSize;
	PIO_STACK_LOCATION pIoStackLocation;
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	ulOutputBufferSize = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	if (ulOutputBufferSize < sizeof(SYS_MODULE_INFO_LIST))
	{
		return STATUS_INFO_LENGTH_MISMATCH;
	}
	pModulesBuf = ExAllocatePoolWithTag(NonPagedPool, sizeof(SYS_MODULE_INFO_LIST), MM_TAG_KNIFE);
	if (!pModulesBuf)
	{	
		return STATUS_UNSUCCESSFUL;
	}
	RtlZeroMemory(pModulesBuf,sizeof(SYS_MODULE_INFO_LIST));
	status = EnumSystemModules(pModulesBuf);
	if (!NT_SUCCESS(status))
	{
		ExFreePool(pModulesBuf);
		return status;
	}
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pModulesBuf, sizeof(SYS_MODULE_INFO_LIST));
	pIrp->IoStatus.Information = sizeof(SYS_MODULE_INFO_LIST);
	ExFreePool(pModulesBuf);
	return status;
}

NTSTATUS IrpEnumSSDT(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp)
{
	NTSTATUS status=STATUS_SUCCESS;
	PVOID pSsdtBuf;
	ULONG ulOutputBufferSize;
	PIO_STACK_LOCATION pIoStackLocation;
	pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	ulOutputBufferSize = pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength;
	if (ulOutputBufferSize < sizeof(SSDT_INFO_LIST))
	{
		return STATUS_INFO_LENGTH_MISMATCH;
	}
	pSsdtBuf=ExAllocatePoolWithTag(NonPagedPool, sizeof(SSDT_INFO_LIST), MM_TAG_KNIFE);
	if (!pSsdtBuf)
	{
		return STATUS_UNSUCCESSFUL;
	}
	RtlZeroMemory(pSsdtBuf,sizeof(SSDT_INFO_LIST));
	status=EnumSSDT(pSsdtBuf);
	if (!NT_SUCCESS(status))
	{
		ExFreePool(pSsdtBuf);
		return status;
	}
	RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pSsdtBuf, sizeof(SSDT_INFO_LIST));
	pIrp->IoStatus.Information=sizeof(SSDT_INFO_LIST);
	ExFreePool(pSsdtBuf);
	return status;
}