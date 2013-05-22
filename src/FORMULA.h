// uic 3.07.2006

#ifndef __FORMULA_H_
#define __FORMULA_H_

#define FORMULA_ARG_CONST 1
#define FORMULA_ARG_FORMULA 2
#define FORMULA_ARG_CHANNEL 3
#define FORMULA_ARG_PARAM 4
#define FORMULA_ARG_PIECEWISE 5
#define FORMULA_ARG_NULL 6

#define FORMULA_MODE_TEMPLATE 0
#define FORMULA_MODE_ARG_TEMPLATE 1
#define FORMULA_MODE_RESULT 2

#define FORMULA_CALC_STATUS_NOT_READY 0
#define FORMULA_CALC_STATUS_IN_PROGRESS 1
#define FORMULA_CALC_STATUS_COMPLETE 2
#define FORMULA_CALC_STATUS_TIMEOUT 3
#define FORMULA_CALC_STATUS_CYCLE 4
#define FORMULA_CALC_STATUS_NULL 5

struct SERV_Channel;
class CParam;

#include <list>
#include <map>
using namespace std;

struct FORMULA_Variable
{
	CString m_name, m_description;
	float m_value, m_init_value;
};

struct FORMULA_Argument
{
	FORMULA_Argument(byte type, int num);
	virtual ~FORMULA_Argument() {}

	float GetValue(byte &status);
	int GetNumber();
	int GetType() { return m_type; }

	virtual int CorrespondingParamNum() { return -1; }
	virtual int CorrespondingChannelNum() { return -1; }

	virtual CString GetText(byte mode, int &layer) = 0;
	virtual CString GetTextForSave() = 0;
	static FORMULA_Argument* CreateFromText(const char* str, int &offset, int num);
	virtual bool FillFromText(const char* str, int &offset) = 0;

	virtual bool IsTimedOut() = 0;

	virtual void Reset() {};

protected:
	virtual float Value(byte &status) = 0;
	byte m_type;
	int m_num;
	float m_last_value;
};

typedef map<int, FORMULA_Argument*> FORMULA_Arguments;

struct FORMULA_Formula
{
	FORMULA_Formula(CString text_template, int max_arguments_num = -1);
	virtual ~FORMULA_Formula();

	virtual int CorrespondingParamNum() { return -1; }
	virtual int CorrespondingChannelNum();
	
	CString GetText(byte mode);
	CString GetText(byte mode, int &layer);
	CString GetTextForSave();
	virtual CString GetAdditionalTextForSave() { return ""; }
	virtual void FillFromAdditionalText(const char* str, int &offset) {}
	static FORMULA_Formula* CreateFromText(const char* str, int &offset);
	static FORMULA_Formula* CreateFromText(const char* str)
	{
		int offset = 0;
		return FORMULA_Formula::CreateFromText(str, offset);
	}

	void ForgetArgument(int num);
	void SetArgument(FORMULA_Argument* argument);
	FORMULA_Argument* GetArgument(int num);
	void TryCaptureArguments(FORMULA_Formula* src);
	int GetArgumentsNum() { return m_max_arguments_num; }

	bool IsTimedOut();
	float Calculate(byte &status);

	virtual void Reset();

	virtual FORMULA_Formula* MakeTemplate() = 0;
	FORMULA_Formula* MakeCopy();

	void InitFormula();

	int m_num_in_map;

protected:

	CString m_text_template;
	int m_max_arguments_num;
	FORMULA_Arguments m_arguments;

	float m_last_value;
	virtual float Function(byte& status) = 0;
};

typedef map<int, FORMULA_Formula*> FORMULA_Formulas;

struct FORMULA_ArgumentConstantValue: FORMULA_Argument
{
	FORMULA_ArgumentConstantValue(int num, float const_value);
	virtual CString GetText(byte mode, int &layer);
	virtual CString GetTextForSave();
	virtual bool FillFromText(const char* str, int &offset);

	float GetConstantValue() { return m_const_value; }
	void SetConstantValue(float val) { m_const_value = m_last_value = val; }

	virtual bool IsTimedOut() { return false; }
protected:
	float m_const_value;
	virtual float Value(byte& status);
};

struct FORMULA_ArgumentFormula: FORMULA_Argument
{
	FORMULA_ArgumentFormula(int num, FORMULA_Formula* formula);
	virtual ~FORMULA_ArgumentFormula();

	virtual int CorrespondingParamNum() { return m_formula->CorrespondingParamNum(); }
	virtual int CorrespondingChannelNum() { return m_formula->CorrespondingChannelNum(); }

	virtual CString GetText(byte mode, int &layer);
	virtual CString GetTextForSave();
	virtual bool FillFromText(const char* str, int &offset);

	FORMULA_Formula* GetFormula() { return m_formula; }
	void SetFormula(FORMULA_Formula* formula) { m_formula = formula; }

	virtual void Reset() { m_formula->Reset(); }

	virtual bool IsTimedOut();
protected:
	FORMULA_Formula* m_formula;
	virtual float Value(byte& status);
};

struct FORMULA_ArgumentChannelValue: FORMULA_Argument
{
	FORMULA_ArgumentChannelValue(int num, int conv_num, int channel_num);

	virtual int CorrespondingChannelNum() { return m_channel_num; }

	virtual CString GetText(byte mode, int &layer);
	virtual CString GetTextForSave();
	virtual bool FillFromText(const char* str, int &offset);

	SERV_Channel* GetChannel();
	void SetChannel(int conv_num, int channel_num) { m_conv_num = conv_num; m_channel_num = channel_num; }

	virtual bool IsTimedOut();

protected:
	int m_channel_num, m_conv_num;
	virtual float Value(byte& status);
};

struct FORMULA_ArgumentParamValue: FORMULA_Argument
{
	FORMULA_ArgumentParamValue(int num, int param_num);

	virtual int CorrespondingParamNum() { return m_param_num; }

	virtual CString GetText(byte mode, int &layer);
	virtual CString GetTextForSave();
	virtual bool FillFromText(const char* str, int &offset);

	CParam* GetParam();
	void SetParam(int param_num) { m_param_num = param_num; }

	virtual bool IsTimedOut();

protected:
	int m_param_num;
	virtual float Value(byte& status);
};

struct FORMULA_PiecewiseTable
{
	float Calculate(float input);

	map<float, float> m_table;
};

struct FORMULA_ArgumentPiecewise: FORMULA_Argument
{
	FORMULA_ArgumentPiecewise(int num);
	~FORMULA_ArgumentPiecewise();

	virtual CString GetText(byte mode, int &layer);
	virtual CString GetTextForSave();
	virtual bool FillFromText(const char* str, int &offset);

	virtual bool IsTimedOut();

	FORMULA_PiecewiseTable m_table;
	FORMULA_Formula* m_formula;

protected:
	virtual float Value(byte& status);
};

struct FORMULA_ArgumentNULL: FORMULA_Argument
{
	FORMULA_ArgumentNULL(int num) : FORMULA_Argument(FORMULA_ARG_NULL, num) {}
	virtual CString GetText(byte mode, int &layer) { return "NIL"; }
	virtual CString GetTextForSave() { return ""; }
	virtual bool FillFromText(const char* str, int &offset) { return true; }

	virtual bool IsTimedOut() { return true; }
protected:
	virtual float Value(byte& status) { status = FORMULA_CALC_STATUS_TIMEOUT; return 0; }
};
extern FORMULA_Formulas FORMULA_all_formulas;
void FORMULA_Init();

#endif