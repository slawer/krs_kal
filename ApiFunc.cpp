#include "stdafx.h"
#include "krs.h"
#include "MainFrm.h"

#include <afxdisp.h>
#include "excel.h"

#include "ApiFunc.h"

void KNBK(int KNBKList, CString KBIPath) 
{
     try
       {
       _Application app;  // app is an _Application object.
       _Workbook book;
       _Worksheet sheet;
       Workbooks books;
       Worksheets sheets;
       Range range;
       char buf[1024];
       LPDISPATCH lpDisp; // IDispatch *; pointer reused many times.
       long count; // Count of the number of worksheets.

       // Common OLE variants. These are easy variants to use for
       // calling arguments.
       COleVariant
                  covTrue((short)TRUE),
                  covFalse((short)FALSE),
                  covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

       // Start Excel and get Application object.
       if(!app.CreateDispatch("Excel.Application"))
       {
        AfxMessageBox("Couldn't CreateDispatch on Excel");
        return;
       }

       // Set visible.
       app.SetVisible(TRUE);

       // Get Workbooks collection.
       lpDisp = app.GetWorkbooks();  // Get an IDispatch pointer

       ASSERT(lpDisp);               // or fail.
       books.AttachDispatch( lpDisp );  // Attach the IDispatch pointer
                                        // to the books object.

          // Open a workbook.
          lpDisp = books.Open(KBIPath,
                         covOptional, covOptional, covOptional, covOptional,
                         covOptional, covOptional, covOptional, covOptional,
                         covOptional, covOptional, covOptional, covOptional,
                         covOptional, covOptional);//, covOptional); // Excel 2000 has 13 parameters
          ASSERT(lpDisp);  // It worked!



       // Attach to a Workbook object.
       book.AttachDispatch( lpDisp );  // Attach the IDispatch pointer
                                       // to the book object.

       // Get sheets.
       lpDisp = book.GetSheets();
       ASSERT(lpDisp);
       sheets.AttachDispatch(lpDisp);

       // Get the number of worksheets in this book.
       count = sheets.GetCount();

	   if(count > KNBKList)
	   {
		   // Enumerate through worksheets in book and activate in
		   // succession.
			// Get the sheet. Note that 1 is added to the index to make sure
			// it is 1-based, not zero-based. Otherwise, you will get odd
			// exceptions.
			lpDisp = sheets.GetItem( COleVariant((short)(KNBKList)) ); // 'Item' in
								  // the Worksheets collection = worksheet #.
			ASSERT(lpDisp);
			sheet.AttachDispatch(lpDisp);
			// Activate and sleep for two seconds so you can see it happen.
			sheet.Activate();
	   }
	   else
	   {
		   sprintf(buf, "%ld Лист с таким номером отсутствует в документе", KNBKList);
		   ::MessageBox(NULL, buf, "Sheet Count", MB_OK | MB_SETFOREGROUND);
	   }
      }  // End of Processing logic.

      catch(COleException *e)
      {
       char buf[1024];
       sprintf(buf, "COleException. SCODE: %08lx.", (long)e->m_sc);
       ::MessageBox(NULL, buf, "COleException", MB_SETFOREGROUND | MB_OK);
      }

      catch(COleDispatchException *e)
      {

       char buf[1024];
       sprintf(buf,
          "COleDispatchException. SCODE: %08lx,Description: \"%s\".",
          (long)e->m_wCode, (LPSTR)e->m_strDescription.GetBuffer(1024));
       ::MessageBox(NULL, buf, "COleDispatchException",
                    MB_SETFOREGROUND | MB_OK);
      }

      catch(...)
      {
       ::MessageBox(NULL, "General Exception caught.", "Catch-All",
                    MB_SETFOREGROUND | MB_OK);
      }
}
