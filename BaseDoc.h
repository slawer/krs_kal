// uic 27.01.2010

#pragma once

#include "KRS.h"
#include <list>

class BaseDoc : public CDocument
{
public:
	DECLARE_DYNCREATE(BaseDoc)

	BaseDoc();
	virtual ~BaseDoc();

    CMenu* m_menu;
    HMENU m_hmenu;
    UINT m_menu_ID;
	CTBLElement m_ListModuleExternal;

	virtual void Serialize(CArchive& ar);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual HMENU GetDefaultMenu();
	void InitMenu();
	
	list<HBITMAP> m_last_menu_bitmaps;
};

////////////////////////////////////////////////////////////////////////////////
// end