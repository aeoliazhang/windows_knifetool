#include "StdAfx.h"
#include "Log.h"

CKnifeLog::CKnifeLog()
{
	m_blThreadRun		= FALSE;
	m_hThreadHandle		= NULL;
	m_dwThreadId		= 0;
	m_hFile				= NULL;
	BeginThread();
}

CKnifeLog::~CKnifeLog()
{
	EndThread(INFINITE);
}

BOOL CKnifeLog::StartLog()
{
	BOOL bRet = FALSE;	

	if (m_hThreadHandle != NULL)
	{
		while (!::PostThreadMessage(m_dwThreadId, KNIFE_MSG_LOG_START, 0, NULL))
		{
			Sleep(50);
		}

		bRet = TRUE;
	}

	return bRet;
}

BOOL CKnifeLog::EndLog()
{
	BOOL bRet = FALSE;

	if (m_hThreadHandle != NULL)
	{
		if (::PostThreadMessage(m_dwThreadId, KNIFE_MSG_LOG_END, 0, NULL))
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CKnifeLog::WriteLog(LPCTSTR strLog)
{
	BOOL bRet = FALSE;	
	if (m_hFile != NULL && strLog != NULL)
	{
		int nLen = _tcslen(strLog);
		TCHAR *pLogData = new TCHAR[nLen+1];
		if(pLogData != NULL)
		{
			memcpy(pLogData, strLog, nLen * sizeof(TCHAR));
			pLogData[nLen] = 0;

			if (::PostThreadMessage(m_dwThreadId, KNIFE_MSG_LOG_WRITE, 0, (LPARAM)pLogData))
			{
				bRet = TRUE;
			}
			else
			{
				delete []pLogData;
			}
		}
	}
	return bRet;
}

static DWORD WINAPI ThreadProc(LPVOID lpPara)
{
	CKnifeLog *pThis = (CKnifeLog *)lpPara;
	
	return pThis->ThreadFunc();
}

BOOL CKnifeLog::BeginThread(void)
{
	BOOL blRet = FALSE;
	
	if (NULL == m_hThreadHandle)
	{
		DWORD id;
		
		m_blThreadRun = TRUE;
		m_hThreadHandle = ::CreateThread(NULL, 0, ThreadProc, this, 0, &id);
		
		if (m_hThreadHandle != NULL)
		{
			m_dwThreadId = id;
			blRet = TRUE;
		}
	}	
	return blRet;
}

BOOL CKnifeLog::EndThread(DWORD dwWaitTime)
{
	BOOL blRet = FALSE;
	
	if (m_hThreadHandle != NULL)
	{
		::PostThreadMessage(m_dwThreadId, KNIFE_MSG_LOG_END_THREAD, 0, NULL);
		
		if (::WaitForSingleObject(m_hThreadHandle, dwWaitTime) != WAIT_OBJECT_0)
		{
			if (!::TerminateThread(m_hThreadHandle, 0))
			{
				return FALSE;
			}
		}
		
		::CloseHandle(m_hThreadHandle);
		m_hThreadHandle = NULL;
		m_dwThreadId	= 0;
		
		blRet = TRUE;
	}	
	return blRet;
}

DWORD CKnifeLog::ThreadFunc(void)
{
	MSG msg; 
	
	while (m_blThreadRun)
	{
		if (GetMessage(&msg, NULL, 0, 0))
		{
			switch (msg.message)
			{
			case KNIFE_MSG_LOG_START:
				{
					HandleStart();
				}
				break;

			case KNIFE_MSG_LOG_END:
				{
					HandleEnd();
				}
				break;

			case KNIFE_MSG_LOG_WRITE:
				{				
					HandleWrite((TCHAR *)msg.lParam);					
				}
				break;

			case KNIFE_MSG_LOG_END_THREAD:
				{
					m_blThreadRun = FALSE;
				}
				break;

			default:
				break;
			}
		}
	}
	
	return 0;
}

void CKnifeLog::HandleStart(void)
{	
	TCHAR strAppPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, strAppPath, MAX_PATH);
	*_tcsrchr(strAppPath,'\\') = 0;	
	TCHAR strLogFile[MAX_PATH] = {0};
	_stprintf_s(strLogFile, _T("%s\\knife.log"), strAppPath);
	
	m_hFile = CreateFile(strLogFile,
		FILE_GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}

void CKnifeLog::HandleEnd(void)
{
	if (m_hFile != NULL)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
}

void CKnifeLog::HandleWrite(const TCHAR *strLogData)
{
	if (m_hFile != NULL && strLogData != NULL)
	{
		WriteDataWithTime(strLogData);
		delete (LPVOID)strLogData;	
	}	
}

void CKnifeLog::WriteData(const TCHAR *strBuf)
{
	if (m_hFile != NULL)
	{
		DWORD	dwWrites = 0;
		int		nLen = _tcslen(strBuf)*sizeof(TCHAR);
		WriteFile(m_hFile, strBuf, nLen, &dwWrites, NULL);		
	}
}

void CKnifeLog::WriteDataWithTime(const TCHAR *strBuf)
{
	if (m_hFile != NULL)
	{
		TCHAR strTime[MAX_PATH] = {0};
		GetTimeString(strTime, MAX_PATH);

		int	nLen = _tcslen(strTime) + _tcslen(strBuf);
		
		TCHAR *strData = new TCHAR[nLen + 1];
		if(strData != NULL)
		{
			_stprintf_s(strData, nLen+1, _T("%s%s"), strTime, strBuf);
			WriteData(strData);
			delete [] strData;
		}
	}
}

void CKnifeLog::GetTimeString(TCHAR *strTime, int nLen)
{
	SYSTEMTIME	CurTime;
	
	GetLocalTime(&CurTime);
	memset(strTime, 0, sizeof(TCHAR)*nLen);

	_stprintf_s(strTime, 
		nLen,
		_T("%02d/%02d/%04d %02d:%02d:%02d.%03d: "), 
		CurTime.wMonth,
		CurTime.wDay,
		CurTime.wYear,
		CurTime.wHour,
		CurTime.wMinute,
		CurTime.wSecond,
		CurTime.wMilliseconds);
}