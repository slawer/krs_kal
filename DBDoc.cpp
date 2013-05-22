// DBDoc.cpp : implementation file
//

#include "stdafx.h"
#include "KRS.h"
#include "DBDoc.h"
#include "DBView.h"
#include "PRJ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDBDoc

IMPLEMENT_DYNCREATE(CDBDoc, BaseDoc)

BEGIN_MESSAGE_MAP(CDBDoc, BaseDoc)
END_MESSAGE_MAP()

CDBDoc::CDBDoc():
	BaseDoc()
{
    m_menu_ID = IDR_KRSRARCHIVETYPE;
}

void CDBDoc::Serialize(CArchive& ar)
{
    BaseDoc::Serialize(ar);
    CDBView* pView = (CDBView*)pApp->GetView(t_archive);
    if (pView != NULL)
		pView->Serialize(ar);
    m_wndDialogBarDB.Serialize(ar);
}

// CDBDoc diagnostics

#ifdef _DEBUG
void CDBDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDBDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG