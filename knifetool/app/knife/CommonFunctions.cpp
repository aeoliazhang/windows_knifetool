#include "StdAfx.h"
#include "CommonFunctions.h"
#include <Wintrust.h>
#include <softpub.h>
#pragma comment( lib, "Wintrust.lib" )  
#pragma comment( lib, "version.lib" )  

#define MAX_ERROR_LEN		1024


CString GetLastErrorString()
{
	CString strErr;
	int nErr = GetLastError();
	TCHAR strError[MAX_ERROR_LEN] = {0};
	FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		nErr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		strError,
		MAX_ERROR_LEN,
		NULL 
		);

	strErr.Format(_T("Error = %d\n%s"), nErr, strError);

	return strErr;
}	

BOOL EnablePrivilege(LPCTSTR szPrivName)
{
	BOOL bRet = FALSE;

	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	
	if( OpenProcessToken( GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken ) )
	{
		if( LookupPrivilegeValue( NULL, szPrivName, &luid) )
		{
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			tp.Privileges[0].Luid = luid;

			if( AdjustTokenPrivileges( hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
			{
				bRet  = TRUE;				
			}
			else
			{
				CloseHandle(hToken);
				AfxMessageBox(GetLastErrorString());
			}
		}
		else
		{
			CloseHandle(hToken);
			AfxMessageBox(GetLastErrorString());
		}
	}
	else
	{
		AfxMessageBox(GetLastErrorString());
	}	

	return false;
}

BOOL HexStrToInt(const char *szHexStr, DWORD &dwHexVal)
{
	DWORD  dw;
	const char *pCH = szHexStr;
	while (*pCH)
	{
		char c = toupper(*pCH++);
		if (c >= 'A' && c <= 'F')
		{
			dw = (DWORD)c - ((DWORD)'A' - 10);
		}
		else if (c >= '0' && c <= '9')
		{
			dw = (DWORD)c - (DWORD)'0';
		}
		else
		{
			return FALSE;
		}
		dwHexVal = (dwHexVal << 4) + dw;
	}

	return TRUE;
}

//A<->W
wchar_t *AnsiToUnicode(const char* pSrc)
{
	wchar_t *pDes = NULL;
	if (pSrc == NULL)
		return pDes;

	int i = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);
	pDes = new TCHAR[i+1];
	MultiByteToWideChar(CP_ACP,   0,   pSrc,   -1,   pDes,   i);

	return pDes;
}


char *UnicodeToAnsi(const wchar_t *pSrc)
{
	char *pDes = NULL;
	if (pSrc == NULL) 
		return pDes;

	int i = WideCharToMultiByte(CP_ACP, 0, pSrc, -1, NULL, 0, NULL, NULL);
	pDes = new char[i+1];
	WideCharToMultiByte(CP_ACP, 0, pSrc, -1, pDes, i, NULL, NULL);

	return pDes;
}

CString AnsiToUnicodeString(const char *pSrc)
{
	CString strDes = _T("");

	wchar_t * pDes = AnsiToUnicode(pSrc);
	if(pDes != NULL)
	{
		strDes = pDes;
		delete []pDes;
	}

	return strDes;
}

string UnicodeToAnsiString(const wchar_t *pSrc)
{
	string strDes = "";
	char *pDes = UnicodeToAnsi(pSrc);
	if(pDes != NULL)
	{
		strDes = pDes;
		delete []pDes;
	}
	return strDes;
}

//U<->UTF8
char* UnicodeToUTF8(const wchar_t* pSrc)
{
	char *pDes = NULL;

	if (pSrc == NULL) 
		return pDes;

	int i = WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, NULL, 0, NULL, NULL);

	pDes = new char[i+1];
	WideCharToMultiByte(CP_UTF8, 0, pSrc, -1, pDes, i, NULL, NULL); 

	return pDes;
}


wchar_t *UTF8ToUnicode(const char* pSrc)
{
	wchar_t *pDes = NULL;
	if (pSrc == NULL) 
		return pDes;

	int i = MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);

	pDes = new WCHAR[i+1];
	MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, pDes, i);

	return pDes;
}

string UnicodeToUTF8String(const wchar_t *pSrc)
{
	string strDes = "";
	char *pDes = UnicodeToUTF8(pSrc);
	if(pDes != NULL)
	{
		strDes = pDes;
		delete []pDes;
	}

	return strDes;
}

CString UTF8ToUnicodeString(const char* pSrc)
{
	CString strDes = _T("");

	wchar_t * pDes = UTF8ToUnicode(pSrc);
	if(pDes != NULL)
	{
		strDes = pDes;
		delete []pDes;
	}

	return strDes;
}

void GetFileDescriptionAndCompany(CString &strFileDescription, CString &strCompany, CString strFile)
{
	if(!strFile.IsEmpty())
	{
		DWORD dwHandle = 0;        
		DWORD dwDataSize = ::GetFileVersionInfoSize(strFile, &dwHandle);    
		if ( dwDataSize > 0 ) 
		{
			BYTE *pVersionData = new BYTE[dwDataSize];    
			GetFileVersionInfo(strFile, dwHandle, dwDataSize, pVersionData);

			UINT nQuerySize;   
			DWORD* pTransTable;   
			if (::VerQueryValue(pVersionData, _T("\\VarFileInfo\\Translation"),   
				(void **)&pTransTable, &nQuerySize) )   
			{   
				DWORD dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));  

				// File description
				UINT nQuerySize;   
				LPVOID lpData;   
				CString strBlockName;   
				strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"), 
					dwLangCharset, _T("FileDescription"));  

				if ( ::VerQueryValue(pVersionData, strBlockName.GetBuffer(0), &lpData, &nQuerySize) )
				{
					strFileDescription.Format(_T("%s"), (LPTSTR)lpData);							
				}

				//Company 

				strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"), 
					dwLangCharset, _T("CompanyName"));  

				if ( ::VerQueryValue(pVersionData, strBlockName.GetBuffer(0), &lpData, &nQuerySize) )
				{
					strCompany.Format(_T("%s"), (LPTSTR)lpData);				
				}
			}  
			delete pVersionData;
		}
	}
}

BOOL VerifyFileSignature(LPCWSTR pszSourceFile)
{	
	BOOL bRet = FALSE;

	WINTRUST_FILE_INFO FileData;
	memset(&FileData, 0, sizeof(FileData));
	FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
	FileData.pcwszFilePath = pszSourceFile;
	FileData.hFile = NULL;
	FileData.pgKnownSubject = NULL;

	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA WinTrustData;

	memset(&WinTrustData, 0, sizeof(WinTrustData));
	WinTrustData.cbStruct = sizeof(WinTrustData);
	WinTrustData.pPolicyCallbackData = NULL;
	WinTrustData.pSIPClientData = NULL;
	WinTrustData.dwUIChoice = WTD_UI_NONE;
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; 
	WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
	WinTrustData.dwStateAction = 0;
	WinTrustData.hWVTStateData = NULL;
	WinTrustData.pwszURLReference = NULL;
	WinTrustData.dwProvFlags = WTD_SAFER_FLAG;
	WinTrustData.dwUIContext = 0;
	WinTrustData.pFile = &FileData;

	if(WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData) == ERROR_SUCCESS)
	{
		bRet = TRUE;
	}

	return bRet;
}