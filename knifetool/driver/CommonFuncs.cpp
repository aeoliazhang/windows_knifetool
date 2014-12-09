#include "Predef.h"
#include "CommonFuncs.h"

static ULONG Cr0Value;

KIRQL WOFF()
{
	KIRQL Irql;

	_asm
	{
		push eax
		mov eax,cr0;
		mov Cr0Value,eax
		and eax,0fffeffffh
		mov cr0,eax
		pop eax
	}

	Irql=KeRaiseIrqlToDpcLevel();
	return Irql;
}

VOID WON(KIRQL Irql)
{
	
	KeLowerIrql(Irql);
	
	_asm
	{
		push eax
		mov eax,Cr0Value;
		mov cr0,eax;
		pop eax
	}		
}

ULONG GetFunctionAddr( IN PCWSTR FunctionName)
{
    UNICODE_STRING UniCodeFunctionName;
    RtlInitUnicodeString( &UniCodeFunctionName, FunctionName );
    return (ULONG)MmGetSystemRoutineAddress( &UniCodeFunctionName );   
}
