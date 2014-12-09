// ProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "ProcessDlg.h"
#include "ProcModulesDlg.h"
#include "ProcThreadsDlg.h"
#include "ProcObjectsDlg.h"

enum
{
	LIST_COLUMN_IMAGENAME,
	LIST_COLUMN_PID,
	LIST_COLUMN_PATH,
	LIST_COLUMN_MEMORY,
	LIST_COLUMN_PRIORITY,
	LIST_COLUMN_THREADS,
	LIST_COLUMN_DESCRIPTION,
	LIST_COLUMN_COMPANY,
};

const LIST_HEADER_INFO v_tableListHeaderInfo[] = 
{
	{ LIST_COLUMN_IMAGENAME,			_T("Image name"),			LVCFMT_LEFT,	150 },
	{ LIST_COLUMN_PID,					_T("PID"), 					LVCFMT_LEFT,	40  },
	{ LIST_COLUMN_PATH,					_T("Image path"),			LVCFMT_LEFT,	250 },
	{ LIST_COLUMN_MEMORY,				_T("Memory"),				LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_PRIORITY,				_T("Priority"),				LVCFMT_LEFT,	60  },
	{ LIST_COLUMN_THREADS,				_T("Threads"),				LVCFMT_LEFT,	60  },
	{ LIST_COLUMN_DESCRIPTION,			_T("Description"),			LVCFMT_LEFT,	150 },
	{ LIST_COLUMN_COMPANY,				_T("Company"),				LVCFMT_LEFT,	150 },
};

//menu item
enum
{
	MENU_REFRESH_PROCESSLIST = 0x1000,
	MENU_SHOW_THREADS,	
	MENU_SHOW_MODULES,
	MENU_SHOW_OBJECTS,
	MENU_TERMINATE_PROCESS,
	MENU_FORCE_TERMINATE_PROCESS,
	MENU_HIDE_PROCESS,
};

const MENU_ITEM_INFO v_tMenuList[] = 
{
	{MENU_REFRESH_PROCESSLIST,			_T("Refresh process list") },
	{MENU_SHOW_THREADS,					_T("Show threads") },
	{MENU_SHOW_MODULES,					_T("Show modules") },	
	{MENU_SHOW_OBJECTS,					_T("Show objects") },	
	{MENU_TERMINATE_PROCESS,			_T("Terminnate process") },	
	{MENU_FORCE_TERMINATE_PROCESS,		_T("Terminate process by APC") },
	{MENU_HIDE_PROCESS,					_T("Hide process") },
};


static int   m_iSortCol = 0;
static BOOL  m_fAsc = TRUE;

// CProcessDlg dialog

IMPLEMENT_DYNAMIC(CProcessDlg, CDialog)

CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessDlg::IDD, pParent)
{

}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ProcessListCtrl);
	DDX_Control(pDX, IDC_STATIC_PROC_CNT, m_stcProcCnt);
}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_ENUM_PROCESS, OnBtnEnumProcess)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, OnRClickList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PROCESS, OnLvnColumnclickList)
	ON_COMMAND_RANGE(MENU_REFRESH_PROCESSLIST, MENU_HIDE_PROCESS, OnMenuCommand)
END_MESSAGE_MAP()


BOOL CProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitDlgCtrls();
	return TRUE;
}

void CProcessDlg::OnDestroy()
{
	CDialog::OnDestroy();
	DestroyMenu();	
}

void CProcessDlg::InitDlgCtrls()
{
	InitSubDlgArray();
	InitProcessListCtrl();	
	InitMenu();
}

void CProcessDlg::InitSubDlgArray()
{
	CRect rc;
	GetClientRect(rc);
	rc.left+= MODILESBAR_SIZE_WIDTH;
	rc.right+=MODILESBAR_SIZE_WIDTH;

	//modules dialog
	CProcModulesDlg *pModulesDlg = new CProcModulesDlg;
	if(pModulesDlg)
	{
		g_SubDlgArray.AddSubDlg(pModulesDlg, IDD_DIALOG_PROC_MODULES, rc);		
	}

	//threads dialog
	CProcThreadsDlg *pThreadsDlg = new CProcThreadsDlg;
	if(pThreadsDlg)
	{
		g_SubDlgArray.AddSubDlg(pThreadsDlg, IDD_DIALOG_PROC_THREADS, rc);
	}

	//objects dialog
	CProcObjectsDlg *pObjectsDlg = new CProcObjectsDlg;
	if(pObjectsDlg)
	{
		g_SubDlgArray.AddSubDlg(pObjectsDlg, IDD_DIALOG_PROC_OBJECTS, rc);
	}
}

void CProcessDlg::InitProcessListCtrl()
{
	DWORD dwStyle = m_ProcessListCtrl.GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_ProcessListCtrl.SetExtendedStyle(dwStyle);

	//set image list
	m_ProcessImageList.Create ( 32, 32, ILC_COLOR16 | ILC_MASK, 4, 1 );
	m_ProcessListCtrl.SetImageList(&m_ProcessImageList, LVSIL_SMALL);

	//add column
	int nColumn = sizeof(v_tableListHeaderInfo) / sizeof(v_tableListHeaderInfo[0]);
	for(int nIndex = 0; nIndex < nColumn; nIndex++)
	{
		m_ProcessListCtrl.InsertColumn(nIndex, 
			v_tableListHeaderInfo[nIndex].lpszFieldName,
			v_tableListHeaderInfo[nIndex].wFormat,
			v_tableListHeaderInfo[nIndex].nWidth);
	}	
}

void CProcessDlg::OnBtnEnumProcess()
{
	BeginWaitCursor();
	PROCESS_INFO_LIST ProcInfoList = {0};
	KEnumProcessesByNativeApi(ProcInfoList);
	CString strProcCnt;
	strProcCnt.Format(_T("Processes: %d"), ProcInfoList.ulCount);
	m_stcProcCnt.SetWindowText(strProcCnt);
	FillProcessList(ProcInfoList);
	EndWaitCursor();
}

void CProcessDlg::FillProcessList(PROCESS_INFO_LIST processList)
{
	//clear
	m_ProcessListCtrl.DeleteAllItems();
	int nImageCnt = m_ProcessImageList.GetImageCount();
	for(int i = 0; i < nImageCnt; i++)   
	{   
		m_ProcessImageList.Remove(0);  
	}   

	//insert
	for(ULONG nIndex = 0; nIndex < processList.ulCount; nIndex++)
	{		
		m_ProcessImageList.Add(GetProcessIcon(processList.ProcInfo[nIndex].ImagePath, processList.ProcInfo[nIndex].ulPID));
		m_ProcessListCtrl.InsertItem(nIndex, _T(""), nIndex);

		CString strPID, strMemory, strPriority, strThreads, strFileDescription, strCompany, strSign; 
		strPID.Format(_T("%d"), processList.ProcInfo[nIndex].ulPID);
		strMemory.Format(_T("%dK"),  processList.ProcInfo[nIndex].ulMemory/1024);
		strPriority.Format(_T("%d"), processList.ProcInfo[nIndex].Priority);
		strThreads.Format(_T("%d"), processList.ProcInfo[nIndex].ulThreads);
		GetFileDescriptionAndCompany(strFileDescription, strCompany, processList.ProcInfo[nIndex].ImagePath);

		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_IMAGENAME, processList.ProcInfo[nIndex].ImageName);
		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_PID, strPID);
		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_PATH, processList.ProcInfo[nIndex].ImagePath);
		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_MEMORY, strMemory);
		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_PRIORITY, strPriority);
		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_THREADS, strThreads);
		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_DESCRIPTION, strFileDescription);
		m_ProcessListCtrl.SetItemText(nIndex, LIST_COLUMN_COMPANY, strCompany);
	}
}

HICON CProcessDlg::GetProcessIcon(CString strFile, DWORD dwPID)
{
	HICON hExeIcon = hExeIcon = AfxGetApp()->LoadIcon(IDI_ICON_SYS_APP);
	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(sfi));
	DWORD dwRet = SHGetFileInfo(strFile, 0, &sfi, sizeof(sfi), SHGFI_ICON);

	if (dwRet > 0)
	{
		if(dwPID == 0 || dwPID == 4)
		{
			hExeIcon = AfxGetApp()->LoadIcon(IDI_ICON_SYS_APP);			
		}
		else
		{
			hExeIcon =sfi.hIcon;		
		}					
	}
	return hExeIcon;
}

void CProcessDlg::OnRClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowMenu();	
}

static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CListCtrl* pListCtrl = (CListCtrl*)lParamSort;
	CString str1 = pListCtrl->GetItemText(lParam1,m_iSortCol);
	CString str2 = pListCtrl->GetItemText(lParam2,m_iSortCol);
	int nRet = str1.CompareNoCase(str2);

	if(m_iSortCol == LIST_COLUMN_PID || m_iSortCol == LIST_COLUMN_MEMORY ||
		m_iSortCol == LIST_COLUMN_PRIORITY || m_iSortCol == LIST_COLUMN_THREADS)
	{
		int n1 = _ttoi(str1);
		int n2 = _ttoi(str2);
		nRet = n1-n2;
	}

	if(m_fAsc)
	{
		nRet *= -1;
	}

	return nRet;
}

void CProcessDlg::OnLvnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nCnt =  m_ProcessListCtrl.GetItemCount(); 

	for(int nIndex = 0; nIndex < nCnt; nIndex++)
	{
		m_ProcessListCtrl.SetItemData(nIndex, nIndex);
	}

	if( pNMLV-> iSubItem == m_iSortCol ) 
	{ 
		m_fAsc = !m_fAsc; 
	} 
	else 
	{ 
		m_fAsc = TRUE; 
		m_iSortCol = pNMLV-> iSubItem; 
	} 
	m_ProcessListCtrl.SortItems(CompareFunc, (LPARAM)&m_ProcessListCtrl );     
	*pResult = 0; 
}

void CProcessDlg::InitMenu()
{
	DestroyMenu();
	m_Menu.CreatePopupMenu();
	int nCnt = sizeof(v_tMenuList) / sizeof(v_tMenuList[0]);
	for (int i = 0; i < nCnt; i++)
	{
		m_Menu.AppendMenu(
			MF_STRING | MF_BYCOMMAND | MF_ENABLED, 
			v_tMenuList[i].uID, 
			v_tMenuList[i].strText);
	}
}

void CProcessDlg::DestroyMenu()
{
	m_Menu.DestroyMenu();
}

void CProcessDlg::ShowMenu()
{
	CPoint pos;
	GetCursorPos(&pos);
	m_Menu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this, NULL);
}

void CProcessDlg::OnMenuCommand(UINT uID)
{
	switch(uID)
	{	
	case MENU_REFRESH_PROCESSLIST:
		{
			RefreshProcessList();
		}
		break;

	case MENU_SHOW_THREADS:
		{
			ShowThreads();
		}
		break;

	case MENU_SHOW_MODULES:
		{
			ShowModules();
		}
		break;

	case MENU_SHOW_OBJECTS:
		{
			ShowObjects();
		}
		break;

	case MENU_TERMINATE_PROCESS:
		{
			TerminateProcessFromList(PROCESS_TERMINATE_NORMAL);	
		}
		break;

	case MENU_FORCE_TERMINATE_PROCESS:
		{
			TerminateProcessFromList(PROCESS_TERMINATE_APC);
		}
		break;

	case MENU_HIDE_PROCESS:
		{
			HideProcess();
		}
		break;

	default:
		break;
	}	
}

void CProcessDlg::RefreshProcessList()
{
	OnBtnEnumProcess();
}

int CProcessDlg::GetSelectedItemPID()
{
	int nPID = -1;
	POSITION pos = m_ProcessListCtrl.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int nItem = m_ProcessListCtrl.GetNextSelectedItem(pos);
		CString strPID = m_ProcessListCtrl.GetItemText(nItem, LIST_COLUMN_PID);
		nPID = _ttoi(strPID.GetBuffer(strPID.GetLength()));
		strPID.ReleaseBuffer();	
	}

	return nPID;
}

void CProcessDlg::ShowThreads()
{
	int nPID = GetSelectedItemPID();
	CProcThreadsDlg* pDlg = dynamic_cast<CProcThreadsDlg*>(g_SubDlgArray.GetSubDlg(IDD_DIALOG_PROC_THREADS));
	{
		pDlg->SetPid(nPID);
		g_SubDlgArray.ShowSubDlg(IDD_DIALOG_PROC_THREADS);		
	}
}

void CProcessDlg::ShowModules()
{
	int nPID = GetSelectedItemPID();
	CProcModulesDlg* pDlg = dynamic_cast<CProcModulesDlg*>(g_SubDlgArray.GetSubDlg(IDD_DIALOG_PROC_MODULES));
	{
		pDlg->SetPid(nPID);
		g_SubDlgArray.ShowSubDlg(IDD_DIALOG_PROC_MODULES);		
	}
}


void CProcessDlg::ShowObjects()
{
	int nPID = GetSelectedItemPID();
	CProcObjectsDlg* pDlg = dynamic_cast<CProcObjectsDlg*>(g_SubDlgArray.GetSubDlg(IDD_DIALOG_PROC_OBJECTS));
	{
		pDlg->SetPid(nPID);
		g_SubDlgArray.ShowSubDlg(IDD_DIALOG_PROC_OBJECTS);		
	}
}

void CProcessDlg::TerminateProcessFromList(PROCESS_TERMINATE_TYPE nTerminateType)
{
	int nPID = GetSelectedItemPID();
	switch(nTerminateType)
	{
	case PROCESS_TERMINATE_NORMAL:
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE, nPID);
			if ( hProcess != NULL )
			{
				if(!TerminateProcess(hProcess, 0))
				{
					AfxMessageBox(GetLastErrorString());
				}
			}
			else
			{
				AfxMessageBox(GetLastErrorString());
			}			
		}
		break;

	case PROCESS_TERMINATE_APC:
		{
			KTerminateProcessByAPC(nPID);
		}
		break;	

	default:
		break;
	}	
}

void CProcessDlg::HideProcess()
{
	int nPID = GetSelectedItemPID();
	if(nPID != -1)
	{
		KHideProcess(nPID);
	}
}