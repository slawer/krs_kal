#pragma once

#include "BaseDoc.h"

class CCementDoc : public BaseDoc
{
protected: 
	DECLARE_DYNCREATE(CCementDoc)
    CCementDoc();

public:
    virtual void Serialize(CArchive& ar);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
