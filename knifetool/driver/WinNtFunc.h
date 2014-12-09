#pragma once

#include "WinNTDef.h"

extern "C"
NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation (
						  __in _SYSTEM_INFORMATION_CLASS SystemInformationClass,
						  __out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
						  __in ULONG SystemInformationLength,
						  __out_opt PULONG ReturnLength
						  );


extern "C"
NTSYSAPI     
NTSTATUS    
NTAPI 
ZwQueryInformationProcess(
						  __in       HANDLE ProcessHandle,
						  __in       PROCESSINFOCLASS ProcessInformationClass,
						  __out      PVOID ProcessInformation,
						  __in       ULONG ProcessInformationLength,
						  __out_opt  PULONG ReturnLength
						  );

