#pragma once    

NTSTATUS EnumSystemModules(OUT LPVOID pModulesBuf);
BOOL EnumProcessModulesByPeb(IN ULONG ulPid, OUT LPVOID pModulesBuf);
BOOL HideProcessModuleByPeb(IN ULONG ulPid, IN ULONG ulModBase);