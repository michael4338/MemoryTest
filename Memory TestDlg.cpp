// Memory TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Memory Test.h"
#include "Memory TestDlg.h"
#include <time.h>
#include <process.h>
#include "BaseCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//全局变量

char* StartWords = "尊敬的同学：\n\r  您好!\n\r  下面是一个关于工作记忆的测试，"
"在测试中您将会看到一张有矩阵方程的图片，请您判断一下图片中的方程是否正确，如果"
"正确就按\"左键\"，不正确就按\"右键\"，在您做出正确反应后，电脑会自动转入一个有圆点"
"的网格，您要尽快记住网格中圆点的位置，紧接着，电脑中央会出现\"？？？\"，这时请您"
"在发给您的网格纸上标出刚才记住的圆点的位置，标好之后请按键，然后计算机会自动进"
"入下一张图片。\n\r  好，下面我们先做练习。准备好了，请按键！";

char* MidWords = "好,下面集中精力,我们开始正式试验。准备好了，请按键。";
 
char* LastWords = "试验结束。";


//一些临时值，用于数据中转
BOOL bPostAnswer = FALSE;

int ViewArray[MAX_VIEW_TIME];
int nCurStep = 0;
int gnCurNum = 0;


int GridArray[MAX_VIEW_TIME];
int gnCurGridNum = 0;

BOOL bFinished = FALSE;

int AnswerArray[MAX_PIC_COUNT] = 
{
1,1,0,0,1,1,0,1,0,0,
1,0,0,1,1,0,0,1,1,0,
0,1,1,0,0,1,1,0,0,1,
1,0,0,1,1,0,0,1,1,0,0,1};

//图片类型数组，1代表矩阵方程，2代表网格，3代表???，4代表开始文字，5代表中间文字，6代表结尾文字
int PicTypeArray[MAX_TEST_TIME] = 
{
4,1,2,1,2,3,1,2,1,2,3,1,2,1,2,3,
5,1,2,1,2,3,1,2,1,2,3,1,2,1,2,3,
1,2,1,2,1,2,3,1,2,1,2,1,2,3,1,2,1,2,1,2,3,
1,2,1,2,1,2,1,2,3,1,2,1,2,1,2,1,2,3,1,2,1,2,1,2,1,2,3,
1,2,1,2,1,2,1,2,1,2,3,1,2,1,2,1,2,1,2,1,2,3,1,2,1,2,1,2,1,2,1,2,3,6};


//等待线程
BOOL TimeOut = FALSE;
CRITICAL_SECTION cs_TimeOut;
HWND hWnd;


VOID WaitThreadFunction(void* secondtime)
{
	EnterCriticalSection(&cs_TimeOut);
	TimeOut = FALSE;
	LeaveCriticalSection(&cs_TimeOut);
	bPostAnswer = FALSE;

	DWORD dwStart = GetTickCount();
	while(GetTickCount()-dwStart < (DWORD)secondtime)
	{ 
		if(bPostAnswer)
			_endthread();
	}

	EnterCriticalSection(&cs_TimeOut);
	TimeOut = TRUE;
	LeaveCriticalSection(&cs_TimeOut);

	PostMessage(hWnd,WM_LBUTTONDBLCLK,NULL,NULL);		
}




/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemoryTestDlg dialog

CMemoryTestDlg::CMemoryTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMemoryTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMemoryTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMemoryTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemoryTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMemoryTestDlg, CDialog)
	//{{AFX_MSG_MAP(CMemoryTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_CORRECT, OnCorrect)
	ON_BN_CLICKED(IDC_BUTTON_NEXTSTEP, OnButtonNextstep)
	ON_BN_CLICKED(ID_WRONG, OnWrong)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemoryTestDlg message handlers

BOOL CMemoryTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here


	GetDlgItem(ID_WRONG)->ShowWindow(SW_HIDE);
	GetDlgItem(ID_CORRECT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_NEXTSTEP)->ShowWindow(SW_HIDE);



	return TRUE;  // return TRUE  unless you set the focus to a control
}


int CMemoryTestDlg::GetRandNum(int lower, int upper)
{
	int RandNum = -1;
	srand((unsigned)time(NULL));
	while(RandNum < lower || RandNum > upper)
	{
		RandNum = (int)rand();
	}
	return RandNum;
}


void CMemoryTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMemoryTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMemoryTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMemoryTestDlg::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}

void CMemoryTestDlg::StartWork()
{
	CBaseCtrl* basectrl = new CBaseCtrl();
	basectrl->Create("this",SS_WHITEFRAME/*|SS_NOTIFY */,CRect(40,20,600,440),this,NULL);

	InitializeCriticalSection(&cs_TimeOut);
	hWnd=GetSafeHwnd();
	
	memset(ViewArray,0,MAX_VIEW_TIME);
	memset(GridArray,0,MAX_VIEW_TIME);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_HIDE);
	
	//得到程序当前路径
	char szBuf[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, szBuf);

	char* MatrixPath = new char[MAX_PATH];
	strcpy(MatrixPath,szBuf);
	char* GridPath = new char[MAX_PATH];
	strcpy(GridPath,szBuf);

	strcat(strcat(MatrixPath, "\\"), MATRIX_PATH);
	strcat(MatrixPath, "\\");
	strcat(strcat(GridPath, "\\"), NETGRID_PATH);
	strcat(GridPath, "\\");

	//为数组赋初值
	for(int i=0;i<MAX_TEST_TIME;i++)
	{
		GetDlgItem(IDC_STATIC_STARTWORDS)->ShowWindow(SW_HIDE);
		switch(PicTypeArray[i])
		{
		case 1://如果是矩阵方程图片
			{
				char filename[15];
				char* CurMatrixPath = new char[MAX_PATH];
				strcpy(CurMatrixPath,MatrixPath);

				GetDlgItem(IDC_BUTTON_NEXTSTEP)->EnableWindow(FALSE);
	            GetDlgItem(IDC_BUTTON_NEXTSTEP)->ShowWindow(SW_HIDE);
	            GetDlgItem(ID_CORRECT)->EnableWindow(FALSE);
	            GetDlgItem(ID_CORRECT)->ShowWindow(SW_HIDE);
                GetDlgItem(ID_WRONG)->EnableWindow(FALSE);
	            GetDlgItem(ID_WRONG)->ShowWindow(SW_HIDE);


				memset(filename,0,15);
				int CurNumber = GetRandNum(0, 41);
				gnCurNum = CurNumber;
				sprintf(filename,"%d",CurNumber);
				strcat(filename,".bmp");
				basectrl->SetPicName(strcat(CurMatrixPath, filename));
                basectrl->ShowWindow(SW_SHOW);
				basectrl->UpdateWindow();
                
				EnterCriticalSection(&cs_TimeOut);
	            TimeOut = FALSE;
	            LeaveCriticalSection(&cs_TimeOut);
				_beginthread(WaitThreadFunction, 0, (void*)4500);
				while(TRUE)
				{
					MSG  msg;  
                    GetMessage(&msg,NULL,0,0);  
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
					EnterCriticalSection(&cs_TimeOut);
	                if(TimeOut == TRUE)
					{
						LeaveCriticalSection(&cs_TimeOut);
						break;
					}
	                LeaveCriticalSection(&cs_TimeOut);
					Sleep(10);
				}
 				basectrl->ShowWindow(SW_HIDE);               
				GetDlgItem(ID_CORRECT)->ShowWindow(SW_SHOW);
				GetDlgItem(ID_CORRECT)->EnableWindow(TRUE);
				GetDlgItem(ID_WRONG)->ShowWindow(SW_SHOW);
				GetDlgItem(ID_WRONG)->EnableWindow(TRUE);
				
				this->ShowWindow(TRUE);
				this->UpdateWindow();

				EnterCriticalSection(&cs_TimeOut);
	            TimeOut = FALSE;
	            LeaveCriticalSection(&cs_TimeOut);
				bPostAnswer = FALSE;
				_beginthread(WaitThreadFunction, 0, (void*)INFINITE);
				while(!bPostAnswer)
				{
					MSG  msg;  
                    GetMessage(&msg,NULL,0,0);  
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
					EnterCriticalSection(&cs_TimeOut);
	                if(TimeOut == TRUE)
					{
						LeaveCriticalSection(&cs_TimeOut);
						break;
					}
	                LeaveCriticalSection(&cs_TimeOut);
					Sleep(10);
				}
                
  				delete CurMatrixPath;
 				break;
			}
		case 2://如果是网格
			{
				char filename[15];
				char* CurGridPath = new char[MAX_PATH];
				strcpy(CurGridPath,GridPath);

				GetDlgItem(IDC_BUTTON_NEXTSTEP)->EnableWindow(FALSE);
	            GetDlgItem(IDC_BUTTON_NEXTSTEP)->ShowWindow(SW_HIDE);
	            GetDlgItem(ID_CORRECT)->EnableWindow(FALSE);
	            GetDlgItem(ID_CORRECT)->ShowWindow(SW_HIDE);
                GetDlgItem(ID_WRONG)->EnableWindow(FALSE);
	            GetDlgItem(ID_WRONG)->ShowWindow(SW_HIDE);


				memset(filename,0,15);
				int CurNumber = GetRandNum(1, MAX_GRID_NUM);
				GridArray[gnCurGridNum++] = CurNumber;
				
				sprintf(filename,"%d",CurNumber);
				strcat(filename,".bmp");
				basectrl->SetPicName(strcat(CurGridPath, filename));
                basectrl->ShowWindow(SW_SHOW);
				basectrl->UpdateWindow();
                
				EnterCriticalSection(&cs_TimeOut);
	            TimeOut = FALSE;
	            LeaveCriticalSection(&cs_TimeOut);
				_beginthread(WaitThreadFunction, 0, (void*)1500);
				while(TRUE)
				{
					MSG  msg;  
                    GetMessage(&msg,NULL,0,0);  
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
					EnterCriticalSection(&cs_TimeOut);
	                if(TimeOut == TRUE)
					{
						LeaveCriticalSection(&cs_TimeOut);
						break;
					}
	                LeaveCriticalSection(&cs_TimeOut);
					Sleep(10);
				}
                	
				basectrl->ShowWindow(SW_HIDE); 
				delete CurGridPath;
				break;
			}
		case 3://如果是???
			{
				char* CurMatrixPath = new char[MAX_PATH];
				strcpy(CurMatrixPath,MatrixPath);
				strcat(CurMatrixPath,"100.bmp");
				basectrl->SetPicName(CurMatrixPath);
                basectrl->ShowWindow(SW_SHOW);
				basectrl->UpdateWindow();
  
				GetDlgItem(IDC_BUTTON_NEXTSTEP)->SetWindowText("完成请按键");
                GetDlgItem(IDC_BUTTON_NEXTSTEP)->EnableWindow(TRUE);
	            GetDlgItem(IDC_BUTTON_NEXTSTEP)->ShowWindow(SW_SHOW);
	            GetDlgItem(ID_CORRECT)->EnableWindow(FALSE);
	            GetDlgItem(ID_CORRECT)->ShowWindow(SW_HIDE);
                GetDlgItem(ID_WRONG)->EnableWindow(FALSE);
	            GetDlgItem(ID_WRONG)->ShowWindow(SW_HIDE);

				EnterCriticalSection(&cs_TimeOut);
	            TimeOut = FALSE;
	            LeaveCriticalSection(&cs_TimeOut);
				bPostAnswer = FALSE;

				_beginthread(WaitThreadFunction, 0, (void*)INFINITE);
				while(!bPostAnswer)
				{
					MSG  msg;  
                    GetMessage(&msg,NULL,0,0);  
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
					EnterCriticalSection(&cs_TimeOut);
	                if(TimeOut == TRUE)
					{
						LeaveCriticalSection(&cs_TimeOut);
						break;
					}
	                LeaveCriticalSection(&cs_TimeOut);
					Sleep(10);
				}

				basectrl->ShowWindow(SW_HIDE); 
				delete CurMatrixPath;
				break;
			}
		case 4://如果是开始文字
			{
				GetDlgItem(IDC_STATIC_STARTWORDS)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC_STARTWORDS)->SetWindowText(StartWords);             
				GetDlgItem(IDC_BUTTON_NEXTSTEP)->SetWindowText("开始请按键");
				GetDlgItem(IDC_BUTTON_NEXTSTEP)->EnableWindow(TRUE);
	            GetDlgItem(IDC_BUTTON_NEXTSTEP)->ShowWindow(SW_SHOW);
	            GetDlgItem(ID_CORRECT)->EnableWindow(FALSE);
	            GetDlgItem(ID_CORRECT)->ShowWindow(SW_HIDE);
                GetDlgItem(ID_WRONG)->EnableWindow(FALSE);
	            GetDlgItem(ID_WRONG)->ShowWindow(SW_HIDE);

				EnterCriticalSection(&cs_TimeOut);
	            TimeOut = FALSE;
	            LeaveCriticalSection(&cs_TimeOut);
				bPostAnswer = FALSE;

				_beginthread(WaitThreadFunction, 0, (void*)INFINITE);
				while(!bPostAnswer)
				{
					MSG  msg;  
                    GetMessage(&msg,NULL,0,0);  
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
					EnterCriticalSection(&cs_TimeOut);
	                if(TimeOut == TRUE)
					{
						LeaveCriticalSection(&cs_TimeOut);
						break;
					}
	                LeaveCriticalSection(&cs_TimeOut);
					Sleep(10);
				}

				break;
			}
		case 5://如果是中间文字
			{
				GetDlgItem(IDC_STATIC_STARTWORDS)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC_STARTWORDS)->SetWindowText(MidWords);
				               
				GetDlgItem(IDC_BUTTON_NEXTSTEP)->EnableWindow(TRUE);
	            GetDlgItem(IDC_BUTTON_NEXTSTEP)->ShowWindow(SW_SHOW);
	            GetDlgItem(ID_CORRECT)->EnableWindow(FALSE);
	            GetDlgItem(ID_CORRECT)->ShowWindow(SW_HIDE);
                GetDlgItem(ID_WRONG)->EnableWindow(FALSE);
	            GetDlgItem(ID_WRONG)->ShowWindow(SW_HIDE);

				EnterCriticalSection(&cs_TimeOut);
	            TimeOut = FALSE;
	            LeaveCriticalSection(&cs_TimeOut);
				bPostAnswer = FALSE;

				_beginthread(WaitThreadFunction, 0, (void*)INFINITE);
				while(!bPostAnswer)
				{
					MSG  msg;  
                    GetMessage(&msg,NULL,0,0);  
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
					EnterCriticalSection(&cs_TimeOut);
	                if(TimeOut == TRUE)
					{
						LeaveCriticalSection(&cs_TimeOut);
						break;
					}
	                LeaveCriticalSection(&cs_TimeOut);
					Sleep(10);
				}

				break;
			}
		case 6://如果是结尾文字
			{
				GetDlgItem(IDC_STATIC_STARTWORDS)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC_STARTWORDS)->SetWindowText(LastWords);
                GetDlgItem(IDC_BUTTON_NEXTSTEP)->EnableWindow(TRUE);
				GetDlgItem(IDC_BUTTON_NEXTSTEP)->ShowWindow(SW_SHOW);
				bFinished = TRUE;
				break;
			}
		default:break;
		}
		UpdateWindow();
	}

	delete MatrixPath;
	delete GridPath;
	delete basectrl;
}
void CMemoryTestDlg::OnCorrect() 
{
	// TODO: Add your control notification handler code here
    if(AnswerArray[gnCurNum] == 1)
		ViewArray[nCurStep++] = 1;
	else
		ViewArray[nCurStep++] = 0;

	bPostAnswer = TRUE;
}

void CMemoryTestDlg::OnButtonNextstep() 
{
	// TODO: Add your control notification handler code here
    bPostAnswer = TRUE;
	if(bFinished)
	{
		char result[5];
		int CorrectTime = 0, WrongTime = 0;
	    char* lastresult=new char[4096];
	    memset(lastresult,0,1000);
		strcat(lastresult, "本次测验结果: \n \n");

		strcat(lastresult, "圆点位置: ");
        for(int i=0;i<MAX_VIEW_TIME && i<gnCurGridNum;i++)
		{
			if(GridArray[i] != 0)
			{
				memset(result,0,5);
			    sprintf(result,"%d",GridArray[i]);
				strcat(lastresult,strcat(result,"  "));
			}
		}
		strcat(lastresult,"\n");
		memset(result,0,5);

		strcat(lastresult,"具体反应结果: ");
	    for(int j=0;j<MAX_VIEW_TIME;j++)
		{
			memset(result,0,5);
		    sprintf(result,"%d",ViewArray[j]);
		    strcat(lastresult,strcat(result,"  "));
			if(ViewArray[j] == 0)
				WrongTime ++;
			else if(ViewArray[j] == 1)
				CorrectTime ++;
		}
		memset(result,0,5);
		sprintf(result,"%d",WrongTime);
		strcat(lastresult,"\n");
		strcat(lastresult,"反应错的次数是:  ");
		strcat(lastresult,result);

		memset(result,0,5);
		sprintf(result,"%d",CorrectTime);
		strcat(lastresult,"\n");
		strcat(lastresult,"反应对的次数是:  ");
		strcat(lastresult,result);

		strcat(lastresult,"\n \n");

		FILE* file = fopen("C:\\Memory Test.txt","a+");
		fwrite(lastresult,sizeof(char),strlen(lastresult),file);
		fclose(file);
		
	    delete lastresult;
		OnOK();
	}
}

void CMemoryTestDlg::OnWrong() 
{
	// TODO: Add your control notification handler code here
	if(AnswerArray[gnCurNum] == 1)
		ViewArray[nCurStep++] = 0;
	else
		ViewArray[nCurStep++] = 1;

	bPostAnswer = TRUE;
}

void CMemoryTestDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);
}

void CMemoryTestDlg::OnButtonStart() 
{
	// TODO: Add your control notification handler code here
	StartWork();
	char* str = "He said 'sorry!'";
	AfxMessageBox(str);
}
