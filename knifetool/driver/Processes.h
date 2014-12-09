#pragma once

NTSTATUS EnumProcessesByNativeApi(OUT LPVOID pProcessesBuf);
NTSTATUS EnumProcessesByActiveList();
NTSTATUS EnumProcessesByCidTable();
BOOL	 CheckProcess(IN ULONG ulEprocess, IN ULONG ulsysEprocess);
NTSTATUS GetProcessImagePath(IN ULONG Pid, OUT PUNICODE_STRING ProcessImagePath);   
BOOL	 GetProcessImagePathByPeb(IN ULONG Pid, OUT PWCHAR wsImagePath); 
NTSTATUS HideProcessByActiveList(IN ULONG Pid);