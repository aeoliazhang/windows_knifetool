#pragma once

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))

typedef struct _DEVICE_EXTENSION 
{
	PDEVICE_OBJECT pDevice;
	UNICODE_STRING ustrDeviceName;	
	UNICODE_STRING ustrSymLinkName;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//
NTSTATUS CreateDevice (IN PDRIVER_OBJECT pDriverObject);
VOID     DriverUnload (IN PDRIVER_OBJECT pDriverObject);
NTSTATUS DispatchIoControl(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);
NTSTATUS DispatchCreate(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);
NTSTATUS DispatchClose(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

//
NTSTATUS IrpEnumProcessesByNativeApi(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);
NTSTATUS IrpTerminateProcessByAPC(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);
NTSTATUS IrpHideProcessByActiveList(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

//
NTSTATUS IrpEnumProcessThreads(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

//
NTSTATUS IrpEnumProcessModules(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);
NTSTATUS IrpHideProcessModuleByPeb(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

//
NTSTATUS IrpEnumProcessObjects(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);
NTSTATUS IrpCloseObject(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);


//
NTSTATUS IrpEnumSystemModules(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);
NTSTATUS IrpEnumSSDT(IN PDEVICE_OBJECT pDevObj, IN PIRP pIrp);

