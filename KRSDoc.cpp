// KRSDoc.cpp : implementation of the CKRSDoc class
//

#include "stdafx.h"
#include "KRS.h"

#include "KRSDoc.h"
#include "MainFrm.h"
#include "KRSView.h"
#include "PRJ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern float fixGlub;

/////////////////////////////////////////////////////////////////////////////
// CKRSDoc

IMPLEMENT_DYNCREATE(CKRSDoc, CDocument)

BEGIN_MESSAGE_MAP(CKRSDoc, CDocument)
	//{{AFX_MSG_MAP(CKRSDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKRSDoc construction/destruction

CKRSDoc::CKRSDoc():
	BaseDoc()
{
    m_menu_ID = IDR_KRSREALTIMETYPE;
}

/////////////////////////////////////////////////////////////////////////////
// CKRSDoc serialization
void CKRSDoc::Serialize(CArchive& ar)
{
    CString name, path;

    if (ar.IsStoring())
	{
        ar << gDolBlok;
        ar << gDolMax;
        ar << gDavlenieAvaria;
        ar << gDavlenieBlok;
        ar << gDavlenieMin;
        ar << gMomentMaxLevel;
        ar << gnFlag1 << gnFlag2 << gnFlag3;
        ar << gMomentDiapazon << gMomentPB;
        ar << gKvadratDlina << gfKvadratPerehodnik << gfKvadratElevator;
        ar << gVesAvaria << gVesBlok << gfVesDiapazon;
        ar << gfTalblokDiapazon << gTalblokAvaria;
        ar << gPorogTime << gPorogVisota;
        ar << gWk;
        ar << (double)0/*gfTalblokDeltaH*/ << (double)0/*gfTalblokDeltaHCur*/;
        ar << gMexInterval << gSPOInterval;
        ar << gStartTimeCirc << gStartTimeBur;
        ar << gflagButtonNull;
        ar << gnTalblok;
        ar << gPorogBur;
        ar << gflZaboiIsSetted;
		ar << gMestorogdenie;
		ar << gKust;
		ar << gRabota;
		ar << gSkvagina;
		ar << gKNBKPath;
		ar << gKNBKList;
		//num_cur_elem = 
		ar << gtblElement.GetNumCurElement();
//----------------------------------------------------------------
        ar << gflgViewPanelBur;
        ar << gflgViewDiag;
        ar << gflgViewDgtRT;
        ar << gflgViewDgtBD;
        ar << gflgViewRealArch;
        ar << gflgViewTimeGlub;
		ar << gflgDataBegEnd;
        ar << gDeltaTimeAlert;
		ar << gflgGrfStupenki;
        ar << gflgScrollAll;
		ar << gflgIndParBur;
		ar << gflgIndDataTime;
//--------------------------------------------------------------
        ar << gGlubinaZaboia;
        ar << gGlubina;
        ar << gIZaboiValue;
        ar << gflgIndHighPanel;
        ar << gflgDgtGraphPanel;
        ar << gflgIndConverter;
		ar << gflgIndRegimeBur;
		
		BaseDoc::Serialize(ar);

		ar << gflgSelectOnlyY;
//--------------------------------------------------------------
	}
	else
	{
        ar >> gDolBlok;
        ar >> gDolMax;
        ar >> gDavlenieAvaria;
        ar >> gDavlenieBlok;
        ar >> gDavlenieMin;
        ar >> gMomentMaxLevel;
        ar >> gnFlag1 >> gnFlag2 >> gnFlag3;
        ar >> gMomentDiapazon >> gMomentPB;
        ar >> gKvadratDlina >> gfKvadratPerehodnik >> gfKvadratElevator;
        ar >> gVesAvaria >> gVesBlok >> gfVesDiapazon;
        ar >> gfTalblokDiapazon >> gTalblokAvaria;
        ar >> gPorogTime >> gPorogVisota;
        ar >> gWk;
        ar >> gfTalblokDeltaH >> gfTalblokDeltaH; gfTalblokDeltaH = 0; 
        ar >> gMexInterval >> gSPOInterval;
        ar >> gStartTimeCirc >> gStartTimeBur;
        ar >> gflagButtonNull;
        ar >> gnTalblok;
        ar >> gPorogBur;
        ar >> gflZaboiIsSetted;
		ar >> gMestorogdenie;
		ar >> gKust;
		ar >> gRabota;
		ar >> gSkvagina;

//----------------------------------------------------------------
		ar >> gKNBKPath;
		ar >> gKNBKList;
		int num_cur_elem;
		ar >> num_cur_elem;
		try
		{
			KRS_app.GetDataFromXLS();
		}
		catch(...)
		{
			pFrm->MessageBox("Отсутствует или повреждён файл " + gKNBKPath, " ОШИБКА", MB_OK);
		}
		gtblElement.SetNumCurElement(num_cur_elem);
//----------------------------------------------------------------
        ar >> gflgViewPanelBur;
        ar >> gflgViewDiag;
        ar >> gflgViewDgtRT;
        ar >> gflgViewDgtBD;
        ar >> gflgViewRealArch;
        ar >> gflgViewTimeGlub;
		ar >> gflgDataBegEnd;
        ar >> gDeltaTimeAlert;
		ar >> gflgGrfStupenki;
        ar >> gflgScrollAll;
		ar >> gflgIndParBur;
		ar >> gflgIndDataTime;
//--------------------------------------------------------------
        ar >> gGlubinaZaboia;
        ar >> gGlubina;
        ar >> gIZaboiValue;
        ar >> gflgIndHighPanel;
        ar >> gflgDgtGraphPanel;
        ar >> gflgIndConverter;
		ar >> gflgIndRegimeBur;

		BaseDoc::Serialize(ar);

		ar >> gflgSelectOnlyY;

		fixGlub = gGlubina;

		if (KRS_project_flag & KRS_PRJ_FLAG_BURENIE)
        {
			pGlubinaZaboia->fCur = gGlubinaZaboia;
			COleDateTime tim = COleDateTime::GetCurrentTime();
			pGlubinaZaboia->AddTail(tim.m_dt);
			pGlubinaZaboia->fCurAvg = gGlubinaZaboia;
			pGlubinaZaboia->fCurPrevious = gGlubinaZaboia;
			if(!gflgRabotaBezShurfa) gtblElement.GetLenFromBegToCurElem();
			else
			{
				pLinstrumenta->fCur = gfLinstrumenta;
				pLinstrumenta->AddTail(tim.m_dt);
				pLinstrumenta->fCurAvg = gfLinstrumenta;
				pLinstrumenta->fCurPrevious = gfLinstrumenta;
			}

			pZaboi->fCur = gIZaboiValue;
			pZaboi->AddTail(tim.m_dt);
			pZaboi->fCurAvg = gIZaboiValue;
			pZaboi->fCurPrevious = gIZaboiValue;

			pGlubina->fCur = gGlubina;
			pGlubina->AddTail(tim.m_dt);
			pGlubina->fCurAvg = gGlubina;
			pGlubina->fCurPrevious = gGlubina;
		}
		KRS_CheckFinishInitialization();
     }

    CKRSView* pView = (CKRSView*)pApp->GetView(t_real_time);
    if (pView != NULL) pView->Serialize(ar);

    m_wndDialogBar.Serialize(ar);
	gflgDataReaded = 1;
}

/////////////////////////////////////////////////////////////////////////////
// CKRSDoc diagnostics

#ifdef _DEBUG
void CKRSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CKRSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG