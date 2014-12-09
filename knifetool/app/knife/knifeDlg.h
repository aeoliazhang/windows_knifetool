
// knifeDlg.h : header file
//

#pragma once

#include "TrayIcon.h"
#include "afxwin.h"


// CknifeDlg dialog
class CknifeDlg : public CDialog
{
// Construction
public:
	CknifeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_KNIFE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	//afx_

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	CMFCOutlookBar		m_ModulesBar;
	CMFCOutlookBarPane	m_FeaturesPane;
	CMFCOutlookBarPane	m_PDMPane;
	CTrayIcon			m_trayIcon;
	CMenu				m_trayMenu;

private:
	void	InitCtrls();
	void	InitModulesBar();
	void	InitSubDlgArray();
	void	InitTrayIcon();
	void	DestroyTrayIcon();
	LRESULT OnTrayIconNotification(WPARAM wParam, LPARAM lParam);
	void	OnTrayIconCommand(UINT uID);
	void	InitTrayMenu();
	void	DestroyTrayMenu();
	void	ShowTrayMenu();
	void    OnProcess();
	void    OnSysModule();
	void    OnSSDT();
	void    OnSymbolLinks();
};
