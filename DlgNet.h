#if !defined(AFX_DLGNET_H__13141028_7F7D_42A4_914B_2C179F279055__INCLUDED_)
#define AFX_DLGNET_H__13141028_7F7D_42A4_914B_2C179F279055__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDlgNet : public CDialog
{
public:
	CDlgNet(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_NET };
	CString	m_PathDB;
	CString	m_PathKonv;
	CString	m_PathServer;
	int		m_PortKonv;
	int		m_PortDB;
	int		m_PortServer;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DLGNET_H__13141028_7F7D_42A4_914B_2C179F279055__INCLUDED_)
