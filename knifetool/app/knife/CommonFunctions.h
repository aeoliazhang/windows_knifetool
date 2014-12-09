#pragma once
//Format last error string information
extern CString GetLastErrorString();

//enable privilege
extern BOOL EnablePrivilege(LPCTSTR szPrivName);

//format hex string to integer
extern BOOL HexStrToInt(const char *szHexStr, DWORD &dwHexVal);

//A<->W
extern wchar_t* AnsiToUnicode(const char* pSrc);				//need delete buffer 
extern char*    UnicodeToAnsi(const wchar_t *pSrc);				//need delete buffer 
extern CString  AnsiToUnicodeString(const char *pSrc);
extern string	UnicodeToAnsiString(const wchar_t *pSrc);

//U<->UTF8
extern char*	UnicodeToUTF8(const wchar_t* pSrc);				//need delete buffer 
extern wchar_t* UTF8ToUnicode(const char* pSrc);				//need delete buffer 
extern string	UnicodeToUTF8String(const wchar_t *pSrc);	
extern CString  UTF8ToUnicodeString(const char* pSrc);

//get executable file information
void GetFileDescriptionAndCompany(CString &strFileDescription, CString &strCompany, CString strFile);

//Digital Signature
BOOL VerifyFileSignature(LPCWSTR pszSourceFile);

