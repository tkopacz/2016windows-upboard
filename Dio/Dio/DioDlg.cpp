// DioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Dio.h"
#include "DioDlg.h"

#include "AaeonIo.h"
#include "GpioDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PCI_INDEX		0xCF8
#define PCI_DATA		0xCFC

#define MCR_PCI_ADDR	0x800000D0
#define MDR_PCI_ADDR	0x800000D4
#define MCRX_PCI_ADDR	0x800000D8

#define CPLD_DIRREG_SIZE 34

DWORD dwPinDir = 0xFFFFFFFF;

BYTE gbPIN_PORT[40] = { 0x00, 0x00, 0xB2, 0x00, 0xB2, 0x00, 0xA8, 0xB2, 0x00, 0xB2,
						0xB2, 0xB2, 0x48, 0x00, 0x48, 0x48, 0x00, 0x48, 0x48, 0x00, 
						0x48, 0x48, 0x48, 0x48, 0x00, 0x48, 0xB2, 0xB2, 0xA8, 0x00, 
						0xA8, 0x48, 0x48, 0x00, 0xB2, 0xB2, 0x48, 0xB2, 0x00, 0xB2 };
WORD gwPIN_OFFSET[40] = {0x0000, 0x0000, 0x5400, 0x0000, 0x5418, 0x0000, 0x4830, 0x4828, 0x0000, 0x4808,
						  0x4800, 0x5C10, 0x4800, 0x0000, 0x4820, 0x4838, 0x0000, 0x4850, 0x4430, 0x0000,
						  0x4418, 0x4818, 0x4420, 0x4438, 0x0000, 0x5820, 0x5408, 0x5428, 0x4848, 0x0000,
						  0x4800, 0x4428, 0x4408, 0x0000, 0x5C30, 0x4818, 0x4830, 0x5C20, 0x0000, 0x5C38};
int gnPIN_CPLD_ORDER[40] = {-1, -1, 0, -1, 1, -1, 2, 15, -1, 16,
							3, 17, 4, -1, 5, 18, -1, 19, 6, -1, 
							7, 20, 8, 21, -1, 22, 9, 23, 10, -1,
							11, 24, 12, -1, 13, 25, 14, 26, -1, 27};
BYTE gbPIN_EN[40] = {0, 0, 1, 0, 1, 0, 1, 1, 0, 1,
					 1, 1, 1, 0, 1, 1, 0, 1, 1, 0,
					 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,
					 1, 1, 1, 0, 1, 1, 1, 1, 0, 1};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDioDlg dialog




CDioDlg::CDioDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDioDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_hDio = NULL;
	
}

void CDioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_GRP, m_CMB_GRP);
	
	DDX_Control(pDX, IDC_CMB_IO0, m_CMB_IO0);
	DDX_Control(pDX, IDC_CMB_IO1, m_CMB_IO1);
	DDX_Control(pDX, IDC_CMB_IO2, m_CMB_IO2);
	DDX_Control(pDX, IDC_CMB_IO3, m_CMB_IO3);
	DDX_Control(pDX, IDC_CMB_IO4, m_CMB_IO4);
	DDX_Control(pDX, IDC_CMB_IO5, m_CMB_IO5);
	DDX_Control(pDX, IDC_CMB_IO6, m_CMB_IO6);
	DDX_Control(pDX, IDC_CMB_IO7, m_CMB_IO7);
	DDX_Control(pDX, IDC_CMB_HL0, m_CMB_HL0);
	DDX_Control(pDX, IDC_CMB_HL1, m_CMB_HL1);
	DDX_Control(pDX, IDC_CMB_HL2, m_CMB_HL2);
	DDX_Control(pDX, IDC_CMB_HL3, m_CMB_HL3);
	DDX_Control(pDX, IDC_CMB_HL4, m_CMB_HL4);
	DDX_Control(pDX, IDC_CMB_HL5, m_CMB_HL5);
	DDX_Control(pDX, IDC_CMB_HL6, m_CMB_HL6);
	DDX_Control(pDX, IDC_CMB_HL7, m_CMB_HL7);
}

BEGIN_MESSAGE_MAP(CDioDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SET, &CDioDlg::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CDioDlg::OnBnClickedBtnRefresh)
	ON_CBN_SELCHANGE(IDC_CMB_GRP, &CDioDlg::OnCbnSelchangeCmbGrp)
	ON_BN_CLICKED(IDC_BTN_SETGPIO, &CDioDlg::OnBnClickedBtnSetgpio)
END_MESSAGE_MAP()


// CDioDlg message handlers

BOOL CDioDlg::OnInitDialog()
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
	m_CMB_GRP.AddString(L"Group1");
	m_CMB_GRP.AddString(L"Group2");
	m_CMB_GRP.AddString(L"Group3");
	m_CMB_GRP.AddString(L"Group4");
	m_CMB_GRP.AddString(L"Group5");
	m_CMB_GRP.SetCurSel(0);

	m_CMB_IO0.AddString(L"Input");
	m_CMB_IO0.AddString(L"Output");
	m_CMB_IO1.AddString(L"Input");
	m_CMB_IO1.AddString(L"Output");
	m_CMB_IO2.AddString(L"Input");
	m_CMB_IO2.AddString(L"Output");
	m_CMB_IO3.AddString(L"Input");
	m_CMB_IO3.AddString(L"Output");
	m_CMB_IO4.AddString(L"Input");
	m_CMB_IO4.AddString(L"Output");
	m_CMB_IO5.AddString(L"Input");
	m_CMB_IO5.AddString(L"Output");
	m_CMB_IO6.AddString(L"Input");
	m_CMB_IO6.AddString(L"Output");
	m_CMB_IO7.AddString(L"Input");
	m_CMB_IO7.AddString(L"Output");
	

	m_CMB_HL0.AddString(L"Low");
	m_CMB_HL0.AddString(L"High");
	m_CMB_HL1.AddString(L"Low");
	m_CMB_HL1.AddString(L"High");
	m_CMB_HL2.AddString(L"Low");
	m_CMB_HL2.AddString(L"High");
	m_CMB_HL3.AddString(L"Low");
	m_CMB_HL3.AddString(L"High");
	m_CMB_HL4.AddString(L"Low");
	m_CMB_HL4.AddString(L"High");
	m_CMB_HL5.AddString(L"Low");
	m_CMB_HL5.AddString(L"High");
	m_CMB_HL6.AddString(L"Low");
	m_CMB_HL6.AddString(L"High");
	m_CMB_HL7.AddString(L"Low");
	m_CMB_HL7.AddString(L"High");

    m_hDio = aaeonioOpen();
	if(m_hDio == NULL)
	{
		AfxMessageBox( _T("It Cannot Open Library!!"), MB_ICONSTOP );
		return FALSE;
	}

	OnCbnSelchangeCmbGrp();

	DWORD dwData = 0x00000000;
	dwData = MsgBusRead(0xB2, 0x5818);
	dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
	//dwData = dwData & 0xFFFFFFFD;
	dwData = dwData | 0x00000002;
	MsgBusWrite(0xB2, 0x5818, dwData);

	dwPinDir = GetAllDirection();

	OnBnClickedBtnSetgpio();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDioDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDioDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDioDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDioDlg::OnCancel()
{
	aaeonioClose(m_hDio);
	CDialog::OnCancel();
}

void CDioDlg::UpdateDioPinState()
{
	if( m_hDio )
	{
		DWORD dwData = 0x00000000;
		BOOL bInput = FALSE;
		BOOL bHigh = FALSE;

		for(int i = 0; i < 8; i++)
		{
			if(arrbPinEn[i] != 0)
			{
				dwData = MsgBusRead(arrbPort[i], arrwOffset[i]);

				bInput = (dwData & 0x00000700) == 0x00000200;

				if(bInput)
				{
					bHigh = (dwData & 0x00000001) == 0x00000001;
					SetDlgItemText(IDC_IO1+i, _T("INPUT"));
				}
				else
				{
					bHigh = (dwData & 0x00000002) == 0x00000002;
					SetDlgItemText(IDC_IO1+i, _T("OUTPUT"));
				}

				if(bHigh)
					SetDlgItemText(IDC_LH1+i, _T("HIGH"));
				else
					SetDlgItemText(IDC_LH1+i, _T("LOW"));
			}
			else
			{
				SetDlgItemText(IDC_IO1+i, _T("N/A"));
				SetDlgItemText(IDC_LH1+i, _T("N/A"));
			}
		}
	}
}

void CDioDlg::SetDioPinState()
{
	if( m_hDio )
	{
		DWORD dwData = 0x00000000;
		BOOL bInput = FALSE;
		BOOL bHigh = FALSE;

		for(int i = 0; i < 8; i++)
		{
			if(arrbPinEn[i] != 0)
			{
				bInput = ((CComboBox*)GetDlgItem(IDC_CMB_IO0+i))->GetCurSel() == 0;
				bHigh = ((CComboBox*)GetDlgItem(IDC_CMB_HL0+i))->GetCurSel() == 0;

				dwData = MsgBusRead(arrbPort[i], arrwOffset[i]);

				if(bInput)
				{
					dwData = (dwData & 0xFFFFF8FF) | 0x00000200;
					dwPinDir = dwPinDir | (0x01 << arrbCpldOrder[i]);
				}
				else
				{
					dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
					dwPinDir = dwPinDir & ~(0x01 << arrbCpldOrder[i]);

					if(bHigh)
					{
						dwData = dwData | 0x00000002;
					}
					else
					{
						dwData = dwData & 0xFFFFFFFD;
					}
				}
				MsgBusWrite(arrbPort[i], arrwOffset[i], dwData);
			}
		}
		SetCpldDirection();
	}
}

void CDioDlg::OnBnClickedBtnSet()
{
	SetDioPinState();
	OnBnClickedBtnRefresh();
}

void CDioDlg::OnBnClickedBtnRefresh()
{
	UpdateDioPinState();
}

void CDioDlg::OnCbnSelchangeCmbGrp()
{
	CString s;

	if(((CComboBox*)GetDlgItem(IDC_CMB_GRP))->GetCurSel() == 0)
	{		
		for(int i = 0; i < 8; i++)
		{
			arrbPort[i] = gbPIN_PORT[i];
			arrwOffset[i] = gwPIN_OFFSET[i];
			arrbCpldOrder[i] = gnPIN_CPLD_ORDER[i];
			arrbPinEn[i] = gbPIN_EN[i];

			s.Format(_T("Pin%d"), i + 1);

			SetDlgItemText(IDC_Pin0 + i, s);
			SetDlgItemText(IDC_DPin0 + i, s);
		}
	}
	else if(((CComboBox*)GetDlgItem(IDC_CMB_GRP))->GetCurSel() == 1)
	{
		for(int i = 0; i < 8; i++)
		{
			arrbPort[i] = gbPIN_PORT[i + 8];
			arrwOffset[i] = gwPIN_OFFSET[i + 8];
			arrbCpldOrder[i] = gnPIN_CPLD_ORDER[i + 8];
			arrbPinEn[i] = gbPIN_EN[i + 8];

			s.Format(_T("Pin%d"), i + 9);

			SetDlgItemText(IDC_Pin0 + i, s);
			SetDlgItemText(IDC_DPin0 + i, s);
		}
	}
	else if(((CComboBox*)GetDlgItem(IDC_CMB_GRP))->GetCurSel() == 2)
	{
		for(int i = 0; i < 8; i++)
		{
			arrbPort[i] = gbPIN_PORT[i + 16];
			arrwOffset[i] = gwPIN_OFFSET[i + 16];
			arrbCpldOrder[i] = gnPIN_CPLD_ORDER[i + 16];
			arrbPinEn[i] = gbPIN_EN[i + 16];

			s.Format(_T("Pin%d"), i + 17);

			SetDlgItemText(IDC_Pin0 + i, s);
			SetDlgItemText(IDC_DPin0 + i, s);
		}
	}
	else if(((CComboBox*)GetDlgItem(IDC_CMB_GRP))->GetCurSel() == 3)
	{
		for(int i = 0; i < 8; i++)
		{
			arrbPort[i] = gbPIN_PORT[i + 24];
			arrwOffset[i] = gwPIN_OFFSET[i + 24];
			arrbCpldOrder[i] = gnPIN_CPLD_ORDER[i + 24];
			arrbPinEn[i] = gbPIN_EN[i + 24];

			s.Format(_T("Pin%d"), i + 25);

			SetDlgItemText(IDC_Pin0 + i, s);
			SetDlgItemText(IDC_DPin0 + i, s);
		}
	}
	else if(((CComboBox*)GetDlgItem(IDC_CMB_GRP))->GetCurSel() == 4)
	{
		for(int i = 0; i < 8; i++)
		{
			arrbPort[i] = gbPIN_PORT[i + 32];
			arrwOffset[i] = gwPIN_OFFSET[i + 32];
			arrbCpldOrder[i] = gnPIN_CPLD_ORDER[i + 32];
			arrbPinEn[i] = gbPIN_EN[i + 32];

			s.Format(_T("Pin%d"), i + 33);

			SetDlgItemText(IDC_Pin0 + i, s);
			SetDlgItemText(IDC_DPin0 + i, s);
		}
	}

	for(int i = 0; i < 8; i++)
	{
		if(arrbPinEn[i] == 0)
		{
			((CComboBox*)GetDlgItem(IDC_CMB_IO0+i))->EnableWindow(FALSE);
			((CComboBox*)GetDlgItem(IDC_CMB_HL0+i))->EnableWindow(FALSE);
		}
		else
		{
			((CComboBox*)GetDlgItem(IDC_CMB_IO0+i))->EnableWindow(TRUE);
			((CComboBox*)GetDlgItem(IDC_CMB_HL0+i))->EnableWindow(TRUE);
		}
	}

	UpdateDioPinState();
}

DWORD CDioDlg::MsgBusRead(BYTE bPort, WORD wOffset)
{
	DWORD dwMCRData = 0x00000000;

	// Write MCRX
	aaeonioWritePortDWord(m_hDio, PCI_INDEX, MCRX_PCI_ADDR);
	aaeonioWritePortDWord(m_hDio, PCI_DATA, wOffset & 0x0000FF00);

	// Write MCR
	dwMCRData = 0x06000000 | (bPort << 16) | (((BYTE)(wOffset & 0x00FF)) << 8) | 0xF0;
	aaeonioWritePortDWord(m_hDio, PCI_INDEX, MCR_PCI_ADDR);
	aaeonioWritePortDWord(m_hDio, PCI_DATA, dwMCRData);

	//CString str;
	//str.Format(_T("dwMCRData=%08X, bPort=%08X, wOffset=%04X"), dwMCRData, bPort << 16, wOffset);
	//AfxMessageBox(str, MB_OK);

	// Read from MDR
	aaeonioWritePortDWord(m_hDio, PCI_INDEX, MDR_PCI_ADDR);
	return aaeonioReadPortDWord(m_hDio, PCI_DATA);
}

VOID CDioDlg::MsgBusWrite(BYTE bPort, WORD wOffset, DWORD dwData)
{
	DWORD dwMCRData = 0x00000000;

	// Write MCRX
	aaeonioWritePortDWord(m_hDio, PCI_INDEX, MCRX_PCI_ADDR);
	aaeonioWritePortDWord(m_hDio, PCI_DATA, wOffset & 0x0000FF00);

	// Write MDR
	aaeonioWritePortDWord(m_hDio, PCI_INDEX, MDR_PCI_ADDR);
	aaeonioWritePortDWord(m_hDio, PCI_DATA, dwData);

	// Write MCR
	dwMCRData = 0x07000000 | (bPort << 16) | (((BYTE)(wOffset & 0x00FF)) << 8) | 0xF0;
	aaeonioWritePortDWord(m_hDio, PCI_INDEX, MCR_PCI_ADDR);
	aaeonioWritePortDWord(m_hDio, PCI_DATA, dwMCRData);
}

DWORD CDioDlg::GetAllDirection()
{
	DWORD dwDir = 0xFFFFFFFF;
	BOOL bInput = FALSE;

	for(int i = 0; i < 40; i++)
	{
		dwDir = MsgBusRead(gbPIN_PORT[i], gwPIN_OFFSET[i]);

		if(gbPIN_PORT[i] != 0 && gwPIN_OFFSET[i] != 0 && gnPIN_CPLD_ORDER[i] != -1)
		{
			bInput = (dwDir & 0x00000700) == 0x00000200;

			if(bInput)
			{
				dwDir = dwDir | (0x01 << gnPIN_CPLD_ORDER[i]);
			}
			else
			{
				dwDir = dwDir & ~(0x01 << gnPIN_CPLD_ORDER[i]);
			}
		}
	}

	return dwDir;
}

BOOL CDioDlg::SetCpldDirection()
{
	DWORD dwData = 0x00000000;
	DWORD dwTempDir = 0xFFFFFFFF;
	BOOL ret = TRUE;

	/* Clear the CPLD*/
	dwData = MsgBusRead(0xA8, 0x4818);
	dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
	dwData = dwData & 0xFFFFFFFD;
	MsgBusWrite(0xA8, 0x4818, dwData);
	Sleep(1);
	dwData = dwData | 0x00000002;
	MsgBusWrite(0xA8, 0x4818, dwData);
	Sleep(1);
	
	/* Reset the CPLD internal counters */
	dwData = MsgBusRead(0x13, 0x4840);
	dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
	dwData = dwData & 0xFFFFFFFD;
	MsgBusWrite(0x13, 0x4840, dwData);
	Sleep(1);
	dwData = dwData | 0x00000002;
	MsgBusWrite(0x13, 0x4840, dwData);
	Sleep(1);

	for(int i = CPLD_DIRREG_SIZE - 1; i >= 0; i--)
	{
		// Strobe output Low
		dwData = MsgBusRead(0x13, 0x4860);
		dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
		dwData = dwData & 0xFFFFFFFD;
		MsgBusWrite(0x13, 0x4860, dwData);

		// Data in output low
		dwData = MsgBusRead(0xA8, 0x4808);
		dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
		if(i <=33 && i >= 28)
		{
			dwData = dwData & 0xFFFFFFFD;
		}
		else
		{
			if(((dwPinDir >> i) & 0x01) == 0x01)	// Input
			{
				dwData = dwData | 0x00000002;
			}
			else									// Output
			{
				dwData = dwData & 0xFFFFFFFD;
			}
		}
		//dwData = dwData & 0xFFFFFFFD;
		MsgBusWrite(0xA8, 0x4808, dwData);

		// Strobe output High
		dwData = MsgBusRead(0x13, 0x4860);
		dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
		dwData = dwData | 0x00000002;
		MsgBusWrite(0x13, 0x4860, dwData);
	}

	for(int i = CPLD_DIRREG_SIZE - 1; i >= 0; i--)
	{
		dwData = MsgBusRead(0x13, 0x4860);
		dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
		dwData = dwData & 0xFFFFFFFD;
		MsgBusWrite(0x13, 0x4860, dwData);
		dwData = dwData | 0x00000002;
		MsgBusWrite(0x13, 0x4860, dwData);

		//dwData = MsgBusRead(0xA8, 0x4858);
		//dwData = (dwData & 0xFFFFF8FF) | 0x00000200;
		//if((dwData & 0x00000001) == 0x00000001)
		//	dwTempDir = dwTempDir | (0x01 << i);
		//else
		//	dwTempDir = dwTempDir & ~(0x01 << i);
	}

	/* Verify that the CPLD dir register was written successfully
	 * In some hardware revisions, data_out_gpio isn't actually
	 * connected so we skip this step if do_verify is not set
	 */
	//if(dwTempDir != dwPinDir)
	//{
	//	ret = FALSE;
	//}

	/* Issue a dummy STB cycle to latch the dir register updates */
	dwData = MsgBusRead(0x13, 0x4860);
	dwData = (dwData & 0xFFFFF8FF) | 0x00000100;
	dwData = dwData & 0xFFFFFFFD;
	MsgBusWrite(0x13, 0x4860, dwData);
	Sleep(1);
	dwData = dwData | 0x00000002;
	MsgBusWrite(0x13, 0x4860, dwData);

	//CString str;
	//str.Format(_T("dwPinDir=%08X"), dwPinDir);
	//AfxMessageBox(str, MB_OK);

	//str.Format(_T("dwTempDir=%08X"), dwTempDir);
	//AfxMessageBox(str, MB_OK);

	return ret;
}

void CDioDlg::OnBnClickedBtnSetgpio()
{
	DWORD dwData = 0x00000000;

	for(int i = 0; i < 40; i++)
	{
		if(gbPIN_PORT[i] != 0 && gwPIN_OFFSET[i] != 0 && gnPIN_CPLD_ORDER[i] != -1)
		{
			dwData = MsgBusRead(gbPIN_PORT[i], gwPIN_OFFSET[i]);
			dwData = (dwData & 0xFF00FFFF) | 0x00908000;
			MsgBusWrite(gbPIN_PORT[i], gwPIN_OFFSET[i], dwData);
			
			// Disable TX Invert and enable open drain
			dwData = MsgBusRead(gbPIN_PORT[i], gwPIN_OFFSET[i] + 4);
			dwData = (dwData & 0xFFFFFF00) | 0x00000008;
			MsgBusWrite(gbPIN_PORT[i], gwPIN_OFFSET[i] + 4, dwData);
		}
	}
	
	// Pin 7
	dwData = MsgBusRead(gbPIN_PORT[6], gwPIN_OFFSET[6]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00908000;
	MsgBusWrite(gbPIN_PORT[6], gwPIN_OFFSET[6], dwData);

	// Pin 19
	dwData = MsgBusRead(gbPIN_PORT[18], gwPIN_OFFSET[18]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00808000;
	MsgBusWrite(gbPIN_PORT[18], gwPIN_OFFSET[18], dwData);

	// Pin 21
	dwData = MsgBusRead(gbPIN_PORT[20], gwPIN_OFFSET[20]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00808000;
	MsgBusWrite(gbPIN_PORT[20], gwPIN_OFFSET[20], dwData);

	// Pin 23
	dwData = MsgBusRead(gbPIN_PORT[22], gwPIN_OFFSET[22]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00808000;
	MsgBusWrite(gbPIN_PORT[22], gwPIN_OFFSET[22], dwData);

	// Pin 24
	dwData = MsgBusRead(gbPIN_PORT[23], gwPIN_OFFSET[23]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00808000;
	MsgBusWrite(gbPIN_PORT[23], gwPIN_OFFSET[23], dwData);

	// Pin 27
	dwData = MsgBusRead(gbPIN_PORT[26], 0x540C);
	dwData = (dwData & 0xFFFFFF0F);
	MsgBusWrite(gbPIN_PORT[26], 0x540C, dwData);

	// Pin 29
	dwData = MsgBusRead(gbPIN_PORT[28], gwPIN_OFFSET[28]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00808000;
	MsgBusWrite(gbPIN_PORT[28], gwPIN_OFFSET[28], dwData);

	// Pin 31
	dwData = MsgBusRead(gbPIN_PORT[30], gwPIN_OFFSET[30]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00808000;
	MsgBusWrite(gbPIN_PORT[30], gwPIN_OFFSET[30], dwData);

	// Pin 33
	dwData = MsgBusRead(gbPIN_PORT[32], gwPIN_OFFSET[32]);
	dwData = (dwData & 0xFF0FFFFF) | 0x00808000;
	MsgBusWrite(gbPIN_PORT[32], gwPIN_OFFSET[32], dwData);
	
}
