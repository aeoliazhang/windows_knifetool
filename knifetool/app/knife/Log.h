#pragma once


class CKnifeLog
{
public:
	CKnifeLog();
	~CKnifeLog();

public:
	BOOL StartLog();
	BOOL EndLog();
	BOOL WriteLog(LPCTSTR strLog);


public:
	DWORD ThreadFunc(void);

private:
	BOOL BeginThread(void);
	BOOL EndThread(DWORD dwWaitTime = 100);
	void HandleStart(void);
	void HandleEnd(void);
	void HandleWrite(const TCHAR *strLogData);
	void WriteData(const TCHAR *strBuf);
	void WriteDataWithTime(const TCHAR *strBuf);
	void GetTimeString(TCHAR *strTime, int nLen);

	BOOL				m_blThreadRun;
	volatile HANDLE		m_hThreadHandle;
	DWORD				m_dwThreadId;	
	HANDLE				m_hFile;
	TCHAR               m_strLogFile[MAX_PATH];
};

