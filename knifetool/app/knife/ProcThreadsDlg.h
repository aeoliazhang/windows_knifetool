#pragma once
#include "afxcmn.h"
#include "afxwin.h"


typedef enum _KTHREAD_STATE {
	Initialized,
	Ready,
	Running,
	Standby,
	Terminated,
	Waiting,
	Transition,
	DeferredReady,
	GateWait,
} KTHREAD_STATE;

// CProcThreadsDlg dialog

class CProcThreadsDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcThreadsDlg)

public:
	CProcThreadsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcThreadsDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

// Dialog Data
	enum { IDD = IDD_DIALOG_PROC_THREADS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void SetPid(DWORD dwPid);

private:
	void InitListCtrl();
	void RefreshList();
	CString GetThreadState(KTHREAD_STATE state);

private:
	CListCtrl m_listctrl;
	CStatic   m_stcThreadCnt;
	DWORD     m_dwPid;
};
