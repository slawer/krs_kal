// KRSDoc.h : interface of the CKRSDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_KRSDOC_H__CACAD846_407C_4D91_B57B_51EBCF09336C__INCLUDED_)
#define AFX_KRSDOC_H__CACAD846_407C_4D91_B57B_51EBCF09336C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDoc.h"

class CKRSDoc : public BaseDoc 
{
protected: // create from serialization only
	CKRSDoc();
	DECLARE_DYNCREATE(CKRSDoc)

public:

	public:
	virtual void Serialize(CArchive& ar);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KRSDOC_H__CACAD846_407C_4D91_B57B_51EBCF09336C__INCLUDED_)
