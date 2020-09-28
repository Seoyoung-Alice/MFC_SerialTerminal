#include "pch.h"
#include "CMyComm.h"


CMyComm::CMyComm()
{
}

CMyComm::~CMyComm()
{
	if (m_isOpen)
	{
		Close();
	}

	delete m_pEvent;
}

// 생성자 overloading
CMyComm::CMyComm(CString port, CString baudrate, CString parity, CString databit, CString stopbit)
{
	// 변수 초기화 작업

	m_isOpen = FALSE;			
	m_comport = port;		
	m_baudrate = baudrate;		
	m_parity = parity;		
	m_dataBit = databit;		
	m_stopBit = stopbit;		
	m_flowCheck = 1;

	m_length = 0;
	memset(m_buf, 0, MAXBUF * 2);
	m_pEvent = new CEvent(FALSE, TRUE);
}

// message receive 함수
int CMyComm::Receive(LPSTR inbuf, int len)
{
	CSingleLock lockObj((CSyncObject*)m_pEvent, FALSE);

	if (len == 0)
	{
		return -1;
	}
	else if (len > MAXBUF)
	{
		return -1;
	}
	else
	{
		//AfxMessageBox(_T("ERROR Receive MAXBUF overflow"));
		//return 0;
	}

	if (m_length == 0)
	{
		inbuf[0] = '\0';
		return 0;
	}
	else if (m_length <= len)
	{
		lockObj.Lock();
		memcpy(inbuf, m_buf, m_length);
		memset(m_buf, 0, MAXBUF * 2);

		int temp = m_length;
		m_length = 0;
		lockObj.Unlock();
		return temp;
	}
	else
	{
		lockObj.Lock();
		memcpy(inbuf, m_buf, len);
		memmove(m_buf, m_buf + len, MAXBUF * 2 - len);
		m_length = m_length - len;
		lockObj.Unlock();
		return len;
	}

}


void CMyComm::Clear()
{
	// 모든 변수 초기화
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	memset(m_buf, 0, MAXBUF * 2);
	m_length = 0;
}


// message send 함수
bool CMyComm::Send(LPCTSTR outbuf, int len)
{
	BOOL bRet = TRUE;
	DWORD ErrorFlag;
	COMSTAT ComStat;

	DWORD ByteWrite;
	DWORD ByteSend = 0;

	ClearCommError(m_hComDev, &ErrorFlag, &ComStat);

	if (!WriteFile(m_hComDev, outbuf, len, &ByteWrite, &m_OLW))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if (WaitForSingleObject(m_OLW.hEvent, 1000) != WAIT_OBJECT_0)
			{
				bRet = FALSE;
			}
			else
			{
				GetOverlappedResult(m_hComDev, &m_OLW, &ByteWrite, FALSE);
			}
			// if end

		}
		// I/O ERROR -> ignore
		else
		{
			bRet = FALSE;
		}
		// if end

	}
	else
	{
		AfxMessageBox(_T("Error SEND WriteFile Error !RETURN bRet = TRUE!"));
	}

	ClearCommError(m_hComDev, &ErrorFlag, &ComStat);

	return bRet;
}


bool CMyComm::Create(HWND hWnd)
{
	m_hWnd = hWnd;

	m_hComDev = CreateFile(m_comport, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

	if (m_hComDev != INVALID_HANDLE_VALUE)
	{
		m_isOpen = TRUE;
	}
	else
	{
		return FALSE;
	}

	ResetSerial();

	m_OLW.Offset = 0;
	m_OLW.OffsetHigh = 0;
	m_OLR.Offset = 0;
	m_OLR.OffsetHigh = 0;

	m_OLR.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_OLR.hEvent == NULL)
	{
		CloseHandle(m_OLR.hEvent);
		return FALSE;
	}
	else
	{
		//AfxMessageBox(_T("Error CREATE m_OLR.hEvent == NULL"));
	}

	m_OLW.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_OLW.hEvent == NULL)
	{
		CloseHandle(m_OLW.hEvent);
		return FALSE;
	}
	else
	{
		//AfxMessageBox(_T("Error CREATE m_OLW.hEvent == NULL"));
	}

	AfxBeginThread(CommThread, (LPVOID)this);
	EscapeCommFunction(m_hComDev, SETDTR);

	return TRUE;

}


void CMyComm::HandleClose()
{
	CloseHandle(m_hComDev);
	CloseHandle(m_OLR.hEvent);
	CloseHandle(m_OLW.hEvent);
}


void CMyComm::Close()
{
	// 열려있지 않으면 통과 == 닫혀있음
	if (!m_isOpen)
	{
		return;
	}

	m_isOpen = FALSE;
	SetCommMask(m_hComDev, 0);
	EscapeCommFunction(m_hComDev, CLRDTR);
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	Sleep(500);
}


void CMyComm::ResetSerial()
{
	DCB dcb;
	DWORD Err;
	COMMTIMEOUTS CommTimeOuts;

	if (!m_isOpen)
	{
		return;
	}

	ClearCommError(m_hComDev, &Err, NULL);
	SetupComm(m_hComDev, INBUFSIZE, OUTBUFSIZE);
	PurgeComm(m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;

	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 1000;
	SetCommTimeouts(m_hComDev, &CommTimeOuts);

	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	GetCommState(m_hComDev, &dcb);

	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;

	if (m_baudrate == "300")
	{
		dcb.BaudRate = CBR_300;
	}
	else if(m_baudrate == "600")
	{
		dcb.BaudRate = CBR_600;
	}
	else if (m_baudrate == "1200")
	{
		dcb.BaudRate = CBR_1200;
	}
	else if (m_baudrate == "2400")
	{
		dcb.BaudRate = CBR_2400;
	}
	else if (m_baudrate == "4800")
	{
		dcb.BaudRate = CBR_4800;
	}
	else if (m_baudrate == "9600")
	{
		dcb.BaudRate = CBR_9600;
	}
	else if (m_baudrate == "14400")
	{
		dcb.BaudRate = CBR_14400;
	}
	else if (m_baudrate == "19200")
	{
		dcb.BaudRate = CBR_19200;
	}
	else if (m_baudrate == "28800")
	{
		dcb.BaudRate = CBR_38400;
	}
	else if (m_baudrate == "33600")
	{
		dcb.BaudRate = CBR_38400;
	}
	else if (m_baudrate == "38400")
	{
		dcb.BaudRate = CBR_56000;
	}
	else if (m_baudrate == "56000")
	{
		dcb.BaudRate = CBR_57600;
	}
	else if (m_baudrate == "57600")
	{
		dcb.BaudRate = CBR_115200;
	}
	else if (m_baudrate == "115200")
	{
		dcb.BaudRate = CBR_128000;
	}
	else if (m_baudrate == "128000")
	{
		dcb.BaudRate = CBR_256000;
	}
	else if (m_baudrate == "PCI_9600")
	{
		dcb.BaudRate = 1075;
	}
	else if (m_baudrate == "PCI_19200")
	{
		dcb.BaudRate = 2212;
	}
	else if (m_baudrate == "PCI_38400")
	{
		dcb.BaudRate = 4300;
	}
	else if (m_baudrate == "PCI_57600")
	{
		dcb.BaudRate = 6450;
	}
	else if (m_baudrate == "PCI_500K")
	{
		dcb.BaudRate = 56000;
	}
	else
	{
		// error
		dcb.BaudRate = 0;
		AfxMessageBox(_T("Error Reset Serial Baud Rate = 0"));
	}

	if (m_parity == "None")
	{
		dcb.Parity = NOPARITY;
	}
	else if (m_parity == "Even")
	{
		dcb.Parity = EVENPARITY;
	}
	else if (m_parity == "Odd")
	{
		dcb.Parity = ODDPARITY;
	}
	else
	{
		dcb.Parity = 0;
		AfxMessageBox(_T("Error Reset Serial Parity = 0"));
	}

	if (m_dataBit == "7 Bit")
	{
		dcb.ByteSize = 7;
	}
	else if (m_dataBit == "8 Bit")
	{
		dcb.ByteSize = 8;
	}
	else
	{
		dcb.ByteSize = 0;
		AfxMessageBox(_T("Error Reset Serial ByteSize = 0"));
	}

	if (m_stopBit == "1 Bit")
	{
		dcb.StopBits = ONESTOPBIT;
	}
	else if(m_stopBit == "1.5 Bit")
	{
		dcb.StopBits = ONE5STOPBITS;
	}
	else if (m_stopBit == "2 Bit")
	{
		dcb.StopBits = TWOSTOPBITS;
	}
	else
	{
		dcb.StopBits = 0;
		AfxMessageBox(_T("Error Reset Serial StopBits = 0"));
	}

	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fOutxCtsFlow = FALSE;

	if (m_flowCheck)
	{
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.XonLim = 2048;
		dcb.XoffLim = 1024;
	}
	else
	{
		dcb.fOutxCtsFlow = TRUE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	}

	SetCommState(m_hComDev, &dcb);
	SetCommMask(m_hComDev, EV_RXCHAR);

}


UINT CMyComm::CommThread(LPVOID lpData)
{
	extern short g_nRemoteStatus;
	DWORD ErrorFlag;
	COMSTAT ComStat;
	DWORD EvtMask;
	char buf[MAXBUF];
	DWORD length;
	int size;
	int insize = 0;

	CMyComm* Comm = (CMyComm*)lpData;

	// serial open 동안 스레드 실행 구문
	while (Comm->m_isOpen)
	{
		EvtMask = 0;
		length = 0;
		insize = 0;
		memset(buf, '\0', MAXBUF);
		WaitCommEvent(Comm->m_hComDev, &EvtMask, NULL);
		ClearCommError(Comm->m_hComDev, &ErrorFlag, &ComStat);

		if ((EvtMask & EV_RXCHAR) && ComStat.cbInQue)
		{
			if (ComStat.cbInQue > MAXBUF)
			{
				size = MAXBUF;
			}
			else
			{
				size = ComStat.cbInQue;
			}

			do 
			{
				ClearCommError(Comm->m_hComDev, &ErrorFlag, &ComStat);
				if (!ReadFile(Comm->m_hComDev, buf + insize, size, &length, &(Comm->m_OLR)))
				{
					TRACE("ERROR in ReadFile\n");
					if (GetLastError() == ERROR_IO_PENDING)
					{
						if (WaitForSingleObject(Comm->m_OLR.hEvent, 1000) != WAIT_OBJECT_0)
						{
							length = 0;
						}
						else
						{
							GetOverlappedResult(Comm->m_hComDev, &(Comm->m_OLR), &length, FALSE);
						}
						// end if
					}
					else
					{
						length = 0;
					}
					// end if

				}
				else
				{
				}
				insize = insize + length;
			} while ((length != 0) && (insize < size));

			ClearCommError(Comm->m_hComDev, &ErrorFlag, &ComStat);

			if (Comm->m_length + insize > MAXBUF * 2)
			{
				insize = (Comm->m_length + insize) - MAXBUF * 2;
			}
			else
			{
				//AfxMessageBox(_T("Error Thread Comm->m_length + insize > MAXBUF * 2 Value ERROR"));
			}

			Comm->m_pEvent->ResetEvent();
			memcpy(Comm->m_buf + Comm->m_length, buf, insize);
			Comm->m_length = Comm->m_length + insize;
			Comm->m_pEvent->SetEvent();
			LPARAM temp = (LPARAM)Comm;
			SendMessage(Comm->m_hWnd, WM_MYRECEIVE, Comm->m_length, temp);
		}
		else
		{
			// relative with "if ((EvtMask & EV_RXCHAR) && ComStat.cbInQue)"
			AfxMessageBox(_T("Error Thread Check Que"));
		}
	}
	// Thread while end

	PurgeComm(Comm->m_hComDev, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	LPARAM temp = (LPARAM)Comm;
	SendMessage(Comm->m_hWnd, WM_MYCLOSE, 0, temp);

	return 0;
	// end connection
}
