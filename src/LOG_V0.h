// uic 18.04.2007

#ifndef __LOG_V0_H_
#define __LOG_V0_H_

#define LOG_V0_MESSAGE 0x00000001
#define LOG_V0_DEBUG_MESSAGE 0x00000002
#define LOG_V0_TO_DB 0x00000004
#define LOG_V0_FROM_DB 0x00000008

void LOG_V0_Init();
void LOG_V0_ShutDown();
void LOG_V0_AddMessage(int index, CString text, bool time = true, bool date = false);
void LOG_V0_Flush();
void LOG_V0_SetMask(DWORD mask);

extern CString LOG_V0_app_dir;

#endif
////////////////////////////////////////////////////////////////////////////////
// end