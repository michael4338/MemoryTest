// Memory Test.h : main header file for the MEMORY TEST application
//

#if !defined(AFX_MEMORYTEST_H__36A0E19A_92E2_420B_AF07_366306FC4510__INCLUDED_)
#define AFX_MEMORYTEST_H__36A0E19A_92E2_420B_AF07_366306FC4510__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMemoryTestApp:
// See Memory Test.cpp for the implementation of this class
//

class CMemoryTestApp : public CWinApp
{
public:
	CMemoryTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemoryTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMemoryTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMORYTEST_H__36A0E19A_92E2_420B_AF07_366306FC4510__INCLUDED_)
