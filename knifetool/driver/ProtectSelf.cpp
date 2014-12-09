#include "Predef.h"
#include "ProtectSelf.h"
#include "Processes.h"

/*
//************宏定义*************************************************
#define  PROCESS_TERMINATE 0x0001
#define  PROCESS_CREATE_PROCESS 0x0080
typedef  (* KIINSERTQUEUEAPC)(
						IN PKAPC Apc,
						IN KPRIORITY Increment);

//************数据定义*************************************************
BYTE OriginalHead[5]={0};
BYTE ReplaceHead[5]={0xE9,0,0,0,0};
extern POBJECT_TYPE *PsProcessType;
extern PDEVICE_EXTENSION  g_DeviceExtension;
KIINSERTQUEUEAPC g_KiInsertQueueApc;
//************内部函数声明*********************************************
VOID
FASTCALL
DatourMyKiInsertQueueApc (
		IN PKAPC Apc,
		IN KPRIORITY Increment);
VOID
FASTCALL
OriginalKiInsertQueueApc(
	    IN PKAPC Apc,
	    IN KPRIORITY Increment);

//
ULONG GetFunctionAddr( IN PCWSTR FunctionName)     //PCWSTR常量指针，指向16位UNICODE
{
	UNICODE_STRING UniCodeFunctionName;
	RtlInitUnicodeString( &UniCodeFunctionName, FunctionName );
	return (ULONG)MmGetSystemRoutineAddress( &UniCodeFunctionName );   
}
//
ULONG GetKiInsertQueueApcAddr()
{
	ULONG sp_code1=0x28,sp_code2=0xe8,sp_code3=0xd88a;  //特征码,sp_code3 windbg显示错误，应该为d88a
	ULONG address=0;
	PUCHAR addr;
	PUCHAR p;
	addr=(PUCHAR)GetFunctionAddr(L"KeInsertQueueApc");
	for(p=addr;p<p+PAGE_SIZE;p++)
	{
		if((*(p-1)==sp_code1)&&(*p==sp_code2)&&(*(PUSHORT)(p+5)==sp_code3))
		{
			address=*(PULONG)(p+1)+(ULONG)(p+5);
			break;
		}
	}
	KdPrint(("[KeInsertQueueApc] addr %x\n",(ULONG)addr));
	KdPrint(("[KiInsertQueueApc] address %x\n",address));
	return address;
}

/************************************************************************
* 函数名称:HookKiInsertQueueApc
* 功能描述:安装过滤KiInsertQueueApc钩子
* 参数列表:
* 返回 值:无
*************************************************************************/
void HookKiInsertQueueApc()
{

	g_KiInsertQueueApc=(KIINSERTQUEUEAPC)GetKiInsertQueueApcAddr();
	RtlCopyMemory(OriginalHead,(BYTE *)g_KiInsertQueueApc,5);
	*(ULONG *)(ReplaceHead+1)=(ULONG)DatourMyKiInsertQueueApc-((ULONG)g_KiInsertQueueApc+5);
	KIRQL Irql;
	Irql=WOFF();
	//写入新的函数头
	RtlCopyMemory((BYTE *)g_KiInsertQueueApc,ReplaceHead,5);
	WON(Irql);
	
};
/************************************************************************
* 函数名称:DatourMyKiInsertQueueApc
* 功能描述:Hook回调函数
* 参数列表:

* 返回 值:状态
*************************************************************************/
VOID FASTCALL DatourMyKiInsertQueueApc(
								  IN PKAPC Apc,
								  IN KPRIORITY Increment)
{
	NTSTATUS status=STATUS_SUCCESS;
	WCHAR TarProcPath;
	BOOL bRet;
	ULONG ulThread;
	ULONG ulEp;
	PEPROCESS pEprocess;
	pEprocess=PsGetCurrentProcess();
	if (pEprocess!=g_DeviceExtension->pEprocess)
	{
		if(MmIsAddressValid((PULONG)((ULONG)Apc+0x008)))    //地址验证 KAPC结构+008--->kthread
			ulThread=*((PULONG)((ULONG)Apc+0x008));
		else
			return ;
		if(MmIsAddressValid((PULONG)((ULONG)ulThread+0x044))) //kthread+30-->KAPC_STATE+10-->eprocess
			ulEp=*((PULONG)((ULONG)ulThread+0x044));
		else
			return ;
		if(((PEPROCESS)ulEp==g_DeviceExtension->pEprocess)&&(Increment==2))
		{
			return;

		}    
	}
	OriginalKiInsertQueueApc(Apc,Increment);
}

_declspec (naked) VOID FASTCALL OriginalKiInsertQueueApc(
	IN PKAPC Apc,
	IN KPRIORITY Increment)
{
	_asm
	{
		mov edi,edi
		push ebp
		mov ebp,esp
		mov eax,g_KiInsertQueueApc
		add eax,5
		jmp eax

	}
}

/************************************************************************
* 函数名称:UnHookProcessCreate
* 功能描述:卸载拦截
* 参数列表:
* 返回 值:无
*************************************************************************/
void UnHookKiInsertQueueApc()
{
	KIRQL Irql;
	Irql=WOFF();
	//写回原来的函数头
	RtlCopyMemory((BYTE *)g_KiInsertQueueApc,OriginalHead,5);
	WON(Irql);
};

