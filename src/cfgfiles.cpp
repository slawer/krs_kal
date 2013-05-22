#include "stdafx.h"
#include "cfgfiles.h"
#include <stdio.h>
#include <string>

using namespace std;

// ======================================================================================= //
CCfgFiles::CCfgFiles(bool write_on_read):
	m_write_on_read(write_on_read)
{
	m_file_name = new char[1024];
	m_file_name[0] = '\0';

	m_tmpread = new char[1024];
}

CCfgFiles::~CCfgFiles()
{
	delete []m_file_name;
	delete []m_tmpread;
}
// ======================================================================================= //

void CCfgFiles::AssignTo(const char *fname)
{
	memcpy(m_file_name, fname, strlen(fname)+1);
}

bool CCfgFiles::WritePar(const char *parname, bool value)
{
	return WritePar_(parname, (void*)&value, CFG_TYPE_BOOL);
}

bool CCfgFiles::WritePar(const char *parname, int value)
{
	return WritePar_(parname, (void*)&value, CFG_TYPE_INT);
}

bool CCfgFiles::WritePar(const char *parname, DWORD value, bool hex)
{
	return WritePar_(parname, (void*)&value, hex?CFG_TYPE_HEX:CFG_TYPE_DWORD);
}

bool CCfgFiles::WritePar(const char *parname, float value)
{
	return WritePar_(parname, (void*)&value, CFG_TYPE_FLOAT);
}

bool CCfgFiles::WritePar(const char *parname, double value)
{
	return WritePar_(parname, (void*)&value, CFG_TYPE_DOUBLE);
}

bool CCfgFiles::WritePar(const char *parname, const char *value)
{
	return WritePar_(parname, (void*)value, CFG_TYPE_STRING);
}

static CString FloatWOZeros(float f, int precision)
{
	CString result, format;
	format.Format("%%.%df", precision);
	result.Format(format, f);
	int len = result.GetLength(), i = len - 1;
	if (result.Find(".") != -1)
	{
		while (result.GetAt(i) == '0') i--;
		if (i < len - 1)
		{
			if (result.GetAt(i) == '.') i--;
			return result.Left(i+1);
		}
	}
	return result;
}

CString CCfgFiles::GetStrValue(const void*value, BYTE type)
{
	CString result;
	switch (type)
	{
		case CFG_TYPE_BOOL:
			result.Format("%s", (*(bool*)value)?"true":"false");
			break;
		case CFG_TYPE_INT:
			result.Format("%i", *(int*)value);
			break;
		case CFG_TYPE_DWORD:
			result.Format("%d", *(DWORD*)value);
			break;
		case CFG_TYPE_HEX:
			result.Format("0x%X", *(DWORD*)value);
			break;
		case CFG_TYPE_FLOAT:
			result.Format("%s", FloatWOZeros(*(float*)value, 6));
			break;
		case CFG_TYPE_DOUBLE:
			result.Format("%s", FloatWOZeros(*(double*)value, 12));
			break;
		case CFG_TYPE_STRING:
			result.Format("\"%s\"", (const char*)value);
			break;
		default:
			result = "WRONG VAULE TYPE";
	}
	return result;
}

void CCfgFiles::GetValueStr(const char *parname, const void* value, BYTE type, char* result)
{
	sprintf(result,"%s = %s\n", parname, GetStrValue(value, type));
}

bool CCfgFiles::WritePar_(const char *parname, const void* value, BYTE type)
{
	FILE *cfg_file, *tmp_file;
	char pname[1000], tmp_file_name[1000], wstr[2000], oldch = '\n';
	string str;
	bool f_parwritten = false, f_skipping = false;

	sprintf(tmp_file_name, "%s.$$$", m_file_name);

	if (	(cfg_file = fopen(m_file_name,"rt")) == NULL && 
			(cfg_file = fopen(m_file_name,"wt")) == NULL
		)
		return false;		
	if ( (tmp_file = fopen(tmp_file_name,"wt")) == NULL)
	{
		fclose(cfg_file);
		return false;		
	}

	while (!feof(cfg_file))
	{
		if ( fgets(m_tmpread, 1024, cfg_file) != NULL )
		{
			int str_len = strlen(m_tmpread);
			if (str_len < 5) 
				continue;
			oldch = m_tmpread[str_len - 1];
			str = m_tmpread;

			if (!f_parwritten)
			{
				int i = -1;
				do
				{
					i++; pname[i] = str[i];
				} while (/*!iswspace(str[i]) && */!(str[i]=='='));
				while (i > 0 && iswspace(pname[i - 1]))
					i--;
				pname[i] = 0;

				if (i != str.length()) 
				{
					if (strcmp(pname, parname)==0)
					{
						GetValueStr(parname, value, type, wstr);
						fputs(wstr, tmp_file);
						f_parwritten = true;
						f_skipping = true;
					}
				}
			}
			if (!f_skipping)
				fputs(m_tmpread, tmp_file);
			else
				f_skipping = false;			
		}
		else
			break;
	}
	if (!f_parwritten)
	{
		if ((oldch!='\n')&&(oldch!='\r')) fputs("\n", tmp_file);
		GetValueStr(parname, value, type, wstr);
		fputs(wstr, tmp_file);
	}

	fclose(cfg_file);
	fclose(tmp_file);
	if (remove(m_file_name) != 0)
	{
		remove(tmp_file_name);
		return false;
	}
	else
		rename(tmp_file_name, m_file_name);
	return true;

}
int CCfgFiles::ReadPar(const char *parname, bool& value, const bool defvalue)
{
	int res = ReadPar_(parname, &value, CFG_TYPE_BOOL);
	if (res==CFGF_ERR_NOTFOUND || res==CFGF_ERR_TYPE)
	{
		value = defvalue;
		if (m_write_on_read)
			if(!WritePar(parname, defvalue)) return CFGF_ERR_WRITEDEF;
	}
	return res;
}
int CCfgFiles::ReadPar(const char *parname, int& value, const int defvalue)
{
	int res = ReadPar_(parname, &value, CFG_TYPE_INT);
	if (res==CFGF_ERR_NOTFOUND || res==CFGF_ERR_TYPE)
	{
		value = defvalue;
		if (m_write_on_read)
			if (!WritePar(parname, defvalue)) return CFGF_ERR_WRITEDEF;
	}
	return res;
}
int CCfgFiles::ReadPar(const char *parname, DWORD& value, const DWORD defvalue, bool hex)
{
	int res = ReadPar_(parname, &value, hex?CFG_TYPE_HEX:CFG_TYPE_DWORD);
	if (res==CFGF_ERR_NOTFOUND || res==CFGF_ERR_TYPE)
	{
		value = defvalue;
		if (m_write_on_read)
			if (!WritePar(parname, defvalue, hex)) return CFGF_ERR_WRITEDEF;
	}
	return res;
}
int CCfgFiles::ReadPar(const char *parname, float& value, const float defvalue)
{
	int res = ReadPar_(parname, &value, CFG_TYPE_FLOAT);
	if (res==CFGF_ERR_NOTFOUND || res==CFGF_ERR_TYPE)
	{
		value = defvalue;
		if (m_write_on_read)
			if (!WritePar(parname, defvalue)) return CFGF_ERR_WRITEDEF;
	}
	return res;
}
int CCfgFiles::ReadPar(const char *parname, double& value, const double defvalue)
{
	int res = ReadPar_(parname, &value, CFG_TYPE_DOUBLE);
	if (res==CFGF_ERR_NOTFOUND || res==CFGF_ERR_TYPE)
	{
		value = defvalue;
		if (m_write_on_read)
			if (!WritePar(parname, defvalue)) return CFGF_ERR_WRITEDEF;
	}
	return res;
}
int CCfgFiles::ReadPar(const char *parname, char* value, const char* defvalue)
{
	int res = ReadPar_(parname,value, CFG_TYPE_STRING);
	if (res==CFGF_ERR_NOTFOUND || res==CFGF_ERR_TYPE)
	{
		sprintf(value, "%s", defvalue);
		if (m_write_on_read)
			if (!WritePar(parname, defvalue)) return CFGF_ERR_WRITEDEF;
	}
	return res;
}

int CCfgFiles::ReadPar_(const char *parname, void *value, BYTE type)
{
	FILE *cfg_file;
	char pname[1024];
	string str;
	int str_len;
	int res = 1;

	if ( (cfg_file = fopen(m_file_name,"rt")) == NULL) return CFGF_ERR_NOTFOUND;// -- uic CFGF_ERR_OPEN; 	// ошибка открытия файла
	
	while (!feof(cfg_file))
	{
		if ( fgets(m_tmpread, 1024, cfg_file) != NULL )
		{
			str = m_tmpread;
			int i = -1;
			str_len = min(1023, str.size());
			if (str_len < 5) 
				continue;
			do
			{
				i++;
				pname[i] = str[i];
			} while (str[i] != '=' && i < str_len);
			while (i > 0 && i < str_len && iswspace(pname[i - 1]))
				i--;
			pname[i] = '\0';
		
			if (i!=(str.length())) {		
				if (strcmp(pname, parname)==0) {
					str.erase(0,i);	
					i = -1;	do{	i++; } while ((str[i]!=0) && !(str[i]=='='));
					if (i!=(str.length())) {		
						str.erase(0,i+1);	
						i = -1;	do{	i++; } while ((str[i]!=0) && (iswspace(str[i])));
						if (i != str.length()) 
						{
							str.erase(0,i);	 
							i = str.length(); do{i--; } while ((i!=0) && (iswspace(str[i])));
							if (str.length()-i>1)
								str.erase(i+1,str.length()-i);
							if ( (str[0]=='"')&&(str[str.length()-1]=='"') )
							{
								if (type != CFG_TYPE_STRING)
									res = CFGF_ERR_TYPE;
								else
								{
									str.erase(0,1);
									str.erase(str.length()-1,1);
									sprintf((char*)value,"%s",str.c_str());
									res = CFGF_OK;
								}
							}
							else
							if (strcmp(str.c_str(),"false") == 0)
							{
								if (type != CFG_TYPE_BOOL)
									res = CFGF_ERR_TYPE;
								else
								{
									*(bool*)value = false;
									res = CFGF_OK;
								}
							}
							else
							if (strcmp(str.c_str(),"true")==0)
							{
								if (type != CFG_TYPE_BOOL)
									res = CFGF_ERR_TYPE;
								else
								{
									*(bool*)value = true;
									res = CFGF_OK;
								}
							}
							else
							if (str.find(".",0) != str.npos)
							{
								if (type != CFG_TYPE_DOUBLE && type != CFG_TYPE_FLOAT)
									res = CFGF_ERR_TYPE;
								else
								{
									if (type == CFG_TYPE_FLOAT)
										*(float*)value = atof(str.c_str());
									else
										*(double*)value = atof(str.c_str());
									res = CFGF_OK;
								}
							}
							else
							{
								if (type == CFG_TYPE_FLOAT)
								{
									*(float*)value = atof(str.c_str());
									res = CFGF_OK;
								}
								else
								if (type == CFG_TYPE_DOUBLE)
								{
									*(double*)value = atof(str.c_str());
									res = CFGF_OK;
								}
								else
								{
									if (str.find("0x") == 0)
										res = (1 == sscanf(str.c_str()+2, "%x", (int*)value))?CFGF_OK:CFGF_ERR_NOTFOUND;
									else
									{
										*(int*)value = atoi(str.c_str());
										res = CFGF_OK;
									}
								}
							}
							fclose(cfg_file);
							return res;
						}

					}
				}
			}
		}
	}
	fclose(cfg_file);
	return CFGF_ERR_NOTFOUND;
}

// ======================================================================================= //
