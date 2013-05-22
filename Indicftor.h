#if !defined(AFX_INDICFTOR_H__6C8CC099_5951_4D72_9C33_275FFDD8FA66__INCLUDED_)
#define AFX_INDICFTOR_H__6C8CC099_5951_4D72_9C33_275FFDD8FA66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Indicftor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIndicftor window

class CIndicftor : public CStatic
{
// Construction
public:
	CIndicftor();

// Attributes
public:
	CWnd *pWndP;
	CString text;
	COLORREF colorText, colorFon, colorTextDop, colorFonDop;
	COLORREF colorTextCur, colorFonCur;
	COLORREF color_fon_blink1, color_fon_blink2, clr1, clr2;
	int numStateW, numStateWS;
	LOGFONT lfW;
	CFont fFont;
	BOOL fl_blink;
	int num_post_blink;

    CDC *m_pDCMem, *m_pDCC;
    CBitmap *m_pBitmap, *m_pbitmapOld;
    CRect rectClip;
    CSize m_sizeBitmap;
	int m_style;
	CString str_data, str_time;

	
// Operations
public:
	void InitInd(CString str, int sizeFont, int state);
	void Update();
	void DrawInd(CDC* pDC);
	void SetText(CString str, BOOL flg = false);
	void SetColor(COLORREF colorText, COLORREF colorFon, BOOL flg = false);
	void SetDopColor(COLORREF colorText, COLORREF colorFon, BOOL flg = false);
	void SetColorBlink(COLORREF color1, COLORREF color2, BOOL flg);
	void SetPosition(CRect& rec);
	void SetState(int numState, BOOL fl_redraw = false);
	int  GetState();
	LOGFONT SetFont(LOGFONT lf);
	LOGFONT GetFont();
	int SetSizeText(int sze_text, BOOL flg = false);
	void SetBlink(BOOL flag_blink);
    BOOL CreateMemoryPaintDC(CDC* pDC);
	void SetStyle(int fl_style, COLORREF color_text, COLORREF color_fon, int size_text) ;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndicftor)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIndicftor();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIndicftor)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDICFTOR_H__6C8CC099_5951_4D72_9C33_275FFDD8FA66__INCLUDED_)
