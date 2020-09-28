#pragma once
#include <afxwin.h>

#define MAXBUF			50000
#define INBUFSIZE		50000
#define OUTBUFSIZE		50000
#define ASCII_XON		0x11
#define ASCII_XOFF		0x13
#define WM_MYRECEIVE	(WM_USER+1)		// ������ ���� ������ �޽���
#define WM_MYCLOSE		(WM_USER+2)		// ���� ������ �޽���

class CMyComm :
	public CCmdTarget
{
public:
	// ������, �Ҹ���
	CMyComm();
	~CMyComm();
	CMyComm(CString port, CString baudrate, CString parity, CString databit, CString stopbit);

public:
	HANDLE m_hComDev;
	HWND m_hWnd;

	BOOL m_isOpen;			// ��� ���� Ȯ�� TRUE, FALSE
	CString m_comport;		// comport �� ����
	CString m_baudrate;		// baud rate �� ����
	CString m_parity;		// parity �� ����
	CString m_dataBit;		// data ���� ����
	CString m_stopBit;		// stop bit �� ����
	BOOL m_flowCheck;

	OVERLAPPED m_OLR, m_OLW;
	char m_buf[MAXBUF * 2];
	int m_length;
	CEvent* m_pEvent;


	// �޽��� receive �Լ�
	int Receive(LPSTR inbuf, int len);
	void Clear();
	// message send �Լ�
	bool Send(LPCTSTR outbuf, int len);
	bool Create(HWND hWnd);
	void HandleClose();
	void Close();
	void ResetSerial();
	static UINT CommThread(LPVOID lpData);
};

