#pragma once

#include "BaseDoc.h"

class CDBDoc : public BaseDoc
{
protected: 
	DECLARE_DYNCREATE(CDBDoc)
	CDBDoc();

public:
    virtual void Serialize(CArchive& ar);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
