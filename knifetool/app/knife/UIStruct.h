#pragma once

//modules bar size
#define MODILESBAR_SIZE_WIDTH		180
#define MODILESBAR_SIZE_HEIGHT		651

//controls position
typedef struct tagCtrlsPos
{
	UINT uID;
	int	 nLeft;
	int  nTop;
	int  nWidth;
	int  nHeight;
}CTRLS_POSITION, *LPCTRLS_POSITION;

//list control 
//column
typedef struct tagListHeaderInfo
{
	int			nField;
	LPCTSTR		lpszFieldName;
	WORD		wFormat;
	int			nWidth;
}LIST_HEADER_INFO, *LPLIST_HEADER_INFO;


//menu
typedef struct tagMenuItemInfo
{
	UINT		uID;				//menu item ID									
	LPCTSTR		strText;			//menu item text
} MENU_ITEM_INFO, *LPMENU_ITEM_INFO;