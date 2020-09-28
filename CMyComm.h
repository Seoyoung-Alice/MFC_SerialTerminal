#pragma once
#include <afxwin.h>

#define MAXBUF			50000
#define INBUFSIZE		50000
#define OUTBUFSIZE		50000
#define ASCII_XON		0x11
#define ASCII_XOFF		0x13
#define WM_MYRECEIVE	(WM_USER+1)		// 데이터 수신 윈도우 메시지
#define WM_MYCLOSE		(WM_USER+2)		// 종료 윈도우 메시지

class CMyComm :
	public CCmdTarget
{
public:
	// 생성자, 소멸자
	CMyComm();
	~CMyComm();
	CMyComm(CString port, CString baudrate, CString parity, CString databit, CString stopbit);

public:
	HANDLE m_hComDev;
	HWND m_hWnd;

	BOOL m_isOpen;			// 통신 연결 확인 TRUE, FALSE
	CString m_comport;		// comport 값 저장
	CString m_baudrate;		// baud rate 값 저장
	CString m_parity;		// parity 값 저장
	CString m_dataBit;		// data 길이 저장
	CString m_stopBit;		// stop bit 값 저장
	BOOL m_flowCheck;

	OVERLAPPED m_OLR, m_OLW;
	char m_buf[MAXBUF * 2];
	int m_length;
	CEvent* m_pEvent;


	// 메시지 receive 함수
	int Receive(LPSTR inbuf, int len);
	void Clear();
	// message send 함수
	bool Send(LPCTSTR outbuf, int len);
	bool Create(HWND hWnd);
	void HandleClose();
	void Close();
	void ResetSerial();
	static UINT CommThread(LPVOID lpData);
};

