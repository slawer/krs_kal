#include "stdafx.h"

#include "KRS.h"
#include "MainFrm.h"
#include "CementDoc.h"
#include "CementView.h"
#include "resource.h"
#include "CementBD.h"

IMPLEMENT_DYNCREATE(CCementBD, CCementView)

CCementBD::CCementBD() : CCementView()
{
    type_view = t_cement_db;
}

CCementBD::~CCementBD()
{
}

void CCementBD::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCementBD, CCementView)
    ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_WM_CREATE()
END_MESSAGE_MAP()


CMenu* CCementBD::SetModulesExtern()
{
	if(gListModuleExternal.GetCount() == 0) return 0;
    //---------------------------------------------------
    //Формируем меню внешних модулей
    CCementDoc* pDoc = (CCementDoc*)GetDocument();
	HINSTANCE hInst = AfxGetResourceHandle();
    pDoc->m_DefaultMenu = 
        ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_KRSCEMENTTYPE));
    if (pDoc->m_DefaultMenu == NULL)
        return 0;

    CMenu* menu = CMenu::FromHandle(pDoc->GetDefaultMenu());
    if(menu)
    {
        int iPos;
        CMenu* pPopup = NULL;
        for(iPos = 0; iPos < (int)menu->GetMenuItemCount(); iPos++)
        {
            if(menu->GetSubMenu(iPos)->GetMenuItemID(0) == ID_NASTROIKA_MODULES) 
            {
                pPopup = menu->GetSubMenu(iPos);
 		int ii = (int)pPopup->GetMenuItemCount();
               break;
            }
        }
        if(pPopup == NULL) return 0;


		CMenu *submenu = menu->GetSubMenu(iPos);
		if(submenu == NULL) return 0;
		int ii = (int)submenu->GetMenuItemCount();


        int cx = ::GetSystemMetrics(SM_CXMENUCHECK);
        int cy = ::GetSystemMetrics(SM_CYMENUCHECK);
        if(pPopup != NULL && gListModuleExternal.GetCount() > 0) 
        {
            pPopup->AppendMenu(MF_SEPARATOR, 0, LPCTSTR(0));

            for(int i = 0; i < gListModuleExternal.GetCount(); i++)
            {
                HICON hIconLarge, hIcon;
//                ExtractIconEx(gListModuleExternal.GetName2(i), 0, &hIconLarge, &hIcon, 1);
				CString Name2 = gListModuleExternal.GetName2txe(i);
				if(Name2.GetLength() == 0) continue;
				ExtractIconEx(Name2, 0, &hIconLarge, &hIcon, 1);
                //HICON hIcon = ExtractIcon(pApp->m_hInstance, gListModuleExternal.GetName2(i), 0);
                if(hIcon != NULL)
                {
                    ICONINFO iconinfo;
                    GetIconInfo(hIcon, &iconinfo);
                    HANDLE hLoad = CopyImage(HANDLE(iconinfo.hbmColor), IMAGE_BITMAP, cx+1, cy+1, LR_COPYFROMRESOURCE);
                    CBitmap *pBmp = CBitmap::FromHandle(HBITMAP(hLoad));
                    CString str;
                    str.Format(" %d", i);
                    str += ".  " + gListModuleExternal.GetName(i);
                    BOOL flg = pPopup->AppendMenu(MF_STRING | MF_ENABLED, ID_MENU_MODULE_EXTERNAL + i, str);
                    pPopup->SetMenuItemBitmaps(i+2, MF_BYPOSITION, pBmp, pBmp);
                    //BOOL res = menu->SetMenuItemBitmaps(2, MF_BYPOSITION, pBmp, pBmp);
                    //int zzz = 0;
                }
            }
        }
        CMDIFrameWnd* frame = ((CMDIChildWnd *) GetParent())->GetMDIFrame();
        frame->MDISetMenu(menu, NULL);
        frame->DrawMenuBar();
    }
    //-------------------------------------------------------------------------
	return menu;
}

void CCementBD::RenameMenuItem(UINT ID_menu_item, CString new_name)
{
    //---------------------------------------------------
    //Формируем меню внешних модулей
    CCementDoc* pDoc = (CCementDoc*)GetDocument();
    pDoc->m_DefaultMenu = 
        ::LoadMenu(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_KRSCEMENTTYPE));
    if (pDoc->m_DefaultMenu == NULL)
        return;

    CMenu* menu = CMenu::FromHandle(pDoc->GetDefaultMenu());
    if(menu)
    {
        int iPos, iMaxPos, jPos, jMaxPos;
        CMenu *submenu = NULL;
		iMaxPos = (int)menu->GetMenuItemCount();
        for(iPos = 0; iPos < iMaxPos; iPos++)
        {
			submenu = menu->GetSubMenu(iPos);
			jMaxPos = (int)submenu->GetMenuItemCount();
			for(jPos = 0; jPos < jMaxPos; jPos++)
			{
				if(submenu->GetMenuItemID(jPos) == ID_menu_item) 
					break;
			}
			if (jPos < jMaxPos)
				break;
        }
        if(submenu == NULL || jPos >= jMaxPos || new_name.GetLength() == 0)
			return;
        BOOL flg = submenu->ModifyMenu(jPos, MF_BYPOSITION, ID_menu_item, new_name);
		//BOOL flg = submenu->ModifyMenu(ID_menu_item, MF_BYCOMMAND, ID_menu_item, new_name);
        CMDIFrameWnd* frame = ((CMDIChildWnd *) GetParent())->GetMDIFrame();
        frame->MDISetMenu(menu, NULL);
        frame->DrawMenuBar();
	}
}

void CCementBD::OnInitialUpdate()
{
//    if(m_flag_init) return;//заходит несколько раз
//    m_flag_init = true;

	m_ptmData	= pgtmDataDB;
	m_pcurGlub	= pgcurGlubDB;
	CCementView::OnInitialUpdate();

//    KRS_CheckFinishInitialization();
}

void CCementBD::OnSetFocus(CWnd* pOldWnd)
{
    CFormView::OnSetFocus(pOldWnd);
    ASSERT(pFrm);
    CheckRegim();
    CheckGrf();
    pFrm->ChangeTypeView((int)type_view, 3);
    SetVisibilityPanelEtap();
	SetModulesExtern();
}
void CCementBD::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 10 && gflgDataReaded != 0)
	{
//		SetModulesExtern();
		KillTimer(nIDEvent);
	}

	CCementView::OnTimer(nIDEvent);
}

int CCementBD::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CCementView::OnCreate(lpCreateStruct) == -1)
		return -1;

    SetTimer(10, 30, NULL); // для нормальной установки меню внешних модулей

	return 0;
}
