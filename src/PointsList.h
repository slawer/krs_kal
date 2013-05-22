// uic 20.06.2007

#ifndef __PointsList_H_
#define __PointsList_H_

#include "SimpleList.h"
#include "FORMULA.h"

#define PointsList_H 20
extern int PointsList_Num;

struct PointsList: SimpleList
{
	//{{AFX_DATA(PointsList)
	//}}AFX_DATA

	//{{AFX_VIRTUAL(PointsList)
	//}}AFX_VIRTUAL

	PointsList(int mode = 0);

	void Rebuild();

	virtual void DrawCell(CDC* dc, int cell_pos, int cell_num, DWORD cell_data, int x, int y);
	void OnLeftDown(int cell_pos, int cell_num, DWORD cell_data, int x, int y);
	void OnLeftDouble(int cell_pos, int cell_num, DWORD cell_data, int x, int y);

	void SetFont(CFont* font) { m_font = font; }

	FORMULA_PiecewiseTable* m_table;

protected:
	CFont* m_font;
	//{{AFX_MSG(PointsList)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end