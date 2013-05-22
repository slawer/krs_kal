// InitProperty.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"
#include "InitProperty.h"

#include <afxdisp.h>
#include "excel.h"

#include "ApiFunc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CString strPathXLS;
int KNBKList = 1;
extern BOOL flgRabotaBezShurfaChanged;
extern BOOL flg_GlI_Changed;
extern BOOL flg_GlX_Changed;
extern BOOL flg_GlX_Changed;
extern float gGlI;
extern float gGlX;
extern BOOL  g_flgRabotaBezShurfa;


/////////////////////////////////////////////////////////////////////////////
// CInitPropDOL property page

IMPLEMENT_DYNCREATE(CInitPropDOL, CPropertyPage)

CInitPropDOL::CInitPropDOL() : CPropertyPage(CInitPropDOL::IDD)
{
	//{{AFX_DATA_INIT(CInitPropDOL)
	m_DolBlok = 0.0f;
	m_DolMax = 0.0f;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropDOL::~CInitPropDOL()
{
}

void CInitPropDOL::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropDOL)
	KRS_DDX_TextFloat(pDX, IDC_EDIT_DOL_BLOK, m_DolBlok);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_DOL_MAX, m_DolMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropDOL, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropDOL)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropDOL message handlers
BOOL CInitPropDOL::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if (pDoloto == NULL)
		return FALSE;
	m_DolBlok = gDolBlok = pDoloto->fBlock;
	m_DolMax = gDolMax = pDoloto->fMax;
	
	UpdateData(false);
	return TRUE;
}

void CInitPropDOL::OnOK() 
{
	UpdateData(true);

	gDolBlok = pDoloto->fBlock = m_DolBlok ;
	gDolMax = pDoloto->fMax = m_DolMax;
}

/////////////////////////////////////////////////////////////////////////////
// CInitPropMOMENT property page

IMPLEMENT_DYNCREATE(CInitPropMOMENT, CPropertyPage)

CInitPropMOMENT::CInitPropMOMENT() : CPropertyPage(CInitPropMOMENT::IDD)
{
	//{{AFX_DATA_INIT(CInitPropMOMENT)
	m_MomentMaxLevel = 0.0f;
	m_nFlag1 = 0;
	m_nFlag2 = 0;
	m_nFlag3 = 0;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropMOMENT::~CInitPropMOMENT()
{
}

void CInitPropMOMENT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropMOMENT)
	KRS_DDX_TextFloat(pDX, IDC_EDIT_MOMENT_MAX_LEVEL, m_MomentMaxLevel);
	DDX_Radio(pDX, IDC_RADIO_MOMENT_DIAPAZON, m_nFlag1);
	DDX_Radio(pDX, IDC_RADIO_NA_PULT_MOMENT, m_nFlag2);
	DDX_Radio(pDX, IDC_RADIO_MOMENT_NA_PULT_DIAPAZON, m_nFlag3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropMOMENT, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropMOMENT)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropMOMENT message handlers
BOOL CInitPropMOMENT::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_MomentMaxLevel = gMomentMaxLevel;

	m_nFlag1 = gnFlag1;
	m_nFlag2 = gnFlag2;
	m_nFlag3 = gnFlag3;

	UpdateData(false);
	
	return TRUE;
}

void CInitPropMOMENT::OnOK() 
{
	UpdateData(true);

	gMomentMaxLevel = m_MomentMaxLevel;
	
	gnFlag1 = m_nFlag1;
	gnFlag2 = m_nFlag2;
	gnFlag3 = m_nFlag3;

	switch(m_nFlag1)
	{
		case 0:
			m_MomentDiapazon = 30;
		break;

		case 1:
			m_MomentDiapazon = 60;
		break;
	}

	switch(m_nFlag3)
	{
		case 0:
			m_MomentPB = 60;
		break;

		case 1:
			m_MomentPB = 30;
		break;

		case 2:
			m_MomentPB = 15;
		break;

		case 3:
			m_MomentPB = 9;
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CInitPropKVADRAT property page

IMPLEMENT_DYNCREATE(CInitPropKVADRAT, CPropertyPage)

CInitPropKVADRAT::CInitPropKVADRAT() : CPropertyPage(CInitPropKVADRAT::IDD)
{
	//{{AFX_DATA_INIT(CInitPropKVADRAT)
	m_KvadratDlina = 0.0f;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropKVADRAT::~CInitPropKVADRAT()
{
}

void CInitPropKVADRAT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropKVADRAT)
	DDX_Control(pDX, IDC_COMBO_KVADRAT_PEREVODNIK, m_KvadratPerehodnik);
	DDX_Control(pDX, IDC_COMBO_KVADRAT_ELEVATOR, m_KvadratElevator);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_KVADRAT_PEREVODNIK, m_fKvadratPerehodnik);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_KVADRAT_ELEVATOR, m_fKvadratElevator);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_KVADRAT_DLINA, m_KvadratDlina);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropKVADRAT, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropKVADRAT)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropKVADRAT message handlers
BOOL CInitPropKVADRAT::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	int index;

	m_KvadratDlina = gKvadratDlina;
	m_fKvadratPerehodnik = gfKvadratPerehodnik;
	m_fKvadratElevator = gfKvadratElevator;
/*
	m_KvadratPerehodnik.AddString("2");
	m_KvadratPerehodnik.AddString("2.5");
	m_KvadratPerehodnik.AddString("3");
	if(m_fKvadratPerehodnik == 2) index = 0;
	else if(m_fKvadratPerehodnik == 2.5) index = 1;
	else if(m_fKvadratPerehodnik == 3.0) index = 2;
	m_KvadratPerehodnik.SetCurSel(index);


	m_KvadratElevator.AddString("4");
	m_KvadratElevator.AddString("4.5");
	m_KvadratElevator.AddString("5");
	if(m_fKvadratElevator == 4) index = 0;
	else if(m_fKvadratElevator == 4.5) index = 1;
	else if(m_fKvadratElevator == 5.0) index = 2;
	m_KvadratElevator.SetCurSel(index);
*/
	UpdateData(false);
	
	return TRUE;
}

void CInitPropKVADRAT::OnOK() 
{
	UpdateData(true);

	gKvadratDlina = m_KvadratDlina;
/*
	int index = m_KvadratPerehodnik.GetCurSel();
	if(index == 0)		m_fKvadratPerehodnik = 2;
	else if(index == 1) m_fKvadratPerehodnik = 2.5;
	else if(index == 2) m_fKvadratPerehodnik = 3.0;
*/
	gfKvadratPerehodnik = m_fKvadratPerehodnik;
/*
	index = m_KvadratElevator.GetCurSel();
	if(index == 0)		m_fKvadratElevator = 4;
	else if(index == 1) m_fKvadratElevator = 4.5;
	else if(index == 2) m_fKvadratElevator = 5.0;
*/
	gfKvadratElevator = m_fKvadratElevator;
}
/////////////////////////////////////////////////////////////////////////////
// CInitPropVES property page

IMPLEMENT_DYNCREATE(CInitPropVES, CPropertyPage)

CInitPropVES::CInitPropVES() : CPropertyPage(CInitPropVES::IDD)
{
	//{{AFX_DATA_INIT(CInitPropVES)
	m_VesAvaria = 0.0f;
	m_VesBlok = 0.0f;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropVES::~CInitPropVES()
{
}

void CInitPropVES::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropVES)
	DDX_Control(pDX, IDC_COMBO_VES_DIAPAZON, m_VesDiapazon);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_VES_AVARIA, m_VesAvaria);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_VES_BLOK, m_VesBlok);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropVES, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropVES)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropVES message handlers
BOOL CInitPropVES::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	int index;

	m_VesAvaria			= gVesAvaria = pKruk->fAvaria;
	m_VesBlok			= gVesBlok = pKruk->fBlock;
	m_fVesDiapazon		= gfVesDiapazon;

	m_VesDiapazon.AddString("100");
	m_VesDiapazon.AddString("150");
	m_VesDiapazon.AddString("200");
	m_VesDiapazon.AddString("250");
	m_VesDiapazon.AddString("300");
	m_VesDiapazon.AddString("350");
	m_VesDiapazon.AddString("400");
	m_VesDiapazon.AddString("500");
	if(m_fVesDiapazon == 100) index = 0;
	else if(m_fVesDiapazon == 150) index = 1;
	else if(m_fVesDiapazon == 200) index = 2;
	else if(m_fVesDiapazon == 250) index = 3;
	else if(m_fVesDiapazon == 300) index = 4;
	else if(m_fVesDiapazon == 350) index = 5;
	else if(m_fVesDiapazon == 400) index = 6;
	else if(m_fVesDiapazon == 500) index = 7;

	m_VesDiapazon.SetCurSel(index);

	UpdateData(false);
	
	return TRUE;
}

void CInitPropVES::OnOK() 
{
	UpdateData(true);

	gVesAvaria	= pKruk->fAvaria = m_VesAvaria;
	gVesBlok	= pKruk->fBlock = m_VesBlok;
	gfVesDiapazon	= m_fVesDiapazon;


	int index = m_VesDiapazon.GetCurSel();
	if(index == 0)		m_fVesDiapazon = 100;
	else if(index == 1) m_fVesDiapazon = 150;
	else if(index == 2) m_fVesDiapazon = 200;
	else if(index == 3) m_fVesDiapazon = 250;
	else if(index == 4) m_fVesDiapazon = 300;
	else if(index == 5) m_fVesDiapazon = 350;
	else if(index == 6) m_fVesDiapazon = 400;
	else if(index == 7) m_fVesDiapazon = 500;
	gfVesDiapazon = m_fVesDiapazon;
}
/////////////////////////////////////////////////////////////////////////////
// CInitPropDAVLENIE property page

IMPLEMENT_DYNCREATE(CInitPropDAVLENIE, CPropertyPage)

CInitPropDAVLENIE::CInitPropDAVLENIE() : CPropertyPage(CInitPropDAVLENIE::IDD)
{
	//{{AFX_DATA_INIT(CInitPropDAVLENIE)
	m_DavlenieAvaria = 0.0f;
	m_DavlenieBlok = 0.0f;
	m_DavlenieMin = 0.0f;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropDAVLENIE::~CInitPropDAVLENIE()
{
}

void CInitPropDAVLENIE::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropDAVLENIE)
	KRS_DDX_TextFloat(pDX, IDC_EDIT_DAVLENIE_AVARIA, m_DavlenieAvaria);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_DAVLENIE_BLOK, m_DavlenieBlok);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_DAVLENIE_MIN, m_DavlenieMin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropDAVLENIE, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropDAVLENIE)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropDAVLENIE message handlers
BOOL CInitPropDAVLENIE::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_DavlenieAvaria = gDavlenieAvaria = pDavlVhoda->fAvaria;
	m_DavlenieBlok = gDavlenieBlok = pDavlVhoda->fBlock;
	m_DavlenieMin = gDavlenieMin = pDavlVhoda->fMin;
	
	UpdateData(false);
	
	return TRUE;
}

void CInitPropDAVLENIE::OnOK() 
{
	UpdateData(true);

	gDavlenieAvaria = pDavlVhoda->fAvaria = m_DavlenieAvaria;
	gDavlenieBlok = pDavlVhoda->fBlock = m_DavlenieBlok;
	gDavlenieMin = pDavlVhoda->fMin = m_DavlenieMin;
}
/////////////////////////////////////////////////////////////////////////////
// CInitPropKNBK property page

IMPLEMENT_DYNCREATE(CInitPropKNBK, CPropertyPage)

CInitPropKNBK::CInitPropKNBK() : CPropertyPage(CInitPropKNBK::IDD)
{
	//{{AFX_DATA_INIT(CInitPropKNBK)
	m_KNBKList = 1;
	m_KNBKPath = _T("");
	m_KNBKNumElement = 1;
	m_KNBKKomponovka = 0;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropKNBK::~CInitPropKNBK()
{
}

void CInitPropKNBK::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropKNBK)
	DDX_Text(pDX, IDC_EDIT_KNBK_NUMLIST, m_KNBKList);
	DDV_MinMaxInt(pDX, m_KNBKList, 1, 100);
	DDX_Text(pDX, IDC_EDIT_KNBK_PATH, m_KNBKPath);
	DDX_Text(pDX, IDC_EDIT_KNBK_NUMELEMENT, m_KNBKNumElement);
	DDV_MinMaxInt(pDX, m_KNBKNumElement, 0, 1000);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_KNBK_KOMPONOVKA, m_KNBKKomponovka);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropKNBK, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropKNBK)
	ON_BN_CLICKED(IDC_BUTTON_KNBK_SELECT_PATH, OnButtonKnbkSelectPath)
	ON_BN_CLICKED(IDC_BUTTON_KNBK_OPEN, OnButtonKnbkOpen)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropKNBK message handlers
BOOL CInitPropKNBK::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_KNBKNumElement	= gtblElement.GetNumCurElement();
	m_KNBKKomponovka = gtblElement.GetLenFromBegToCurElem();
	m_KNBKPath			= gKNBKPath;
	m_KNBKList			= gKNBKList;

	UpdateData(false);
	
	return TRUE;
}

void CInitPropKNBK::OnButtonKnbkSelectPath() 
{
	UpdateData(true);
	CFileDialog dlgFile(
	TRUE,
	_T(".xls"),
	NULL,
	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	_T("Access Files (*.xls)|*.xls|All Files (*.*)|*.*||"));
	dlgFile.m_ofn.lpstrTitle = "Выбрать фай компоновки бурильного инструмента";
	

	char cd[2048];
	GetCurrentDirectory(2048, cd);

	if (dlgFile.DoModal() == IDOK)
	{
		m_KNBKPath = dlgFile.GetPathName();
		UpdateData(false);
	}
	SetCurrentDirectory(cd);
}

void CInitPropKNBK::OnButtonKnbkOpen() 
{
	UpdateData(true);
	KNBKList = m_KNBKList;

	KNBK(m_KNBKList, gKNBKPath);


/*
     try
       {
       _Application app;  // app is an _Application object.
       _Workbook book;
       _Worksheet sheet;
       Workbooks books;
       Worksheets sheets;
       Range range;
       char buf[1024];
       LPDISPATCH lpDisp; // IDispatch *; pointer reused many times.
       long count; // Count of the number of worksheets.

       // Common OLE variants. These are easy variants to use for
       // calling arguments.
       COleVariant
                  covTrue((short)TRUE),
                  covFalse((short)FALSE),
                  covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

       // Start Excel and get Application object.
       if(!app.CreateDispatch("Excel.Application"))
       {
        AfxMessageBox("Couldn't CreateDispatch on Excel");
        return;
       }

       // Set visible.
       app.SetVisible(TRUE);

       // Get Workbooks collection.
       lpDisp = app.GetWorkbooks();  // Get an IDispatch pointer

       ASSERT(lpDisp);               // or fail.
       books.AttachDispatch( lpDisp );  // Attach the IDispatch pointer
                                        // to the books object.

          // Open a workbook.
          lpDisp = books.Open("C:\\KBI.xls",
                         covOptional, covOptional, covOptional, covOptional,
                         covOptional, covOptional, covOptional, covOptional,
                         covOptional, covOptional, covOptional, covOptional,
                         covOptional, covOptional);//, covOptional); // Excel 2000 has 13 parameters
          ASSERT(lpDisp);  // It worked!



       // Attach to a Workbook object.
       book.AttachDispatch( lpDisp );  // Attach the IDispatch pointer
                                       // to the book object.

       // Get sheets.
       lpDisp = book.GetSheets();
       ASSERT(lpDisp);
       sheets.AttachDispatch(lpDisp);

       // Get the number of worksheets in this book.
       count = sheets.GetCount();

	   if(count > m_KNBKList)
	   {
		   // Enumerate through worksheets in book and activate in
		   // succession.
			// Get the sheet. Note that 1 is added to the index to make sure
			// it is 1-based, not zero-based. Otherwise, you will get odd
			// exceptions.
			lpDisp = sheets.GetItem( COleVariant((short)(m_KNBKList)) ); // 'Item' in
								  // the Worksheets collection = worksheet #.
			ASSERT(lpDisp);
			sheet.AttachDispatch(lpDisp);
			// Activate and sleep for two seconds so you can see it happen.
			sheet.Activate();
	   }
	   else
	   {
		   sprintf(buf, "%ld Лист с таким номером отсутствует в документе", m_KNBKList);
		   ::MessageBox(NULL, buf, "Sheet Count", MB_OK | MB_SETFOREGROUND);
	   }
      }  // End of Processing logic.

      catch(COleException *e)
      {
       char buf[1024];
       sprintf(buf, "COleException. SCODE: %08lx.", (long)e->m_sc);
       ::MessageBox(NULL, buf, "COleException", MB_SETFOREGROUND | MB_OK);
      }

      catch(COleDispatchException *e)
      {

       char buf[1024];
       sprintf(buf,
          "COleDispatchException. SCODE: %08lx,Description: \"%s\".",
          (long)e->m_wCode, (LPSTR)e->m_strDescription.GetBuffer(1024));
       ::MessageBox(NULL, buf, "COleDispatchException",
                    MB_SETFOREGROUND | MB_OK);
      }

      catch(...)
      {
       ::MessageBox(NULL, "General Exception caught.", "Catch-All",
                    MB_SETFOREGROUND | MB_OK);
      }
*/
	UpdateData(false);
}

void CInitPropKNBK::OnButtonRefresh() 
{
	UpdateData(true);
	((CKRSApp*)AfxGetApp())->GetDataFromXLS();
	gtblElement.SetNumCurElement(m_KNBKNumElement);

	if(m_KNBKNumElement >= gtblElement.GetCount()) 
		m_KNBKNumElement = gtblElement.GetCount() - 1;

	m_KNBKKomponovka = gtblElement.GetLenFromBegToCurElem();
	UpdateData(false);
}

void CInitPropKNBK::OnOK() 
{
	UpdateData(true);
	((CKRSApp*)AfxGetApp())->GetDataFromXLS();
	gtblElement.SetNumCurElement(m_KNBKNumElement);

	m_KNBKKomponovka = gtblElement.GetLenFromBegToCurElem();
	gKNBKPath		= m_KNBKPath;
	gKNBKList		= m_KNBKList;
	gtblElement.SetNumCurElement(m_KNBKNumElement);
}

/////////////////////////////////////////////////////////////////////////////
// CInitPropIZABOI property page

IMPLEMENT_DYNCREATE(CInitPropIZABOI, CPropertyPage)

CInitPropIZABOI::CInitPropIZABOI() : CPropertyPage(CInitPropIZABOI::IDD)
, m_flgRabotaBezShurfa(FALSE)
{
	//{{AFX_DATA_INIT(CInitPropIZABOI)
	m_IZaboiValue = 0.0f;
	m_PorogBur = 0.0f;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropIZABOI::~CInitPropIZABOI()
{
}

void CInitPropIZABOI::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CInitPropIZABOI)
    KRS_DDX_TextFloat(pDX, IDC_EDIT_IZABOI_VALUE, m_IZaboiValue);
    KRS_DDX_TextFloat(pDX, IDC_EDIT_POROG_BUR, m_PorogBur);
    DDV_MinMaxFloat(pDX, m_PorogBur, 0.f, 10.f);
    //}}AFX_DATA_MAP
    DDX_Check(pDX, IDC_CHECK_RABOTA_BEZ_SHURFA, m_flgRabotaBezShurfa);
}


BEGIN_MESSAGE_MAP(CInitPropIZABOI, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropIZABOI)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_CHECK_RABOTA_BEZ_SHURFA, &CInitPropIZABOI::OnBnClickedCheckRabotaBezShurfa)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropIZABOI message handlers
BOOL CInitPropIZABOI::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_IZaboiValue = gIZaboiValue;
	m_PorogBur = gPorogBur;
    m_flgRabotaBezShurfa = gflgRabotaBezShurfa;
	
	UpdateData(false);
	return TRUE;
}

void CInitPropIZABOI::OnOK() 
{
	UpdateData(true);

	gIZaboiValue = m_IZaboiValue;
	pApp->WriteFloatValueToRegistry("InitData","IZaboiValue", m_IZaboiValue);

	gPorogBur = m_PorogBur;
	pApp->WriteFloatValueToRegistry("InitData","PorogBur", m_PorogBur);
	
	gflZaboiIsSetted = true;
    gflgRabotaBezShurfa = m_flgRabotaBezShurfa;
    pApp->WriteIntegerValueToRegistry("InitData","RabotaBezShurfa", gflgRabotaBezShurfa);
}


void CInitPropIZABOI::OnBnClickedCheckRabotaBezShurfa()
{
	if(flgRabotaBezShurfaChanged)
	{
		UpdateData(FALSE);
		return;
	}
	UpdateData(TRUE);
	g_flgRabotaBezShurfa = m_flgRabotaBezShurfa;
	flgRabotaBezShurfaChanged = true;
}

/////////////////////////////////////////////////////////////////////////////
// CInitPropTALBLOK property page

IMPLEMENT_DYNCREATE(CInitPropTALBLOK, CPropertyPage)

CInitPropTALBLOK::CInitPropTALBLOK() : CPropertyPage(CInitPropTALBLOK::IDD)
{
	//{{AFX_DATA_INIT(CInitPropTALBLOK)
	m_TalblokAvaria = 0.0f;
	m_PorogTime = 0.0f;
	m_PorogVisota = 0.0f;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropTALBLOK::~CInitPropTALBLOK()
{
}

void CInitPropTALBLOK::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropTALBLOK)
	DDX_Control(pDX, IDC_COMBO_TALBLOK_DIAPAZON, m_TalblokDiapazon);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_TALBLOK_AVARIA, m_TalblokAvaria);
	if (!pDX->m_bSaveAndValidate)
	{
		CString str;
		str.Format("%.3f", m_GlX);
		DDX_Text(pDX, IDC_EDIT_SET_GLX, str);
	}
	else
		KRS_DDX_TextDouble(pDX, IDC_EDIT_SET_GLX, m_GlX);
	if (!pDX->m_bSaveAndValidate)
	{
		CString str;
		str.Format("%.3f", m_GlI);
		DDX_Text(pDX, IDC_EDIT_SET_GLI, str);
	}
	else
		KRS_DDX_TextDouble(pDX, IDC_EDIT_SET_GLI, m_GlI);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_POROG_TIME, m_PorogTime);
//	DDV_MinMaxFloat(pDX, m_PorogTime, 1.e-002f, 120.f);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_POROG_VISOTA, m_PorogVisota);
//	DDV_MinMaxFloat(pDX, m_PorogVisota, 1.e-002f, 30.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropTALBLOK, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropTALBLOK)
	ON_BN_CLICKED(IDC_BUTTON_SET_GLX, OnButtonSetGlX)
	ON_BN_CLICKED(IDC_BUTTON_SET_GLI, OnButtonSetGlI)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropTALBLOK message handlers
BOOL CInitPropTALBLOK::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	int index;

	m_TalblokAvaria			= gTalblokAvaria = pTalblock->fAvaria;
	m_fTalblokDiapazon		= gfTalblokDiapazon;
	m_PorogTime				= gPorogTime;
	m_PorogVisota			= gPorogVisota;
	m_GlX					= pGlubina->fCurAvg;
	m_GlI					= pGlubinaZaboia->fCurAvg;

	m_TalblokDiapazon.AddString("20");
	m_TalblokDiapazon.AddString("25");
	m_TalblokDiapazon.AddString("30");
	m_TalblokDiapazon.AddString("35");
	m_TalblokDiapazon.AddString("40");
	m_TalblokDiapazon.AddString("45");
	m_TalblokDiapazon.AddString("50");
	m_TalblokDiapazon.AddString("60");
	if(m_fTalblokDiapazon == 20) index = 0;
	else if(m_fTalblokDiapazon == 25) index = 1;
	else if(m_fTalblokDiapazon == 30) index = 2;
	else if(m_fTalblokDiapazon == 35) index = 3;
	else if(m_fTalblokDiapazon == 40) index = 4;
	else if(m_fTalblokDiapazon == 45) index = 5;
	else if(m_fTalblokDiapazon == 50) index = 6;
	else if(m_fTalblokDiapazon == 60) index = 7;

	m_TalblokDiapazon.SetCurSel(index);

	UpdateData(false);
	
	return TRUE;
}

void CInitPropTALBLOK::OnButtonSetGlX() 
{
	UpdateData(TRUE);
	gGlX = m_GlX;
	flg_GlX_Changed = true;
}
void CInitPropTALBLOK::OnButtonSetGlI() 
{
	UpdateData(TRUE);
	gGlI = m_GlI;
	flg_GlI_Changed = true;
}

void CInitPropTALBLOK::OnOK() 
{
	UpdateData(true);

	gTalblokAvaria		= pTalblock->fAvaria = m_TalblokAvaria;
	gfTalblokDiapazon	= m_fTalblokDiapazon;
	gPorogTime			= m_PorogTime;
	gPorogVisota		= m_PorogVisota;

	int index = m_TalblokDiapazon.GetCurSel();
	if(index == 0)		m_fTalblokDiapazon = 20;
	else if(index == 1) m_fTalblokDiapazon = 25;
	else if(index == 2) m_fTalblokDiapazon = 30;
	else if(index == 3) m_fTalblokDiapazon = 35;
	else if(index == 4) m_fTalblokDiapazon = 40;
	else if(index == 5) m_fTalblokDiapazon = 45;
	else if(index == 6) m_fTalblokDiapazon = 50;
	else if(index == 7) m_fTalblokDiapazon = 60;
	gfTalblokDiapazon = m_fTalblokDiapazon;

	pApp->WriteFloatValueToRegistry("InitData","TalblokAvaria", m_TalblokAvaria);
	pApp->WriteFloatValueToRegistry("InitData","fTalblokDiapazon", m_fTalblokDiapazon);
	pApp->WriteFloatValueToRegistry("InitData","PorogTime", m_PorogTime);
	pApp->WriteFloatValueToRegistry("InitData","PorogVisota", m_PorogVisota);
}

/////////////////////////////////////////////////////////////////////////////
// CPropCirculiaciya property page

IMPLEMENT_DYNCREATE(CPropCirculiaciya, CPropertyPage)

CPropCirculiaciya::CPropCirculiaciya() : CPropertyPage(CPropCirculiaciya::IDD)
{
	//{{AFX_DATA_INIT(CPropCirculiaciya)
	m_StartTimeCirc = 0.0f;
	m_StartTimeBur = 0.0f;
	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CPropCirculiaciya::~CPropCirculiaciya()
{
}

void CPropCirculiaciya::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropCirculiaciya)
	KRS_DDX_TextFloat(pDX, IDC_EDIT_TIME_CIRCULIACII_START, m_StartTimeCirc);
	DDV_MinMaxFloat(pDX, m_StartTimeCirc, 0.f, 24000.f);
	KRS_DDX_TextFloat(pDX, IDC_EDIT_TIME_BURENIYA_START, m_StartTimeBur);
	DDV_MinMaxFloat(pDX, m_StartTimeBur, 0.f, 24000.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropCirculiaciya, CPropertyPage)
	//{{AFX_MSG_MAP(CPropCirculiaciya)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropCirculiaciya message handlers
BOOL CPropCirculiaciya::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_StartTimeCirc = KRS_time_circ_lifetime*60.0;
	m_StartTimeBur = KRS_time_bur_lifetime*60.0;
	
	UpdateData(false);
	
	return TRUE; 
}

void CPropCirculiaciya::OnOK() 
{
	UpdateData(true);

	KRS_time_circ_lifetime = m_StartTimeCirc/60.0;
	pApp->WriteFloatValueToRegistry("InitData","StartTimeCirc", KRS_time_circ_lifetime);

	KRS_time_bur_lifetime = m_StartTimeBur/60.0;
	pApp->WriteFloatValueToRegistry("InitData","StartTimeBur", KRS_time_bur_lifetime);

	// Write Работа.ini >>
	PRJ_WriteWorkFile();
}
/////////////////////////////////////////////////////////////////////////////
// CInitPropAdd property page

IMPLEMENT_DYNCREATE(CInitPropAdd, CPropertyPage)

CInitPropAdd::CInitPropAdd() : CPropertyPage(CInitPropAdd::IDD)
{
	//{{AFX_DATA_INIT(CInitPropAdd)
	m_Mestorogdenie = _T("");
	m_Kust = _T("");
	m_Rabota = _T("");
	m_Skvagina = _T("");
	m_flgViewMarkers = FALSE;
    m_flgViewPanelBur = FALSE;
    m_flgViewDiag = FALSE;
	m_flgDataBegEnd = FALSE;
	m_gflgGrfStupenki = false;
    m_gflgScrollAll = false;
	m_gflgIndParBur = false;
	m_gflgIndHighPanel = false;
	m_gflgDgtGraphPanel = false;
	m_gflgIndDataTime = false;
    m_gflgIndConverter = false;

	//}}AFX_DATA_INIT
    m_psp.dwFlags &= ~PSP_HASHELP;
}

CInitPropAdd::~CInitPropAdd()
{
}

void CInitPropAdd::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitPropAdd)
	DDX_Text(pDX, IDC_EDIT_MESTOROGDENIE, m_Mestorogdenie);
	DDX_Text(pDX, IDC_EDIT_KUST, m_Kust);
	DDX_Text(pDX, IDC_EDIT_RABOTA, m_Rabota);
	DDX_Text(pDX, IDC_EDIT_SKVAGINA, m_Skvagina);
	DDX_Check(pDX, IDC_CHECK_VIEW_REPER, m_flgViewMarkers);
    DDX_Check(pDX, IDC_CHECK_VIEW_PANEL_BUR, m_flgViewPanelBur);
    DDX_Check(pDX, IDC_CHECK_VIEW_DAGN, m_flgViewDiag);
    DDX_Check(pDX, IDC_CHECK_VIEW_DGT_RT, m_flgViewDgtRT);
    DDX_Check(pDX, IDC_CHECK_VIEW_DGT_BD, m_flgViewDgtBD);
    DDX_Check(pDX, IDC_CHECK_VIEW_REAL_ARCH, m_flgViewRealArch);
    DDX_Check(pDX, IDC_CHECK_VIEW_TIME_GLUB, m_flgViewTimeGlub);
	DDX_Check(pDX, IDC_CHECK_DATA_BEGEND, m_flgDataBegEnd);
	DDX_Check(pDX, IDC_CHECK_STUPENKI, m_gflgGrfStupenki);
    DDX_Check(pDX, IDC_CHECK_SCROLL_ALL, m_gflgScrollAll);
	DDX_Check(pDX, IDC_CHECK_PARAM_BUR, m_gflgIndParBur);
	DDX_Check(pDX, IDC_CHECK_HIGH_PANEL, m_gflgIndHighPanel);
	DDX_Check(pDX, IDC_CHECK_GRAPH_DIGIT, m_gflgDgtGraphPanel);
	DDX_Check(pDX, IDC_CHECK_DATA_TIME, m_gflgIndDataTime);
    DDX_Check(pDX, IDC_CHECK_CONVERTER, m_gflgIndConverter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitPropAdd, CPropertyPage)
	//{{AFX_MSG_MAP(CInitPropAdd)
	ON_BN_CLICKED(IDC_CHECK_VIEW_REPER, OnCheckViewReper)
    ON_BN_CLICKED(IDC_CHECK_VIEW_PANEL_BUR, OnCheckViewPanelBur)
    ON_BN_CLICKED(IDC_CHECK_VIEW_DAGN, OnCheckViewDiag)
    ON_BN_CLICKED(IDC_CHECK_VIEW_DGT_RT, OnCheckViewDgtRT)
    ON_BN_CLICKED(IDC_CHECK_VIEW_DGT_BD, OnCheckViewDgtBD)
    ON_BN_CLICKED(IDC_CHECK_VIEW_REAL_ARCH, OnCheckViewRealArch)
    ON_BN_CLICKED(IDC_CHECK_VIEW_TIME_GLUB, OnCheckViewTimeGlub)
	ON_BN_CLICKED(IDC_CHECK_DATA_BEGEND, OnCheckDataBegEnd)
	ON_BN_CLICKED(IDC_CHECK_STUPENKI, OnCheckGrfStupenki)
    ON_BN_CLICKED(IDC_CHECK_SCROLL_ALL, OnCheckScrollAll)
	ON_BN_CLICKED(IDC_CHECK_PARAM_BUR, OnCheckParamBur)
	ON_BN_CLICKED(IDC_CHECK_HIGH_PANEL, OnCheckHighPanel)
	ON_BN_CLICKED(IDC_CHECK_GRAPH_DIGIT, OnCheckGraphDigit)
	ON_BN_CLICKED(IDC_CHECK_DATA_TIME, OnCheckIndDataTime)
    ON_BN_CLICKED(IDC_CHECK_CONVERTER, OnCheckIndConverter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitPropAdd message handlers
BOOL CInitPropAdd::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_Mestorogdenie     = gMestorogdenie;
	m_Kust			    = gKust;
	m_Rabota		    = gRabota;
	m_Skvagina		    = gSkvagina;
	m_flgViewMarkers    = gflgViewMarkers;
    m_flgViewPanelBur   = gflgViewPanelBur;
    m_flgViewDiag       = gflgViewDiag;
    m_flgViewDgtRT      = gflgViewDgtRT;
    m_flgViewDgtBD      = gflgViewDgtBD;
    m_flgViewRealArch   = gflgViewRealArch;
    m_flgViewTimeGlub   = gflgViewTimeGlub;
	m_flgDataBegEnd     = gflgDataBegEnd;	
	m_gflgGrfStupenki   = gflgGrfStupenki;	
    m_gflgScrollAll     = gflgScrollAll;
	m_gflgIndParBur     = gflgIndParBur;
	m_gflgIndHighPanel  = gflgIndHighPanel;
	m_gflgDgtGraphPanel = gflgDgtGraphPanel;
	m_gflgIndDataTime   = gflgIndDataTime;
    m_gflgIndConverter  = gflgIndConverter;
	UpdateData(false);
	
	return TRUE; 
}

void CInitPropAdd::OnOK() 
{
	UpdateData(true);

	gMestorogdenie	= m_Mestorogdenie;
	gKust			= m_Kust;
	gRabota			= m_Rabota;
	gSkvagina		= m_Skvagina;
	gflgViewMarkers	= m_flgViewMarkers;
    gflgViewPanelBur= m_flgViewPanelBur;
    gflgViewDiag    = m_flgViewDiag;
    gflgViewDgtRT   = m_flgViewDgtRT;
    gflgViewDgtBD   = m_flgViewDgtBD;
    gflgViewRealArch= m_flgViewRealArch;
    gflgViewTimeGlub= m_flgViewTimeGlub;
	gflgDataBegEnd  = m_flgDataBegEnd;	
	gflgGrfStupenki  = m_gflgGrfStupenki;	
    gflgScrollAll    = m_gflgScrollAll;
	gflgIndParBur     = m_gflgIndParBur;
	gflgIndHighPanel  = m_gflgIndHighPanel;
	gflgDgtGraphPanel = m_gflgDgtGraphPanel;
	gflgIndDataTime   = m_gflgIndDataTime;
    gflgIndConverter    = m_gflgIndConverter;

    pApp->WriteProfileString("InitData", "Mestorogdenie", gMestorogdenie);
    pApp->WriteProfileString("InitData", "Kust", gKust);
    pApp->WriteProfileString("InitData", "Rabota", gRabota);
    pApp->WriteProfileString("InitData", "Skvagina", gSkvagina);
}

void CInitPropAdd::OnCheckViewReper() 
{
	UpdateData(true);
	gflgViewMarkers	= m_flgViewMarkers;
	pApp->WriteIntegerValueToRegistry("InitData", "flgViewMarkers", gflgViewMarkers);

    ASSERT(pFrm);
	pFrm->ptmData->Set_flg_view_reper(gflgViewMarkers);

	CView* pView = pApp->GetActiveView();
	if(pView != NULL) pView->PostMessage(PM_UPDATE_VIEW, 0, 0);
}
void CInitPropAdd::OnCheckViewPanelBur() 
{
    UpdateData(true);
    gflgViewPanelBur	= m_flgViewPanelBur;
    ASSERT(pFrm);
    pFrm->SetViewPanelBur(true);
}
void CInitPropAdd::OnCheckViewDiag() 
{
    UpdateData(true);
    gflgViewDiag	= m_flgViewDiag;
    ASSERT(pFrm);
    m_wndDialogBar.SetViewDiag();
}
void CInitPropAdd::OnCheckViewDgtRT() 
{
    UpdateData(true);
    gflgViewDgtRT   = m_flgViewDgtRT;
    ASSERT(pFrm);
    pFrm->SetViewDgtRT();
}
void CInitPropAdd::OnCheckViewDgtBD() 
{
    UpdateData(true);
    gflgViewDgtBD   = m_flgViewDgtBD;
    ASSERT(pFrm);
    pFrm->SetViewDgtBD();
}
void CInitPropAdd::OnCheckViewRealArch() 
{
    UpdateData(true);
    gflgViewRealArch   = m_flgViewRealArch;
    ASSERT(pFrm);
    m_wndDialogBarH.SetViewRealArch();
}
void CInitPropAdd::OnCheckViewTimeGlub() 
{
    UpdateData(true);
    gflgViewTimeGlub   = m_flgViewTimeGlub;
    ASSERT(pFrm);
    m_wndDialogBarH.SetViewTimeGlub();
}
void CInitPropAdd::OnCheckDataBegEnd() 
{
	UpdateData(true);
	gflgDataBegEnd   = m_flgDataBegEnd;
	pApp->ChangeStyleAllView(FLD_DATA_ON_FIELD, gflgDataBegEnd);
}
void CInitPropAdd::OnCheckGrfStupenki() 
{
	UpdateData(true);
	gflgGrfStupenki  = m_gflgGrfStupenki;	
	pApp->ChangeStyleAllView(FLD_LINE_JAGGIES, gflgGrfStupenki);
}
void CInitPropAdd::OnCheckScrollAll() 
{
    UpdateData(true);
    gflgScrollAll    = m_gflgScrollAll;
}
void CInitPropAdd::OnCheckParamBur() 
{
	UpdateData(true);
	gflgIndParBur    = m_gflgIndParBur;
    ASSERT(pFrm);
	m_wndDialogBar.SetIndParamBur();
}
void CInitPropAdd::OnCheckHighPanel() 
{
	UpdateData(true);
	gflgIndHighPanel    = m_gflgIndHighPanel;
    ASSERT(pFrm);
	pFrm->SetHighPanel();
}
void CInitPropAdd::OnCheckGraphDigit() 
{
	UpdateData(true);
	gflgDgtGraphPanel    = m_gflgDgtGraphPanel;
    ASSERT(pFrm);
	pFrm->SetGraphDigit();
}
void CInitPropAdd::OnCheckIndDataTime() 
{
	UpdateData(true);
	gflgIndDataTime    = m_gflgIndDataTime;
    ASSERT(pFrm);
	m_wndDialogBar.SetIndDataTime();
}
void CInitPropAdd::OnCheckIndConverter() 
{
    UpdateData(true);
    gflgIndConverter    = m_gflgIndConverter;
    ASSERT(pFrm);
    m_wndDialogBar.SetIndConverter();
}

// CInitROTOR dialog

IMPLEMENT_DYNAMIC(CInitROTOR, CPropertyPage)

CInitROTOR::CInitROTOR()
: CPropertyPage(CInitROTOR::IDD)
, m_RotorControl(FALSE)
{
    //{{AFX_DATA_INIT(CInitROTOR)
    m_PorogRotorBur = 10.0;
    m_VesRotorBur = 0.0;
    //}}AFX_DATA_INIT
}

CInitROTOR::~CInitROTOR()
{
}

void CInitROTOR::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CInitROTOR)
    KRS_DDX_TextDouble(pDX, IDC_EDIT_POROG_MOMENT_1, m_PorogRotorBur);
    KRS_DDX_TextDouble(pDX, IDC_EDIT_VES_ROTOR_BUR_1, m_VesRotorBur);
    //}}AFX_DATA_MAP
    DDX_Check(pDX, IDC_CHECK_ROTOR, m_RotorControl);
}
BOOL CInitROTOR::OnInitDialog() 
{
    CDialog::OnInitDialog();

    m_PorogRotorBur = pApp->ReadFloatValueFromRegistry("InitData","PorogRotorBur", 50);
    m_VesRotorBur = pApp->ReadFloatValueFromRegistry("InitData","VesRotorBur", 50);
    m_RotorControl = pApp->ReadIntegerValueFromRegistry("InitData","ControlRotorBur", 0);

    UpdateData(false);

    return TRUE;  
}
void CInitROTOR::OnOK() 
{
    UpdateData(true);

    gPorogRotorBur = m_PorogRotorBur;
    pApp->WriteFloatValueToRegistry("InitData","PorogRotorBur", (float)m_PorogRotorBur);

    gVesRotorBur = m_VesRotorBur;
    pApp->WriteFloatValueToRegistry("InitData","VesRotorBur", (float)m_VesRotorBur);

    gControlRotorBur = m_RotorControl;
    pApp->WriteIntegerValueToRegistry("InitData","ControlRotorBur", (int)m_RotorControl);

    CDialog::OnOK();
}


BEGIN_MESSAGE_MAP(CInitROTOR, CPropertyPage)
    ON_BN_CLICKED(IDC_CHECK_ROTOR, &CInitROTOR::OnBnClickedCheckRotor)
END_MESSAGE_MAP()


// CInitROTOR message handlers
void CInitROTOR::OnBnClickedCheckRotor()
{
    UpdateData(true);

    gControlRotorBur = m_RotorControl;
    pApp->WriteIntegerValueToRegistry("InitData","ControlRotorBur", (int)m_RotorControl);
}

/////////////////////////////////////////////////////////////////////////////
// CInitProperty

IMPLEMENT_DYNAMIC(CInitProperty, CPropertySheet)

CInitProperty::CInitProperty(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_pgDOL);
	AddPage(&m_pgMOMENT);
	AddPage(&m_pgKVADRAT);
	AddPage(&m_pgVES);
	AddPage(&m_pgDAVLENIE);
	AddPage(&m_pgKNBK);
	AddPage(&m_pgIZABOI);
	AddPage(&m_pgTALBLOK);
	AddPage(&m_pgCIRCULIACIYA);
//	AddPage(&m_pgAdd);
    AddPage(&m_pgRotor);
    
    m_psh.dwFlags |= PSH_NOAPPLYNOW;
    m_psh.dwFlags &= ~PSH_HASHELP;
 }

CInitProperty::CInitProperty(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CInitProperty::~CInitProperty()
{
}


BEGIN_MESSAGE_MAP(CInitProperty, CPropertySheet)
	//{{AFX_MSG_MAP(CInitProperty)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
