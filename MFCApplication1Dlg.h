
// MFCApplication1Dlg.h: 헤더 파일
//

#pragma once

#include "CMyComm.h"


// CMFCSerialTerminalDlg 대화 상자
class CMFCSerialTerminalDlg : public CDialog
{
// 생성입니다.
public:
	CMFCSerialTerminalDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox ListRecvData;
	CString listStr;
	// Send data control value of edit
	CEdit editSendData;
	CString editStr;
	CComboBox comboComport;
	CComboBox comboBaud;
	CString strComport;
	CString strBaud;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnCbnDropdownComboComport();

	// 추가 한 내용
	CString str_list;
	CMyComm* m_comm;
	LRESULT OnThreadClose(WPARAM length, LPARAM lpara);
	LRESULT OnReceive(WPARAM length, LPARAM lpara);

	BOOL comportState;
//	afx_msg void OnCbnSelchangeComboBaudrate();
	afx_msg void OnCbnSelchangeComboComport();
	afx_msg void OnCbnSelchangeComboBaudrate();
};
