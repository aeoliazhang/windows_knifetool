// FirstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "FirstDlg.h"


// CFirstDlg dialog

IMPLEMENT_DYNAMIC(CFirstDlg, CDialog)

CFirstDlg::CFirstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFirstDlg::IDD, pParent)
{

}

CFirstDlg::~CFirstDlg()
{
}

void CFirstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_KNIFE_PIC, m_stcKnifePic);
}


BEGIN_MESSAGE_MAP(CFirstDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFirstDlg message handlers

BOOL CFirstDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	HBITMAP hBmp = ::LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_KNIFE));
	m_stcKnifePic.SetBitmap(hBmp);
	return TRUE; 
}

void CFirstDlg::OnDestroy()
{
	CDialog::OnDestroy();
}
