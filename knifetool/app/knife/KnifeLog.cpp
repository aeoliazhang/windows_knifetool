#include "StdAfx.h"
#include "Log.h"

#define MAX_LOG_LEN		1024

static CKnifeLog m_log;

void InitKnifeLog()
{	
	m_log.StartLog();
}

void CloseKnifeLog()
{
   m_log.EndLog();
}

void KnifeLog(const TCHAR *strLog, ...)
{
	va_list  va;
	va_start (va, strLog);
	TCHAR LogBuf[MAX_LOG_LEN] = {0};
	_vsnwprintf_s(LogBuf,MAX_LOG_LEN, (const wchar_t *)strLog, va);
	va_end(va);

	m_log.WriteLog(LogBuf);	
}