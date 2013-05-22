#if !defined(AFX_HEADERGRF1_H__CE0644D2_F90D_4BC1_AB83_5E42DAED3CAA__INCLUDED_)
#define AFX_HEADERGRF1_H__CE0644D2_F90D_4BC1_AB83_5E42DAED3CAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeaderGrf.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDgtHeadParam window
//class CParam; // предобъ€вление

class CDgtHeadParam : public CStatic
{
// Construction
public:
	CDgtHeadParam();

// Attributes
public:
	float m_MinHead, m_MaxHead;

	void *pParamV, *pGraph; 

	CFont fFontName;
	LOGFONT  m_logfontN;
	COLORREF colorLineHead;

	int m_num_grf;

// Operations
public:
	void SetShowDgtLabel(BOOL bfShow);
	void SetPrintFont(float scl, CFont& fFontN, BOOL flOrient);
	void SetNormalFont();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDgtHeadParam)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawHeader(CDC* pDC, CWnd *pWnd = NULL, float sclX = 1.0f, 
		float sclY = 1.0f, CSize *prnShift = NULL){};
	virtual void SetParamHeader(void* pPrm, void* pGrf) = 0;
	virtual CSize GetSizeText(CString cs){return CSize(0,0);};
	virtual void SetFontDgt() = 0;

	virtual ~CDgtHeadParam();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDgtHeadParam)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CHeaderGrf window

class CHeaderGrf : public CDgtHeadParam
{
// Construction
public:
	CHeaderGrf();

// Implementation
public:
	COLORREF GetColorAlert(void* pPar);
	void DrawHeaderVertGrf(CDC* pDC, CWnd *pWnd = NULL, float sclX = 1.0f, 
        float sclY = 1.0f, CSize *prnShift = NULL);
    void DrawHeaderHorGrf(CDC* pDC, CWnd *pWnd = NULL, float sclX = 1.0f, 
        float sclY = 1.0f, CSize *prnShift = NULL);
	void DrawHeader(CDC* pDC, CWnd *pWnd = NULL, float sclX = 1.0f, 
		float sclY = 1.0f, CSize *prnShift = NULL);
	void SetParamHeader(void* pPrm, void* pGrf);
	void SetFontDgt();
	CSize GetSizeText(CString cs);

	virtual ~CHeaderGrf();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHeaderGrf)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// COutParamPanel window

class COutParamPanel : public CDgtHeadParam
{
// Construction
public:
	COutParamPanel();

// Implementation
public:
	void DrawHeader(CDC* pDC, CWnd *pWnd = NULL, float sclX = 1.0f,
		float sclY = 1.0f, CSize *prnShift = NULL);
	void SetParamHeader(void* pPrm, void* pGrf);
	void SetFontDgt();
	CSize GetSizeText(CString cs);

	virtual ~COutParamPanel();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutParamPanel)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADERGRF1_H__CE0644D2_F90D_4BC1_AB83_5E42DAED3CAA__INCLUDED_)
