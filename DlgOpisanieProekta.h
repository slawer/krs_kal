#pragma once


// CDlgOpisanieProekta dialog

class CDlgOpisanieProekta : public CDialog
{
	DECLARE_DYNAMIC(CDlgOpisanieProekta)

public:
	CDlgOpisanieProekta(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOpisanieProekta();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPISANIE_PROEKTA };
	CString	m_Mestorogdenie;
	CString	m_Kust;
	CString	m_Rabota;
	CString	m_Skvagina;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
