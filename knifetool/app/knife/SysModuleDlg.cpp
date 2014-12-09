// SysModuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "SysModuleDlg.h"

enum
{
	LIST_COLUMN_NAME,
	LIST_COLUMN_IAMGE_BASE,
	LIST_COLUMN_IAMGE_SIZE,
	LIST_COLUMN_FLAGS,
	LIST_COLUMN_LOAD_ORDER_INDEX,
	LIST_COLUMN_PATH,	
};

const LIST_HEADER_INFO v_tableListHeaderInfo[] = 
{
	{ LIST_COLUMN_NAME,					_T("Name"),					LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_IAMGE_BASE,			_T("Image base"), 			LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_IAMGE_SIZE,			_T("Image size"),			LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_FLAGS,				_T("Flags"),				LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_LOAD_ORDER_INDEX,		_T("Load order index"),		LVCFMT_LEFT,	50  },
	{ LIST_COLUMN_PATH,					_T("Path"),					LVCFMT_LEFT,	300 },	
};

//menu item
enum
{
	MENU_REFRESH_LIST = 0x1000,	
};

const MENU_ITEM_INFO v_tMenuList[] = 
{
	{MENU_REFRESH_LIST,			_T("Refresh") },	
};

// CSysModuleDlg dialog

IMPLEMENT_DYNAMIC(CSysModuleDlg, CDialog)

CSysModuleDlg::CSysModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSysModuleDlg::IDD, pParent)
{

}

CSysModuleDlg::~CSysModuleDlg()
{
}

void CSysModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SYSMODULE, m_listctrl);
	DDX_Control(pDX, IDC_STATIC_SYS_MODS, m_stcModsCnt);
}


BEGIN_MESSAGE_MAP(CSysModuleDlg, CDialog)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SYSMODULE, &CSysModuleDlg::OnNMRClickListSysmodule)
	ON_COMMAND_RANGE(MENU_REFRESH_LIST, MENU_REFRESH_LIST, OnMenuCommand)
END_MESSAGE_MAP()


// CSysModuleDlg message handlers

BOOL CSysModuleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitDlgCtrls();
	RefreshList();
	return TRUE; 
}

void CSysModuleDlg::OnDestroy()
{
	CDialog::OnDestroy();	
	DestroyMenu();
}

void CSysModuleDlg::InitDlgCtrls()
{
	InitListCtrl();	
	InitMenu();
}

void CSysModuleDlg::InitListCtrl()
{
	//create
	RECT rc = {0};

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

void CSysModuleDlg::InitMenu()
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

void CSysModuleDlg::DestroyMenu()
{
	m_Menu.DestroyMenu();
}

void CSysModuleDlg::ShowMenu()
{
	CPoint pos;
	GetCursorPos(&pos);
	m_Menu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this, NULL);
}

void CSysModuleDlg::OnMenuCommand(UINT uID)
{
	switch(uID)
	{	
	case MENU_REFRESH_LIST:
		{
			RefreshList();
		}
		break;

	default:
		break;
	}	
}

void CSysModuleDlg::RefreshList()
{	
	BeginWaitCursor();
	SYS_MODULE_INFO_LIST SysModeList = {0};
	KEnumSystemModules(SysModeList);
	CString strCnt;
	strCnt.Format(_T("System modules: %d"), SysModeList.ulCount);
	m_stcModsCnt.SetWindowText(strCnt);

	m_listctrl.DeleteAllItems();	
	for(ULONG nIndex = 0; nIndex < SysModeList.ulCount; nIndex++)
	{
		CString strPath = AnsiToUnicodeString(SysModeList.ModuleInfo[nIndex].ModulePath);
		int nPos = strPath.ReverseFind('\\');
		CString strName = strPath.Mid(nPos+1, strPath.GetLength()-nPos);

		CString strImageBase, strSize, strFlags, strLoadOrderIndex;
		strImageBase.Format(_T("0x%08X"), SysModeList.ModuleInfo[nIndex].ImageBase);
		strSize.Format(_T("0x%08X"), SysModeList.ModuleInfo[nIndex].ImageSize);
		strFlags.Format(_T("0x%08X"), SysModeList.ModuleInfo[nIndex].Flags);	
		strLoadOrderIndex.Format(_T("%d"), SysModeList.ModuleInfo[nIndex].LoadOrderIndex);

		m_listctrl.InsertItem(nIndex, _T(""), nIndex);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_NAME, strName);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_IAMGE_BASE, strImageBase);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_IAMGE_SIZE, strSize);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_FLAGS, strFlags);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_LOAD_ORDER_INDEX, strLoadOrderIndex);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_PATH, strPath);
	}
	EndWaitCursor();
}

void CSysModuleDlg::OnNMRClickListSysmodule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ShowMenu();	
	*pResult = 0;
}
