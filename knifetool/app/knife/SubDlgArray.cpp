#include "StdAfx.h"
#include "SubDlgArray.h"


#define DLG_NULL_ID		0


CSubDlgArray::CSubDlgArray(void)
{
	m_SubDlgArray.clear();
	m_nVisibleID = DLG_NULL_ID;
	m_nPreVisibleID = DLG_NULL_ID;
}

CSubDlgArray::~CSubDlgArray(void)
{
	for(SUB_DLG_ARRAY::iterator iter = m_SubDlgArray.begin(); iter != m_SubDlgArray.end(); iter++)
	{	
		if(iter->pSubDlg->m_hWnd != NULL)
		{
			iter->pSubDlg->DestroyWindow();
		}
		delete iter->pSubDlg;
	}
}

void CSubDlgArray::SetParentWnd(HWND hWnd)
{
	m_hParentWnd = hWnd;
}

void CSubDlgArray::AddSubDlg(CDialog *pDlg, UINT uID, CRect rcWnd)
{
	if(pDlg != NULL)
	{
		SUB_DLG_PARAM dlgParam;
		memset(&dlgParam, 0, sizeof(SUB_DLG_PARAM));
		if(pDlg->m_hWnd == NULL)
		{
			pDlg->Create(uID, CWnd::FromHandle(m_hParentWnd));			
			pDlg->MoveWindow(rcWnd);
		}
		dlgParam.pSubDlg = pDlg;
		dlgParam.uID = uID;
		dlgParam.rcWnd = rcWnd;	
		m_SubDlgArray.push_back(dlgParam);
	}
}

void CSubDlgArray::ShowSubDlg(UINT uID)
{
	m_nPreVisibleID = m_nVisibleID;
	m_nVisibleID = DLG_NULL_ID;
	CDialog *pCurrDlg = NULL;
	int nSizeType = 0;
	for(SUB_DLG_ARRAY::iterator iter = m_SubDlgArray.begin(); iter != m_SubDlgArray.end(); iter++)
	{
		if(iter->uID == uID)
		{	
			pCurrDlg = iter->pSubDlg;
			nSizeType = iter->nSizeType;
			m_nVisibleID = iter->uID;
		}
		else
		{
			iter->pSubDlg->ShowWindow(SW_HIDE);
		}
	}

	if(pCurrDlg != NULL)
	{
		pCurrDlg->ShowWindow(SW_SHOW);				
	}
}

int CSubDlgArray::GetVisibleDlgID()
{
	return m_nVisibleID;
}

int CSubDlgArray::GetPreVisibleDlgID()
{
	return m_nPreVisibleID;
}

CDialog *CSubDlgArray::GetSubDlg(UINT uID)
{
	CDialog *pDlg = NULL;
	for(SUB_DLG_ARRAY::iterator iter = m_SubDlgArray.begin(); iter != m_SubDlgArray.end(); iter++)
	{
		if(iter->uID == uID)
		{
			pDlg = iter->pSubDlg;
			break;
		}		
	}
	return pDlg;
}