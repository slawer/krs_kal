// DlgEditText.cpp : implementation file
//

#include "stdafx.h"
#include "krs.h"
#include "DlgEditText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgEditText dialog

DlgEditText::DlgEditText(CString text, CRect rect, bool bold, int alignment, CWnd* pParent /*=NULL*/)
	: CDialog(DlgEditText::IDD, pParent), m_bold(bold), m_alignment(alignment), m_bold_font(NULL), m_main_font(NULL)
{
	m_rect = rect;
	//{{AFX_DATA_INIT(DlgEditText)
	m_text_centered = _T(text);
	m_text_left = _T(text);
	m_text_right = _T(text);
	//}}AFX_DATA_INIT
}

void DlgEditText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditText)
	DDX_Text(pDX, IDC_EDIT_TEXT_CENTERED, m_text_centered);
	DDX_Text(pDX, IDC_EDIT_TEXT_LEFT, m_text_left);
	DDX_Text(pDX, IDC_EDIT_TEXT_RIGHT, m_text_right);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DlgEditText, CDialog)
	//{{AFX_MSG_MAP(DlgEditText)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_USER+1, OnEnforceFlose)
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_CANCELMODE()
END_MESSAGE_MAP()

void DlgEditText::OnCloseDialog()
{
	if (m_bold_font)
	{
		m_bold_font->DeleteObject();
		delete m_bold_font;
	}
}

void DlgEditText::OnCancel()
{
	m_res = IDCANCEL;
	OnCloseDialog();
	CDialog::OnCancel();
}

void DlgEditText::OnOK()
{
	m_res = IDOK;
	OnCloseDialog();
	CDialog::OnOK();
}

BOOL DlgEditText::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_res = -1;

	CEdit* edit = (CEdit*)GetDlgItem((m_alignment == 0) ? IDC_EDIT_TEXT_CENTERED : ((m_alignment == -1)?IDC_EDIT_TEXT_LEFT:IDC_EDIT_TEXT_RIGHT));
	int w = m_rect.right - m_rect.left;
	int h = m_rect.bottom - m_rect.top;
	int y = m_rect.top;
	int x = m_rect.left;
	SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER);
	x = 1;
	y = 1;
	w -= 4;
	h -= 4;
	edit->SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER);
	edit->ShowWindow(SW_NORMAL);

	if (m_main_font)
		edit->SetFont(m_main_font, FALSE);
		
	if (m_bold)
	{
		CFont *fnt = edit->GetFont();
		LOGFONT lf;
		fnt->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;
		m_bold_font = new CFont();
		m_bold_font->CreateFontIndirect(&lf);
		edit->SetFont(m_bold_font);
	}

	return TRUE;
}

void DlgEditText::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    CDialog::OnWindowPosChanging(lpwndpos);

//	TRACE1("%08X\n", lpwndpos->flags);
	
    if (lpwndpos != NULL && lpwndpos->flags == (SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE))
    {
        if (m_res == -1)
        {
            m_res = IDCANCEL;
			PostMessage(WM_USER+1, 0, NULL);
        }
    }
}

LRESULT DlgEditText::OnEnforceFlose(WPARAM, LPARAM)
{
    EndDialog(IDCANCEL);
    return 0;
}

CString DlgEditText::GetText()
{
	return (m_alignment == 0)?m_text_centered:((m_alignment == -1)?m_text_left:m_text_right);
}