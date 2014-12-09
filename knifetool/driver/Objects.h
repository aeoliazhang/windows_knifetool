#pragma once    

NTSTATUS EnumProcessObjects(IN ULONG ulPid, OUT LPVOID pObjectsBuf);
NTSTATUS CloseProcessObject(IN ULONG ulPid, ULONG handle);