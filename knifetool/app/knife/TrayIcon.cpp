#include "StdAfx.h"
#include "TrayIcon.h"

CTrayIcon::CTrayIcon()
{
	m_bVisibled = FALSE;
}

CTrayIcon::~CTrayIcon(void)
{
	DeleteIcon();
}

void CTrayIcon::InitTrayIcon(HWND hWnd,UINT uID,UINT uNotifyMsgID,HICON hIcon,LPCTSTR lpTip, UINT uTimeOut)
{
	memset(&m_nid, 0, sizeof(m_nid));
	m_nid.cbSize = NOTIFYICONDATA_V2_SIZE;
	m_nid.uID = uID;
	m_nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
	m_nid.uCallbackMessage = uNotifyMsgID;
	m_nid.hIcon = hIcon;
	if(lpTip != NULL)
	{
		_tcsncpy_s(m_nid.szTip, lpTip, sizeof(m_nid.szTip)/sizeof(TCHAR)-1);
	}
	else
	{
		m_nid.szTip[0] = _T('\0');
	}
	m_nid.uTimeout = uTimeOut;
	m_nid.hWnd=hWnd;
}

BOOL CTrayIcon::IsVisibled()
{
	return m_bVisibled;
}

BOOL CTrayIcon::ShowIcon()
{
	if(!m_bVisibled)
	{
		m_nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
		m_bVisibled=Shell_NotifyIcon(NIM_ADD,&m_nid);
	}
	return m_bVisibled;
}

BOOL CTrayIcon::DeleteIcon()
{
	BOOL bRet= FALSE;
	if(m_bVisibled)
	{
		bRet = Shell_NotifyIcon(NIM_DELETE,&m_nid);
		m_bVisibled = FALSE;
	}
	return bRet;
}

BOOL CTrayIcon::SetIcon(HICON hNewIcon)
{
	BOOL bRet = FALSE;

	m_nid.uFlags = NIF_ICON; 

	m_nid.hIcon = hNewIcon;
	if(m_bVisibled)
	{		
		bRet = Shell_NotifyIcon(NIM_MODIFY,&m_nid);		
	}
	return bRet;	
}

BOOL CTrayIcon::SetTipText(LPCTSTR lpNewTipText)
{
	BOOL bRet = FALSE;

	m_nid.uFlags = NIF_TIP;

	if(lpNewTipText != NULL)
	{
		memset(m_nid.szTip, 0, sizeof(m_nid.szTip));
		_tcsncpy_s(m_nid.szTip, lpNewTipText, sizeof(m_nid.szTip)/sizeof(TCHAR)-1);
		if(m_bVisibled)
		{
			bRet=Shell_NotifyIcon(NIM_MODIFY,&m_nid);
		}
	}
	return bRet;
}

BOOL CTrayIcon::SetInfoText(LPCTSTR lpNewInfoText)
{
	BOOL bRet = FALSE;
	m_nid.uFlags = NIF_INFO;

	if(lpNewInfoText != NULL)
	{
		memset(m_nid.szInfo, 0, sizeof(m_nid.szTip));
		_tcsncpy_s(m_nid.szInfo, lpNewInfoText, sizeof(m_nid.szInfo)/sizeof(TCHAR)-1);
		if(m_bVisibled)
		{
			bRet=Shell_NotifyIcon(NIM_MODIFY,&m_nid);
		}
	}
	return bRet;
}

BOOL CTrayIcon::Rebuild()
{
	BOOL bRet = FALSE;
	if(m_bVisibled)
	{
		DeleteIcon();		
		bRet = ShowIcon();
	}

	return 	bRet;
}

