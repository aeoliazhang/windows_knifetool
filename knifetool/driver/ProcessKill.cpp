#include "ProcessKill.h"

#define ACTIVETHREADS					0x1a0 
#define LISTENTRY						0x190
#define THEADOFFSET						0x22c
#define CROSSTHREADFLAGS				0x248
#define PS_CROSS_THREAD_FLAGS_SYSTEM	0x00000010UL
#define MM_TAG_KILL						'KILL'


typedef struct _KILLMODULE
{
	ULONG  ProcessId;
	ULONG  BaseAddress;
}KILLMODULE,*PKILLMODULE;


typedef enum _KAPC_ENVIRONMENT {
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertApcEnvironment
} KAPC_ENVIRONMENT;

typedef VOID (NTAPI* KEINITIALIZEAPC) (
									   __out PRKAPC Apc,
									   __in PRKTHREAD Thread,
									   __in KAPC_ENVIRONMENT Environment,
									   __in PKKERNEL_ROUTINE KernelRoutine,
									   __in_opt PKRUNDOWN_ROUTINE RundownRoutine,
									   __in_opt PKNORMAL_ROUTINE NormalRoutine,
									   __in_opt KPROCESSOR_MODE ApcMode,
									   __in_opt PVOID NormalContext
									   );

typedef BOOLEAN (NTAPI* KEINSERTQUEUEAPC) (
	__inout PRKAPC Apc,
	__in_opt PVOID SystemArgument1,
	__in_opt PVOID SystemArgument2,
	__in KPRIORITY Increment
	);


VOID KernelKillThreadRoutine(
							 IN struct _KAPC *Apc,
							 IN OUT PKNORMAL_ROUTINE *NormalRoutine,
							 IN OUT PVOID *NormalContext,
							 IN OUT PVOID *SystemArgument1,
							 IN OUT PVOID *SystemArgument2
							 );

NTSTATUS TerminateThreadByApc(__in PETHREAD pEThread)
{
	NTSTATUS status=STATUS_SUCCESS;
	PKAPC ExitApc=NULL;
	UNICODE_STRING str1,str2;
	KEINITIALIZEAPC KeInitializeApc;
	KEINSERTQUEUEAPC KeInsertQueueApc;
	if (MmIsAddressValid((PVOID)pEThread))
	{
		RtlInitUnicodeString(&str1,L"KeInitializeApc");
		RtlInitUnicodeString(&str2,L"KeInsertQueueApc");
		KeInitializeApc=(KEINITIALIZEAPC)MmGetSystemRoutineAddress(&str1);
		KeInsertQueueApc=(KEINSERTQUEUEAPC)MmGetSystemRoutineAddress(&str2);
		ExitApc=(PKAPC)ExAllocatePoolWithTag(NonPagedPool,sizeof(KAPC),MM_TAG_KILL);
		if(ExitApc==NULL)
		{
			KdPrint(("APC malloc memory failed \n"));
			return STATUS_UNSUCCESSFUL;
		}
		KeInitializeApc(ExitApc,
			(PRKTHREAD)pEThread,                         //线程
			OriginalApcEnvironment,
			KernelKillThreadRoutine,
			NULL,
			NULL,
			KernelMode,
			NULL);//为线程初始化APC
		status=KeInsertQueueApc(ExitApc,ExitApc,NULL,2);   //插入Apc到线程队列
	}
	else
		status=STATUS_UNSUCCESSFUL;
	return status;
}

NTSTATUS TerminateProcessByApc(DWORD dwPID)
{
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS pEproc = NULL;

	status = PsLookupProcessByProcessId((HANDLE)dwPID, &pEproc);

	if(NT_SUCCESS(status) && MmIsAddressValid(pEproc))
	{
		ULONG ulThreadCnt =*(ULONG *)((ULONG)pEproc+ACTIVETHREADS);
		PULONG ListEntry=(ULONG *)((ULONG)pEproc+LISTENTRY);
		for (ULONG i = 0 ; i < ulThreadCnt; i++)
		{
			ListEntry=(ULONG *)((VOID *)((PLIST_ENTRY)ListEntry)->Flink);
			PETHREAD pEThread=(PETHREAD)((ULONG)ListEntry-THEADOFFSET);
			KdPrint(("PETHREAD:[0x%x]",pEThread));
			TerminateThreadByApc(pEThread);
		}
		ObDereferenceObject(pEproc);
	}                         

	return status;
}

VOID KernelKillThreadRoutine(
							 IN struct _KAPC *Apc,
							 IN OUT PKNORMAL_ROUTINE *NormalRoutine,
							 IN OUT PVOID *NormalContext,
							 IN OUT PVOID *SystemArgument1,
							 IN OUT PVOID *SystemArgument2
							 )
{
	PULONG ThreadFlags;
	ExFreePool(Apc);  //释放APC
	ThreadFlags=(ULONG *)((ULONG)PsGetCurrentThread()+CROSSTHREADFLAGS);  //ETHREAD中CrossThreadFlags的偏移量为0x248
	if(MmIsAddressValid(ThreadFlags))   //地址进行下验证
	{
		*ThreadFlags=(*ThreadFlags) | PS_CROSS_THREAD_FLAGS_SYSTEM; //修改为系统权限
		PsTerminateSystemThread(STATUS_SUCCESS); //结束系统线程，需要修改权限
		//PspExitThread(STATUS_SUCCESS);根据PspTerminateThreadByPointer定位PspExitThread地址
	}
}

NTSTATUS TerminateProcessModule(__in PVOID pBufInfo)
{
	NTSTATUS status=STATUS_SUCCESS;
	PKILLMODULE pKillModule;
	HANDLE ProcessHandle;
	CLIENT_ID ClientId;
	OBJECT_ATTRIBUTES ObjectAttributes;
	pKillModule=(PKILLMODULE)pBufInfo;
	ClientId.UniqueProcess=(HANDLE)pKillModule->ProcessId;
	ClientId.UniqueThread=0;
	ObjectAttributes.Length=sizeof(OBJECT_ATTRIBUTES);
	ObjectAttributes.ObjectName=NULL;
	ObjectAttributes.RootDirectory=NULL;
	ObjectAttributes.SecurityDescriptor=NULL;
	ObjectAttributes.SecurityQualityOfService=NULL;
	ObjectAttributes.Attributes=0;
	status=ZwOpenProcess(&ProcessHandle,
		PROCESS_ALL_ACCESS,
		&ObjectAttributes,
		&ClientId);
	if (NT_SUCCESS(status))
	{
		status=ZwUnmapViewOfSection(ProcessHandle,
			(PVOID)pKillModule->BaseAddress);
		ZwClose(ProcessHandle);
	}
	return status;

}