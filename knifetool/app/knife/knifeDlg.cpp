
// knifeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "knife.h"
#include "knifeDlg.h"
#include "FirstDlg.h"
#include "SettingDlg.h"
#include "ProcessDlg.h"
#include "SysModuleDlg.h"
#include "SsdtDlg.h"
#include "SymbolLinksDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//systray menu item
enum
{
	SYSTRAY_MENU_SHOW = 0x1000,	
	SYSTRAY_MENU_QUIT,
};

const MENU_ITEM_INFO v_tTrayMenuList[] = 
{
	{ SYSTRAY_MENU_SHOW,		_T("Show Window") },
	{ SYSTRAY_MENU_QUIT,		_T("Quit")		  },	
};

//register task bar restart message
const UINT KNIFE_MSG_TASK_BAR_RESTART = ::RegisterWindowMessage(_T("TaskbarCreated"));

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CknifeDlg dialog
CknifeDlg::CknifeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CknifeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CknifeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CknifeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_MESSAGE(KNIFE_MSG_TRAY_ICON_NOTIFICATION, OnTrayIconNotification)
	ON_COMMAND_RANGE(SYSTRAY_MENU_SHOW, SYSTRAY_MENU_QUIT, OnTrayIconCommand)
	ON_COMMAND(IDC_BTN_PROCESS, OnProcess)
	ON_COMMAND(IDC_BTN_SYSMOD, OnSysModule)
	ON_COMMAND(IDC_BTN_SSDT, OnSSDT)
	ON_COMMAND(IDC_BTN_SYMBOLLINKS, OnSymbolLinks)
	
END_MESSAGE_MAP()


// CknifeDlg message handlers
BOOL CknifeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitCtrls();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CknifeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CknifeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CknifeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CknifeDlg::OnDestroy()
{
	CDialog::OnDestroy();
	DestroyTrayIcon();
	DestroyTrayMenu();
}

void CknifeDlg::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CknifeDlg::InitCtrls()
{
	InitModulesBar();
	InitSubDlgArray();
	InitTrayIcon();
	InitTrayMenu();
}

void CknifeDlg::InitModulesBar()
{
	//CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
	//CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	//m_ModulesBar.SetMode2003();

	if(m_ModulesBar.Create(L"", this, CRect(0, 0, MODILESBAR_SIZE_WIDTH, MODILESBAR_SIZE_HEIGHT), 
		IDC_MODULES_BAR, WS_CHILD | WS_VISIBLE | CBRS_LEFT))
	{
		CMFCOutlookBarTabCtrl* pCtrl = (CMFCOutlookBarTabCtrl*)m_ModulesBar.GetUnderlyingWindow();

		if (pCtrl != NULL)
		{
			pCtrl->EnableInPlaceEdit(TRUE);
			DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;
			CRect rectDummy(0, 0, 0, 0);
			const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

			//feature pane
			m_FeaturesPane.Create( &m_ModulesBar, AFX_DEFAULT_TOOLBAR_STYLE,IDC_FEATURES_PANE);
			m_FeaturesPane.EnableTextLabels(TRUE);	
			HICON hIconProcess = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
			m_FeaturesPane.AddButton(hIconProcess, L"Process", IDC_BTN_PROCESS);
			m_FeaturesPane.AddButton(hIconProcess, L"System Module", IDC_BTN_SYSMOD);
			m_FeaturesPane.AddButton(hIconProcess, L"SSDT", IDC_BTN_SSDT);
			m_FeaturesPane.AddButton(hIconProcess, L"Symbol links", IDC_BTN_SYMBOLLINKS);

			pCtrl->AddControl(&m_FeaturesPane, L"Features", 2, TRUE, dwStyle);

			//
			m_PDMPane.Create( &m_ModulesBar, AFX_DEFAULT_TOOLBAR_STYLE,IDC_PDM_PANE);
			m_PDMPane.EnableTextLabels(TRUE);
			pCtrl->AddControl(&m_PDMPane, L"PDM", 3, TRUE, dwStyle);

			m_ModulesBar.SetPaneStyle(m_ModulesBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_LEFT);
			pCtrl->SetImageList(IDB_BITMAP_MODULES, 24);
			pCtrl->RecalcLayout();		

			m_ModulesBar.SetButtonsFont(&afxGlobalData.fontBold);	
		}	
	}
}

void CknifeDlg::InitSubDlgArray()
{
	g_SubDlgArray.SetParentWnd(this->m_hWnd);
	CRect rc;
	GetClientRect(rc);
	CRect rcModulesBar;
	m_ModulesBar.GetClientRect(rcModulesBar);
	rc.left+= rcModulesBar.Width();

	//first dialog
	CFirstDlg *pFirstDlg = new CFirstDlg;
	if(pFirstDlg)
	{
		g_SubDlgArray.AddSubDlg(pFirstDlg, IDD_DIALOG_FIRST, rc);		
	}

	//setting dialog
	CSettingDlg *pSettingDlg = new CSettingDlg;
	if(pSettingDlg)
	{
		g_SubDlgArray.AddSubDlg(pSettingDlg, IDD_DIALOG_SETTINGS, rc);		
	}

	//process dialog
	CProcessDlg *pProcessDlg = new CProcessDlg;
	if(pProcessDlg)
	{
		g_SubDlgArray.AddSubDlg(pProcessDlg, IDD_DIALOG_PROCESS, rc);		
	}
	//System module dialog
	CSysModuleDlg *pSysModuleDlg = new CSysModuleDlg;
	if(pSysModuleDlg)
	{
		g_SubDlgArray.AddSubDlg(pSysModuleDlg, IDD_DIALOG_SYSMODULE, rc);		
	}

	//SSDT dialog
	CSsdtDlg *pSsdtDlg = new CSsdtDlg;
	if(pSsdtDlg)
	{
		g_SubDlgArray.AddSubDlg(pSsdtDlg, IDD_DIALOG_SSDT, rc);		
	}

	//symbol links dialog
	
	CSymbolLinksDlg *pSymbolLinksDlg = new CSymbolLinksDlg;
	if(pSymbolLinksDlg)
	{
		g_SubDlgArray.AddSubDlg(pSymbolLinksDlg, IDD_DIALOG_SYMBOL_LINKS, rc);		
	}

#ifdef _DEBUG
	g_SubDlgArray.ShowSubDlg(IDD_DIALOG_SETTINGS);
#else
	g_SubDlgArray.ShowSubDlg(IDD_DIALOG_FIRST);
#endif

}	

void CknifeDlg::InitTrayIcon()
{
	m_trayIcon.InitTrayIcon(m_hWnd,	IDR_MAINFRAME, KNIFE_MSG_TRAY_ICON_NOTIFICATION, AfxGetApp()->LoadIcon(IDR_MAINFRAME), _T("knife tool"));
	m_trayIcon.ShowIcon();	
}

void CknifeDlg::DestroyTrayIcon(void)
{
	m_trayIcon.DeleteIcon();	
}

LRESULT CknifeDlg::OnTrayIconNotification(WPARAM wParam, LPARAM lParam)
{
	DWORD dwMessage = (DWORD)lParam;

	switch (dwMessage)
	{
	case WM_RBUTTONUP:
		{
			CPoint pos;
			GetCursorPos(&pos);
			SetForegroundWindow();	
			ShowTrayMenu();
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			SetForegroundWindow();
			ShowWindow(SW_SHOW);			
		}
		break;

	default:
		break;
	}

	return 0;
}

void CknifeDlg::OnTrayIconCommand(UINT uID)
{
	switch(uID)
	{
	case SYSTRAY_MENU_SHOW:
		{
			ShowWindow(SW_SHOW);
		}
		break;

	case SYSTRAY_MENU_QUIT:
		{
			::PostQuitMessage(0);
		}
		break;

	default:
		break;
	}		
}

void CknifeDlg::InitTrayMenu()
{
	DestroyTrayMenu();
	m_trayMenu.CreatePopupMenu();
	int nCnt = sizeof(v_tTrayMenuList) / sizeof(v_tTrayMenuList[0]);
	for (int i = 0; i < nCnt; i++)
	{
		m_trayMenu.AppendMenu(
			MF_STRING | MF_BYCOMMAND | MF_ENABLED, 
			v_tTrayMenuList[i].uID, 
			v_tTrayMenuList[i].strText);
	}
}

void CknifeDlg::DestroyTrayMenu()
{
	m_trayMenu.DestroyMenu();
}

void CknifeDlg::ShowTrayMenu()
{
	CPoint pos;
	GetCursorPos(&pos);
	m_trayMenu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this, NULL);
}

LRESULT CknifeDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == KNIFE_MSG_TASK_BAR_RESTART)
	{
		m_trayIcon.Rebuild();
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CknifeDlg::OnProcess()
{
	g_SubDlgArray.ShowSubDlg(IDD_DIALOG_PROCESS);
}

void CknifeDlg::OnSysModule()
{
	g_SubDlgArray.ShowSubDlg(IDD_DIALOG_SYSMODULE);
}

void CknifeDlg::OnSSDT()
{
	g_SubDlgArray.ShowSubDlg(IDD_DIALOG_SSDT);
}

void CknifeDlg::OnSymbolLinks()
{
	g_SubDlgArray.ShowSubDlg(IDD_DIALOG_SYMBOL_LINKS);
}


