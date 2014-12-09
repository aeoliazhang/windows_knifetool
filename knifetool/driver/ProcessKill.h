#pragma once
#include "Predef.h"

NTSTATUS TerminateProcessByApc(ULONG dwPID);
NTSTATUS TerminateThreadByApc(__in PETHREAD pEThread);
NTSTATUS TerminateProcessModule(__in PVOID pKillModule);