////////////////////////////////////////////////////////////////////////////////
// uic 5.06.2003

#ifndef __CHECK_H_
#define __CHECK_H_

#define CRUSH(str) { ::MessageBox(0, str, "CRUSH!", MB_OK); ASSERT(false); }

#ifdef _DEBUG
	#define CHECK(cond) if(!(cond)) CRUSH("Condition:\n"###cond##"\nFAILED!!!")
#else
	#define CHECK(cond)
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
// end