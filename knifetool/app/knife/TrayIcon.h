#pragma once

class CTrayIcon
{
public:
	CTrayIcon();
	~CTrayIcon(void);

public:
	void InitTrayIcon(HWND hWnd,UINT uID,UINT uNotifyMsgID, HICON hIcon,LPCTSTR lpTip = NULL, UINT uTimeOut = 10000);
	BOOL IsVisibled();
	BOOL ShowIcon();
	BOOL DeleteIcon();
	BOOL SetIcon(HICON hNewIcon);
	BOOL SetTipText(LPCTSTR lpNewTipText);
	BOOL SetInfoText(LPCTSTR lpNewInfoText);
	BOOL Rebuild();

private:
	NOTIFYICONDATA	m_nid;
	BOOL			m_bVisibled;
};


