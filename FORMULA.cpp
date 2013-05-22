// uic 3.07.2006

#include <stdafx.h>
#include <CHECK.h>
#include "FORMULA.h"
#include "BS.h"
#include "TXT.h"
#include "../KRS.h"
#include "../MainFrm.h"

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_Argument +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_Argument::FORMULA_Argument(byte type, int num):
	m_type(type), m_num(num), m_last_value(0)
{
}

float FORMULA_Argument::GetValue(byte &status)
{
	m_last_value = Value(status);
	return m_last_value;
}

int FORMULA_Argument::GetNumber()
{
	return m_num;
}

FORMULA_Argument* FORMULA_Argument::CreateFromText(const char* str, int &offset, int num)
{
	int type;
	const char* ptr = str + offset;
	if (num != 1)
	{
		if (*ptr != ',')
			return NULL;
		ptr++;
		offset++;
	}
	if (sscanf(ptr, "A<%d:", &type) != 1)
	{
		if (memcmp(ptr, "A<NULL>", 7) == 0)
			offset += 7;
		return NULL;
	}
	FORMULA_Argument* argument = NULL;
	switch (type)
	{
		case FORMULA_ARG_CONST: argument = new FORMULA_ArgumentConstantValue(num, 0); break;
		case FORMULA_ARG_FORMULA: argument = new FORMULA_ArgumentFormula(num, NULL); break;
		case FORMULA_ARG_CHANNEL: argument = new FORMULA_ArgumentChannelValue(num, -1, -1); break;
		case FORMULA_ARG_PARAM: argument = new FORMULA_ArgumentParamValue(num, -1); break;
		case FORMULA_ARG_PIECEWISE: argument = new FORMULA_ArgumentPiecewise(num); break;
		case FORMULA_ARG_GROUP: argument = new FORMULA_ArgumentGroup(num); break;
	}
	offset += 4; // A<x:
	while (type >= 10)
	{
		offset++;
		type /= 10;
	}
	if (argument == NULL)
		return NULL;
	if (!argument->FillFromText(str, offset))
	{
		delete argument;
		return NULL;
	}
	offset++; // >
	return argument;
}

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_ArgumentConstantValue +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_ArgumentConstantValue::FORMULA_ArgumentConstantValue(int num, float const_value):
	FORMULA_Argument(FORMULA_ARG_CONST, num), m_const_value(const_value)
{
}

float FORMULA_ArgumentConstantValue::Value(byte& status)
{
	status = FORMULA_CALC_STATUS_COMPLETE;
	return m_const_value;
}

CString FORMULA_ArgumentConstantValue::GetText(byte mode, int &layer)
{
	return BS_FloatWOZeros(m_const_value, 3);
}

CString FORMULA_ArgumentConstantValue::GetTextForSave()
{
	return BS_FloatWOZeros(m_const_value, 3);
}

bool FORMULA_ArgumentConstantValue::FillFromText(const char* str, int &offset)
{
	const char* ptr = str + offset;
	if (sscanf(ptr, "%f", &m_const_value) != 1)
		return false;
	while (isdigit(*ptr) || *ptr == '-' || *ptr == '+' || *ptr == '.')
	{
		offset++;
		ptr++;
	}
	//offset += BS_FloatWOZeros(m_const_value, 6).GetLength(); // ������� ����� :)
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_ArgumentFormula +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_ArgumentFormula::FORMULA_ArgumentFormula(int num, FORMULA_Formula* formula):
	FORMULA_Argument(FORMULA_ARG_FORMULA, num), m_formula(formula)
{
}

FORMULA_ArgumentFormula::~FORMULA_ArgumentFormula()
{
	if (m_formula)
		delete m_formula;
}

float FORMULA_ArgumentFormula::Value(byte &status)
{
	if (m_formula == NULL)
	{
		status = FORMULA_CALC_STATUS_NULL;
		return float(INT_MAX);
	}
	return m_formula->Calculate(status);
}

CString FORMULA_ArgumentFormula::GetText(byte mode, int &layer)
{
	CHECK(m_formula);
	layer++;
	if (layer == 1 || m_formula->GetArgumentsNum() == 1)
		return m_formula->GetText(mode, layer);
	return CString("(") + m_formula->GetText(mode, layer) + ")";
}

CString FORMULA_ArgumentFormula::GetTextForSave()
{
	return m_formula->GetTextForSave();
}

bool FORMULA_ArgumentFormula::FillFromText(const char* str, int &offset)
{
	m_formula = FORMULA_Formula::CreateFromText(str, offset);
	return m_formula != NULL;
}

bool FORMULA_ArgumentFormula::IsTimedOut()
{
	return (m_formula == NULL) || m_formula->IsTimedOut();
}

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_ArgumentChannelValue +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_ArgumentChannelValue::FORMULA_ArgumentChannelValue(int num, int conv_num, int channel_num):
	FORMULA_Argument(FORMULA_ARG_CHANNEL, num), m_conv_num(conv_num), m_channel_num(channel_num)
{
}

SERV_Channel* FORMULA_ArgumentChannelValue::GetChannel()
{
	SERV_Channel* channel = NULL;
	SERV_DeviceCfg* cfg = (m_conv_num == 0)?&m_convertor_cfg:SERV_current_device_cfg;
	if (cfg != NULL)
		channel = cfg->GetChannel(m_channel_num);
	return channel;
}

float FORMULA_ArgumentChannelValue::Value(byte &status)
{
	SERV_Channel* channel = GetChannel();
	if (channel == NULL)
	{
		status = FORMULA_CALC_STATUS_NULL;
		return (float)INT_MAX;
	}
	if (channel->GetDevice() != NULL && channel->GetDevice()->InCalibrationPause())
	{
		status = FORMULA_CALC_STATUS_COMPLETE;
		return m_last_value;
	}
	status = channel->IsTimedOut() ? FORMULA_CALC_STATUS_TIMEOUT : FORMULA_CALC_STATUS_COMPLETE;
	return channel->GetLastValue();
}

CString FORMULA_ArgumentChannelValue::GetText(byte mode, int &layer)
{
	SERV_Channel* channel = GetChannel();
	if (channel == NULL)
		return TXT("(NULL channel %d,%d)") << m_conv_num << m_channel_num;
	return channel->GetInfo();
}

CString FORMULA_ArgumentChannelValue::GetTextForSave()
{
	return TXT("%d,%d")<<m_conv_num << m_channel_num;
}

bool FORMULA_ArgumentChannelValue::FillFromText(const char* str, int &offset)
{
	const char* ptr = str + offset;
	if (sscanf(ptr, "%d,%d", &m_conv_num, &m_channel_num) != 2)
		return false;
	int cn = m_conv_num, un = m_channel_num;
	offset += 3;
	while (cn >= 10)
	{
		offset++;
		cn /= 10;
	}
	while (un >= 10)
	{
		offset++;
		un /= 10;
	}
	return true;
}

bool FORMULA_ArgumentChannelValue::IsTimedOut()
{
	SERV_Channel* channel = GetChannel();
	return channel == NULL || channel->IsTimedOut();
}

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_ArgumentParamValue +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_ArgumentParamValue::FORMULA_ArgumentParamValue(int num, int param_num):
	FORMULA_Argument(FORMULA_ARG_PARAM, num), m_param_num(param_num)
{
}

float FORMULA_ArgumentParamValue::Value(byte& status)
{
	CParam* param = GetParam();
	if (param == NULL)
	{
		status = FORMULA_CALC_STATUS_NULL;
		return float(INT_MAX);
	}
	param->GetCurrentValue(status);
	return param->fCur;
}

CParam* FORMULA_ArgumentParamValue::GetParam()
{
	CParam* param = NULL;
	m_MapParam.Lookup(m_param_num, param);
	return param;
}

CString FORMULA_ArgumentParamValue::GetText(byte mode, int &layer)
{
	CParam* param = GetParam();
	if (param == NULL)
        return TXT("(NULL param � %d)") << m_param_num;
	return param->GetNameForShow();
}

CString FORMULA_ArgumentParamValue::GetTextForSave()
{
	return TXT("%d")<<m_param_num;
}

bool FORMULA_ArgumentParamValue::FillFromText(const char* str, int &offset)
{
	const char* ptr = str + offset;
	if (sscanf(ptr, "%d", &m_param_num) != 1)
		return false;

	int param_num = m_param_num;
	offset += 1;
	while (param_num >= 10)
	{
		offset++;
		param_num /= 10;
	}
	return true;
}

bool FORMULA_ArgumentParamValue::IsTimedOut()
{
	CParam* param = GetParam();
	return param == NULL || param->IsTimedOut()/* || !param->IsActive()*/;
}

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_PiecewiseTable +++
////////////////////////////////////////////////////////////////////////////////
float FORMULA_PiecewiseTable::Calculate(float input)
{
	if (m_table.size() < 2)
		return float(INT_MAX);

	map<float, float>::iterator current = m_table.begin(), next = current;
	next++;
	if (input < current->first)
		return current->second;
	while (true)
	{
		if (current->first <= input && next->first >= input)
		{
			float x1 = current->first;
			float x2 = next->first;
			float y1 = current->second;
			float y2 = next->second;
			float k = (input - x1)/(x2 - x1);
			return k*(y2 - y1) + y1;
		}
		current++;
		next++;
        if (next == m_table.end())
            return current->second;
	}
}

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_ArgumentPiecewise +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_ArgumentPiecewise::FORMULA_ArgumentPiecewise(int num):
	FORMULA_Argument(FORMULA_ARG_PIECEWISE, num), m_formula(NULL)
{
}

FORMULA_ArgumentPiecewise::~FORMULA_ArgumentPiecewise()
{
	if (m_formula)
		delete m_formula;
}

float FORMULA_ArgumentPiecewise::Value(byte& status)
{
	if (m_formula == NULL)
	{
		status = FORMULA_CALC_STATUS_NULL;
		return (float)INT_MAX;
	}
	float input = m_formula->Calculate(status);
    if (status == FORMULA_CALC_STATUS_COMPLETE)
	    return m_table.Calculate(input);
    else
        return (float)INT_MAX;
}

CString FORMULA_ArgumentPiecewise::GetText(byte mode, int &layer)
{
	if (m_formula)
		return TXT("���, �����: %d, ����: %s") << m_table.m_table.size() << m_formula->GetText(FORMULA_MODE_RESULT);
	else
		return "ERR";
}

CString FORMULA_ArgumentPiecewise::GetTextForSave()
{
	if (!m_formula)
		return "ERR";
	CString res = TXT("��� %d:") << m_table.m_table.size();
	map<float, float>::iterator current = m_table.m_table.begin();
	while (current != m_table.m_table.end())
	{
		if (current != m_table.m_table.begin())
			res += ",";
		res += TXT("(%s, %s)") << BS_FloatWOZeros(current->first, 3) << BS_FloatWOZeros(current->second, 3);
		current++;
	}
	res += TXT(";%s") << m_formula->GetTextForSave();
	return res;
}

bool FORMULA_ArgumentPiecewise::FillFromText(const char* str, int &offset)
{
	int table_size;
	const char* ptr = str + offset;
	if (sscanf(ptr, "��� %d:", &table_size) != 1 && table_size > 1)
		return false;
	while(*ptr != ':')
	{
		ptr++;
		offset++;
	}
	ptr++;
	offset++;
	FORMULA_PiecewiseTable table;
	float x, y;
	ptr = str + offset;
	for (int i = 0; i < table_size; i++)
	{
		if (sscanf(ptr, "(%f,%f)", &x, &y) != 2)
			return false;
		table.m_table[x] = y;
		while(*ptr != ')')
		{
			ptr++;
			offset++;
		}
		ptr++;
		offset++;
		if (i < table_size - 1)
		{
			if (*ptr != ',')
				return false;
			ptr++;
			offset++;
		}
	}
	if (*ptr != ';')
		return false;
	ptr++;
	offset++;
	FORMULA_Formula* formula = FORMULA_Formula::CreateFromText(str, offset);
	if (formula == NULL)
		return false;
	// ���������
	m_table = table;
	if (m_formula)
		delete m_formula;
	m_formula = formula;
	return true;
}

bool FORMULA_ArgumentPiecewise::IsTimedOut()
{
	return m_formula == NULL || m_formula->IsTimedOut();
}

////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_ArgumentGroup +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_ArgumentGroup::FORMULA_ArgumentGroup(int num):
FORMULA_Argument(FORMULA_ARG_PIECEWISE, num)
{
}
float FORMULA_ArgumentGroup::Value(byte& status)
{
	float res;
	FORMULA_Formulas::iterator current = m_formulas.begin();
	while (current != m_formulas.end())
	{
		if (current->second == NULL)
		{
			status = FORMULA_CALC_STATUS_NULL;
			res = (float)INT_MAX;
		}
		else
			res = current->second->Calculate(status);
		current++;
	}
	return res;
}

CString FORMULA_ArgumentGroup::GetText(byte mode, int &layer)
{
	return TXT("������ ������ (%d ��)") << m_formulas.size();
}

CString FORMULA_ArgumentGroup::GetTextForSave()
{
	CString res = TXT("%d")<<m_formulas.size();
	FORMULA_Formulas::iterator current = m_formulas.begin();
	while (current != m_formulas.end())
	{
		res += "|" + current->second->GetTextForSave();
		current++;
	}		
	return res;
}

bool FORMULA_ArgumentGroup::FillFromText(const char* str, int &offset)
{
	int safe_counter = 0;
	int size = 0, formula_num = 0;
	const char* ptr = str + offset;
	FORMULA_Formula* formula = NULL;
	if (sscanf(ptr, "%d", &size) != 1)
		return false;
	while (formula_num < size)
	{
		while (*ptr != '|' && safe_counter < 10000)
		{
			safe_counter++;
			ptr++;
			offset++;
		}
		if (safe_counter == 10000)
		{
			offset -= 10000;
			return false;
		}
		else
		{
			safe_counter++;
			ptr++;
			offset++;
			formula = FORMULA_Formula::CreateFromText(ptr, offset);
			if (formula)
				m_formulas[formula_num++] = formula;
		}
	}
	return true;
}

bool FORMULA_ArgumentGroup::IsTimedOut()
{
	return false;
}


////////////////////////////////////////////////////////////////////////////////
// +++ FORMULA_Formula +++
////////////////////////////////////////////////////////////////////////////////
FORMULA_Formula::FORMULA_Formula(CString text_template, int max_arguments_num):
	m_text_template(text_template), m_max_arguments_num(max_arguments_num)
{
	if (m_max_arguments_num == -1)
	{
		// ������� ���-�� ����������
		CString arg_template;
		m_max_arguments_num = 0;
		int pos;
		while (true)
		{
			arg_template.Format("%%s%d", m_max_arguments_num + 1);
			pos = m_text_template.Find(arg_template);
			if (pos == -1)
				break;
			m_max_arguments_num++;
		}
	}
}

FORMULA_Formula::~FORMULA_Formula()
{
	FORMULA_Arguments::iterator current = m_arguments.begin();
	while (current != m_arguments.end())
	{
		CHECK(current->second);
		delete current->second;
		current++;
	}
	m_arguments.clear();
}

void FORMULA_Formula::InitFormula()
{
	FORMULA_all_formulas[m_num_in_map] = this;
}

CString FORMULA_Formula::GetText(byte mode)
{
	int layer = 0;
	return GetText(mode, layer);
}

CString FORMULA_Formula::GetText(byte mode, int &layer)
{
	FORMULA_Argument *argument;
	CString result_text = m_text_template, arg_text, arg_template;
	int pos, this_formula_layer = layer;
	for (int arg_num = 1; arg_num < m_max_arguments_num + 1; arg_num++)
	{
		arg_template.Format("%%s%d", arg_num);
		pos = result_text.Find(arg_template);
		if (pos == -1)
			continue;
		argument = GetArgument(arg_num);
		if (argument != NULL &&
				(
					(argument->GetType() == FORMULA_ARG_FORMULA && mode != FORMULA_MODE_ARG_TEMPLATE)
					||
					mode == FORMULA_MODE_RESULT
				)
			)
			arg_text = argument->GetText(mode, layer);
		else
			arg_text.Format("%c%d", 'A' + this_formula_layer, arg_num);

		result_text.Replace(arg_template, arg_text);
	}
	return result_text;
}

float FORMULA_Formula::Calculate(byte &status)
{
	m_last_value = Function(status);
	return m_last_value;
}

void FORMULA_Formula::Reset()
{
	FORMULA_Argument *argument;
	for (int arg_num = 1; arg_num < m_max_arguments_num + 1; arg_num++)
	{
		argument = GetArgument(arg_num);
		if (argument != NULL)
			argument->Reset();
	}
}

bool FORMULA_Formula::IsTimedOut()
{
	FORMULA_Argument* argument;
	for (int arg_num = 1; arg_num < m_max_arguments_num + 1; arg_num++)
	{
		argument = GetArgument(arg_num);
		if (argument == NULL || argument->IsTimedOut())
			return true;
	}
	return false;
}

void FORMULA_Formula::ForgetArgument(int num)
{
	m_arguments.erase(num);
}

void FORMULA_Formula::SetArgument(FORMULA_Argument* argument)
{
	CHECK(argument);
	int num = argument->GetNumber();
	if (num < 1 || num > m_max_arguments_num)
		CRUSH("����� ��������� ��������� ��� ����������� ���������");
	m_arguments[num] = argument;
}

FORMULA_Argument* FORMULA_Formula::GetArgument(int num)
{
	if (m_arguments.find(num) == m_arguments.end())
		return NULL;
	return m_arguments[num];
}

void FORMULA_Formula::TryCaptureArguments(FORMULA_Formula* src)
{
	if (GetArgumentsNum() != src->GetArgumentsNum())
		return;
	FORMULA_Arguments::iterator src_current = src->m_arguments.begin();
	while (src_current != src->m_arguments.end())
	{
		if (src_current->second)
			SetArgument(src_current->second);
		src_current++;
	}
	src->m_arguments.clear();
}

CString FORMULA_Formula::GetTextForSave()
{
	CString res = TXT("F#%d(") << m_num_in_map;
	CString add = GetAdditionalTextForSave();
	if (add.GetLength() > 0)
		res += "<" + add + ">";
	FORMULA_Argument* argument;
	for (int i = 1; i <= m_max_arguments_num; i++)
	{
		argument = GetArgument(i);
		if (i != 1)
			res += ",";
		res += TXT("A<")<<i;
		if (argument == NULL)
			res += "NULL";
		else
			res += (TXT("%d:") << argument->GetType()) + argument->GetTextForSave();
		res += ">";
	}
	return res + ")";
}

FORMULA_Formula* FORMULA_Formula::CreateFromText(const char* str, int &offset)
{
	FORMULA_Formula* formula = NULL;
	const char* ptr = str + offset;
	int num;
	if (sscanf(ptr, "F#%d(", &num) != 1)
		return NULL;
	formula = FORMULA_all_formulas[num]->MakeTemplate();
	offset += 4; // F#x(
	while (num >= 10)
	{
		num /= 10;
		offset++;
	}
	ptr = str + offset;
	if (*ptr == '<')
	{
		offset++;
		formula->FillFromAdditionalText(str, offset);
	}
	FORMULA_Argument* argument;
	int prev_offset;
	for (int i = 1; i <= formula->GetArgumentsNum(); i++)
	{
		prev_offset = offset;
		argument = FORMULA_Argument::CreateFromText(str, offset, i);
		if (argument != NULL)
			formula->SetArgument(argument);
        else
        if (prev_offset == offset) // �� ������� "A<NULL>"
		{
			delete formula;
			return NULL;
		}
	}
	offset++;// )
	return formula;
}

FORMULA_Formula* FORMULA_Formula::MakeCopy()
{
	CString text = GetTextForSave();
	return FORMULA_Formula::CreateFromText(text);
}

////////////////////////////////////////////////////////////////////////////////
// ������� >>
////////////////////////////////////////////////////////////////////////////////

// F = X
struct FormulaX: FORMULA_Formula
{
	FormulaX():
		FORMULA_Formula("%s1")
	{
		m_num_in_map = 0;
	}
	virtual FORMULA_Formula* MakeTemplate() { return new FormulaX; }
	
	virtual int CorrespondingParamNum() { return m_arguments[1]->CorrespondingParamNum(); }
	
protected:
	virtual float Function(byte &status)
	{
		return m_arguments[1]->GetValue(status);
	}
};

// F = A + B
struct FormulaPlus: FORMULA_Formula
{
	FormulaPlus():
		FORMULA_Formula("%s1 + %s2")
	{
		m_num_in_map = 1;
	}
	virtual FORMULA_Formula* MakeTemplate() { return new FormulaPlus; }
protected:
	virtual float Function(byte &status)
	{
		byte status1, status2;
		float res = m_arguments[1]->GetValue(status1) + m_arguments[2]->GetValue(status2);
		status = max(status1, status2);
		return res;
	}
};

// F = A - B
struct FormulaMinus: FORMULA_Formula
{
	FormulaMinus():
		FORMULA_Formula("%s1 - %s2")
	{
		m_num_in_map = 2;
	}
	virtual FORMULA_Formula* MakeTemplate() { return new FormulaMinus; }
protected:
	virtual float Function(byte &status)
	{
		byte status1, status2;
		float res = m_arguments[1]->GetValue(status1) - m_arguments[2]->GetValue(status2);
		status = max(status1, status2);
		return res;
	}
};

// F = A * B
struct FormulaMul: FORMULA_Formula
{
	FormulaMul():
		FORMULA_Formula("%s1 * %s2")
	{
		m_num_in_map = 3;
	}
	virtual FORMULA_Formula* MakeTemplate() { return new FormulaMul; }
protected:
	virtual float Function(byte &status)
	{
		byte status1, status2;
		float res = m_arguments[1]->GetValue(status1) * m_arguments[2]->GetValue(status2);
		status = max(status1, status2);
		return res;
	}
};

// F = A / B
struct FormulaDiv: FORMULA_Formula
{
	FormulaDiv():
		FORMULA_Formula("%s1 / %s2")
	{
		m_num_in_map = 4;
	}
	virtual FORMULA_Formula* MakeTemplate() { return new FormulaDiv; }
protected:
	virtual float Function(byte &status)
	{
		byte status1, status2;
		float val1 = m_arguments[1]->GetValue(status1), val2 = m_arguments[2]->GetValue(status2);
		status = max(status1, status2);
		return (fabs(val2) < 1e-6) ? INT_MAX : (val1 / val2);
	}
};

// F = 1eX
struct FormulaEx: FORMULA_Formula
{
	FormulaEx():
		FORMULA_Formula("10^%s1")
	{
		m_num_in_map = 5;
	}
	virtual FORMULA_Formula* MakeTemplate() { return new FormulaEx; }
protected:
	virtual float Function(byte &status)
	{
		return pow(10, m_arguments[1]->GetValue(status));
	}
};

// F = Sigma
struct FormulaSummirovanie: FORMULA_Formula
{
	FormulaSummirovanie():
		FORMULA_Formula("������������(%s1)")
	{
		m_num_in_map = 6;
		m_summa = 0;
	}

	virtual void Reset()
	{
		FORMULA_Formula::Reset();
		m_summa = 0;
	}

	virtual CString GetAdditionalTextForSave()
	{
		return TXT("%.6f")<<m_summa;
	}
	virtual void FillFromAdditionalText(const char* str, int &offset)
	{
		int safe_counter = 0;
		const char* ptr = str + offset;
		if (sscanf(ptr, "%f", &m_summa) == 1)
		{
		}
		while (*ptr != '>' && safe_counter < 20)
		{
			safe_counter++;
			ptr++;
			offset++;
		}
		if (safe_counter == 20)
			offset -= 20;
		else
			offset++;
	}

	virtual FORMULA_Formula* MakeTemplate() { return new FormulaSummirovanie; }
protected:
	float m_summa;
	virtual float Function(byte &status)
	{
		float add = m_arguments[1]->GetValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE)
			m_summa += add;
		return m_summa;
	}
};

struct FormulaDelta: FORMULA_Formula
{
	FormulaDelta():
	FORMULA_Formula("����������(%s1)"), m_start(true)
	{
		m_num_in_map = 7;
	}

	virtual void Reset()
	{
		FORMULA_Formula::Reset();
		m_start = true;
	}

	virtual CString GetAdditionalTextForSave()
	{
		return TXT("%d %.6f") << (m_start?1:0) << m_f0;
	}
	virtual void FillFromAdditionalText(const char* str, int &offset)
	{
		int safe_counter = 0, start;
		const char* ptr = str + offset;
		if (sscanf(ptr, "%d %f", &start, &m_f0) == 2)
		{
			m_start = (start == 1);
		}
		while (*ptr != '>' && safe_counter < 20)
		{
			safe_counter++;
			ptr++;
			offset++;
		}
		if (safe_counter == 20)
			offset -= 20;
		else
			offset++;
	}

	virtual FORMULA_Formula* MakeTemplate() { return new FormulaDelta; }
protected:
	float m_f0;
	bool m_start;

	virtual float Function(byte &status)
	{
		float val = m_arguments[1]->GetValue(status);
		if (status != FORMULA_CALC_STATUS_COMPLETE)
			return m_last_value;
		if (m_start)
		{
			m_f0 = val;
			m_start = false;
		}
		return val - m_f0;
	}
};

struct FormulaMovingAvegare: FORMULA_Formula
{
	FormulaMovingAvegare():
		FORMULA_Formula("���������� �������(%s1) �� %s2 ��������")
	{
		m_num_in_map = 8;
		m_window_size = 0;
		m_window_data = NULL;
		m_collected = 0;
	}
	~FormulaMovingAvegare()
	{
		if (m_window_data)
			delete [] m_window_data;
	}
	virtual FORMULA_Formula* MakeTemplate() { return new FormulaMovingAvegare; }
protected:
	int m_window_size;
	float* m_window_data;
	int m_collected;

	virtual float Function(byte &status)
	{
		if (status == FORMULA_CALC_STATUS_COMPLETE)
			return m_last_value;
			
		int w = (int)m_arguments[2]->GetValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE)
		{
			float new_val = m_arguments[1]->GetValue(status);
			if (status == FORMULA_CALC_STATUS_COMPLETE)
			{
				if (m_window_size != w) // ��������� ������� ���� ������������
				{
					if (w <= 0)
						return new_val;
					float* new_window_data = new float[w];
					int sz_to_copy = min(w - 1, m_collected);
					if (sz_to_copy > 0)
						memcpy(new_window_data, m_window_data, sz_to_copy*sizeof(float));
					if (m_window_data)
						delete[] m_window_data;
					m_window_data = new_window_data;
					m_window_size = w;
					m_collected = min(m_collected, m_window_size - 1); // �� ���� ������ ����, ����� ���� ���� ��������
				}
				if (m_collected >= m_window_size) // ����� ��������
				{
					memcpy(m_window_data, m_window_data + 1, (m_window_size - 1)*sizeof(float));
					m_collected = m_window_size - 1;
				}
				*(m_window_data + m_collected) = new_val; // ���������� ����� ������
				m_collected++;
			}
			else
			{
				m_collected = 0;
				return 0;
			}
		}
		else
		{
			m_collected = 0;
			return 0;
		}
		float summa = 0;
		for (int i = 0; i < m_collected; i++)
			summa += *(m_window_data + i);
		return summa / m_collected;
	}
};

struct FormulaSuckenDelta: FORMULA_Formula
{
	FormulaSuckenDelta():
		FORMULA_Formula("���(%s1)"), m_start(true)
	{
		m_num_in_map = 9;
		m_last_value = 0;
	}

	virtual void Reset()
	{
		FORMULA_Formula::Reset();
		m_start = true;
	}

	virtual CString GetAdditionalTextForSave()
	{
		return TXT("%d %.6f %.6f") << (m_start?1:0) << m_f0 << m_last_value;
	}
	virtual void FillFromAdditionalText(const char* str, int &offset)
	{
		int safe_counter = 0, start;
		const char* ptr = str + offset;
		if (sscanf(ptr, "%d %f %f", &start, &m_f0, &m_last_value) == 3)
		{
			m_start = (start == 1);
		}
		while (*ptr != '>' && safe_counter < 200)
		{
			safe_counter++;
			ptr++;
			offset++;
		}
		if (safe_counter == 200)
		{
			// tbd �������� �� �������
			offset -= 200;
		}
		else
			offset++;
	}

	virtual FORMULA_Formula* MakeTemplate() { return new FormulaSuckenDelta; }
protected:
	float m_f0;
	bool m_start;

	virtual float Function(byte &status)
	{
		float val = m_arguments[1]->GetValue(status);
		if (status != FORMULA_CALC_STATUS_COMPLETE)
			return m_last_value;
		if (m_start)
		{
			m_f0 = val;
			m_start = false;
		}
		// ����� ������� �����! >>
		if (val - m_f0 < m_last_value)
			m_f0 = val - m_last_value;
		// ����� ������� �����! <<
		return val - m_f0;
	}
};

struct FormulaMin: FORMULA_Formula
{
	FormulaMin():
		FORMULA_Formula("����������� �������� (%s1)")
	{
		m_num_in_map = 10;
		m_min = (float)INT_MAX;
	}

	virtual void Reset()
	{
		FORMULA_Formula::Reset();
		m_min = (float)INT_MAX;
	}

	virtual CString GetAdditionalTextForSave()
	{
		return TXT("%.6f")<<m_min;
	}
	
	virtual void FillFromAdditionalText(const char* str, int &offset)
	{
		int safe_counter = 0;
		const char* ptr = str + offset;
		if (sscanf(ptr, "%f", &m_min) == 1)
		{
		}
		while (*ptr != '>' && safe_counter < 20)
		{
			safe_counter++;
			ptr++;
			offset++;
		}
		if (safe_counter == 20)
			offset -= 20;
		else
			offset++;
	}

	virtual FORMULA_Formula* MakeTemplate() { return new FormulaMin; }
protected:
	float m_min;
	virtual float Function(byte &status)
	{
		float arg = m_arguments[1]->GetValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE && m_min > arg)
			m_min = arg;
		return m_min;
	}
};

struct FormulaMax: FORMULA_Formula
{
	FormulaMax():
		FORMULA_Formula("������������ �������� (%s1)")
	{
		m_num_in_map = 11;
		m_max = INT_MIN;
	}

	virtual void Reset()
	{
		FORMULA_Formula::Reset();
		m_max = INT_MIN;
	}

	virtual CString GetAdditionalTextForSave()
	{
		return TXT("%.6f")<<m_max;
	}

	virtual void FillFromAdditionalText(const char* str, int &offset)
	{
		int safe_counter = 0;
		const char* ptr = str + offset;
		if (sscanf(ptr, "%f", &m_max) == 1)
		{
		}
		while (*ptr != '>' && safe_counter < 20)
		{
			safe_counter++;
			ptr++;
			offset++;
		}
		if (safe_counter == 20)
			offset -= 20;
		else
			offset++;
	}

	virtual FORMULA_Formula* MakeTemplate() { return new FormulaMax; }
protected:
	float m_max;
	virtual float Function(byte &status)
	{
		float arg = m_arguments[1]->GetValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE && m_max < arg)
			m_max = arg;
		return m_max;
	}
};

struct FormulaArithmeticMean: FORMULA_Formula
{
	FormulaArithmeticMean():
		FORMULA_Formula("������� �������������� (%s1)")
	{
		m_num_in_map = 12;
		m_arithmetic_mean = 0;
		m_num_steps = 0;
	}

	virtual void Reset()
	{
		FORMULA_Formula::Reset();
		m_arithmetic_mean = 0;
		m_num_steps = 0;
	}

	virtual CString GetAdditionalTextForSave()
	{
		return TXT("%.6f %d")<<m_arithmetic_mean<<m_num_steps;
	}

	virtual void FillFromAdditionalText(const char* str, int &offset)
	{
		int safe_counter = 0;
		const char* ptr = str + offset;
		if (sscanf(ptr, "%f %d", &m_arithmetic_mean, &m_num_steps) == 2)
		{
		}
		while (*ptr != '>' && safe_counter < 20)
		{
			safe_counter++;
			ptr++;
			offset++;
		}
		if (safe_counter == 20)
			offset -= 20;
		else
			offset++;
	}

	virtual FORMULA_Formula* MakeTemplate() { return new FormulaArithmeticMean; }
protected:
	float m_arithmetic_mean;
	int m_num_steps;
	virtual float Function(byte &status)
	{
		float arg = m_arguments[1]->GetValue(status);
		if (status == FORMULA_CALC_STATUS_COMPLETE)
		{
			double k = 1.0/(m_num_steps + 1);
			m_arithmetic_mean = (m_arithmetic_mean * k * m_num_steps) + (arg * k);
			m_num_steps++;
		}
		return m_arithmetic_mean;
	}
};

////////////////////////////////////////////////////////////////////////////////
// �������
////////////////////////////////////////////////////////////////////////////////
FORMULA_Formulas FORMULA_all_formulas;

FormulaX g_formula_x;
FormulaPlus g_formula_plus;
FormulaMinus g_formula_minus;
FormulaMul g_formula_mul;
FormulaDiv g_formula_div;
FormulaEx g_formula_Ex;
FormulaSummirovanie g_formula_summirovanie;
FormulaMovingAvegare g_formula_moving_average;
FormulaDelta g_formula_delta;
FormulaSuckenDelta g_formula_sucken_delta;
FormulaMin g_formula_min;
FormulaMax g_formula_max;
FormulaArithmeticMean g_formula_arithmetic_mean;

////////////////////////////////////////////////////////////////////////////////
// ������� <<
////////////////////////////////////////////////////////////////////////////////

void FORMULA_Init()
{
	g_formula_x.InitFormula();
	g_formula_plus.InitFormula();
	g_formula_minus.InitFormula();
	g_formula_div.InitFormula();
	g_formula_mul.InitFormula();
	g_formula_Ex.InitFormula();
	g_formula_summirovanie.InitFormula();
	g_formula_moving_average.InitFormula();
	g_formula_delta.InitFormula();
	g_formula_sucken_delta.InitFormula();
	g_formula_min.InitFormula();
	g_formula_max.InitFormula();
	g_formula_arithmetic_mean.InitFormula();
}

////////////////////////////////////////////////////////////////////////////////
// end