#if !defined(AFX_INITPROPERTY_H__7893CB0A_F63D_43CF_81E0_EE1E8E42A9A3__INCLUDED_)
#define AFX_INITPROPERTY_H__7893CB0A_F63D_43CF_81E0_EE1E8E42A9A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InitProperty.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CInitPropDOL dialog

class CInitPropDOL : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropDOL)

// Construction
public:
	CInitPropDOL();
	~CInitPropDOL();

// Dialog Data
	//{{AFX_DATA(CInitPropDOL)
	enum { IDD = IDD_INIT_PROP_DOL };
	float	m_DolBlok;
	float	m_DolMax;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropDOL)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropDOL)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CInitPropMOMENT dialog

class CInitPropMOMENT : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropMOMENT)

// Construction
public:
	CInitPropMOMENT();
	~CInitPropMOMENT();

	float m_MomentDiapazon, m_MomentPB;

// Dialog Data
	//{{AFX_DATA(CInitPropMOMENT)
	enum { IDD = IDD_INIT_PROP_MOMENT };
	float	m_MomentMaxLevel;
	int		m_nFlag1;
	int		m_nFlag2;
	int		m_nFlag3;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropMOMENT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropMOMENT)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CInitPropKVADRAT dialog

class CInitPropKVADRAT : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropKVADRAT)

// Construction
public:
	CInitPropKVADRAT();
	~CInitPropKVADRAT();

	float m_fKvadratPerehodnik, m_fKvadratElevator;

// Dialog Data
	//{{AFX_DATA(CInitPropKVADRAT)
	enum { IDD = IDD_INIT_PROP_KVADRAT };
	CComboBox	m_KvadratPerehodnik;
	CComboBox	m_KvadratElevator;
	float	m_KvadratDlina;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropKVADRAT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropKVADRAT)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CInitPropVES dialog

class CInitPropVES : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropVES)

// Construction
public:
	CInitPropVES();
	~CInitPropVES();

	float m_fVesDiapazon;

// Dialog Data
	//{{AFX_DATA(CInitPropVES)
	enum { IDD = IDD_INIT_PROP_VES };
	CComboBox	m_VesDiapazon;
	float	m_VesAvaria;
	float	m_VesBlok;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropVES)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropVES)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CInitPropDAVLENIE dialog

class CInitPropDAVLENIE : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropDAVLENIE)

// Construction
public:
	CInitPropDAVLENIE();
	~CInitPropDAVLENIE();

// Dialog Data
	//{{AFX_DATA(CInitPropDAVLENIE)
	enum { IDD = IDD_INIT_PROP_DAVLENIE };
	float	m_DavlenieAvaria;
	float	m_DavlenieBlok;
	float	m_DavlenieMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropDAVLENIE)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropDAVLENIE)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CInitPropKNBK dialog

class CInitPropKNBK : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropKNBK)

// Construction
public:
	CInitPropKNBK();
	~CInitPropKNBK();

	int numCurElement;

// Dialog Data
	//{{AFX_DATA(CInitPropKNBK)
	enum { IDD = IDD_INIT_PROP_KNBK };
	int		m_KNBKList;
	CString	m_KNBKPath;
	int		m_KNBKNumElement;
	float	m_KNBKKomponovka;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropKNBK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropKNBK)
	afx_msg void OnButtonKnbkSelectPath();
	afx_msg void OnButtonKnbkOpen();
	afx_msg void OnButtonRefresh();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CInitPropIZABOI dialog

class CInitPropIZABOI : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropIZABOI)

// Construction
public:
	CInitPropIZABOI();
	~CInitPropIZABOI();

// Dialog Data
	//{{AFX_DATA(CInitPropIZABOI)
	enum { IDD = IDD_INIT_PROP_IZABOI };
	float	m_IZaboiValue;
	float	m_PorogBur;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropIZABOI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropIZABOI)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedCheckRabotaBezShurfa();
public:
    BOOL m_flgRabotaBezShurfa;
};
/////////////////////////////////////////////////////////////////////////////
// CInitPropTALBLOK dialog

class CInitPropTALBLOK : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropTALBLOK)

// Construction
public:
	CInitPropTALBLOK();
	~CInitPropTALBLOK();

	float m_fTalblokDiapazon;

// Dialog Data
	//{{AFX_DATA(CInitPropTALBLOK)
	enum { IDD = IDD_INIT_PROP_TALBLOK };
	CComboBox	m_TalblokDiapazon;
	float	m_TalblokAvaria;
	float	m_PorogTime;
	float	m_PorogVisota;
	double  m_GlX;
	double  m_GlI;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropTALBLOK)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropTALBLOK)
	afx_msg void OnButtonSetGlX();
	afx_msg void OnButtonSetGlI();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPropCirculiaciya dialog

class CPropCirculiaciya : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropCirculiaciya)

// Construction
public:
	CPropCirculiaciya();
	~CPropCirculiaciya();

// Dialog Data
	//{{AFX_DATA(CPropCirculiaciya)
	enum { IDD = IDD_INIT_CIRCULIACIYA };
	float	m_StartTimeCirc;
	float	m_StartTimeBur;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropCirculiaciya)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropCirculiaciya)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CInitPropAdd dialog

class CInitPropAdd : public CPropertyPage
{
	DECLARE_DYNCREATE(CInitPropAdd)

// Construction
public:
	CInitPropAdd();
	~CInitPropAdd();

// Dialog Data
	//{{AFX_DATA(CInitPropAdd)
	enum { IDD = IDD_INIT_PROP_ADD };
	CString	m_Mestorogdenie;
	CString	m_Kust;
	CString	m_Rabota;
	CString	m_Skvagina;
	int		m_flgViewMarkers;
    int		m_flgViewPanelBur;
    int		m_flgViewDiag;
    int     m_flgViewDgtRT;
    int     m_flgViewDgtBD;
    int     m_flgViewRealArch;
    int     m_flgViewTimeGlub;
	int		m_flgDataBegEnd;
	int		m_gflgGrfStupenki;
    int		m_gflgScrollAll;
	int		m_gflgIndParBur;
	int		m_gflgIndHighPanel;
	int		m_gflgDgtGraphPanel;
	int		m_gflgIndDataTime;
    int     m_gflgIndConverter;

    //}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInitPropAdd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInitPropAdd)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckViewReper();
    afx_msg void OnCheckViewPanelBur();
    afx_msg void OnCheckViewDiag();
    afx_msg void OnCheckViewDgtRT();
    afx_msg void OnCheckViewDgtBD();
    afx_msg void OnCheckViewRealArch();
    afx_msg void OnCheckViewTimeGlub();
	afx_msg void OnCheckDataBegEnd();
	afx_msg void OnCheckGrfStupenki();
    afx_msg void OnCheckScrollAll();
	afx_msg void OnCheckParamBur();
	afx_msg void OnCheckHighPanel();
	afx_msg void OnCheckGraphDigit();
	afx_msg void OnCheckIndDataTime();
    afx_msg void OnCheckIndConverter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
// CInitROTOR dialog

class CInitROTOR : public CPropertyPage
{
    DECLARE_DYNAMIC(CInitROTOR)

public:
    CInitROTOR();
    virtual ~CInitROTOR();

    // Dialog Data
    enum { IDD = IDD_INIT_ROTOR };
    //{{AFX_DATA(CDlgDebagParam)
    double	m_MexInterval;
    double	m_SPOInterval;
    double	m_PorogRotorBur;
    double	m_VesRotorBur;
    //}}AFX_DATA

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDlgDebagParam)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
    BOOL m_RotorControl;
public:
    afx_msg void OnBnClickedCheckRotor();
};

/////////////////////////////////////////////////////////////////////////////
// CInitProperty

class CInitProperty : public CPropertySheet
{
	DECLARE_DYNAMIC(CInitProperty)

// Construction
public:
	CInitProperty(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CInitProperty(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CInitPropDOL		m_pgDOL;
	CInitPropMOMENT		m_pgMOMENT;
	CInitPropKVADRAT	m_pgKVADRAT;
	CInitPropVES		m_pgVES;
	CInitPropDAVLENIE	m_pgDAVLENIE;
	CInitPropKNBK		m_pgKNBK;
	CInitPropIZABOI		m_pgIZABOI;
	CInitPropTALBLOK	m_pgTALBLOK;
	CPropCirculiaciya	m_pgCIRCULIACIYA;
	CInitPropAdd		m_pgAdd;
    CInitROTOR          m_pgRotor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitProperty)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInitProperty();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInitProperty)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INITPROPERTY_H__7893CB0A_F63D_43CF_81E0_EE1E8E42A9A3__INCLUDED_)


