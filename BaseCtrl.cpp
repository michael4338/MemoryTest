// BaseCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Memory Test.h"
#include "BaseCtrl.h"
#include"PictureObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseCtrl

CBaseCtrl::CBaseCtrl()
{
}

CBaseCtrl::~CBaseCtrl()
{
}


BEGIN_MESSAGE_MAP(CBaseCtrl, CStatic)
	//{{AFX_MSG_MAP(CBaseCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseCtrl message handlers


void CBaseCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	CBitmap mbmp, *oldbmp;
	
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);
	
	if (mbmp.CreateCompatibleBitmap(&dc, 512, 384))
        oldbmp = mdc.SelectObject(&mbmp);
	else
		AfxMessageBox("error!");
	
	if (DrawInstrument(&mdc))
		dc.BitBlt(0, 0, 512, 384, &mdc, 0, 0, SRCCOPY);
				
	mdc.SelectObject(oldbmp);
	mbmp.DeleteObject();
	mdc.DeleteDC();
	
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CBaseCtrl::DrawInstrument(CDC *memdc)
{
	CRect rect;
	GetClientRect(&rect);

	CPictureObj m_pic;

	if(m_pic.Load(m_PicName))
	{
		m_pic.Draw(memdc,rect);
	}
	else
	{
		AfxMessageBox("无法读文件，请检查路径");
		return FALSE;
	}
	return TRUE;
}
