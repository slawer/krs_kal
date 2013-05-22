#pragma once


// CDlgMestorogdenie dialog

class CDlgMestorogdenie : public CDialog
{
	DECLARE_DYNAMIC(CDlgMestorogdenie)

public:
	CDlgMestorogdenie(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMestorogdenie();

// Dialog Data
	enum { IDD = IDD_OPISANIE_MESTOROGDENIJA };
	CString	m_Mestorogdenie;
	CString	m_Kust;
	CString	m_Rabota;
	CString	m_Skvagina;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual BOOL OnInitDialog();
};
