#ifndef __CFG_H_
#define __CFG_H_

#define CFG_TYPE_BOOL 0
#define CFG_TYPE_INT 1
#define CFG_TYPE_DWORD 2
#define CFG_TYPE_HEX 3
#define CFG_TYPE_FLOAT 4
#define CFG_TYPE_DOUBLE 5
#define CFG_TYPE_STRING 6

#define CFGF_OK				0
#define CFGF_ERR_OPEN   	-1
#define CFGF_ERR_READ   	-2
#define CFGF_ERR_NOTFOUND	-3
#define CFGF_ERR_TYPE		-4
#define CFGF_ERR_WRITEDEF	-5

struct CCfgFiles
{
	CCfgFiles(bool write_on_read = true);
	~CCfgFiles();

	bool m_write_on_read;
	void AssignTo(const char *fname);				

	int ReadPar(const char *parname, DWORD& value, const DWORD defvalue = 0, bool hex = false);	
	int ReadPar(const char *parname, int& value, const int defvalue = 0);	
	int ReadPar(const char *parname, bool& value, const bool defvalue = false);	
	int ReadPar(const char *parname, float& value, const float defvalue = 0);	
	int ReadPar(const char *parname, double& value, const double defvalue = 0);	
	int ReadPar(const char *parname, char* value, const char* defvalue = "");	

	bool WritePar(const char *parname, DWORD value, bool hex = false);
	bool WritePar(const char *parname, int value);
	bool WritePar(const char *parname, bool value);
	bool WritePar(const char *parname, float value);
	bool WritePar(const char *parname, double value);
	bool WritePar(const char *parname, const char *value);

	virtual bool WritePar_(const char *parname, const void *value, BYTE type);
	virtual int ReadPar_(const char *parname, void* value, BYTE type);	

	static CString GetStrValue(const void*value, BYTE type);

	CString GetFilename() { return m_file_name; }
protected:
	char *m_file_name;								
	char *m_tmpread;

    static void GetValueStr(const char *parname, const void* value, BYTE type, char* result);
};

#endif