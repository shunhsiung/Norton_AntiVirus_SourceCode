// qconres.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE QconresDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("QCONRES.DLL Initializing!\n");
		
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hInstance);

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(QconresDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.         

		new CDynLinkLibrary(QconresDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("QCONRES.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(QconresDLL);
	}
	return 1;   // ok
}




////////////////////////////////////////////////////////////////////////////////
//
// Function name	: InitQConsoleResources
// Description	    : Used to force loading of this library.
// Return type		: BOOL 
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL _declspec( dllexport ) InitQConsoleResources()
    {
    // Just return true for now
    return TRUE;
    }