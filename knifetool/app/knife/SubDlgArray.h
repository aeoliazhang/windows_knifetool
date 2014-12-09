#pragma once

class CSubDlgArray
{

public:
	CSubDlgArray(void);
	~CSubDlgArray(void);

	typedef struct tagSubDlgParam
	{
		CDialog *pSubDlg;
		UINT        uID;
		CRect		rcWnd;
		int			nSizeType;
	}SUB_DLG_PARAM;

public:
	void SetParentWnd(HWND hWnd);
	void AddSubDlg(CDialog *pDlg, UINT uID, CRect rcWnd);
	void ShowSubDlg(UINT uID);
	int  GetVisibleDlgID();
	int  GetPreVisibleDlgID();
	CDialog *GetSubDlg(UINT uID);


public:
	typedef vector<SUB_DLG_PARAM> SUB_DLG_ARRAY;
	SUB_DLG_ARRAY   m_SubDlgArray;
	HWND			m_hParentWnd;
	UINT			m_nVisibleID;
	UINT			m_nPreVisibleID;
};
