#if !defined(AFX_BASECTRL_H__A603DC9A_C0D3_45EE_B3E5_FBFD7434FA03__INCLUDED_)
#define AFX_BASECTRL_H__A603DC9A_C0D3_45EE_B3E5_FBFD7434FA03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBaseCtrl window


#include "PictureObj.h"



class CBaseCtrl : public CStatic
{
// Construction
public:
	CBaseCtrl();
// Attributes
protected:
	char* m_PicName;

// Operations
public:
    void SetPicName(char* picname){m_PicName = picname;}

// Override
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	CString GetPartFilePath(int nIndex);
	BOOL DrawInstrument(CDC* memdc);
	virtual ~CBaseCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBaseCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASECTRL_H__A603DC9A_C0D3_45EE_B3E5_FBFD7434FA03__INCLUDED_)
