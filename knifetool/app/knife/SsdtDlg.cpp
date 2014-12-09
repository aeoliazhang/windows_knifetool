// SsdtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "SsdtDlg.h"

enum
{
	LIST_COLUMN_INDEX,
	LIST_COLUMN_CURRADDRESS,
	LIST_COLUMN_MODULENAME,
	LIST_COLUMN_ORGADDRESS,	
	LIST_COLUMN_FUNCNAME,	
};

const LIST_HEADER_INFO v_tableListHeaderInfo[] = 
{
	{ LIST_COLUMN_INDEX,		_T("Index"),			LVCFMT_LEFT,	50 },
	{ LIST_COLUMN_CURRADDRESS,	_T("Current Address"),	LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_MODULENAME,	_T("Module Name"),		LVCFMT_LEFT,	200 },
	{ LIST_COLUMN_ORGADDRESS,	_T("Original Address"),	LVCFMT_LEFT,	100 },
	{ LIST_COLUMN_FUNCNAME,		_T("Fucntion Name"),	LVCFMT_LEFT,	200 },
};

// CSsdtDlg dialog

IMPLEMENT_DYNAMIC(CSsdtDlg, CDialog)

CSsdtDlg::CSsdtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSsdtDlg::IDD, pParent)
{

}

CSsdtDlg::~CSsdtDlg()
{
}

void CSsdtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SSDT, m_listctrl);
}


BEGIN_MESSAGE_MAP(CSsdtDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSsdtDlg message handlers


BOOL CSsdtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitListCtrl();
	RefreshListCtrl();
	return TRUE; 
}

void CSsdtDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CSsdtDlg::InitListCtrl()
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

void CSsdtDlg::RefreshListCtrl()
{
	SSDT_INFO_LIST ssdtList = {0};
	KEnumSSDT(ssdtList);
	
	m_listctrl.DeleteAllItems();

	for(ULONG nIndex = 0; nIndex < ssdtList.ulCount; nIndex++)
	{
		m_listctrl.InsertItem(nIndex, _T(""), nIndex);

		CString strServiceNum, strCurrAddr, strOrgAddr, strModule, strFunc;
		strServiceNum.Format(_T("0x%X"), ssdtList.SsdtInfo[nIndex].ulIndex);
		strCurrAddr.Format(_T("0x%08X"), ssdtList.SsdtInfo[nIndex].ulCurrAddress);
		strOrgAddr.Format(_T("0x%08X"), ssdtList.SsdtInfo[nIndex].ulOrgAddress);
		strModule = AnsiToUnicodeString(ssdtList.SsdtInfo[nIndex].ModuleName);
		strFunc = AnsiToUnicodeString(ssdtList.SsdtInfo[nIndex].FuncName);

		m_listctrl.SetItemText(nIndex, LIST_COLUMN_INDEX, strServiceNum);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_CURRADDRESS, strCurrAddr);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_MODULENAME, strModule);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_ORGADDRESS, strOrgAddr);
		m_listctrl.SetItemText(nIndex, LIST_COLUMN_FUNCNAME, strFunc);
	}
}
