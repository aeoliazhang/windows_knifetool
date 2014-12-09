// ProcModulesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "ProcModulesDlg.h"

enum
{
	LIST_COLUMN_BASE,
	LIST_COLUMN_PATH,
	LIST_COLUMN_IAMGESIZE,
	LIST_COLUMN_DESCRIPTION,
	LIST_COLUMN_COMPANY,
};

const LIST_HEADER_INFO v_tableListHeaderInfo[] = 
{
	{ LIST_COLUMN_BASE,			_T("Base"),			LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_PATH,			_T("Path"), 		LVCFMT_LEFT,	300 },
	{ LIST_COLUMN_IAMGESIZE,	_T("Image Size"), 	LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_DESCRIPTION,	_T("Description"), 	LVCFMT_LEFT,	120 },
	{ LIST_COLUMN_COMPANY,		_T("Company"), 		LVCFMT_LEFT,	120 },
};


//menu item
enum
{
	MENU_REFRESH_MODULELIST = 0x1000,
	MENU_HIDE_MODULE,
};

const MENU_ITEM_INFO v_tMenuList[] = 
{
	{MENU_REFRESH_MODULELIST,			_T("Refresh Module list") },
	{MENU_HIDE_MODULE,					_T("Hide module") },
};



// CProcModulesDlg dialog

IMPLEMENT_DYNAMIC(CProcModulesDlg, CDialog)

CProcModulesDlg::CProcModulesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcModulesDlg::IDD, pParent)
{
	m_dwPid = -1;
}

CProcModulesDlg::~CProcModulesDlg()
{
}

void CProcModulesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODULES, m_listctrl);
	DDX_Control(pDX, IDC_STATIC_MODULES_CNT, m_stcModCnt);
}

BEGIN_MESSAGE_MAP(CProcModulesDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MODULES, &CProcModulesDlg::OnNMRClickListModules)
	ON_COMMAND_RANGE(MENU_REFRESH_MODULELIST, MENU_HIDE_MODULE, OnMenuCommand)
END_MESSAGE_MAP()


// CProcModulesDlg message handlers

BOOL CProcModulesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitMenu();
	InitListCtrl();
	return TRUE; 
}

void CProcModulesDlg::OnDestroy()
{
	CDialog::OnDestroy();	
	DestroyMenu();
}

void CProcModulesDlg::SetPid(DWORD dwPid)
{
	m_dwPid = dwPid;
}

void CProcModulesDlg::InitListCtrl()
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

void CProcModulesDlg::RefreshList()
{
	BeginWaitCursor();
	if(m_dwPid != -1)
	{	
		PROCESS_MODULE_INFO_LIST ProcModList = {0};
		KEnumProcessModules(m_dwPid, ProcModList);
		CString strCnt;
		strCnt.Format(_T("Modules: %d"), ProcModList.ulCount);
		m_stcModCnt.SetWindowText(strCnt);

		m_listctrl.DeleteAllItems();	
		for(ULONG nIndex = 0; nIndex < ProcModList.ulCount; nIndex++)
		{
			m_listctrl.InsertItem(nIndex, _T(""), nIndex);
			CString strDllBase, strImageSize, strFileDescription, strCompany;	
			strDllBase.Format(_T("0x%08X"), ProcModList.ModuleInfo[nIndex].DllBase);
			strImageSize.Format(_T("0x%08X"), ProcModList.ModuleInfo[nIndex].SizeOfImage);
			
			GetFileDescriptionAndCompany(strFileDescription, strCompany, ProcModList.ModuleInfo[nIndex].DllPath);

			m_listctrl.SetItemText(nIndex, LIST_COLUMN_BASE, strDllBase);
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_PATH, ProcModList.ModuleInfo[nIndex].DllPath);
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_IAMGESIZE, strImageSize);
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_DESCRIPTION, strFileDescription);
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_COMPANY, strCompany);
			m_listctrl.SetItemData(nIndex,ProcModList.ModuleInfo[nIndex].DllBase );
		}
	}

	EndWaitCursor();
}
void CProcModulesDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		RefreshList();
	}
}

void CProcModulesDlg::InitMenu()
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

void CProcModulesDlg::DestroyMenu()
{
	m_Menu.DestroyMenu();
}

void CProcModulesDlg::ShowMenu()
{
	CPoint pos;
	GetCursorPos(&pos);
	m_Menu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this, NULL);
}

void CProcModulesDlg::OnNMRClickListModules(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ShowMenu();
	*pResult = 0;
}


void CProcModulesDlg::OnMenuCommand(UINT uID)
{
	switch(uID)
	{	
	case MENU_REFRESH_MODULELIST:
		{
			RefreshList();
		}
		break;

	case MENU_HIDE_MODULE:
		{
			DWORD dwDllBase = GetSelectedItemDllBase();
			if(m_dwPid != -1 && dwDllBase != -1)
			{
				PROCESS_MODULE_BASE ProcModBase = {0};
				ProcModBase.Pid = m_dwPid;
				ProcModBase.ModBase = dwDllBase;
				KHideProcessModule(ProcModBase);	
			}			
		}
		break;

	default:
		break;
	}	
}

DWORD CProcModulesDlg::GetSelectedItemDllBase()
{
	DWORD dwDllBase = -1;
	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{	
		int nItem = m_listctrl.GetNextSelectedItem(pos);
		dwDllBase = m_listctrl.GetItemData(nItem);
	}

	return dwDllBase;
}