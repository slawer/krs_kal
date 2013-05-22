// CementDoc.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "CementDoc.h"
#include "CementView.h"
#include "PRJ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCementDoc

IMPLEMENT_DYNCREATE(CCementDoc, BaseDoc)

BEGIN_MESSAGE_MAP(CCementDoc, BaseDoc)
END_MESSAGE_MAP()

CCementDoc::CCementDoc():
	BaseDoc()
{
    m_menu_ID = IDR_KRSCEMENTTYPE;
}


void CCementDoc::Serialize(CArchive& ar)
{
    BaseDoc::Serialize(ar);

    CCementView* pView = (CCementView*)pApp->GetView(gt_cement);
    if (pView != NULL)
		pView->Serialize(ar);
    m_wndDialogBarCM.Serialize(ar);
    m_wndDialogPanelCement.Serialize(ar);

    KRS_CheckFinishInitialization();
}

// CCementDoc diagnostics

#ifdef _DEBUG
void CCementDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CCementDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG