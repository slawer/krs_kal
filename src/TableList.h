#ifndef __TableList_H_
#define __TableList_H_

#include "SimpleList.h"
#include "SERV.h"

#define TableList_H 20
extern int TableList_Num;

#define TableList_MODE_DSP 0
#define TableList_MODE_PERCENT 1
#define TableList_MODE_VOLTAGE 2

struct TableList: SimpleList
{
	//{{AFX_DATA(TableList)
	//}}AFX_DATA

	//{{AFX_VIRTUAL(TableList)
	//}}AFX_VIRTUAL

	TableList();

	void Rebuild();

	virtual void DrawCell(CDC* dc, int cell_pos, int cell_num, DWORD cell_data, int x, int y);
	void OnLeftDown(int cell_pos, int cell_num, DWORD cell_data, int x, int y);
	void OnLeftDouble(int cell_pos, int cell_num, DWORD cell_data, int x, int y);

	SERV_CalibrationTable* m_table;
	bool m_use_voltage, m_calc_outer_points, m_scaling;
	int m_k;
	float m_voltage_k, m_v0;
	byte m_col1_mode;
	DWORD m_rebuilt_message;

	CString GetTextFirstCol(WORD dsp_val);
	CString GetTextSecondCol(WORD phys_val);
	WORD GetDSPByFloat(float f);
	CString GetSignalTxt();
	void ClearTable();

	void SetFont(CFont* font) { m_font = font; }

protected:
	CFont* m_font;
	//{{AFX_MSG(PointsList)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
////////////////////////////////////////////////////////////////////////////////
// end