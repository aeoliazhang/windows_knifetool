#pragma once
#include "afxcmn.h"

typedef struct _SYMBOL_LINK_INFO
{
	TCHAR strName[MAX_PATH];
	TCHAR strLink[MAX_PATH];
}SYMBOL_LINK_INFO, *PSYMBOL_LINK_INFO;

typedef vector<SYMBOL_LINK_INFO> SYMBOL_LINK_INFO_LIST;

// CSymbolLinksDlg dialog

class CSymbolLinksDlg : public CDialog
{
	DECLARE_DYNAMIC(CSymbolLinksDlg)

public:
	CSymbolLinksDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSymbolLinksDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
// Dialog Data
	enum { IDD = IDD_DIALOG_SYMBOL_LINKS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void InitListCtrl();
	void RefreshList();
	void EnumSymbolLinks(SYMBOL_LINK_INFO_LIST &symbolLinks);
	void InitMenu();
	void DestroyMenu();
	void ShowMenu();
	void OnMenuCommand(UINT uID);

private:
	CListCtrl m_listctrl;
	CMenu     m_Menu;
public:
	afx_msg void OnNMRClickListSymbollinks(NMHDR *pNMHDR, LRESULT *pResult);
};
