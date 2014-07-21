#include "stdafx.h"
#include "PictureObj.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define HIMETRIC_INCH	2540
#define RADIAN(angle) ((angle)*PI/180.0) //convert angle to radian

static const OLECHAR szDrawObjInfo[] = OLESTR("\005CPictureObjList");


void HimetricToPixels(CDC*pDC,CSize& size)
{
	ASSERT(pDC);
	// convert himetric to pixels
	size.cx	= MulDiv(size.cx, pDC->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
	size.cy	= MulDiv(size.cy, pDC->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);
}

void HimetricToPixels(CWnd*pWnd,CSize& size)
{
	CClientDC dc(pWnd);
	HimetricToPixels(&dc,size);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CPictureObj,CObject,0);

CPictureObj::CPictureObj():
	m_eHorizAlign(H_Left),
	m_eVertAlign(V_Top),
	m_bSelected(false),
	m_bVisible(true),
	m_rcPosition(0,0,0,0),
	m_startRect(0,0,0,0),
	m_trackerStyle(CRectTracker::hatchedBorder),
	m_zoomX(100),
	m_zoomY(100),
	m_PicName(""),	//图片全路径名
	m_RotCenter(0,0),//旋转中心点
	m_MaxAngle(0),	//最大角度
	m_MinAngle(0),	//最小角度
	m_CurAngle(0),	//当前角度
	m_DisRatio(1)	//显示比例
{
}

CPictureObj::CPictureObj(const CRect position):
	m_eHorizAlign(H_Left),
	m_eVertAlign(V_Top),
	m_bSelected(false),
	m_bVisible(true),
	m_rcPosition(position),
	m_startRect(position),
	m_trackerStyle(CRectTracker::hatchedBorder),
	m_zoomX(100),
	m_zoomY(100),
	m_PicName(""),	//图片全路径名
	m_RotCenter(0,0),//旋转中心点
	m_MaxAngle(0),	//最大角度
	m_MinAngle(0),	//最小角度
	m_CurAngle(0),	//当前角度
	m_DisRatio(1)	//显示比例
{

}

CPictureObj::~CPictureObj()
{
	ReleasePicture();
}

/******************************************************************************
*
*  Draw(CDC* pDC, CRect rc, BOOL bZoom)

*  参数说明：
*	CDC*		pDC		//元件绘制的设备上下文
*	CRect		rc		//在其中绘制图像的矩形
*	BOOL		bZoom	//是否保持图像比例，1保持，0不保持
*
*  函数说明：

*    在矩形rc中绘制图像，bZoom为1时保持图像的比例，否则拉伸（缩放）填充矩形rc
*
********************************************************************************/
BOOL CPictureObj::Draw(CDC* pDC, CRect rc, BOOL bZoom)
{
	CRect rcClient;		
	
	CRect	zrc(rc);	

	if (!bZoom)
		rcClient = rc;	//以给定的矩形为边界填充矩形
	else
	{
		//获得等比例缩放后的图像尺寸
		double dWidth = rc.Width();
		double dHeight = rc.Height();
		double dAspectRatio = dWidth / dHeight;

		double dPictureWidth = this->GetSize().cx;
		double dPictureHeight = this->GetSize().cy;
		double dPictureAspectRatio = dPictureWidth / dPictureHeight;

		if (dPictureAspectRatio > dAspectRatio)
		{
			int nNewHeight = (int)(dWidth/dPictureWidth*dPictureHeight);
			zrc.SetRect( 0, 0, (int)dWidth, nNewHeight); 
		}
		else if (dPictureAspectRatio < dAspectRatio)
		{
			int nNewWidth =  (int)(dHeight/dPictureHeight*dPictureWidth);
			zrc.SetRect(0, 0, nNewWidth, (int)(dHeight));
		}

		rcClient = zrc;	//保持图像的比例填充给定的矩形
	}
		
	Draw(pDC, rcClient);

	return TRUE;
}
/**************************************************************************
*
*	函数名：GetPicName
*
*	参数说明：
*		OBJSTRUCT obj	//元件属性结构
*
*	函数说明：
*
*		该函数通过元件属性结构提供的数据获得当前图片的名称
*
***************************************************************************/
CString CPictureObj::GetPicName(CString pStr, double nCurpic)
{
	CString str = "";
	int i = 0, np = 0;
	int nStart = 0, nEnd = 0, nl = 0;

	//获取元件对象中的ObjCurState对应的图像
	if (pStr == "" || pStr == " ")
	{
		pStr = "";
		return pStr;
	}

	nl = pStr.GetLength();
	for ( i = 0; i < nl; i++)
	{
		if ((pStr.GetAt(i)) == ' ')
			np++;
	}

	np++;

	//元件结构中只有一幅图像
	if( np == 1 )
		return pStr;

	//元件结构中包含多幅图像
	if( nCurpic == 0)	//获取第一幅图
	{
		nEnd = pStr.Find(" ", nStart);
		str = pStr.Mid(nStart, (nEnd - nStart));
		return str;
	}

	double pos = 0;
	while(pos != nCurpic)
	{
		nEnd = pStr.Find(" ", nStart + 1);
		pos++;
		nStart = nEnd;
	}
	
	if(nCurpic == (np - 1))
		str = pStr.Mid(nStart + 1);
	else
	{
		nEnd = pStr.Find(" ", nStart+1) - 1;
		str = pStr.Mid(nStart + 1, (nEnd - nStart));
	}

	//判断是否是合法的图像文件名称
	if(str != "")
	{
		CString ext = str.Right(3);
		if(str.GetLength() < 5 || ((ext != "jpg") && (ext != "bmp")
			&& (ext != "ico") && (ext != "rle") && (ext != "dib")
			&& (ext != "gif") && (ext != "wmf") && (ext != "emf")))
		{
			AfxMessageBox("Picture name error!");
			str = "";
			return str;
		}
	}
	return str;
}
/*****************************************************************************
*
*	函数名：GetPicFullName
*
*	参数说明：
*		OBJSTRUCT obj	//元件属性结构
*
*	函数说明：
*
*		该函数通过元件属性结构提供的数据获得当前图片的绝对路径
*
*******************************************************************************/
CString CPictureObj::GetPicFullName(CString sPath, CString allName, double dN)
{
	CString     picPath;
	CString		pName;
	
	//获取图像的相对路径
	CString	picName = this->GetPicName(allName, dN);
	if(picName == "")
	{
		pName = "";
		return pName;
	}
	picPath = sPath + "//" + picName;

	//获得图片的绝对路径
	CFile file(picPath, CFile::modeRead);
	pName = file.GetFilePath();
	file.Close();

	return pName;
}

/*****************************************************************************
*
*	函数名：RotateImage(CDC* pDC, double angle, CPoint pCentre, CRect* offrc)
*
*	参数说明：
*		
*	输入：
*		CDC*	pDC			//显示设备上下文指针
*		double	angle		//旋转角度
*		CPoint	pCentre		//旋转中心点
*		CRect	offrc		//输入：显示窗口大小，输出：输出位图大小
*
*	返回值：HBITMAP			//旋转成功返回旋转后的位图指针，否则返回NULL
*
*	函数说明：
*
*		该函数把m_pPict指定的位图以pCentre为中心在给定的矩形offrc中进行任意角度的旋转
*
*******************************************************************************/
HBITMAP CPictureObj::RotateImage(CDC* pDC, double angle, CPoint pCentre, CRect* offrc)
{
	HBITMAP rhBmp = NULL;//, tBmp = NULL;
	HPALETTE hPalette = NULL;

	//获取m_pPict(IPicture对象)指向的图像的位图句柄
	if (!GetBmphHandle(&rhBmp, &hPalette))
	{
		AfxMessageBox("Get bmp HANDLE failed!");
		return NULL;
	}

	//保持比例在offrc指定的矩形中缩放位图
	rhBmp = Zoom(pDC, rhBmp, *offrc);
	if (rhBmp == NULL)
		return NULL;
	
	//旋转位图
	rhBmp = RotateImage(pDC, &rhBmp, angle, pCentre, offrc);
	if (rhBmp == NULL)
		return NULL;

	return rhBmp;
}

/*****************************************************************************
*
*	函数名：RotateImage
*
*	参数说明：
*		CDC		*pDC			//设备上下文句柄
*		HBITMAP *phBmp			//要旋转的位图句柄指针
*		HBITMAP	*thBmp			//旋转后位图的句柄指针
*		double	angle			//旋转角度
*		CPoint	rCenterPoint	//旋转中心点
*		BOOL	bHoldSize		//是否保持固定比例
*		
*	返回值：BOOL				//旋转成功后返回1，否则返回0
*
*	函数说明：
*
*		该函数把由picName指定位图以给定点rCenterPoint为中心进行任意角度的旋转
*	范围是+180度（顺时针）到-180度（逆时针）
*
*		※该函数目前仅适合旋转正方形图像
*
*******************************************************************************/
BOOL CPictureObj::RotateImage(HBITMAP* phBmp, HBITMAP* thBmp, double angle, 
							CPoint rotatePoint,BOOL bHoldSize)
{
	CBitmap bmp,tbmp;
	BITMAP bm,tbm;
	BYTE *pbuf, *ptbuf,*pnbuf;							//用来存储像素数据
	
	double ANGLE = -1 * PI * (angle / 180);
	if ( *phBmp == NULL)
	{
		AfxMessageBox("Bitmap is NULL");
		return false;
	}
	
	//将位图与原位图句柄连接
	if (!bmp.Attach(*phBmp))
	{
		AfxMessageBox("Bitmap is NULL");
		return false;
	}

	if (!tbmp.Attach(*thBmp))
	{
		AfxMessageBox("Bitmap is NULL");
		return false;
	}
	
	//获得位图结构信息
	bmp.GetBitmap(&bm);
	tbmp.GetBitmap(&tbm);
	
	int i = tbm.bmWidth;
	int j = tbm.bmHeight;
	int wt = tbm.bmWidthBytes;

	int w1 = bm.bmWidthBytes;
	int PixelBytes = bm.bmBitsPixel / 8;
	if (PixelBytes != 3 && PixelBytes != 4)
	{
		AfxMessageBox("Not support bmp format!");
		return false;
	}
		
	int size = bm.bmWidthBytes * bm.bmHeight;	
	pbuf  = new BYTE[size];
	memset(pbuf,(BYTE)255,size);
		
	//读取数据段	
	bmp.GetBitmapBits(size, pbuf);
		
	int w,h,n = 0;
	int w2 = w1 - 2;
	
	//旋转
	double		SrcX1,SrcY1,SrcX2,SrcY2,SrcX3,SrcY3,SrcX4,SrcY4;
	double		DstX1,DstY1,DstX2,DstY2,DstX3,DstY3,DstX4,DstY4;
	int			Wold,Hold,Wnew,Hnew;
	int			x0,y0,x1,y1;
	double		cosa,sina;
	double		num1,num2;

	//计算旋转角的sin和cos值
	cosa=(double)cos(ANGLE);
	sina=(double)sin(ANGLE);
	
	//获得原图的尺寸
	Wold  = bm.bmWidth;
	Hold  = bm.bmHeight;

	//原图的四角以中心点为坐标原点的坐标值
	SrcX1 = -(Wold - 1) / 2;
	SrcY1 =  (Hold - 1) / 2;
	SrcX2 =  (Wold - 1) / 2;
	SrcY2 =  (Hold - 1) / 2;
	SrcX3 = -(Wold - 1) / 2;
	SrcY3 = -(Hold - 1) / 2;
	SrcX4 =  (Wold - 1) / 2;
	SrcY4 = -(Hold - 1) / 2;

	//最终图四角的坐标值
	DstX1 =  cosa * SrcX1 + sina * SrcY1;
	DstY1 = -sina * SrcX1 + cosa * SrcY1;
	DstX2 =  cosa * SrcX2 + sina * SrcY2;
	DstY2 = -sina * SrcX2 + cosa * SrcY2;
	DstX3 =  cosa * SrcX3 + sina * SrcY3;
	DstY3 = -sina * SrcX3 + cosa * SrcY3;
	DstX4 =  cosa * SrcX4 + sina * SrcY4;
	DstY4 = -sina * SrcX4 + cosa * SrcY4;

	//旋转后位图的尺寸
	Wnew = (DWORD)(max(fabs(DstX4 - DstX1), fabs(DstX3 - DstX2)) + 0.5);	//宽
    Hnew = (DWORD)(max(fabs(DstY4 - DstY1), fabs(DstY3 - DstY2)) + 0.5);	//高

	int lb = (Wnew * 3 + 3) & (~3);
	int nsize = Hnew * lb;
	ptbuf = new BYTE[nsize];
	memset(ptbuf,(BYTE)255,nsize);
	
	//计算两个常数
	num1 = -0.5 * (Wnew - 1) * cosa - 0.5 * (Hnew - 1) * sina + 0.5 * (Wold - 1);
	num2 =  0.5 * (Wnew - 1) * sina - 0.5 * (Hnew - 1) * cosa + 0.5 * (Hold - 1);

	//linebytes -= 2;
	for(y1 = 0; y1 < Hnew; y1++)
	{
		for(x1 = 0; x1 < Wnew; x1++)
		{
			x0 = (DWORD)(x1 * cosa + y1 * sina + num1 + 0.5);
			y0 = (DWORD)((-1.0f * x1) * sina + y1 * cosa + num2 + 0.5);

			if( (x0>=0) && (x0<Wold) && (y0>=0) && (y0<Hold))
			{
				if (pbuf[w2 * y0 + x0 * 3] == 255 
					&& pbuf[w2 * y0 + x0 * 3 + 1] == 255
					&& pbuf[w2 * y0 + x0 * 3 + 2] == 255)
					continue;
				
				ptbuf[lb * y1 + x1 * 3] = pbuf[w2 *  y0 + x0 * 3];
				ptbuf[lb * y1 + x1 * 3 + 1] = pbuf[w2 * y0 + x0 * 3 + 1];
				ptbuf[lb * y1 + x1 * 3 + 2] = pbuf[w2 *  y0 + x0 * 3 + 2];
			}
			
		}
	}

	//取出旋转后的部分位图
	int offy = (Hnew - Hold) / 2;
	int offx = (Wnew - Wold) / 2;

	int ymax = (Hnew + Hold) / 2;
	if (Hnew < Hold)
	{
		y1 = offy = 0;
		ymax = Hnew;
	}
	int xmax = (Wnew + Wold) / 2;
	if (Wnew < Wold)
	{
		x1 = offx = 0;
		xmax = Wnew;
	}
	
	for(y1 = offy; y1 < ymax; y1++)
	{
		for(x1 = offx; x1 < xmax; x1++)
		{
			x0 = x1 - offx;
			y0 = y1 - offy;
			pbuf[w2 * y0 + 3 * x0] = ptbuf[lb * y1 + 3 * x1];
			pbuf[w2 * y0 + 3 * x0 + 1] = ptbuf[lb * y1 + 3 * x1 + 1];
			pbuf[w2 * y0 + 3 * x0 + 2] = ptbuf[lb * y1 + 3 * x1 + 2];
		}
	}
	//回写到原位图中	
	//bmp.SetBitmapBits(size, pbuf);
	
	nsize = tbm.bmWidthBytes * tbm.bmHeight;
	pnbuf = new BYTE[nsize];
	memset(pnbuf, (BYTE)0, nsize);
	tbmp.GetBitmapBits(nsize, pnbuf);

	int b = 255, g = 255, r = 255;
	int tpB = tbm.bmBitsPixel / 8, pB = bm.bmBitsPixel / 8;
	n = 0;
	for (h = 0; h < tbm.bmHeight; h++)
		for(w = 0; w < tbm.bmWidth; w++)
		{
			b = pnbuf[wt * h + tpB * w];
			g = pnbuf[wt * h + tpB * w + 1];
			r = pnbuf[wt * h + tpB * w + 2];
			if ( b == 0 && g == 0 && r == 0)
				n++;
		}
	
	n = 0;
	//int tpB = tbm.bmBitsPixel / 8, pB = bm.bmBitsPixel / 8;
	for(h = 0; h < tbm.bmHeight; h++)
	{
		for(w = 0; w < tbm.bmWidth; w++)
		{			
			b = pnbuf[wt * h + tpB * w] = pbuf[w2 * h + pB * w];
			g = pnbuf[wt * h + tpB * w + 1] = pbuf[w2 * h + pB * w + 1];
			r = pnbuf[wt * h + tpB * w + 2] = pbuf[w2 * h + pB * w + 2];
			if ( b == 0 && g == 0 && r == 0)
				n++;			
		}
	}
	
	tbmp.SetBitmapBits(nsize, pnbuf);

	bmp.Detach();
	tbmp.Detach();

	//删除临时指针内存
	delete[] ptbuf;
	ptbuf = NULL;

	delete[] pbuf;
	pbuf = NULL;

	delete[] pnbuf;
	pnbuf = NULL;
	
	return TRUE;
}

/*****************************************************************************
*
*	函数名：RotateImage
*
*	参数说明：
*		
*		CDC*	pDC			//显示设备上下文指针
*		HBITMAP *hBmp		//要旋转的位图句柄指针
*		double	angle		//旋转角度
*		CPoint	pCentre		//旋转中心点
*
*	返回值：HBITMAP			//旋转成功返回旋转后位图，否则返回NULL
*
*	函数说明：
*
*		该函数把由hBmp指定位图以给定点pCentre为中心进行任意角度的旋转
*
*******************************************************************************/
HBITMAP CPictureObj::RotateImage(CDC* pDC, HBITMAP* hBmp, double angle, CPoint pCentre, CRect* offrc)
{
	HBITMAP rhBmp = NULL;
	
	CBitmap		bmp, rbmp;
	BITMAP		bm, rbm;
	BYTE*		buf, *rbuf;
	double		cosa, sina;
	double		SrcX1, SrcY1, SrcX2, SrcY2, SrcX3, SrcY3, SrcX4, SrcY4;
	double		DstX1, DstY1, DstX2, DstY2, DstX3, DstY3, DstX4, DstY4;
	int			Wold, Hold, Wnew, Hnew;
	int			x0, y0, x1, y1;

	//判断被旋转位图是否为空
	if ( *hBmp == NULL)
	{
		AfxMessageBox("Bitmap is NULL");
		return NULL;
	}
	
	//将位图与原位图句柄连接
	if (!bmp.Attach(*hBmp))
	{
		AfxMessageBox("Attach Bitmap failed!");
		return NULL;
	}
		
	//获得位图结构信息
	bmp.GetBitmap(&bm);
	
	DWORD w2 = (DWORD)(bm.bmWidthBytes);
	int m = (bm.bmWidth * bm.bmBitsPixel / 8) % 4;
	if (m != 0)
		w2 = bm.bmWidthBytes - 3 + m;
	else
		w2 = bm.bmWidthBytes;

	DWORD height = bm.bmHeight, width = bm.bmWidth;
	
	int pB = bm.bmBitsPixel / 8;
	if (pB != 3 && pB != 4)
	{
		AfxMessageBox("Not support this bmp format!");
		return false;
	}
	//读取被旋转位图数据段
	int size = bm.bmWidthBytes * bm.bmHeight;
	buf  = new BYTE[size];
	memset(buf,(BYTE)0,size);
	bmp.GetBitmapBits(size, buf);

	//旋转位图并保存到需要返回的位图中
	double RotateAngle = (double)RADIAN(-1 * angle);
	cosa = (double)cos((double)RotateAngle);
	sina = (double)sin((double)RotateAngle);

	Wold = bm.bmWidth;				//被旋转位图宽度
	Hold = bm.bmHeight;				//被旋转位图高度
	//被旋转位图顶点坐标
	SrcX1=(double)(offrc->left);
	SrcY1=(double)(offrc->top);
	SrcX2=(double)(offrc->right);
	SrcY2=(double)(offrc->top);
	SrcX3=(double)(offrc->right);
	SrcY3=(double)(offrc->bottom);
	SrcX4=(double)(offrc->left);
	SrcY4=(double)(offrc->bottom);

	//旋转后被旋转位图的顶点坐标
	DstX1 =  cosa*(offrc->left - pCentre.x) + sina * (offrc->top - pCentre.y) + pCentre.x;
	DstY1 = -sina*(offrc->left - pCentre.x) + cosa * (offrc->top - pCentre.y) + pCentre.y;
	DstX2 =  cosa*(offrc->right - pCentre.x) + sina*(offrc->top - pCentre.y) + pCentre.x;
	DstY2 = -sina*(offrc->right - pCentre.x) + cosa*(offrc->top - pCentre.y) + pCentre.y;
	DstX3 =  cosa*(offrc->right - pCentre.x) + sina*(offrc->bottom - pCentre.y) + pCentre.x;
	DstY3 = -sina*(offrc->right - pCentre.x) + cosa*(offrc->bottom - pCentre.y) + pCentre.y;
	DstX4 =  cosa*(offrc->left - pCentre.x) + sina *(offrc->bottom - pCentre.y) + pCentre.x;
	DstY4 = -sina*(offrc->left - pCentre.x) + cosa *(offrc->bottom - pCentre.y) + pCentre.y;

	//A==>B
	CPoint A(0, 0);
	CPoint* B = &A;	
	B->x = (DWORD)(min(min(DstX1,DstX2),min(DstX3,DstX4)) + 0.5);	//位图旋转后横坐标偏移值
	B->y = (DWORD)(min(min(DstY1,DstY2),min(DstY3,DstY4)) + 0.5);	//位图旋转后纵坐标偏移值

	//旋转有位图的宽和高
	Wnew = (DWORD)(max(max(DstX4,DstX1),max(DstX3,DstX2)) - min(min(DstX1,DstX2),min(DstX3,DstX4)) +0.5);
    Hnew = (DWORD)(max(max(DstY4,DstY1),max(DstY3,DstY2)) - min(min(DstY1,DstY2),min(DstY3,DstY4)) +0.5);

	//创建返回位图
	rbmp.CreateCompatibleBitmap(pDC, Wnew, Hnew);
	rbmp.GetBitmap(&rbm);
	rhBmp = (HBITMAP)rbmp;

	//创建返回位图的数据缓冲区
	int rsize = rbm.bmWidthBytes * rbm.bmHeight;
	rbuf = new BYTE[rsize];
	memset(rbuf, (BYTE)255, rsize);
	
	//计算位图每行数据在缓冲区所占字节数
	DWORD lb = 0;
	int rpB = rbm.bmBitsPixel / 8;
	m = (bm.bmWidth * bm.bmBitsPixel / 8) % 4;
	if (m != 0)
		lb = rbm.bmWidthBytes - 3 + m;
	else
		lb = rbm.bmWidthBytes;

	//进行二次插值计算时与目标点相邻的4个点的指针和目标点的指针
	BYTE * Pa, *Pb, *Pc, *Pd, *P;

	for(y1 = B->y; y1 < (B->y + Hnew); y1++)
		for(x1 = B->x;x1 < (B->x + Wnew); x1++)
		{
			double sx = cosa * (x1 - pCentre.x) - sina * (y1 - pCentre.y) + pCentre.x;
			double sy = sina * (x1 - pCentre.x) + cosa * (y1 - pCentre.y) + pCentre.y;
			
			x0 =(DWORD)sx;
			y0 =(DWORD)sy;

			if ((x0 >= offrc->left) && (x0 < offrc->right) && (y0 >= offrc->top)
				&& (y0 < offrc->bottom) && (sx > 0) && (sy>0))
			{
				double n = y0 + 1 - sy;
				double b = x0 + 1 - sx;

				Pa = &buf[w2 * y0 + x0 * pB];
				Pb = &buf[w2 * y0 + (x0 + 1) * pB];
				Pc = &buf[w2 * (y0 + 1) + x0 * pB];
				Pd = &buf[w2 * (y0 + 1) + (x0 + 1) * pB];
				if (x0 == (bm.bmWidth - 1))
				{
					Pb = Pa;
					Pd = Pc;
				}
				if (y0 == (bm.bmHeight - 1))
				{
					Pc = Pa;
					Pd = Pb;
				}

				P = &rbuf[lb * (y1-B->y) + (x1-B->x) * rpB];

				for (int k = 0; k < rpB; k++)
				{
					if ((*Pa == *Pb) && (*Pb == *Pc) && (*Pc == *Pd))
					{
						*P = *Pa;
						*P++; *Pa++; *Pb++; *Pd++;
						continue;
					}
					*P++ = (BYTE)(n * b * (*Pa++) + n * (1 - b) * (*Pb++)
						+ (1 - n) * b * (*Pc++) + (1 - n) * (1 - b) * (*Pd++));
				}
										 
			}
		}

	//把缓冲区中的数据写回到返回位图中
	rbmp.SetBitmapBits(rsize, rbuf);

	offrc->left = B->x;
	offrc->top = B->y;
	offrc->right = B->x + Wnew;
	offrc->bottom = B->y + Hnew;

	//使位图与句柄分离
	rbmp.Detach();
	bmp.Detach();

	//清除位图对象
	rbmp.DeleteObject();
	bmp.DeleteObject();
	
	//清理缓冲区
	if (buf != NULL)
	{
		delete[] buf;
		buf = NULL;
	}

	if (rbuf != NULL)
	{
		delete[] rbuf;
		rbuf = NULL;
	}

	return rhBmp;
}

/*****************************************************************************
*
*	函数名：Zoom
*
*	参数说明：
*		
*		CDC* pDC			//设备上下文指针
*		HBITMAP hBmp		//被缩放位图的句柄
*		CRect	drc			//缩放后的矩形尺寸
*
*	返回值：HBITMAP			//缩放后的设备相关位图句柄
*
*	函数说明：
*
*		该函数把hBmp指定位图以drc给定的矩形为边界，按照原图像比例进行缩放
*	本函数采用了二次插值算法
*
*******************************************************************************/
HBITMAP CPictureObj::Zoom(CDC* pDC, HBITMAP hBmp, CRect drc)
{
	HBITMAP dhbmp = NULL;
	CBitmap bmp, dbmp;
	BITMAP	bm, dbm;
	BYTE*	buf, *dbuf;
	int		size, dsize;

	bmp.Attach(hBmp);
	bmp.GetBitmap(&bm);
	size = bm.bmWidthBytes * bm.bmHeight;
	buf  = new BYTE[size];
	bmp.GetBitmapBits(size, buf);
	
	//获得等比例缩放后的图像尺寸
	CRect	rc(drc);
	double dWidth = drc.Width();
	double dHeight = drc.Height();
	double dAspectRatio = dWidth / dHeight;

	double dPictureWidth = bm.bmWidth;
	double dPictureHeight = bm.bmHeight;
	double dPictureAspectRatio = dPictureWidth / dPictureHeight;

	if (dPictureAspectRatio > dAspectRatio)
	{
		int nNewHeight = (int)(dWidth/dPictureWidth*dPictureHeight);
		rc.SetRect( 0, 0, (int)dWidth, nNewHeight); 
	}
	else if (dPictureAspectRatio < dAspectRatio)
	{
		int nNewWidth =  (int)(dHeight/dPictureHeight*dPictureWidth);
		rc.SetRect(0, 0, nNewWidth, (int)(dHeight));
	}

	//创建缩放尺寸的空白兼容位图
	dbmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	memset(&dbm, 0, sizeof(BITMAP));
	dbmp.GetBitmap(&dbm);
	dhbmp = (HBITMAP)dbmp;
	dsize = dbm.bmWidthBytes * dbm.bmHeight;
	dbuf  = new BYTE[dsize];
	memset(dbuf, (BYTE)0, dsize);

	//进行缩放
	int m = (bm.bmWidth * bm.bmBitsPixel / 8) % 4;
	int sl = 0, dl = dbm.bmWidthBytes;
	if (m != 0)
		sl = bm.bmWidthBytes - 3 + m;
	else
		sl = bm.bmWidthBytes;
	int sw = bm.bmWidth - 1, sh = bm.bmHeight - 1, dw = dbm.bmWidth - 1, dh = dbm.bmHeight - 1;
	int B, N, x, y;
	int nPixelSize = bm.bmBitsPixel / 8;
	int ndPixelSize = dbm.bmBitsPixel / 8;
	BYTE * pLinePrev, *pLineNext;
    BYTE * pDest;
    BYTE * pA, *pB, *pC, *pD;
    for ( int i = 0; i <= dh; ++i )
	{
		//pDest = ( BYTE * )aDest->ScanLine[i];
		y = i * sh / dh;					//缩放前位图的y坐标
		N = dh - i * sh % dh;				//N系数
		pLinePrev = &buf[y * sl];
		y++;
		pLineNext = ( N == (dh - sh) ) ? pLinePrev : &buf[y * sl];
		for ( int j = 0; j <= dw; ++j )
		{
			x = j * sw / dw * nPixelSize;	//缩放前位图的x坐标
			B = dw - j * sw % dw;			//B系数
			pA = pLinePrev + x;
			pB = pA + nPixelSize;
			pC = pLineNext + x;
			pD = pC + nPixelSize;
			if ( B == (dw - sw) )
			{
				pB = pA;
				pD = pC;
			}
			pDest = &dbuf[i * dl + j * ndPixelSize]; 
			for ( int k = 0; k < nPixelSize; k++ )
				*pDest++ = ( BYTE )( int )(
					( B * N * ( *pA++ - *pB - *pC + *pD ) + dw * N * *pB++
					+ dh * B * *pC++ + ( dw * dh - dh * B - dw * N ) * *pD++
					+ dw * dh / 2 ) / ( dw * dh )
				);
		}
	}
	//把缓冲区中的缩放结果数据写回位图
	dbmp.SetBitmapBits(dsize, dbuf);

	bmp.Detach();
	dbmp.Detach();

	dbmp.DeleteObject();
	bmp.DeleteObject();

	delete[] buf;
	buf = NULL;

	delete[] dbuf;
	dbuf = NULL;
	
	return dhbmp;
}

/***************************************************************************
*
*	函数名：CopyBitmap
*
*	参数说明：
*		
*		HBITMAP  newhbm		被复制目标位图文件句柄
*
*	返回值：
*
*		HBITMAP	复制后的位图句柄
*
*	函数说明：
*
*		该函数根据hbm提供的位图向newhbm位图复制一份
*
****************************************************************************/
HBITMAP CPictureObj::CopyBitmap(CDC* pDC, HBITMAP* hBmp)
{
	CBitmap bmp;
	BITMAP	bm;
	HBITMAP	htBitmap;

	CBitmap tbmp;
	//BITMAP	tbm;
	//BYTE*	sbuf,* dbuf;

	int		w = 0, h = 0, size = 0, nsize = 0;
	int		pB = 0, tpB = 0, lb = 0, tlb = 0;
	
	bmp.Attach(*hBmp);
	bmp.GetBitmap(&bm);

	htBitmap = (HBITMAP)::CopyImage(*hBmp, IMAGE_BITMAP, bm.bmWidth, bm.bmHeight, NULL);
	//htBitmap = CreateCompatibleBitmap(pDC->m_hDC, bm.bmWidth, bm.bmHeight);

	bmp.Detach();

	return htBitmap;
}

/***************************************************************************
*
*	函数名：DrawTransparent
*
*	参数说明：
*		CDC*	 pDC		设备上下文指针
*		HBITMAP  hbm		透明处理的位图文件指针
*		CRect	 rect		图像显示区域
*		COLORREF crColor	透明色
*
*	函数说明：
*
*		该函数根据提供的透明色对透明色区域在指定的矩形区域中绘制位图
*
****************************************************************************/
BOOL CPictureObj::DrawTransparent(CDC* pDC, HBITMAP *hBmp, CRect rect, COLORREF crColor)
{
	CBitmap bmp;
	BITMAP bm;
	CDC dcImage;
	
	if (!(*hBmp))
	{
		//DWORD LastError = GetLastError();
		AfxMessageBox("Image File is NULL");
		return false;
	}
	//构造位图对象
	if (!bmp.Attach(*hBmp))
	{
		AfxMessageBox("Bitmap could be attached");
		return false;
	}
	
	//获得位图结构信息
	bmp.GetBitmap(&bm);
	int nWidth=bm.bmWidth,nHeight=bm.bmHeight;

	//为图像创建一个DC并把图像装入image DC
	dcImage.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmapImage=dcImage.SelectObject(&bmp);

	//OsVerCheck();
	//TRACE("\r\n%X",WINVER);

#if (_MFC_VER >= 0x0700)
	//在目标区域绘制透明图像	
	if (!pDC->TransparentBlt(rect.left, rect.top, rect.Width(),
		rect.Height(), &dcImage, 0, 0, nWidth, nHeight, crColor))
	{
		AfxMessageBox("DrawTransparent Image failed");
		return false;
	}
#else
	//为“掩码”位图创建一个DC
	CDC dcMask;
	dcMask.CreateCompatibleDC(pDC);
	//为“掩码”位图创建一个单色bitmap
	CBitmap bitmapMask;	
	bitmapMask.CreateCompatibleBitmap(pDC, nWidth, nHeight);
	
	//把mask位图装入mask DC
	CBitmap* pOldBitmapMask = dcMask.SelectObject(&bitmapMask);
	
	//用透明色创建“掩码”位图
	dcImage.SetBkColor(crColor);	//crColor是位图中的透明色
	dcMask.BitBlt(rect.left, rect.top, nWidth, nHeight, &dcImage, 0, 0, SRCCOPY);
	
	//分3步进行实际的绘制
	pDC->BitBlt(rect.left, rect.top, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	pDC->BitBlt(rect.left, rect.top, nWidth, nHeight, &dcMask, 0, 0, SRCAND);
	pDC->BitBlt(rect.left, rect.top, nWidth, nHeight, &dcImage, 0, 0, SRCINVERT);
	
	//恢复原先设置	
	dcMask.SelectObject(pOldBitmapMask);
	dcMask.DeleteDC();
	
#endif
	
	//恢复原先设置
	dcImage.SelectObject(pOldBitmapImage);
	dcImage.DeleteDC();

	bmp.Detach();
	bmp.DeleteObject();

	return true;
}


/***************************************************************************
*
*	函数名：DrawTransparent
*
*	参数说明：
*		CDC*	 pDC		设备上下文指针
*		HBITMAP  hbm		透明处理的位图文件指针
*		CRect	 rect		图像显示区域
*		COLORREF crColor	透明色
*
*	函数说明：
*
*		该函数根据提供的透明色对透明色区域在指定的矩形区域中绘制位图
*
***************************************************************************
BOOL CPictureObj::DrawTransparent(CDC* pDC, CString szFileName, CRect rect, COLORREF crColor)
{
	CBitmap bmp;
	HBITMAP hBitmap;
	BITMAP bm;
	HPALETTE hPalette;
	CDC dcImage;

	//根据szFileName提供的文件名称（含路径）加载位图文件
	if (!LoadBmpFile(szFileName, &hBitmap, &hPalette))
		return false;
	if (!hBitmap)
	{
		//DWORD LastError = GetLastError();
		AfxMessageBox("Load Image File failed");
		return false;
	}
	//构造位图对象
	if (!bmp.Attach(hBitmap))
	{
		AfxMessageBox("Bitmap could be attached");
		return false;
	}
	
	//获得位图结构信息
	bmp.GetBitmap(&bm);
	int nWidth=bm.bmWidth,nHeight=bm.bmHeight;

	//为图像创建一个兼容DC
	dcImage.CreateCompatibleDC(pDC);
	
	//把图像载入到imageDC中
	CBitmap* pOldBitmapImage=dcImage.SelectObject(&bmp);

	//在目标区域绘制透明图像
	if (!pDC->TransparentBlt(rect.left, rect.top, rect.Width(),
		rect.Height(), &dcImage, 0, 0, nWidth, nHeight, crColor))
	{
		AfxMessageBox("DrawTransparent Image failed");
		return false;
	}
	
	//恢复原先设置
	dcImage.SelectObject(pOldBitmapImage);

	bmp.DeleteObject();
	
	return true;
}
*/
/******************************************************************************
*
*	函数名：LoadBmpFile
*
*	参数说明：
*
*		szFileName		//bmp文件路径
*		phBmp		//bmp文件图形句柄
*		phPalette		//bmp文件调色板
*
*	函数说明：
*
*		该函数通过LoadImage API函数把bitmap文件加载为一个DIBSection，然后由
*		DIBSection的颜色表创建一个调色板。如果没有颜色表，就使用一个中间色调色板 
*
******************************************************************************/
BOOL CPictureObj::LoadBmpFile(CString szFileName, HBITMAP *phBmp, HPALETTE *phPalette)
{
	BITMAP  bm;
	*phBmp = NULL;
	*phPalette = NULL;

	// Use LoadImage() to get the image loaded into a DIBSection
	*phBmp = (HBITMAP)LoadImage( NULL, szFileName, IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	if( *phBmp == NULL )
		return FALSE;

	// Get the color depth of the DIBSection
	GetObject(*phBmp, sizeof(BITMAP), &bm );

	// If the DIBSection is 256 color or less, it has a color table
	if( ( bm.bmBitsPixel * bm.bmPlanes ) <= 8 )
	{
		HDC           hMemDC;
		HBITMAP       hOldBitmap;
		RGBQUAD       rgb[256];
		LPLOGPALETTE  pLogPal;
		WORD          i;

		// Create a memory DC and select the DIBSection into it
		hMemDC = CreateCompatibleDC( NULL );
		hOldBitmap = (HBITMAP)SelectObject( hMemDC, *phBmp );
		// Get the DIBSection's color table
		GetDIBColorTable( hMemDC, 0, 256, rgb );
		// Create a palette from the color tabl
		pLogPal = (LOGPALETTE *)malloc( sizeof(LOGPALETTE) + (256*sizeof(PALETTEENTRY)) );
		pLogPal->palVersion = 0x300;
		pLogPal->palNumEntries = 256;
		for(i=0;i<256;i++)
		{
			pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
			pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
			pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
		}
		*phPalette = CreatePalette( pLogPal );
		// Clean up
		free( pLogPal );
		SelectObject( hMemDC, hOldBitmap );
		DeleteDC( hMemDC );
	}
	else   // It has no color table, so use a halftone palette
	{
		HDC    hRefDC;
		
		hRefDC = ::GetDC( NULL );
		*phPalette = CreateHalftonePalette( hRefDC );
		::ReleaseDC( NULL, hRefDC );
	}

	return TRUE;
}

/******************************************************************************
*
*	函数名：GetBmphHandle
*
*	参数说明：
*		
*		phBmp		//bmp文件图形句柄
*		phPalette		//bmp文件调色板
*
*	函数说明：
*
*		由IPicture对象m_pPict
*
******************************************************************************/
BOOL CPictureObj::GetBmphHandle(HBITMAP *phBmp, HPALETTE *phPalette)
{
	BOOL bSuccess = FALSE;
	short nType = PICTYPE_UNINITIALIZED;

	if (SUCCEEDED(m_pPict->get_Type(&nType)) && (nType == PICTYPE_BITMAP))
	{
		OLE_HANDLE hBitmap;
		OLE_HANDLE hPalette;

		if (SUCCEEDED(m_pPict->get_Handle(&hBitmap)) &&
			SUCCEEDED(m_pPict->get_hPal(&hPalette)))
		{
			//*phBmp = (HBITMAP)GetHandle();
			*phBmp = (HBITMAP)hBitmap;
			//m_pPict->get_hPal(&hPalette);
			*phPalette = (HPALETTE)hPalette;
			bSuccess = TRUE;
		}
		else
		{
			AfxMessageBox("Not a Bitmaip File");
			bSuccess = FALSE;
		}
	}

	return bSuccess;
}

OLE_HANDLE	CPictureObj::GetHandle()
{
	OLE_HANDLE handle = NULL;
	if(m_pPict != NULL)
		m_pPict->get_Handle(&handle);
	return handle;
}

// In himetric
CSize CPictureObj::GetSize()
{
	CSize size(0,0);
	if(m_pPict != NULL)
	{
		m_pPict->get_Width(&size.cx);
		m_pPict->get_Height(&size.cy);
	}
	return size;
}

// In pixels
CSize CPictureObj::GetSize(CDC*pDC)
{
	ASSERT(pDC);
	CSize size(0,0);
	if (m_pPict != NULL)
	{
		m_pPict->get_Width(&size.cx);
		m_pPict->get_Height(&size.cy);
		// convert himetric to pixels
		::HimetricToPixels(pDC,size);
	}
	return size;
}

// In pixels
CSize CPictureObj::GetSize(CWnd*pWnd)
{
	CClientDC dc(pWnd);
	return GetSize(&dc);
}

void CPictureObj::Copy( CPictureObj &right)
{
	m_eHorizAlign	= right.m_eHorizAlign ;
	m_eVertAlign	= right.m_eVertAlign ;
	m_bSelected		= right.m_bSelected ;
	m_bVisible		= right.m_bVisible ;
	m_rcPosition	= right.m_rcPosition ;
	m_trackerStyle	= right.m_trackerStyle ;

	ReleasePicture();
	COleStreamFile file;
	if(file.CreateMemoryStream())
	{
		IStream* pStream = file.GetStream() ;
		ASSERT(pStream);
		if(pStream)
		{
			HRESULT hr = right.WriteToStream(pStream);
			ASSERT(SUCCEEDED(hr));
			if(FAILED(hr))
				return;
			LARGE_INTEGER dlibMove = {0,0};
			pStream->Seek(dlibMove,STREAM_SEEK_SET,0);
			hr = ReadFromStream(pStream);
			ASSERT(SUCCEEDED(hr));
		}
	}
	else
	{
		ASSERT(0);
	}
}

void CPictureObj::CalcZoom()
{
	CRect curRect	= m_rcPosition;
	CRect startRect	= m_startRect;
	m_zoomX = MulDiv(curRect.Width(),100,startRect.Width());
	m_zoomY = MulDiv(curRect.Height(),100,startRect.Height());
}

void CPictureObj::SetZoomToPosition()
{
	CRect rect	= m_startRect;
	CRect r		= m_rcPosition;
	int w = rect.Width();
	int h = rect.Height();
	w = MulDiv(w,m_zoomX,100);
	h = MulDiv(h,m_zoomY,100);

	if (m_rcPosition.left > m_rcPosition.right)
		r.left	= r.right + w;
	else
		r.right	= r.left + w;

	if (m_rcPosition.top < m_rcPosition.bottom)
		r.bottom = r.top + h;
	else
		r.top	= r.bottom + h;

	m_rcPosition = r;
}

void CPictureObj::ZoomIn()
{
	CalcZoom();
	if(m_zoomX < 500)
		m_zoomX += 10;
	if(m_zoomY < 500)
		m_zoomY += 10;
	SetZoomToPosition();
}

void CPictureObj::ZoomOut()
{
	CalcZoom();
	m_zoomX -= 10;
	if(m_zoomX < 10)
		m_zoomX = 10;
	m_zoomY -= 10;
	if(m_zoomY < 10)
		m_zoomY = 10;
	SetZoomToPosition();
}

HRESULT CPictureObj::WriteToStream(IStream* pStream)
{
	ATLASSERT(pStream);

	CIStream ostream(pStream);
	ostream
		<< (int)m_eHorizAlign
		<< (int)m_eVertAlign
		<< m_bSelected
		<< m_bVisible
		<< m_rcPosition
		<< m_trackerStyle
		<< m_startRect
		<< m_zoomX 
		<< m_zoomY;

	HRESULT hr = ostream.Result();
	if(FAILED(hr))
		return hr; 

	LARGE_INTEGER  dlibMove = {0,0};
	ULARGE_INTEGER SizePosition = {0,0};
	ULARGE_INTEGER CurPosition = {0,0};

	// 保存图象流的大小位置
	hr = pStream->Seek(dlibMove,STREAM_SEEK_CUR,&SizePosition);
	ATLASSERT(SUCCEEDED(hr));

	// 初始化图象尺寸为0
	LONG lSize = 0;
	CComVariant varSize(lSize);
	hr = varSize.WriteToStream(pStream);
	ATLASSERT(SUCCEEDED(hr));
	if(FAILED(hr))
		return hr; 

	if(m_pPict != NULL)
	{

		// 保存图象
		hr = m_pPict->SaveAsFile(pStream, FALSE,&lSize);
		ATLASSERT(SUCCEEDED(hr));

		// 获得图象的结束位置
		ULARGE_INTEGER EndPosition = {0,0};
		dlibMove.QuadPart = 0;
		hr = pStream->Seek(dlibMove,STREAM_SEEK_CUR,&EndPosition);
		ATLASSERT(SUCCEEDED(hr));

		// 寻找大小位置
		dlibMove.QuadPart = SizePosition.QuadPart;
		hr = pStream->Seek(dlibMove,STREAM_SEEK_SET,&CurPosition);
		ATLASSERT(SUCCEEDED(hr));
		ATLASSERT(SizePosition.QuadPart == CurPosition.QuadPart);

	
		// 将图象大小写入图象流
		CComVariant var(lSize);
		VERIFY(SUCCEEDED(var.WriteToStream(pStream)));

		// 保存图像流游标
		dlibMove.QuadPart = EndPosition.QuadPart;
		hr = pStream->Seek(dlibMove,STREAM_SEEK_SET,&CurPosition);
		ATLASSERT(SUCCEEDED(hr));
		ATLASSERT(EndPosition.QuadPart == CurPosition.QuadPart);
	
	}
	return hr;
}

HRESULT CPictureObj::ReadFromStream(IStream* pStream)
{
	ATLASSERT(pStream);
	LONG lSize = 0;
	ReleasePicture();
	
	CIStream istream(pStream);
	istream
		>> (int&)m_eHorizAlign
		>> (int&)m_eVertAlign
		>> m_bSelected
		>> m_bVisible
		>> m_rcPosition
		>> m_trackerStyle
		>> m_startRect
		>> m_zoomX 
		>> m_zoomY;

	HRESULT hr = istream.Result();
	if(FAILED(hr))
		return hr; 

	// It is needed to know is picture in the stream
	// getting the size of the picture
	CComVariant varSize;
	hr = varSize.ReadFromStream(pStream);
	VERIFY(SUCCEEDED(hr));

	if(SUCCEEDED(hr))
	{
		ATLASSERT(varSize.vt == VT_I4);
		ATLASSERT(lSize >= 0 );
		lSize = varSize.lVal;

		if(lSize > 0)
		{
			// Warning !!! Microsoft bug!!! It is needed to set stream position manualy!!!
			LARGE_INTEGER Move = {0,0};
			ULARGE_INTEGER StartPosition = {0,0};
			// save stream position
			hr = pStream->Seek(Move,STREAM_SEEK_CUR,&StartPosition);
			ATLASSERT(SUCCEEDED(hr));

			hr = ::OleLoadPicture(pStream, lSize, FALSE, IID_IPicture, (LPVOID *)&m_pPict);
			ATLASSERT(SUCCEEDED(hr));
			
			// set correct next position
			Move.QuadPart = StartPosition.QuadPart + lSize;
			hr = pStream->Seek(Move,STREAM_SEEK_SET,&StartPosition);
			ATLASSERT(SUCCEEDED(hr));
		}
	}
	return hr;
}

BOOL CPictureObj::Load(LPCTSTR szFile)
{
	m_strPathName = szFile;

	HRESULT hr = S_OK;
	ReleasePicture();
	CFileStatus status;
	if(!CFile::GetStatus(szFile,status))
		return false;
	hr = ::OleLoadPicturePath(CComBSTR(szFile),NULL,0,0,IID_IPicture,(LPVOID *)&m_pPict);

	if(SUCCEEDED(hr))
		return true;

	return false;
}

BOOL CPictureObj::CreateFromFile(const CPoint pt)
{
	static char BASED_CODE szExt[]	= "jpg";
	static char BASED_CODE szName[] = "Image";
	static char BASED_CODE szFilter[] =
	"JPEG (*.jpg)|*.jpg|"
	"Windows Bitmap (*.bmp)|*.bmp|"
	"GIF Images (*.gif)|*.gif|"
	"Windows Icons (*.ico)|*.ico||"
	;


	CFileDialog dlg(TRUE,szExt,szName,
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter);

	if(dlg.DoModal() != IDOK)
		return false;
	CString strPathName = dlg.GetPathName();
	if(Load(strPathName))
	{
		CSize size = GetSize((CWnd*)NULL);

		m_rcPosition.SetRect(pt.x,pt.y,size.cx,size.cy);
		m_startRect = m_rcPosition;
		CalcZoom();
		return true;
	}
	return false;
}

void CPictureObj::MoveTo(const CRect& position, CWnd* pView)
{
	ASSERT_VALID(this);

	if (position == m_rcPosition)
		return;
	const dw = 8;
	CRect rect = m_rcPosition;
	rect.InflateRect(dw,dw);

	if (pView != NULL)
		pView->InvalidateRect(rect);
	
	m_rcPosition = position;
	rect = m_rcPosition;
	rect.InflateRect(dw,dw);

	if (pView != NULL)
		pView->InvalidateRect(rect);
}

void CPictureObj::DrawTracker(CDC* pDC, const CRect& rect)
{
	if (m_pPict == NULL)
		return;
	CRectTracker tracker;
	tracker.m_rect = rect;
	pDC->LPtoDP(tracker.m_rect);
	tracker.m_nStyle |= m_trackerStyle;
	tracker.Draw(pDC);
}

void CPictureObj::DrawTracker(CDC* pDC)
{
	DrawTracker(pDC,GetPosition());
}


void CPictureObj::Draw(CDC* pDC, const CRect& rcPosition, const CRect& rcBounds)
{
	if (m_pPict == NULL)
		return;
	if(m_bVisible)
		Render(pDC,rcPosition,rcBounds);
	if(m_bSelected && !pDC->IsPrinting())
	{
		DrawTracker(pDC,rcBounds);
	}
}

void CPictureObj::Draw(CDC* pDC, CRect& rcBounds)
{
	Draw(pDC,rcBounds,rcBounds);
}


void CPictureObj::Draw(CDC* pDC) 
{
	if (m_pPict == NULL)
		return;

	// get size in pixels
	CSize size = GetSize(pDC);
	CRect rcBounds,rcRender(0,0,size.cx,size.cy);
	pDC->GetClipBox(&rcBounds);

	if(!m_rcPosition.IsRectEmpty())
		rcRender = m_rcPosition;

	// Offset picture using alignment style
	CPoint offset(0,0);
	switch(m_eHorizAlign)
	{
		default:
			ASSERT(0);
		case H_Center:
			offset.x = (rcBounds.Width() - size.cx)/2;
			break;
		case H_Left:
			break;
		case H_Right:
			offset.x = (rcBounds.Width() - size.cx);
			break;
	}

	switch(m_eVertAlign)
	{
		default:
			ASSERT(0);
		case V_Center:
			offset.y = ((rcBounds.Height() - size.cy)/2);
			break;
		case V_Top:
			break;
		case V_Bottom:
			offset.y = ((rcBounds.Height() - size.cy));
			break;
	}

	rcRender.OffsetRect(offset);
	Draw(pDC,rcRender);

}


//////////////////////////////////////////////////////////////////////
// CPictureObjList

CPictureObjList::~CPictureObjList()
{
	RemoveAndDestroy();
}

void CPictureObjList::RemoveAndDestroy()
{
	if(!GetCount())
		return;
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CPictureObj* pObj = GetNext(pos);
		delete pObj;
	}
	RemoveAll();
}

void CPictureObjList::DeselectAll() 
{
	POSITION pos = GetHeadPosition();
	while(pos != NULL)
	{
		CPictureObj* pObj = GetNext(pos);
		pObj->SetSelected(false);
	}
}

CPictureObj* CPictureObjList::FindSelected()
{
	POSITION pos = GetHeadPosition();
	while(pos != NULL)
	{
		CPictureObj* pObj = GetNext(pos);
		if(pObj->GetSelected())
			return pObj;
	}
	return NULL;
}

bool CPictureObjList::Remove(CPictureObj* pObj)
{
	POSITION pos = Find(pObj);
	if(pos != NULL)
	{
		RemoveAt(pos);
	}
	return (pos != NULL);
}

// in himetric
CSize CPictureObjList::ComputeMaxSize()
{
	CSize sizeMax(0,0);
	POSITION pos = GetHeadPosition();
	while(pos != NULL)
	{
		CPictureObj* pObj = GetNext(pos);
		CSize size = pObj->GetSize();
		
		sizeMax.cx = max(sizeMax.cx, size.cx);
		sizeMax.cy = max(sizeMax.cy, size.cy);
	}
	return sizeMax;
}

// in pixel
CSize CPictureObjList::ComputeMaxSize(CDC* pDC)
{
	CSize sizeMax(ComputeMaxSize());
	::HimetricToPixels(pDC,sizeMax);
	return sizeMax;
}

// in pixel
CSize CPictureObjList::ComputeMaxSize(CWnd* pWnd)
{
	CSize sizeMax(ComputeMaxSize());
	::HimetricToPixels(pWnd,sizeMax);
	return sizeMax;
}

CRect CPictureObjList::GetRect()
{
	CRect rectMax(0,0,0,0);
	POSITION pos = GetHeadPosition();
	while(pos != NULL)
	{
		CPictureObj* pObj = GetNext(pos);
		CRect rect = pObj->GetPosition();
		rectMax.UnionRect(rectMax,rect);
		
	}
	return rectMax;
}

void CPictureObjList::Draw(CDC* pDC)
{
	POSITION pos = GetTailPosition();
	while (pos != NULL)
	{
		CPictureObj* pObj = GetPrev(pos);
		pObj->Draw(pDC);
	}
}

// find object that contain point
CPictureObj* CPictureObjList::ObjectAt(const CPoint& pt)
{
	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CPictureObj* pObj = GetNext(pos);
		CRect rect = pObj->GetPosition();
		rect.NormalizeRect();
		if (rect.PtInRect(pt))
			return pObj;
	}
	return NULL;
}

HRESULT CPictureObjList::WriteToStream(IStream* pStream)
{
	ATLASSERT(pStream);

	CIStream ostream(pStream);
	int count = GetCount();
	ostream << count;

	HRESULT hr = ostream.Result();
	if(FAILED(hr))
		return hr; 

	POSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		CPictureObj* pObj = GetNext(pos);
		hr = pObj->WriteToStream(pStream);
		if(FAILED(hr))
			break;
	}
	return hr;
}

HRESULT CPictureObjList::ReadFromStream(IStream* pStream)
{
	ATLASSERT(pStream);
	CIStream istream(pStream);
	int count = 0;
	istream >> count;

	HRESULT hr = istream.Result();
	if(FAILED(hr))
		return hr; 
	while(count--)
	{
		CPictureObj* pObj = new CPictureObj;
		hr = pObj->ReadFromStream(pStream);
		if(FAILED(hr))
			break;
		AddTail(pObj);
	}
	return hr;
}

BOOL CPictureObjList::ReadFromStorage(LPSTORAGE lpRootStg)
{
	if (lpRootStg != NULL)
	{
		CComPtr<IStream>spStream;

		if (FAILED(lpRootStg->OpenStream(szDrawObjInfo, 
				   NULL, STGM_SHARE_EXCLUSIVE|STGM_READ, 
				   0, &spStream)))
		{
			TRACE(_T("OpenStream failed\n"));
			return FALSE;
		}
		else
		{
			if(FAILED(ReadFromStream(spStream)))
			{
				TRACE(_T("ReadFromStream failed\n"));
				return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPictureObjList::WriteToStorage(LPSTORAGE lpRootStg)
{
	if (lpRootStg != NULL)
	{
		CComPtr<IStream>spStream;
		if (FAILED(lpRootStg->CreateStream(szDrawObjInfo, 
				   STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 
				   0, 0, &spStream)))
		{
			TRACE(_T("CreateStream failed\n"));
			return FALSE;
		}
		else
		{
			if(FAILED(WriteToStream(spStream)))
			{
				TRACE(_T("WriteToStream failed\n"));
				return FALSE;
			}
			lpRootStg->Commit(STGC_DEFAULT);
			return TRUE;
		}
	}
	return FALSE;
}



