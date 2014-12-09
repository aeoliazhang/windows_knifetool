// ProcObjectsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "ProcObjectsDlg.h"

//list
enum
{
	LIST_COLUMN_TYPE,
	LIST_COLUMN_NAME,
	LIST_COLUMN_HANDLE,
};

const LIST_HEADER_INFO v_tableListHeaderInfo[] = 
{
	{ LIST_COLUMN_TYPE,		_T("Type"),		LVCFMT_LEFT,	150 },
	{ LIST_COLUMN_NAME,		_T("Name"), 	LVCFMT_LEFT,	500 },
	{ LIST_COLUMN_HANDLE,	_T("Handle"), 	LVCFMT_LEFT,	100 },
};

//menu
enum
{
	MENU_REFRESH_OBJECTLIST = 0x1000,
	MENU_CLOSE_OBJECT,
};

const MENU_ITEM_INFO v_tMenuList[] = 
{
	{MENU_REFRESH_OBJECTLIST,			_T("Refresh") },
	{MENU_CLOSE_OBJECT,					_T("Close handle") },
};



// CProcObjectsDlg dialog

IMPLEMENT_DYNAMIC(CProcObjectsDlg, CDialog)

CProcObjectsDlg::CProcObjectsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcObjectsDlg::IDD, pParent)
{

}

CProcObjectsDlg::~CProcObjectsDlg()
{
}

void CProcObjectsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OBJECTS, m_listctrl);
	DDX_Control(pDX, IDC_STATIC_OBJECTS_CNT, m_stcObjectCnt);
}


BEGIN_MESSAGE_MAP(CProcObjectsDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_OBJECTS, &CProcObjectsDlg::OnNMRClickListObjects)
	ON_COMMAND_RANGE(MENU_REFRESH_OBJECTLIST, MENU_CLOSE_OBJECT, OnMenuCommand)
END_MESSAGE_MAP()


// CProcObjectsDlg message handlers

BOOL CProcObjectsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	
	InitMenu();
	InitListCtrl();
	return TRUE;  
}

void CProcObjectsDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CProcObjectsDlg::SetPid(DWORD dwPid)
{
	m_dwPid = dwPid;
}

void CProcObjectsDlg::InitListCtrl()
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

void CProcObjectsDlg::RefreshList()
{
	BeginWaitCursor();

	if(m_dwPid != -1)
	{	
		PROCESS_OBJECT_INFO_LIST ProcObjList = {0};		
		KEnumProcessObjects(m_dwPid, ProcObjList);
		CString strCnt;
		strCnt.Format(_T("Objects: %d"), ProcObjList.ulCount);
		m_stcObjectCnt.SetWindowText(strCnt);

		m_listctrl.DeleteAllItems();	
		for(ULONG nIndex = 0; nIndex < ProcObjList.ulCount; nIndex++)
		{
			m_listctrl.InsertItem(nIndex, _T(""), nIndex);
			CString strHandle;		
			strHandle.Format(_T("0x%X"), ProcObjList.ObjectInfo[nIndex].Handle);	

			m_listctrl.SetItemText(nIndex, LIST_COLUMN_TYPE, ProcObjList.ObjectInfo[nIndex].Type);
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_NAME, ProcObjList.ObjectInfo[nIndex].Name);		
			m_listctrl.SetItemText(nIndex, LIST_COLUMN_HANDLE, strHandle);				
		}
	}

	EndWaitCursor();
}

void CProcObjectsDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		RefreshList();
	}
}

void CProcObjectsDlg::InitMenu()
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

void CProcObjectsDlg::DestroyMenu()
{
	m_Menu.DestroyMenu();
}

void CProcObjectsDlg::ShowMenu()
{
	CPoint pos;
	GetCursorPos(&pos);
	m_Menu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this, NULL);
}

void CProcObjectsDlg::OnNMRClickListObjects(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ShowMenu();
	*pResult = 0;
}

void CProcObjectsDlg::OnMenuCommand(UINT uID)
{
	switch(uID)
	{	
	case MENU_REFRESH_OBJECTLIST:
		{
			RefreshList();
		}
		break;

	case MENU_CLOSE_OBJECT:
		{		
			if(m_dwPid != -1)
			{
				POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
				if(pos != NULL)
				{	
					int nItem = m_listctrl.GetNextSelectedItem(pos);
					CString strHanlde = m_listctrl.GetItemText(nItem, LIST_COLUMN_HANDLE);				
					strHanlde.TrimLeft(_T("0x"));	
					string str = UnicodeToAnsiString(strHanlde);
					DWORD dwHandle = 0;
					HexStrToInt(str.c_str(), dwHandle);

					PROCESS_OBJECT_HANDLE objHanle = {0};
					objHanle.Pid = m_dwPid;
					objHanle.Handle = dwHandle; 
					KCloseHandle(objHanle);
				}
			}
		}
		break;

	default:
		break;
	}	
}