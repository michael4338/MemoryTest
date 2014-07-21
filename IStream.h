// IStream.h: interface for the CIStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISTREAM_H__9C373D6E_F164_4945_A514_C056C96E720B__INCLUDED_)
#define AFX_ISTREAM_H__9C373D6E_F164_4945_A514_C056C96E720B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>

class CIStream  
{
public:
	CIStream(LPSTREAM lpStream);
	virtual ~CIStream();

	bool IsBad();
	bool IsOk();
	HRESULT Result();
	void ClearError();

	CIStream& operator<<(CComVariant& from);
	CIStream& operator<<(CString& from);
	CIStream& operator<<(long from);
	CIStream& operator<<(BYTE from);
	CIStream& operator<<(short from);
	CIStream& operator<<(int from);
	CIStream& operator<<(float from);
	CIStream& operator<<(double from);
	CIStream& operator<<(bool from);
	CIStream& operator<<(CRect from);
	

	CIStream& operator>>(CComVariant& to);
	CIStream& operator>>(CString& to);
	CIStream& operator>>(long& to);
	CIStream& operator>>(BYTE& to);
	CIStream& operator>>(short& to);
	CIStream& operator>>(int& to);
	CIStream& operator>>(float& to);
	CIStream& operator>>(double& to);
	CIStream& operator>>(bool& to);
	CIStream& operator>>(CRect& to);
	
	

protected:

	LPSTREAM	m_lpStream;
	HRESULT		m_hr;

};

inline
HRESULT CIStream::Result()
{
	return m_hr;
}
inline
bool CIStream::IsOk()
{
	return SUCCEEDED(m_hr);
}

inline
bool CIStream::IsBad()
{
	return FAILED(m_hr);
}
inline
void CIStream::ClearError()
{
	m_hr = S_OK;
}


#endif // !defined(AFX_ISTREAM_H__9C373D6E_F164_4945_A514_C056C96E720B__INCLUDED_)
