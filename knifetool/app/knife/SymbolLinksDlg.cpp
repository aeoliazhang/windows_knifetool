// SymbolLinksDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "SymbolLinksDlg.h"

enum
{
	LIST_COLUMN_NAME,
	LIST_COLUMN_LINK,
};

const LIST_HEADER_INFO v_tableListHeaderInfo[] = 
{
	{ LIST_COLUMN_NAME,			_T("Name"),			LVCFMT_LEFT,	350 },
	{ LIST_COLUMN_LINK,			_T("Link to"), 		LVCFMT_LEFT,	400 },
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


// CSymbolLinksDlg dialog

IMPLEMENT_DYNAMIC(CSymbolLinksDlg, CDialog)

CSymbolLinksDlg::CSymbolLinksDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSymbolLinksDlg::IDD, pParent)
{

}

CSymbolLinksDlg::~CSymbolLinksDlg()
{
}

void CSymbolLinksDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SYMBOLLINKS, m_listctrl);
}


BEGIN_MESSAGE_MAP(CSymbolLinksDlg, CDialog)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SYMBOLLINKS, &CSymbolLinksDlg::OnNMRClickListSymbollinks)
	ON_COMMAND_RANGE(MENU_REFRESH_LIST, MENU_REFRESH_LIST, OnMenuCommand)
END_MESSAGE_MAP()


// CSymbolLinksDlg message handlers

BOOL CSymbolLinksDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitMenu();
	InitListCtrl();
	RefreshList();
	return TRUE;  
}

void CSymbolLinksDlg::OnDestroy()
{
	CDialog::OnDestroy();
	DestroyMenu();
}

void CSymbolLinksDlg::InitListCtrl()
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

void CSymbolLinksDlg::RefreshList()
{
	SYMBOL_LINK_INFO_LIST symbolLinks;
	symbolLinks.clear();
	EnumSymbolLinks(symbolLinks);
	int nIndex = 0;
	for(SYMBOL_LINK_INFO_LIST::iterator iter = symbolLinks.begin(); iter != symbolLinks.end(); ++iter, ++nIndex)
	{
		m_listctrl.InsertItem(nIndex, _T(""), nIndex);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_NAME, iter->strName);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_LINK, iter->strLink);		
	}
}

void CSymbolLinksDlg::EnumSymbolLinks(SYMBOL_LINK_INFO_LIST &symbolLinks)
{
	symbolLinks.clear();
	DWORD dwSize = 0x4000;
	TCHAR *pTarget = new TCHAR[dwSize];
	DWORD dwRet = 0;
	while( pTarget != 0 && 
		(dwRet = QueryDosDevice(NULL, pTarget, dwSize)) == 0)
	{
		delete pTarget;
		pTarget = NULL;
		dwSize*=2;
		pTarget = new wchar_t[dwSize];
	}

	if(dwRet > 0)
	{
		TCHAR* pCard = pTarget;
		while ( '\0' != *pCard )
		{
			SYMBOL_LINK_INFO sli = {0};
			_tcscpy_s(sli.strName, pCard);			
			QueryDosDevice(pCard, sli.strLink, MAX_PATH);	
			symbolLinks.push_back(sli);
			pCard = pCard + _tcslen(pCard) + 1;
		}
	}

	if(pTarget != NULL)
	{
		delete pTarget;
		pTarget = NULL;
	}
}

void CSymbolLinksDlg::InitMenu()
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

void CSymbolLinksDlg::DestroyMenu()
{
	m_Menu.DestroyMenu();
}

void CSymbolLinksDlg::ShowMenu()
{
	CPoint pos;
	GetCursorPos(&pos);
	m_Menu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this, NULL);
}

void CSymbolLinksDlg::OnMenuCommand(UINT uID)
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
void CSymbolLinksDlg::OnNMRClickListSymbollinks(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ShowMenu();
	*pResult = 0;
}