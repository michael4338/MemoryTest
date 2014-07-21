// IStream.cpp: implementation of the CIStream class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IStream.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIStream::CIStream(LPSTREAM lpStream):
	m_hr(S_OK)
{
	ATLASSERT(lpStream != NULL);
	m_lpStream = lpStream;
}

CIStream::~CIStream()
{

}

CIStream& CIStream::operator<<(CComVariant& from)
{
	if(IsOk())
		m_hr = from.WriteToStream(m_lpStream);
	return *this;
}

CIStream& CIStream::operator<<(long from)
{
	CComVariant value(from);
	return *this << value;
}

CIStream& CIStream::operator<<(CString& from)
{
	CComVariant value(from);
	return *this << value;
}
CIStream& CIStream::operator<<(BYTE from)
{
	CComVariant value(from);
	return *this << value;
}
CIStream& CIStream::operator<<(short from)
{
	CComVariant value(from);
	return *this << value;
}
CIStream& CIStream::operator<<(float from)
{
	CComVariant value(from);
	return *this << value;
}
CIStream& CIStream::operator<<(double from)
{
	CComVariant value(from);
	return *this << value;
}
CIStream& CIStream::operator<<(int from)
{
	CComVariant value(from);
	return *this << value;
}
CIStream& CIStream::operator<<(bool from)
{
	CComVariant value(from);
	return *this << value;
}

CIStream& CIStream::operator<<(CRect from)
{
	return *this << from.left << from.top << from.right << from.bottom;
}

//////////////////////////////////////////////////////////////////////////
//

CIStream& CIStream::operator>>(CComVariant& to)
{
	if(IsOk())
		m_hr = to.ReadFromStream(m_lpStream);
	return *this;
}

CIStream& CIStream::operator>>(CString& to)
{
	CComVariant var;
	*this >> var; to = var.bstrVal;
	return *this;
}
CIStream& CIStream::operator>>(long& to)
{
	CComVariant var;
	*this >> var; to = var.lVal;
	return *this;
}
CIStream& CIStream::operator>>(BYTE& to)
{
	CComVariant var;
	*this >> var; to = var.bVal;
	return *this;
}
CIStream& CIStream::operator>>(short& to)
{
	CComVariant var;
	*this >> var; 	to = var.iVal;
	return *this;
}
CIStream& CIStream::operator>>(int& to)
{
	CComVariant var;
	*this >> var; to = var.lVal;
	return *this;
}
CIStream& CIStream::operator>>(float& to)
{
	CComVariant var;
	*this >> var; to = var.fltVal;
	return *this;
}
CIStream& CIStream::operator>>(double& to)
{
	CComVariant var;
	*this >> var; to = var.dblVal;
	return *this;
}
CIStream& CIStream::operator>>(bool& to)
{
	CComVariant var;
	*this >> var; 
	to = var.boolVal == VARIANT_TRUE ? true : false;
	return *this;
}

CIStream& CIStream::operator>>(CRect& to)
{
	return *this >> to.left >> to.top >> to.right >> to.bottom;
}
