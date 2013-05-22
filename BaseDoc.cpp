#include <stdafx.h>

#include "BaseDoc.h"
#include "MainView.h"

IMPLEMENT_DYNCREATE(BaseDoc, CDocument)

BaseDoc::BaseDoc():
    m_menu(NULL), m_hmenu(NULL)
{
}

BaseDoc::~BaseDoc()
{
/*
	if (m_menu)
	{
		m_menu->DestroyMenu();
		delete m_menu;
	}
*/
}

HMENU BaseDoc::GetDefaultMenu()
{
	InitMenu();
	return m_hmenu;
}

void BaseDoc::InitMenu()
{
	if (m_hmenu == NULL)
	{
		m_hmenu = ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(m_menu_ID));
		if (m_hmenu == NULL)
			return;
	}
	if (m_menu)
		m_menu->DeleteTempMap();
	m_menu = CMenu::FromHandle(m_hmenu);

	list<HBITMAP>::iterator current = m_last_menu_bitmaps.begin();
	while (current != m_last_menu_bitmaps.end())
	{
		DeleteObject(*current);
		current++;
	}
	m_last_menu_bitmaps.clear();

	int iPos, sz1 = m_menu->GetMenuItemCount(), sz2;
	CMenu *modules_pop = NULL, *pop;
	for (iPos = 0; iPos < sz1; iPos++)
	{
		pop = m_menu->GetSubMenu(iPos);
		sz2 = pop->GetMenuItemCount();
		if (sz2 > 0 && pop->GetMenuItemID(0) == ID_NASTROIKA_MODULES)
		{
			modules_pop = pop;
			break;
		}
	}
	if (modules_pop == NULL)
		return;

	while (modules_pop->GetMenuItemCount() > 1)
		modules_pop->DeleteMenu(1, MF_BYPOSITION);

	if (m_ListModuleExternal.GetCount() == 0)
		return;

	int cx = ::GetSystemMetrics(SM_CXMENUCHECK);
	int cy = ::GetSystemMetrics(SM_CYMENUCHECK);

	modules_pop->AppendMenu(MF_SEPARATOR, 0, LPCTSTR(0));
	for(int i = 0; i < m_ListModuleExternal.GetCount(); i++)
	{
		HICON hIconLarge, hIcon;
		CString Name2 = m_ListModuleExternal.GetName2txe(i);
		if (Name2.GetLength() == 0)
			continue;
		ExtractIconEx(Name2, 0, &hIconLarge, &hIcon, 1);
		if (hIcon != NULL)
		{
			ICONINFO iconinfo;
			GetIconInfo(hIcon, &iconinfo);
			HANDLE hLoad = CopyImage(HANDLE(iconinfo.hbmColor), IMAGE_BITMAP, cx+1, cy+1, LR_COPYFROMRESOURCE);                    

			CBitmap *pBmp = CBitmap::FromHandle((HBITMAP)hLoad/*iconinfo.hbmColor*/);
			CString str;
			str.Format(" %d", i);
			str += ".  " + m_ListModuleExternal.GetName(i);
			BOOL flg = modules_pop->AppendMenu(MF_STRING | MF_ENABLED, ID_MENU_MODULE_EXTERNAL + i, str);
			modules_pop->SetMenuItemBitmaps(i+2, MF_BYPOSITION, pBmp, pBmp);
			m_last_menu_bitmaps.push_back((HBITMAP)hLoad/*iconinfo.hbmColor*/);

			//DeleteObject(hLoad);
			DeleteObject(iconinfo.hbmColor);
			DeleteObject(iconinfo.hbmMask);
		}
		if (hIconLarge != NULL)
			DestroyIcon(hIconLarge);
		if (hIcon != NULL)
			DestroyIcon(hIcon);
	}
}

void BaseDoc::Serialize(CArchive& ar)
{
	CString name, path;
	int count;

	if (ar.IsStoring())
	{
		count = m_ListModuleExternal.GetCount();
		ar << count;
		for(int i = 0; i < 10; i++) 
		{
			if (i < count)
			{
				name = m_ListModuleExternal.GetName(i);
				path = m_ListModuleExternal.GetName2(i);
			}
			else
			{
				name.Format("NoItem %d", i);
				path.Format("NoPath %d", i);
			}
			ar << name;
			ar << path;
		}
	}
	else
	{
		m_ListModuleExternal.Init();
		ar >> count;
		for(int i = 0; i < 10; i++) 
		{
			ar >> name; 
			ar >> path; 
			if (i < count) m_ListModuleExternal.Add(name, path, 0);
		}
	}
}

void BaseDoc::SetTitle(LPCTSTR lpszTitle) 
{
    int nn = strlen(lpszTitle);
    char bf[512];

    strncpy(bf, lpszTitle, nn + 1);
    if(nn) bf[nn - 1] = '\0';
    CString cs = bf;
    cs = "ÑÊÁ ÎÐÅÎË ã.Ìîñêâà";

    CDocument::SetTitle(cs);
}
////////////////////////////////////////////////////////////////////////////////
// end