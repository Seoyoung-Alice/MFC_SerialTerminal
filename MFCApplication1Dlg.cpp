
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCSerialTerminalDlg 대화 상자



CMFCSerialTerminalDlg::CMFCSerialTerminalDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MFCAPPLICATION1_DIALOG, pParent)
	, listStr(_T(""))
	, editStr(_T(""))
	, strComport(_T(""))
	, strBaud(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	comportState = true;
}

void CMFCSerialTerminalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RECVDATA, ListRecvData);
	DDX_LBString(pDX, IDC_LIST_RECVDATA, listStr);
	DDX_Control(pDX, IDC_EDIT_SEND, editSendData);
	DDX_Text(pDX, IDC_EDIT_SEND, editStr);
	DDX_Control(pDX, IDC_COMBO_COMPORT, comboComport);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, comboBaud);
	DDX_CBString(pDX, IDC_COMBO_COMPORT, strComport);
	DDX_CBString(pDX, IDC_COMBO_BAUDRATE, strBaud);
}

BEGIN_MESSAGE_MAP(CMFCSerialTerminalDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CMFCSerialTerminalDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CMFCSerialTerminalDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMFCSerialTerminalDlg::OnBnClickedButtonSend)
	ON_CBN_DROPDOWN(IDC_COMBO_COMPORT, &CMFCSerialTerminalDlg::OnCbnDropdownComboComport)
	//ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, &CMFCSerialTerminalDlg::OnCbnSelchangeComboBaudrate)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, &CMFCSerialTerminalDlg::OnCbnSelchangeComboComport)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, &CMFCSerialTerminalDlg::OnCbnSelchangeComboBaudrate)

	// USER WM
	ON_MESSAGE(WM_MYRECEIVE, &CMFCSerialTerminalDlg::OnReceive)
	ON_MESSAGE(WM_MYCLOSE, &CMFCSerialTerminalDlg::OnThreadClose)

END_MESSAGE_MAP()


// CMFCSerialTerminalDlg 메시지 처리기

BOOL CMFCSerialTerminalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 데이터 초기화 작업
	comboBaud.AddString(_T("1200"));
	comboBaud.AddString(_T("2400"));
	comboBaud.AddString(_T("4800"));
	comboBaud.AddString(_T("9600"));
	comboBaud.AddString(_T("14400"));
	comboBaud.AddString(_T("38400"));
	comboBaud.AddString(_T("115200"));
	

	
	comportState = false;
	/*GetDlgItem(IDC_BUTTON_OPEN)->SetWindowTextA(_T("OPEN"));
	strComport = _T("COM10");
	strBaud = _T("115200");
	UpdateData(false);*/


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCSerialTerminalDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCSerialTerminalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCSerialTerminalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCSerialTerminalDlg::OnBnClickedButtonOpen()
{
	if (comportState)
	{
		// comport가 존재하면 실행
		if (m_comm)
		{
			m_comm->Close();
			m_comm = NULL;
			AfxMessageBox(_T("COM PORT CLOSE"));
			comportState = false;
			GetDlgItem(IDC_BUTTON_OPEN)->SetWindowTextA(_T("OPEN"));
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(false);
		}
		else
		{
			// comport exist
			//AfxMessageBox(_T("OPEN ERROR"));
		}
	}
	else
	{
		// serial initial 
		m_comm = new CMyComm(_T("\\\\.\\") + strComport, strBaud, _T("None"), _T("8 Bit"), _T("1 Bit"));
		if (m_comm->Create(GetSafeHwnd()) != 0)
		{
			AfxMessageBox(_T("COM PORT OPEN"));
			comportState = true;
			GetDlgItem(IDC_BUTTON_OPEN)->SetWindowTextA(_T("CLOSE"));
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(true);
		}
		else
		{
			AfxMessageBox(_T("ERROR SERIAL TERMINAL"));
		}
	}
}


// message list box clear
void CMFCSerialTerminalDlg::OnBnClickedButtonClear()
{
	while (ListRecvData.GetCount() > 0)
	{
		// 한줄 씩 없어짐
		ListRecvData.DeleteString(0);
	}

	str_list.Format(_T("message box clear!"));
	AfxMessageBox(str_list);
}


void CMFCSerialTerminalDlg::OnBnClickedButtonSend()
{
	CString str;
	GetDlgItem(IDC_EDIT_SEND)->GetWindowTextA(str);
	str = str + "\r\n";
	m_comm->Send(str, str.GetLength());
}


// 컨트롤 박스 드롭다운 될 경우에 실행
// COM PORT 레지스터 값 자동으로 잃어와 리스트 출력
// 출력 예시 : COM 3, COM 4
void CMFCSerialTerminalDlg::OnCbnDropdownComboComport()
{
	HKEY hKey;	// 레지스터 키값 핸들

	RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), &hKey);

	TCHAR szData[20], szName[100];
	DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;
	
	// 콤보 박스 데이터 리셋
	comboComport.ResetContent();

	memset(szData, 0x00, sizeof(szData));
	memset(szName, 0x00, sizeof(szName));

	while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL))
	{
		index++;

		RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);
		comboComport.AddString(CString(szData));

		memset(szData, 0x00, sizeof(szData));
		memset(szName, 0x00, sizeof(szName));

		dwSize = 100;
		dwSize2 = 20;
	}

	RegCloseKey(hKey);
}


LRESULT CMFCSerialTerminalDlg::OnThreadClose(WPARAM length, LPARAM lpara)
{
	((CMyComm*)lpara)->HandleClose();
	delete ((CMyComm*)lpara);

	return 0;
}


LRESULT CMFCSerialTerminalDlg::OnReceive(WPARAM length, LPARAM lpara)
{
	CString str;
	char data[20000];
	if (m_comm)
	{
		m_comm->Receive(data, length);
		data[length] = _T('\0');
		str = str + _T("\r\n");

		for (int i = 0; i < length; i++)
		{
			str = str + data[i];
		}

		ListRecvData.AddString(str);
		str = "";

		ListRecvData.SetCurSel(-1);
	}

	return 0;
}


//void CMFCSerialTerminalDlg::OnCbnSelchangeComboBaudrate()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}


void CMFCSerialTerminalDlg::OnCbnSelchangeComboComport()
{
	UpdateData();
}


void CMFCSerialTerminalDlg::OnCbnSelchangeComboBaudrate()
{
	UpdateData();
}
