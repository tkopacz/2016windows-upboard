// DioDlg.h : header file
//



#include "afxwin.h"

#pragma once


// CDioDlg dialog
class CDioDlg : public CDialog
{
// Construction
public:
	CDioDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DIO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    HBITMAP m_hBitmap;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	HANDLE m_hDio;
	BYTE arrbPort[8];
	WORD arrwOffset[8];
	BYTE arrbCpldOrder[8];
	BYTE arrbPinEn[8];

	CComboBox	m_CMB_GRP;

	CComboBox	m_CMB_IO0;
	CComboBox	m_CMB_IO1;
	CComboBox	m_CMB_IO2;
	CComboBox	m_CMB_IO3;
	CComboBox	m_CMB_IO4;
	CComboBox	m_CMB_IO5;
	CComboBox	m_CMB_IO6;
	CComboBox	m_CMB_IO7;
	CComboBox	m_CMB_HL0;
	CComboBox	m_CMB_HL1;
	CComboBox	m_CMB_HL2;
	CComboBox	m_CMB_HL3;
	CComboBox	m_CMB_HL4;
	CComboBox	m_CMB_HL5;
	CComboBox	m_CMB_HL6;
	CComboBox	m_CMB_HL7;

	afx_msg void OnCancel();
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnCbnSelchangeCmbGrp();

	void UpdateDioPinState();
	void SetDioPinState();

	DWORD MsgBusRead(BYTE bPort, WORD wOffset);
	VOID MsgBusWrite(BYTE bPort, WORD wOffset, DWORD dwData);
	DWORD GetAllDirection();
	BOOL SetCpldDirection();
	afx_msg void OnBnClickedBtnSetgpio();
};
