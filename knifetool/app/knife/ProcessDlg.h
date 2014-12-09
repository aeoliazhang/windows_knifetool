#pragma once
#include "afxcmn.h"
#include "afxwin.h"

typedef enum
{
	PROCESS_TERMINATE_NORMAL,
	PROCESS_TERMINATE_APC,
}PROCESS_TERMINATE_TYPE;

// CProcessDlg dialog
class CProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcessDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBtnEnumProcess();
	afx_msg void OnRClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult);	
	// Dialog Data
	enum { IDD = IDD_DIALOG_PROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void  InitDlgCtrls();
	void  InitSubDlgArray();
	void  InitProcessListCtrl();
	void  FillProcessList(PROCESS_INFO_LIST processList);
	HICON GetProcessIcon(CString strFile, DWORD dwPID);
	void  InitMenu();
	void  DestroyMenu();
	void  ShowMenu();
	void  OnMenuCommand(UINT uID);
	void  RefreshProcessList();
	int   GetSelectedItemPID();
	void  ShowThreads();
	void  ShowModules();
	void  ShowObjects();
	void  TerminateProcessFromList(PROCESS_TERMINATE_TYPE nTerminateType);
	void  HideProcess();

private:
	CListCtrl   m_ProcessListCtrl;
	CImageList  m_ProcessImageList;
	CButton		m_btnEnumProcess;
	CMenu		m_Menu;	

public:
	CStatic m_stcProcCnt;
};
