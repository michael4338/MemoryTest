// Memory TestDlg.h : header file
//

#if !defined(AFX_MEMORYTESTDLG_H__5FCFD7B0_459E_4F7E_89E8_BF58ECCD22C0__INCLUDED_)
#define AFX_MEMORYTESTDLG_H__5FCFD7B0_459E_4F7E_89E8_BF58ECCD22C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#define MAX_TEST_TIME 114
#define MAX_PIC_COUNT 42
#define MAX_VIEW_TIME 48
#define MAX_GRID_NUM  25
#define MATRIX_PATH "matrix"

#define NETGRID_PATH "grid"

VOID WaitThreadFunction(void*);

/////////////////////////////////////////////////////////////////////////////
// CMemoryTestDlg dialog

class CMemoryTestDlg : public CDialog
{
// Construction
public:
	CMemoryTestDlg(CWnd* pParent = NULL);	// standard constructor

public:
	int GetRandNum(int lower, int upper);

	void StartWork();
// Dialog Data
	//{{AFX_DATA(CMemoryTestDlg)
	enum { IDD = IDD_MEMORYTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemoryTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMemoryTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnCorrect();
	afx_msg void OnButtonNextstep();
	afx_msg void OnWrong();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMORYTESTDLG_H__5FCFD7B0_459E_4F7E_89E8_BF58ECCD22C0__INCLUDED_)
