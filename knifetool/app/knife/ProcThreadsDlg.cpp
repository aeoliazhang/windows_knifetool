// ProcThreadsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "ProcThreadsDlg.h"

enum
{
	LIST_COLUMN_TID,	
	LIST_COLUMN_STARTADDRESS,
	LIST_COLUMN_MODULE,
	LIST_COLUMN_STATE,
	LIST_COLUMN_CONTEXTSWITCHES,
	LIST_COLUMN_PRIORITY,
};

const LIST_HEADER_INFO v_tableListHeaderInfo[] = 
{
	{ LIST_COLUMN_TID,				_T("Tid"),				LVCFMT_LEFT,	60  },
	{ LIST_COLUMN_STARTADDRESS,		_T("Start Address"),	LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_MODULE,			_T("Module"),			LVCFMT_LEFT,	300 },
	{ LIST_COLUMN_STATE,			_T("State"),			LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_CONTEXTSWITCHES,  _T("Context Switches"),	LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_PRIORITY,			_T("Priority"),			LVCFMT_LEFT,	60 },
};

// CProcThreadsDlg dialog

IMPLEMENT_DYNAMIC(CProcThreadsDlg, CDialog)

CProcThreadsDlg::CProcThreadsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcThreadsDlg::IDD, pParent)
{
	m_dwPid = -1;
}

CProcThreadsDlg::~CProcThreadsDlg()
{
}

void CProcThreadsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_THREADS, m_listctrl);
	DDX_Control(pDX, IDC_STATIC_THREAD_CNT, m_stcThreadCnt);
}


BEGIN_MESSAGE_MAP(CProcThreadsDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CProcThreadsDlg message handlers

BOOL CProcThreadsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitListCtrl();
	return TRUE; 	
}

void CProcThreadsDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CProcThreadsDlg::SetPid(DWORD dwPid)
{
	m_dwPid = dwPid;
}

void CProcThreadsDlg::InitListCtrl()
{
	DWORD dwStyle = m_listctrl.GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_listctrl.SetExtendedStyle(dwStyle);

	//add column
	int nColumn = sizeof(v_tableListHeaderInfo) / sizeof(v_tableListHeaderInfo[0]);
	for(int nIndex = 0; nIndex < nColumn; nIndex++)
	{
		m_listctrl.InsertColumn(nIndex, 
			v_tableListHeaderInfo[nIndex].lpszFieldName,
			v_tableListHeaderInfo[nIndex].wFormat,
			v_tableListHeaderInfo[nIndex].nWidth);
	}	
}

void CProcThreadsDlg::RefreshList()
{
	BeginWaitCursor();
	if(m_dwPid != -1)
	{	
		PROCESS_THREAD_INFO_LIST ProcThreadList = { 0 };
		KEnumProcessThreads(m_dwPid, ProcThreadList);
		CString strCnt;
		strCnt.Format(_T("Thread: %d"), ProcThreadList.ulCount);
		m_stcThreadCnt.SetWindowText(strCnt);

		m_listctrl.DeleteAllItems();	
		for(ULONG nIndex = 0; nIndex < ProcThreadList.ulCount; nIndex++)
		{
			m_listctrl.InsertItem(nIndex, _T(""), nIndex);
			CString strTid, strStartAddr, strState, strContextSwitches, strPriority, strBasePriority;
			strTid.Format(_T("%d"), ProcThreadList.ThreadInfo[nIndex].Tid);	
			strStartAddr.Format(_T("0x%08X"), ProcThreadList.ThreadInfo[nIndex].StartAddress);				
			strState = GetThreadState((KTHREAD_STATE)ProcThreadList.ThreadInfo[nIndex].State);
			strContextSwitches.Format(_T("%d"), ProcThreadList.ThreadInfo[nIndex].ContextSwitches);	
			strPriority.Format(_T("%d"), ProcThreadList.ThreadInfo[nIndex].Priority);	

			m_listctrl.SetItemText(nIndex, LIST_COLUMN_TID, strTid);
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_STARTADDRESS, strStartAddr);		
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_MODULE, ProcThreadList.ThreadInfo[nIndex].ModuleName);	
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_STATE, strState);
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_CONTEXTSWITCHES, strContextSwitches);		
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_PRIORITY, strPriority);
		}
	}

	EndWaitCursor();
}

void CProcThreadsDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		RefreshList();
	}
}

CString CProcThreadsDlg::GetThreadState(KTHREAD_STATE state)
{

	CString strState = _T("Unknown");
	switch(state)
	{
	case Initialized:
		{
			strState = _T("Initialized");
		}
		break;

	case Ready:
		{
			strState = _T("Ready");
		}
		break;

	case Running:
		{
			strState = _T("Running");
		}
		break;

	case Standby:
		{
			strState = _T("Standby");
		}
		break;

	case Terminated:
		{
			strState = _T("Terminated");
		}
		break;

	case Waiting:
		{
			strState = _T("Waiting");
		}
		break;

	case Transition:
		{
			strState = _T("Transition");
		}
		break;

	case DeferredReady:
		{
			strState = _T("DeferredReady");
		}
		break;

	case GateWait:
		{
			strState = _T("GateWait");
		}
		break;

	default:
		{
			strState = _T("Unknown");
		}
		break;
	}

	return strState;
}
