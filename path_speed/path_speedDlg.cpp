
// path_speedDlg.cpp: 實作檔案
//

#include "pch.h"
#include "framework.h"
#include "path_speed.h"
#include "path_speedDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
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


// CpathspeedDlg 對話方塊


CpathspeedDlg::CpathspeedDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PATH_SPEED_DIALOG, pParent)
	
	, m_dSampleTime(1.0)
	, m_dThetaMax(10.0)
	, m_dPlotXmin(0.0)
	, m_dPlotXmax(0.0)
	, m_dPlotYmin(0.0)
	, m_dPlotYmax(0.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpathspeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dSampleTime);
	DDX_Text(pDX, IDC_EDIT4, m_dThetaMax);
	DDX_Control(pDX, IDC_COMBO_PLOT_TYPE, m_cbPlotType);
	DDX_Text(pDX, IDC_EDIT_X_MIN, m_dPlotXmin);
	DDX_Text(pDX, IDC_EDIT_X_MAX, m_dPlotXmax);
	DDX_Text(pDX, IDC_EDIT_Y_MIN, m_dPlotYmin);
	DDX_Text(pDX, IDC_EDIT_Y_MAX, m_dPlotYmax);
}

BEGIN_MESSAGE_MAP(CpathspeedDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CpathspeedDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CpathspeedDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CpathspeedDlg::OnBnClickedButtonSelectCommand)
	ON_BN_CLICKED(IDC_BUTTON2, &CpathspeedDlg::OnBnClickedButtonCaculate)
	ON_BN_CLICKED(IDC_BUTTON_PLOT, &CpathspeedDlg::OnBnClickedButtonPlot)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM, &CpathspeedDlg::OnBnClickedButtonZoom)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PLOT, &CpathspeedDlg::OnBnClickedButtonSelectPlot)
	ON_BN_CLICKED(IDC_BUTTON_SIMULATION, &CpathspeedDlg::OnBnClickedButtonSimulation)
	ON_CBN_SELCHANGE(IDC_COMBO_PLOT_TYPE, &CpathspeedDlg::OnCbnSelchangeComboPlotType)
//	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CpathspeedDlg 訊息處理常式

BOOL CpathspeedDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
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

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	m_bPlotFlag = FALSE;
	m_bZoomFlag = FALSE;
	m_bSimulationFlag = FALSE;
	m_bPlotVflag = FALSE;
	m_bPlotAflag = FALSE;
	GetDlgItem(IDC_CHART)->GetWindowRect(&m_rectPlotSpace);
	ScreenToClient(&m_rectPlotSpace);
	m_rectChartSpace.left = m_rectPlotSpace.left + PLOTGAP;
	m_rectChartSpace.top = m_rectPlotSpace.top + PLOTTOPGAP;
	m_rectChartSpace.bottom = m_rectPlotSpace.bottom - PLOTBOTTOMGAP;
	m_rectChartSpace.right = m_rectChartSpace.bottom - m_rectChartSpace.top + m_rectChartSpace.left;
	m_cbPlotType.SetCurSel(PLOTXY);
	m_cInputPathName = _T("");
	SelectFile(); // defult outputfile and tmp file path
	m_cPlotPathName = _T("");
	m_dDistance = 0.0; // 總移動距離, Vs=0, Vend =0, L=L1+L2+...+Ln
	m_dMaxOutX = 0.0;
	m_dMaxOutY = 0.0;
	m_dMaxOutV = 0.0;
	m_dMinOutX = 0.0;
	m_dMinOutY = 0.0;
	m_dMaxOutVx = 0.0;
	m_dMaxOutVy = 0.0;
	m_dMinOutVx = 0.0;
	m_dMinOutVy = 0.0;
	m_dMaxOutA = 0.0;
	m_dMinOutA = 0.0;
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CpathspeedDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CpathspeedDlg::OnPaint()
{	
	if (m_bPlotFlag)
	{
		CPaintDC dc(this);
		dc.BitBlt(m_rectPlotSpace.left, m_rectPlotSpace.top, m_rectPlotSpace.right, m_rectPlotSpace.bottom, &m_dcMem, m_rectPlotSpace.left, m_rectPlotSpace.top, SRCCOPY);
	}
	
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CpathspeedDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CpathspeedDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CDialogEx::OnOK();
}


void CpathspeedDlg::OnBnClickedCancel()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CDialogEx::OnCancel();
}


void CpathspeedDlg::GetNewDCMem()
{
	m_dcMem.DeleteDC();
	m_bmp.DeleteObject();
	CClientDC dc(this);
	m_dcMem.CreateCompatibleDC(&dc);
	m_bmp.CreateCompatibleBitmap(&dc, m_rectPlotSpace.right, m_rectPlotSpace.bottom);
	m_dcMem.SelectObject(&m_bmp);
	m_dcMem.FillSolidRect(m_rectPlotSpace, RGB(255, 255, 255));
	m_dcMem.Rectangle(m_rectChartSpace);
}


void CpathspeedDlg::SelectInputFile(CString& pathName)
{
	TCHAR szFilters[] = _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("txt"), _T("*.txt"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		pathName = fileDlg.GetPathName();
	}
}


void CpathspeedDlg::SelectPlotType()
{
	int iPlotType = m_cbPlotType.GetCurSel();
	double dTmin = 0;
	double dVmin = 0;

	switch (iPlotType)
	{
	case(PLOTXY):
	{
		m_bPlotVflag = FALSE;
		m_bPlotAflag = FALSE;
		m_iXlabel = 1;
		m_iYlabel = 2;
		m_dPlotXmin = m_dResultData[3]; // Xmin
		m_dPlotXmax = m_dResultData[2]; // Xmax
		m_dPlotYmin = m_dResultData[5]; // Ymin
		m_dPlotYmax = m_dResultData[4]; // Ymax
		m_strPlotXlabel = _T("X");
		m_strPlotYlabel = _T("Y");
		break;
	}

	case(PLOTXT):
	{
		m_bPlotVflag = FALSE;
		m_bPlotAflag = FALSE;
		m_iXlabel = 0;
		m_iYlabel = 1;
		m_dPlotXmin = dTmin; // Xmin
		m_dPlotXmax = m_dResultData[0]; // Xmax
		m_dPlotYmin = m_dResultData[3]; // Ymin
		m_dPlotYmax = m_dResultData[2]; // Ymax
		m_strPlotXlabel = _T("T (s)");
		m_strPlotYlabel = _T("X");
		break;
	}

	case(PLOTYT):
	{
		m_bPlotVflag = FALSE;
		m_bPlotAflag = FALSE;
		m_iXlabel = 0;
		m_iYlabel = 2;
		m_dPlotXmin = dTmin; // Xmin
		m_dPlotXmax = m_dResultData[0]; // Xmax
		m_dPlotYmin = m_dResultData[5]; // Ymin
		m_dPlotYmax = m_dResultData[4]; // Ymax
		m_strPlotXlabel = _T("T (s)");
		m_strPlotYlabel = _T("Y");
		break;
	}

	case(PLOTVXT):
	{
		m_bPlotVflag = TRUE;
		m_bPlotAflag = FALSE;
		m_iXlabel = 0;
		m_iYlabel = 1;
		m_dPlotXmin = dTmin; // Xmin
		m_dPlotXmax = m_dResultData[0]; // Xmax
		m_dPlotYmin = m_dResultData[7]; // Ymin
		m_dPlotYmax = m_dResultData[6]; // Ymax
		m_strPlotXlabel = _T("T (s)");
		m_strPlotYlabel = _T("Vx");
		break;
	}

	case(PLOTVYT):
	{
		m_bPlotVflag = TRUE;
		m_bPlotAflag = FALSE;
		m_iXlabel = 0;
		m_iYlabel = 2;
		m_dPlotXmin = dTmin; // Xmin
		m_dPlotXmax = m_dResultData[0]; // Xmax
		m_dPlotYmin = m_dResultData[9]; // Ymin
		m_dPlotYmax = m_dResultData[8]; // Ymax
		m_strPlotXlabel = _T("T (s)");
		m_strPlotYlabel = _T("Vx");
		break;
	}

	case(PLOTAXT):
	{
		m_bPlotVflag = TRUE;
		m_bPlotAflag = TRUE;
		m_iXlabel = 0;
		m_iYlabel = 1;
		m_dPlotXmin = dTmin; // Xmin
		m_dPlotXmax = m_dResultData[0]; // Xmax
		m_dPlotYmin = m_dResultData[11]; // Ymin
		m_dPlotYmax = m_dResultData[10]; // Ymax
		m_strPlotXlabel = _T("T (s)");
		m_strPlotYlabel = _T("Ax");
		break;
	}

	case(PLOTAYT):
	{
		m_bPlotVflag = TRUE;
		m_bPlotAflag = TRUE;
		m_iXlabel = 0;
		m_iYlabel = 2;
		m_dPlotXmin = dTmin; // Xmin
		m_dPlotXmax = m_dResultData[0]; // Xmax
		m_dPlotYmin = m_dResultData[11]; // Ymin
		m_dPlotYmax = m_dResultData[10]; // Ymax
		m_strPlotXlabel = _T("T (s)");
		m_strPlotYlabel = _T("Yx");
		break;
	}

	default:
	{
		MessageBox(_T("Please select the plot type!"));
		break;
	}
	}

}

void CpathspeedDlg::SelectOutputFile(CString& pathName)
{
	TCHAR szFilters[] = _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("csv"), _T("*.csv"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		pathName = fileDlg.GetPathName();
	}
}

void CpathspeedDlg::PlotGrid()
{
	CString strPlotStr;
	POINTXY PlotOrgPoint;
	double dPlotGridX, dPlotGridY;

	PlotOrgPoint.x = 1.0 * m_rectChartSpace.left;
	PlotOrgPoint.y = 1.0 * m_rectChartSpace.bottom;
	dPlotGridX = (m_dPlotXmax - m_dPlotXmin) / 5;
	dPlotGridY = (m_dPlotYmax - m_dPlotYmin) / 5;

	m_dXscale = m_rectChartSpace.Width() / (m_dPlotXmax - m_dPlotXmin);
	m_dYscale = -m_rectChartSpace.Height() / (m_dPlotYmax - m_dPlotYmin);

	if (m_dPlotXmin == m_dPlotXmax && m_dPlotYmin == m_dPlotYmax)
	{
		m_dXscale = 0;
		m_dYscale = 0;
		m_dXscaleConst = PlotOrgPoint.x + 0.5 * m_rectChartSpace.Width();
		m_dYscaleConst = PlotOrgPoint.y - 0.5 * m_rectChartSpace.Height();
		strPlotStr.Format(_T("%.2f"), m_dPlotXmin);
		m_dcMem.TextOutW(m_dXscaleConst, m_rectChartSpace.bottom + 2, strPlotStr); // X axis Min

		m_dcMem.MoveTo(m_dXscaleConst, m_rectChartSpace.bottom);
		m_dcMem.LineTo(m_dXscaleConst, m_rectChartSpace.top);

		strPlotStr.Format(_T("%.2f"), m_dPlotYmin);
		m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, m_dYscaleConst, strPlotStr); // Y axis Min

		m_dcMem.MoveTo(m_rectChartSpace.left, m_dYscaleConst);
		m_dcMem.LineTo(m_rectChartSpace.right, m_dYscaleConst);
	}
	else if (m_dPlotXmin == m_dPlotXmax)
	{
		m_dXscale = 0;
		m_dXscaleConst = PlotOrgPoint.x + 0.5 * m_rectChartSpace.Width();
		m_dYscaleConst = PlotOrgPoint.y - m_dPlotYmin * m_dYscale;
		strPlotStr.Format(_T("%.2f"), m_dPlotXmin);
		m_dcMem.TextOutW(m_dXscaleConst, m_rectChartSpace.bottom + 2, strPlotStr); // X axis Min

		m_dcMem.MoveTo(m_dXscaleConst, m_rectChartSpace.bottom);
		m_dcMem.LineTo(m_dXscaleConst, m_rectChartSpace.top);

		strPlotStr.Format(_T("%.2f"), m_dPlotYmin);
		m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, PlotOrgPoint.y, strPlotStr); // Y axis Min

		for (int i = 0; i < 4; i++)
		{
			strPlotStr.Format(_T("%.2f"), m_dPlotYmin + dPlotGridY);
			m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, PlotOrgPoint.y + dPlotGridY * m_dYscale, strPlotStr);
			m_dcMem.MoveTo(m_rectChartSpace.left, PlotOrgPoint.y + dPlotGridY * m_dYscale);
			m_dcMem.LineTo(m_rectChartSpace.right, PlotOrgPoint.y + dPlotGridY * m_dYscale);
			dPlotGridY += (m_dPlotYmax - m_dPlotYmin) / 5;
		}

		strPlotStr.Format(_T("%.2f"), m_dPlotYmax);
		m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, PlotOrgPoint.y - m_rectChartSpace.Height(), strPlotStr); // Y axis Max
	}
	else if (m_dPlotYmax == m_dPlotYmin)
	{
		m_dYscale = 0;
		m_dXscaleConst = PlotOrgPoint.x - m_dPlotXmin * m_dXscale;
		m_dYscaleConst = PlotOrgPoint.y - 0.5 * m_rectChartSpace.Height();
		strPlotStr.Format(_T("%.2f"), m_dPlotXmin);
		m_dcMem.TextOutW(PlotOrgPoint.x, m_rectChartSpace.bottom + 2, strPlotStr); // X axis Min

		for (int i = 0; i < 4; i++)
		{
			strPlotStr.Format(_T("%.2f"), m_dPlotXmin + dPlotGridX);
			m_dcMem.TextOutW(PlotOrgPoint.x + dPlotGridX * m_dXscale, m_rectChartSpace.bottom + 2, strPlotStr);
			m_dcMem.MoveTo(PlotOrgPoint.x + dPlotGridX * m_dXscale, m_rectChartSpace.bottom);
			m_dcMem.LineTo(PlotOrgPoint.x + dPlotGridX * m_dXscale, m_rectChartSpace.top);
			dPlotGridX += (m_dPlotXmax - m_dPlotXmin) / 5;
		}

		strPlotStr.Format(_T("%.2f"), m_dPlotXmax);
		m_dcMem.TextOutW(PlotOrgPoint.x + m_rectChartSpace.Width(), m_rectChartSpace.bottom + 2, strPlotStr); // X axis Max

		strPlotStr.Format(_T("%.2f"), m_dPlotYmin);
		m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, m_dYscaleConst, strPlotStr); // Y axis Min

		m_dcMem.MoveTo(m_rectChartSpace.left, m_dYscaleConst);
		m_dcMem.LineTo(m_rectChartSpace.right, m_dYscaleConst);
	}
	else
	{
		m_dXscaleConst = PlotOrgPoint.x - m_dPlotXmin * m_dXscale;
		m_dYscaleConst = PlotOrgPoint.y - m_dPlotYmin * m_dYscale;
		strPlotStr.Format(_T("%.2f"), m_dPlotXmin);
		m_dcMem.TextOutW(PlotOrgPoint.x, m_rectChartSpace.bottom + 2, strPlotStr); // X axis Min

		for (int i = 0; i < 4; i++)
		{
			strPlotStr.Format(_T("%.2f"), m_dPlotXmin + dPlotGridX);
			m_dcMem.TextOutW(PlotOrgPoint.x + dPlotGridX * m_dXscale, m_rectChartSpace.bottom + 2, strPlotStr);
			m_dcMem.MoveTo(PlotOrgPoint.x + dPlotGridX * m_dXscale, m_rectChartSpace.bottom);
			m_dcMem.LineTo(PlotOrgPoint.x + dPlotGridX * m_dXscale, m_rectChartSpace.top);
			dPlotGridX += (m_dPlotXmax - m_dPlotXmin) / 5;
		}

		strPlotStr.Format(_T("%.2f"), m_dPlotXmax);
		m_dcMem.TextOutW(PlotOrgPoint.x + m_rectChartSpace.Width(), m_rectChartSpace.bottom + 2, strPlotStr); // X axis Max

		strPlotStr.Format(_T("%.2f"), m_dPlotYmin);
		m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, PlotOrgPoint.y, strPlotStr); // Y axis Min

		for (int i = 0; i < 4; i++)
		{
			strPlotStr.Format(_T("%.2f"), m_dPlotYmin + dPlotGridY);
			m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, PlotOrgPoint.y + dPlotGridY * m_dYscale, strPlotStr);
			m_dcMem.MoveTo(m_rectChartSpace.left, PlotOrgPoint.y + dPlotGridY * m_dYscale);
			m_dcMem.LineTo(m_rectChartSpace.right, PlotOrgPoint.y + dPlotGridY * m_dYscale);
			dPlotGridY += (m_dPlotYmax - m_dPlotYmin) / 5;
		}

		strPlotStr.Format(_T("%.2f"), m_dPlotYmax);
		m_dcMem.TextOutW(m_rectChartSpace.left - PLOTYMAXMINGAP, PlotOrgPoint.y - m_rectChartSpace.Height(), strPlotStr); // Y axis Max
	}
	m_dcMem.TextOutW(m_rectChartSpace.right + PLOTXLABELGAP, m_rectChartSpace.bottom + 2, m_strPlotXlabel);
	m_dcMem.TextOutW(m_rectChartSpace.left - 15, m_rectChartSpace.top - PLOTYLABELGAP, m_strPlotYlabel);
}


void CpathspeedDlg::PlotChart(int PlotFlag)
{
	CPen* pOldPen;
	CPen penLine(PS_SOLID, 1.8, RGB(99, 184, 255));
	pOldPen = m_dcMem.SelectObject(&penLine);

	POINTXY PlotBeginPoint, PlotEndPoint;
	double dLastResultDataX = 0;
	double dLastResultDataY = 0;
	double dLastV = 0;
	double dCurrentV = 0;

	while (fscanf(fpResult, "%lf,%lf,%lf\n", &m_dResultData[0], &m_dResultData[1], &m_dResultData[2]) != EOF)
	{
		if (PlotFlag == 1)
		{
			PlotBeginPoint.x = m_dResultData[m_iXlabel] * m_dXscale + m_dXscaleConst;
			PlotBeginPoint.y = m_dResultData[m_iYlabel] * m_dYscale + m_dYscaleConst;
			if (m_bPlotVflag)
			{
				dCurrentV = 0;
				PlotBeginPoint.y = dCurrentV * m_dYscale + m_dYscaleConst;
				dLastResultDataX = m_dResultData[m_iXlabel];
				dLastResultDataY = m_dResultData[m_iYlabel];
				dLastV = dCurrentV;
			}
			m_dcMem.MoveTo(PlotBeginPoint.x, PlotBeginPoint.y);
		}
		else
		{
			PlotEndPoint.x = m_dResultData[m_iXlabel] * m_dXscale + m_dXscaleConst;
			PlotEndPoint.y = m_dResultData[m_iYlabel] * m_dYscale + m_dYscaleConst;
			if (m_bPlotVflag)
			{
				dCurrentV = (m_dResultData[m_iYlabel] - dLastResultDataY) / (m_dResultData[m_iXlabel] - dLastResultDataX);
				PlotEndPoint.y = dCurrentV * m_dYscale + m_dYscaleConst;
				if (m_bPlotAflag)
				{
					PlotEndPoint.y = ((dCurrentV - dLastV) / (m_dResultData[m_iXlabel] - dLastResultDataX)) * m_dYscale + m_dYscaleConst;
				}
				dLastResultDataX = m_dResultData[m_iXlabel];
				dLastResultDataY = m_dResultData[m_iYlabel];
				dLastV = dCurrentV;
			}
			double dEdgeXmax = m_dPlotXmax * m_dXscale + m_dXscaleConst;
			double dEdgeXmin = m_dPlotXmin * m_dXscale + m_dXscaleConst;
			double dEdgeYmax = m_dPlotYmax * m_dYscale + m_dYscaleConst;
			double dEdgeYmin = m_dPlotYmin * m_dYscale + m_dYscaleConst;
			if (PlotEndPoint.x > dEdgeXmax || PlotEndPoint.x < dEdgeXmin || PlotEndPoint.y < dEdgeYmax || PlotEndPoint.y > dEdgeYmin)
			{
				m_dcMem.MoveTo(PlotEndPoint.x, PlotEndPoint.y);
			}
			else if (PlotBeginPoint.x > dEdgeXmax || PlotBeginPoint.x < dEdgeXmin || PlotBeginPoint.y < dEdgeYmax || PlotBeginPoint.y > dEdgeYmin)
			{
				m_dcMem.MoveTo(PlotEndPoint.x, PlotEndPoint.y);
			}
			else
			{
				m_dcMem.LineTo(PlotEndPoint.x, PlotEndPoint.y);
			}
			PlotBeginPoint.x = PlotEndPoint.x;
			PlotBeginPoint.y = PlotEndPoint.y;
		}
		PlotFlag++;
	}
	fclose(fpResult);
	m_bmp.DeleteObject();
}


void CpathspeedDlg::ReadCommand(CString pathName)
{
	CFile FileCMD;
	CString strFileStr = _T("");
	char strFileChar;
	CString strFileCmd;
	int iflag; // 逗點flag
	CMD Cmd;

	m_arrCmdArray.RemoveAll();
	FileCMD.Open(pathName, CFile::modeRead);
	while (FileCMD.Read(&strFileChar, 1))
	{
		strFileStr = strFileStr + strFileChar;
		if (strFileChar == '\n')
		{
			strFileCmd = strFileStr.SpanExcluding(_T(" "));
			if (strFileCmd == "Begin")
			{
				Cmd.m_iID = BEGIN;
			}
			else if (strFileCmd == "Speed")
			{
				Cmd.m_iID = SPEED;
			}
			else if (strFileCmd == "Acc")
			{
				Cmd.m_iID = ACC;
			}
			else if (strFileCmd == "LineXY")
			{
				Cmd.m_iID = LINEXY;
			}
			else if (strFileCmd == "FLineXY")
			{
				Cmd.m_iID = FLINEXY;
			}
			else if (strFileCmd == "ArcXY")
			{
				Cmd.m_iID = ARCXY;
			}
			else if (strFileCmd == "FArcXY")
			{
				Cmd.m_iID = FARCXY;
			}
			else if (strFileCmd.SpanExcluding(_T(" ,\r\n")) == "End")
			{
				Cmd.m_iID = END;
			}
			else
			{
				MessageBox(_T("Cmd Error"));
			}
			strFileStr = strFileStr.TrimLeft(strFileCmd).TrimLeft(_T(" ,\n"));
			iflag = 0;
			while (strFileStr.IsEmpty() == 0 && iflag < sizeof(Cmd.m_dParams))
			{
				Cmd.m_dParams[iflag] = _tstof(strFileStr.SpanExcluding(_T(" ,\n")));
				strFileStr = strFileStr.TrimLeft(strFileStr.SpanExcluding(_T(" ,\n"))).TrimLeft(_T(" ,\n"));
				iflag++;
			}
			m_arrCmdArray.Add(Cmd);
			strFileStr.Empty();
		}
	}
	FileCMD.Close();
}


void CpathspeedDlg::SelectPathVAmax(int cmd, double speed[2], double acc[2])
{
	PARAMS_VA_MAX VAmax;

	if (cmd == LINEXY || cmd == ARCXY)
	{
		VAmax.m_dVmax = speed[0];
		VAmax.m_dAmax = acc[0];
		m_arrPathVAMaxArray.Add(VAmax);
	}
	else if (cmd == FLINEXY || cmd == FARCXY)
	{
		VAmax.m_dVmax = speed[1];
		VAmax.m_dAmax = acc[1];
		m_arrPathVAMaxArray.Add(VAmax);
	}
	else
	{
		// do nothing
	}
}


void CpathspeedDlg::PlotResult()
{
	GetNewDCMem();
	
	int iPlotFlag = 0;
	iPlotFlag = GetResultHead(iPlotFlag);
	SelectPlotType();
	
	if (m_bZoomFlag)
	{
		UpdateData(TRUE);
	}
	else
	{
		UpdateData(FALSE);
	}

	PlotGrid();
	PlotChart(iPlotFlag);
	
	InvalidateRect(m_rectPlotSpace, TRUE);
	UpdateWindow();
}


double CpathspeedDlg::CheckTheta(double CosTheta, double SinTheta, double Theta)
{
	if (SinTheta < 0) // 三四象限
	{
		if (CosTheta > 0) // 第四象限
		{
			Theta = 2.0 * PI - Theta;
		}
		else if (CosTheta < 0) // 第三象限
		{
			Theta = Theta + (PI - Theta);
		}
		else // y軸上
		{
			Theta = 1.5 * PI;
		}
	}
	return Theta;
}

double CpathspeedDlg::CheckMax(double a, double b)
{
	if (a < b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

double CpathspeedDlg::CheckMin(double a, double b)
{
	if (a > b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

int CpathspeedDlg::GetResultHead(int PlotFlag)
{
	//抓head,取max/min參數
	fpResult = fopen(CT2A(m_cPlotPathName), "r");
	//maxTime, maxV, maxX, minX, maxY, minY, maxVx, minVx, maxVy, minVy, maxA, minA
	if (fscanf(fpResult, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", &m_dResultData[0], &m_dResultData[1], &m_dResultData[2], &m_dResultData[3], &m_dResultData[4], &m_dResultData[5], &m_dResultData[6], &m_dResultData[7], &m_dResultData[8], &m_dResultData[9], &m_dResultData[10], &m_dResultData[11]) != EOF)
	{
		PlotFlag++;
	}
	return PlotFlag;
}

void CpathspeedDlg::SelectFile()
{
	TCHAR* path = new TCHAR[MAX_PATH];
	ZeroMemory(path, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, path);
	m_cOutTmpPathName.Format(_T("%s"), path);
	m_cOutTmpPathName.Append(_T("\\tmpResult.csv"));
	m_cOutputPathName.Format(_T("%s"), path);
	m_cOutputPathName.Append(_T("\\result.csv"));
}


double CpathspeedDlg::GetThetaPath(double Direction, double ThetaStart, double ThetaEnd)
{
	double ThetaPath;
	
	if (Direction >= 0.0) // 逆時針
	{
		if (ThetaStart == ThetaEnd) // 逆時針完整圓
		{
			ThetaPath = 2.0 * PI;
		}
		else
		{
			ThetaPath = ThetaEnd - ThetaStart;
			if (ThetaPath < 0.0)
			{
				ThetaPath = ThetaPath + 2.0 * PI;
			}

		}
	}
	else
	{
		if (ThetaStart == ThetaEnd) // 順時針完整圓
		{
			ThetaPath = 2.0 * PI;
		}
		else
		{
			ThetaPath = ThetaStart - ThetaEnd;
			if (ThetaPath < 0.0)
			{
				ThetaPath = ThetaPath + 2.0 * PI;
			}
		}
	}

	return ThetaPath;
}


void CpathspeedDlg::CalArcPoint(POINTXY BeginP, CMD Cmd, double speed[2], double acc[2])
{
	if (Cmd.m_iID == ARCXY || Cmd.m_iID == FARCXY)
	{
		POINTXY ArcEndPoint, ArcCenterPoint, ArcPoint;
		double dDirection, dRadius, dCosThetaStart, dSinThetaStart, dCosThetaEnd, dSinThetaEnd, dThetaStart, dThetaEnd, dThetaPath;
		double dTheta = 1.0 * PI / 180.0;

		ArcEndPoint.x = Cmd.m_dParams[0];
		ArcEndPoint.y = Cmd.m_dParams[1];
		ArcCenterPoint.x = Cmd.m_dParams[2];
		ArcCenterPoint.y = Cmd.m_dParams[3];
		dDirection = Cmd.m_dParams[4];
		dRadius = sqrt(pow(ArcEndPoint.x - ArcCenterPoint.x, 2) + pow(ArcEndPoint.y - ArcCenterPoint.y, 2));
		if (dRadius == 0.0)
		{
			m_arrPathPointArray.RemoveAll();
			MessageBox(_T("半徑為0"));
		}
		else
		{
			dCosThetaStart = (BeginP.x - ArcCenterPoint.x) / dRadius;
			dSinThetaStart = (BeginP.y - ArcCenterPoint.y) / dRadius;
			dThetaStart = acos((BeginP.x - ArcCenterPoint.x) / dRadius); // 範圍[0, PI]
			dThetaStart = CheckTheta(dCosThetaStart, dSinThetaStart, dThetaStart); // 轉為[0, 2*PI]
			dCosThetaEnd = (ArcEndPoint.x - ArcCenterPoint.x) / dRadius;
			dSinThetaEnd = (ArcEndPoint.y - ArcCenterPoint.y) / dRadius;
			dThetaEnd = acos((ArcEndPoint.x - ArcCenterPoint.x) / dRadius); // 範圍[0, PI]
			dThetaEnd = CheckTheta(dCosThetaEnd, dSinThetaEnd, dThetaEnd); // 轉為[0, 2*PI]
			if (dDirection >= 0.0) // 逆時針
			{
				dThetaPath = GetThetaPath(dDirection, dThetaStart, dThetaEnd);

				while (dTheta < dThetaPath)
				{
					ArcPoint.x = ArcCenterPoint.x + dRadius * cos(dThetaStart + dTheta);
					ArcPoint.y = ArcCenterPoint.y + dRadius * sin(dThetaStart + dTheta);
					dTheta += 1.0 * PI / 180.0;
					if (dTheta >= dThetaPath)
					{
						break;
					}
					m_arrPathPointArray.Add(ArcPoint);
					SelectPathVAmax(Cmd.m_iID, speed, acc);
				}

			}
			else // 順時針
			{
				dThetaPath = GetThetaPath(dDirection, dThetaStart, dThetaEnd);

				while (dTheta < dThetaPath)
				{
					ArcPoint.x = ArcCenterPoint.x + dRadius * cos(dThetaStart - dTheta);
					ArcPoint.y = ArcCenterPoint.y + dRadius * sin(dThetaStart - dTheta);
					dTheta += 1.0 * PI / 180.0;
					if (dTheta >= dThetaPath)
					{
						break;
					}
					m_arrPathPointArray.Add(ArcPoint);
					SelectPathVAmax(Cmd.m_iID, speed, acc);
				}
			}
		}
	}
	else
	{
		// do nothing
	}
}


void CpathspeedDlg::OnBnClickedButtonSelectCommand()
{
	SelectInputFile(m_cInputPathName);
	GetDlgItem(IDC_STATIC_SELT_CMD)->SetWindowTextW(m_cInputPathName);
}


void CpathspeedDlg::GetCurrentCMDinfo(int i)
{
	m_CurrentCmd = m_arrCmdArray[i];

	m_dEndPoint[0] = m_CurrentCmd.m_dParams[0];
	m_dEndPoint[1] = m_CurrentCmd.m_dParams[1];

	m_BeginPoint.x = m_dBeginPoint[0];
	m_BeginPoint.y = m_dBeginPoint[1];
	m_CurrentPoint.x = m_dEndPoint[0];
	m_CurrentPoint.y = m_dEndPoint[1];

	// 加入起點
	m_arrPathPointArray.Add(m_BeginPoint);
	m_dCurrentDistance = sqrt(pow((m_CurrentPoint.x - m_BeginPoint.x), 2) + pow((m_CurrentPoint.y - m_BeginPoint.y), 2));

	// 起終點同點處理
	if (m_dCurrentDistance == 0.0)
	{
		if (m_iCurrentCmd == LINEXY || m_iCurrentCmd == FLINEXY)
		{
			i = m_iCmdFlag;
			m_dBeginPoint[0] = m_dEndPoint[0];
			m_dBeginPoint[1] = m_dEndPoint[1];
			m_arrPathPointArray.RemoveAll();
		}
		else // Arc 正圓
		{
			// 加入路徑點與Vmax, Amax
			CalArcPoint(m_BeginPoint, m_CurrentCmd, m_dSpeed, m_dAcc);
		}
	}
	else
	{
		// 確認是否是Arc
		if (m_iCurrentCmd == ARCXY || m_iCurrentCmd == FARCXY)
		{
			CalArcPoint(m_BeginPoint, m_CurrentCmd, m_dSpeed, m_dAcc);
		}
	}

	// 加入終點
	if (m_arrPathPointArray.IsEmpty() == 0)
	{
		// 加入終點
		m_arrPathPointArray.Add(m_CurrentPoint);
		// 加入終點最大速度與加速度
		SelectPathVAmax(m_CurrentCmd.m_iID, m_dSpeed, m_dAcc);
	}
}


void CpathspeedDlg::CheckPathAngle()
{
	double dCosTheta;

	// 讀下一條命令，判斷夾角
	while (m_arrCmdArray[m_iCmdFlag + 1].m_iID == LINEXY || m_arrCmdArray[m_iCmdFlag + 1].m_iID == FLINEXY || m_arrCmdArray[m_iCmdFlag + 1].m_iID == ARCXY || m_arrCmdArray[m_iCmdFlag + 1].m_iID == FARCXY)
	{
		m_NextCmd = m_arrCmdArray[m_iCmdFlag + 1];
		m_iNextCmd = m_NextCmd.m_iID;
		m_NextPoint.x = m_NextCmd.m_dParams[0];
		m_NextPoint.y = m_NextCmd.m_dParams[1];
		m_dNextDistance = sqrt(pow((m_NextPoint.x - m_CurrentPoint.x), 2) + pow((m_NextPoint.y - m_CurrentPoint.y), 2));
		if (m_dNextDistance == 0.0)
		{
			if (m_iNextCmd == LINEXY || m_iNextCmd == FLINEXY)
			{
				m_iCmdFlag += 1;
			}
			else // Arc
			{
				if (m_NextPoint.x == m_NextCmd.m_dParams[3] && m_NextPoint.y == m_NextCmd.m_dParams[4]) // 半徑為0
				{
					m_iCmdFlag += 1;
				}
				else
				{
					m_BeginPoint.x = m_CurrentPoint.x;
					m_BeginPoint.y = m_CurrentPoint.y;
					m_CurrentPoint.x = m_NextPoint.x;
					m_CurrentPoint.y = m_NextPoint.y;

					CalArcPoint(m_BeginPoint, m_NextCmd, m_dSpeed, m_dAcc);
					m_arrPathPointArray.Add(m_NextPoint);
					SelectPathVAmax(m_iNextCmd, m_dSpeed, m_dAcc);
					m_iCmdFlag += 1;
				}
			}
		}
		else
		{
			if (m_iNextCmd == ARCXY || m_iNextCmd == FARCXY)
			{
				POINTXY ArcEndPoint, ArcCenterPoint, ArcPoint;

				double dDirection, dRadius, dCosThetaStart, dSinThetaStart, dCosThetaEnd, dSinThetaEnd, dThetaStart, dThetaEnd, dThetaPath;
				double dTheta = 1.0 * PI / 180.0;

				ArcEndPoint.x = m_NextCmd.m_dParams[0];
				ArcEndPoint.y = m_NextCmd.m_dParams[1];
				ArcCenterPoint.x = m_NextCmd.m_dParams[2];
				ArcCenterPoint.y = m_NextCmd.m_dParams[3];
				dDirection = m_NextCmd.m_dParams[4];
				dRadius = sqrt(pow(ArcEndPoint.x - ArcCenterPoint.x, 2) + pow(ArcEndPoint.y - ArcCenterPoint.y, 2));
				if (dRadius == 0.0)
				{
					break;
				}
				else
				{
					dCosThetaStart = (m_BeginPoint.x - ArcCenterPoint.x) / dRadius;
					dSinThetaStart = (m_BeginPoint.y - ArcCenterPoint.y) / dRadius;
					dThetaStart = acos((m_BeginPoint.x - ArcCenterPoint.x) / dRadius); // 範圍[0, PI]
					dThetaStart = CheckTheta(dCosThetaStart, dSinThetaStart, dThetaStart); // 轉為[0, 2*PI]
					dCosThetaEnd = (ArcEndPoint.x - ArcCenterPoint.x) / dRadius;
					dSinThetaEnd = (ArcEndPoint.y - ArcCenterPoint.y) / dRadius;
					dThetaEnd = acos((ArcEndPoint.x - ArcCenterPoint.x) / dRadius); // 範圍[0, PI]
					dThetaEnd = CheckTheta(dCosThetaEnd, dSinThetaEnd, dThetaEnd); // 轉為[0, 2*PI]

					dThetaPath = GetThetaPath(dDirection, dThetaStart, dThetaEnd);
					if (dThetaPath < dTheta)
					{
						dTheta = dThetaPath;
					}

					if (dDirection >= 0.0) // 逆時針
					{
						ArcPoint.x = ArcCenterPoint.x + dRadius * cos(dThetaStart + dTheta);
						ArcPoint.y = ArcCenterPoint.y + dRadius * sin(dThetaStart + dTheta);
					}
					else // 順時針
					{
						ArcPoint.x = ArcCenterPoint.x + dRadius * cos(dThetaStart - dTheta);
						ArcPoint.y = ArcCenterPoint.y + dRadius * sin(dThetaStart - dTheta);
					}
				}

				m_NextPoint.x = ArcPoint.x;
				m_NextPoint.y = ArcPoint.y;
			}

			dCosTheta = ((m_CurrentPoint.x - m_BeginPoint.x) * (m_NextPoint.x - m_CurrentPoint.x) + (m_CurrentPoint.y - m_BeginPoint.y) * (m_NextPoint.y - m_CurrentPoint.y)) / (m_dCurrentDistance * m_dNextDistance);

			if (dCosTheta > cos(m_dThetaMax * PI / 180.0))
			{
				m_NextPoint.x = m_NextCmd.m_dParams[0];
				m_NextPoint.y = m_NextCmd.m_dParams[1];
				m_BeginPoint.x = m_CurrentPoint.x;
				m_BeginPoint.y = m_CurrentPoint.y;
				m_CurrentPoint.x = m_NextPoint.x;
				m_CurrentPoint.y = m_NextPoint.y;
				// 更新終點
				if (m_iNextCmd == LINEXY || m_iNextCmd == FLINEXY)
				{
					m_arrPathPointArray.Add(m_NextPoint);
					SelectPathVAmax(m_iNextCmd, m_dSpeed, m_dAcc);
				}
				else // Arc
				{
					if (m_NextPoint.x == m_NextCmd.m_dParams[3] && m_NextPoint.y == m_NextCmd.m_dParams[4]) // 半徑為0
					{
						MessageBox(_T("Arc起點終點不同, 半徑不可為0"));
						break;
					}
					else
					{
						CalArcPoint(m_BeginPoint, m_NextCmd, m_dSpeed, m_dAcc);
						m_arrPathPointArray.Add(m_NextPoint);
						SelectPathVAmax(m_iNextCmd, m_dSpeed, m_dAcc);
					}
				}
				m_iCmdFlag += 1;
			}
			else
			{
				break;
			}
		}
	}
}


void CpathspeedDlg::GetPathDistance()
{
	for (int i = 0; i < m_arrPathPointArray.GetSize() - 1; i++)
	{
		m_dDistance += sqrt(pow((m_arrPathPointArray.GetAt(i + 1).x - m_arrPathPointArray.GetAt(i).x), 2) + pow((m_arrPathPointArray.GetAt(i + 1).y - m_arrPathPointArray.GetAt(i).y), 2));
	}
}


void CpathspeedDlg::GetResultMaxMin()
{
	m_dMaxOutX = CheckMax(m_dMaxOutX, m_OutPoint.x);
	m_dMaxOutY = CheckMax(m_dMaxOutY, m_OutPoint.y);
	m_dMinOutX = CheckMin(m_dMinOutX, m_OutPoint.x);
	m_dMinOutY = CheckMin(m_dMinOutY, m_OutPoint.y);
	m_dMaxOutV = CheckMax(m_dMaxOutV, m_OutVT.v);
	m_dMaxOutVx = CheckMax(m_dMaxOutVx, m_OutVxVy.x);
	m_dMinOutVx = CheckMin(m_dMinOutVx, m_OutVxVy.x);
	m_dMaxOutVy = CheckMax(m_dMaxOutVy, m_OutVxVy.y);
	m_dMinOutVy = CheckMin(m_dMinOutVy, m_OutVxVy.y);
}


void CpathspeedDlg::GetResultPath()
{
	// 開始計算線段
	double dTimeTotal = 0.0; // 總時間
	double dXRatio, dYRatio; // 換算X, Y
	double dDistanceSum;

	// 規則判斷參數
	double Vm; // 加速段與減速段交會處之速度
	double S1; // 加速段位移
	double S2; // 均速段位移
	double S3; // 減速段位移
	double T1; // 加速時間
	double T2; // 均速時間
	double T3; // 減速時間
	double S; // 位移

	double dDistanceNow = 0.0; // 目前已算完的距離
	double dNextVmax;

	m_OutVT.v = m_dVStart;
	m_OutVT.t = m_dTimeStart;

	for (int i = 0; i < m_arrPathPointArray.GetSize() - 1; i++)
	{
		m_dCurrentDistance = sqrt(pow((m_arrPathPointArray.GetAt(i + 1).x - m_arrPathPointArray.GetAt(i).x), 2) + pow((m_arrPathPointArray.GetAt(i + 1).y - m_arrPathPointArray.GetAt(i).y), 2));
		dXRatio = (m_arrPathPointArray.GetAt(i + 1).x - m_arrPathPointArray.GetAt(i).x) / m_dCurrentDistance;
		dYRatio = (m_arrPathPointArray.GetAt(i + 1).y - m_arrPathPointArray.GetAt(i).y) / m_dCurrentDistance;

		m_dVmax = m_arrPathVAMaxArray.GetAt(i).m_dVmax;
		m_dAmax = m_arrPathVAMaxArray.GetAt(i).m_dAmax;

		m_dMaxOutA = CheckMax(m_dMaxOutA, m_dAmax);
		m_dMinOutA = -m_dMaxOutA;

		dNextVmax = m_arrPathVAMaxArray.GetAt(i + 1).m_dVmax;

		dDistanceSum = 0;

		if (dNextVmax != 0.0) // 還有下一段, 因此 Vend != 0
		{
			S = m_dDistance - dDistanceNow;
			m_dVEnd = 0.0;
			S1 = (pow(m_dVmax, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
			S3 = (pow(m_dVmax, 2) - pow(m_dVEnd, 2)) / (2.0 * m_dAmax);
			S2 = S - S1 - S3;
			if (S2 <= 0.0) // 沒有均速階段, 剩下的距離之速度規劃為三角形
			{
				S1 = (2.0 * m_dAmax * S - pow(m_dVStart, 2)) / (4.0 * m_dAmax);
				if (S1 < 0.0) // 只有減速
				{
					m_dVEnd = sqrt(pow(m_dVStart, 2) - 2.0 * m_dAmax * m_dCurrentDistance);
					dTimeTotal = 2.0 * m_dCurrentDistance / (m_dVStart + m_dVEnd);
					while (dDistanceSum < m_dCurrentDistance)
					{
						dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
						m_OutVT.v -= m_dAmax * m_dt;
						if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
						{
							break;
						}
						m_OutVT.t += m_dt;
						m_dt = m_dSampleTime * pow(10, -3);
						AddOutData(dXRatio, dYRatio, dDistanceSum, i);
						GetResultMaxMin();
					}
				}
				else // 加速到Vm再減速
				{
					Vm = sqrt((2.0 * m_dAmax * S + pow(m_dVStart, 2)) / 2.0);
					S1 = (pow(Vm, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
					T1 = 2.0 * S1 / (m_dVStart + Vm);
					if (m_dCurrentDistance <= S1) // 此小線段都在加速區域
					{
						m_dVEnd = sqrt(pow(m_dVStart, 2) + 2.0 * m_dAmax * m_dCurrentDistance);
						if (m_dVEnd > dNextVmax) // Vend設為Vmax,next，重算Vm，此小線段會進減速區域
						{
							m_dVEnd = dNextVmax;
							Vm = sqrt((2.0 * m_dAmax * m_dCurrentDistance + pow(m_dVStart, 2) + pow(m_dVEnd, 2)) / 2.0);
							S1 = (pow(Vm, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
							T1 = 2.0 * S1 / (m_dVStart + Vm);
							T3 = (2.0 * (m_dCurrentDistance - S1)) / (Vm + m_dVEnd);
							dTimeTotal = T1 + T3;
							while (dDistanceSum < S1)
							{
								dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
								m_OutVT.v += m_dAmax * m_dt;
								m_OutVT.t += m_dt;
								m_dt = m_dSampleTime * pow(10, -3);
								if (dDistanceSum > S1)
								{
									dDistanceSum = dDistanceSum - (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - Vm);
									m_OutVT.v = Vm - m_dAmax * (m_OutVT.t - m_dTimeStart - T1);
								}
								AddOutData(dXRatio, dYRatio, dDistanceSum, i);
								GetResultMaxMin();
							}
							while (dDistanceSum >= S1 && dDistanceSum < m_dCurrentDistance)
							{
								dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
								m_OutVT.v -= m_dAmax * m_dt;
								if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
								{
									break;
								}
								m_OutVT.t += m_dt;
								m_dt = m_dSampleTime * pow(10, -3);
								AddOutData(dXRatio, dYRatio, dDistanceSum, i);
								GetResultMaxMin();
							}
						}
						else // 此小線段都在加速區域
						{
							dTimeTotal = 2.0 * m_dCurrentDistance / (m_dVStart + m_dVEnd);
							while (dDistanceSum < S1)
							{
								dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
								m_OutVT.v += m_dAmax * m_dt;
								if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v >= m_dVEnd)
								{
									break;
								}
								m_OutVT.t += m_dt;
								m_dt = m_dSampleTime * pow(10, -3);
								AddOutData(dXRatio, dYRatio, dDistanceSum, i);
								GetResultMaxMin();
							}
						}
					}
					else // 此小線段會進減速區域
					{
						m_dVEnd = sqrt(2.0 * m_dAmax * (S - m_dCurrentDistance));
						if (m_dVEnd > dNextVmax) // Vend設為Vmax,next，重算Vm
						{
							m_dVEnd = dNextVmax;
							Vm = sqrt((2.0 * m_dAmax * m_dCurrentDistance + pow(m_dVStart, 2) + pow(m_dVEnd, 2)) / 2.0);
							S1 = (pow(Vm, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
							T1 = 2.0 * S1 / (m_dVStart + Vm);
							T3 = (2.0 * (m_dCurrentDistance - S1)) / (Vm + m_dVEnd);
							dTimeTotal = T1 + T3;
							while (dDistanceSum < S1)
							{
								dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
								m_OutVT.v += m_dAmax * m_dt;
								m_OutVT.t += m_dt;
								m_dt = m_dSampleTime * pow(10, -3);
								if (dDistanceSum > S1)
								{
									dDistanceSum = dDistanceSum - (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - Vm);
									m_OutVT.v = Vm - m_dAmax * (m_OutVT.t - m_dTimeStart - T1);
								}
								AddOutData(dXRatio, dYRatio, dDistanceSum, i);
								GetResultMaxMin();
							}
							while (dDistanceSum >= S1 && dDistanceSum < m_dCurrentDistance)
							{
								dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
								m_OutVT.v -= m_dAmax * m_dt;
								if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
								{
									break;
								}
								m_OutVT.t += m_dt;
								m_dt = m_dSampleTime * pow(10, -3);
								AddOutData(dXRatio, dYRatio, dDistanceSum, i);
								GetResultMaxMin();
							}
						}
						else // 此小線段會進減速區域
						{
							T3 = (2.0 * (m_dCurrentDistance - S1)) / (Vm + m_dVEnd);
							dTimeTotal = T1 + T3;
							while (dDistanceSum < S1)
							{
								dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
								m_OutVT.v += m_dAmax * m_dt;
								m_OutVT.t += m_dt;
								m_dt = m_dSampleTime * pow(10, -3);
								if (dDistanceSum > S1)
								{
									dDistanceSum = dDistanceSum - (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - Vm);
									m_OutVT.v = Vm - m_dAmax * (m_OutVT.t - m_dTimeStart - T1);
								}
								AddOutData(dXRatio, dYRatio, dDistanceSum, i);
								GetResultMaxMin();
							}
							while (dDistanceSum >= S1 && dDistanceSum < m_dCurrentDistance)
							{
								dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
								m_OutVT.v -= m_dAmax * m_dt;
								if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
								{
									break;
								}
								m_OutVT.t += m_dt;
								m_dt = m_dSampleTime * pow(10, -3);
								AddOutData(dXRatio, dYRatio, dDistanceSum, i);
								GetResultMaxMin();
							}
						}
					}
				}
			}
			else // 剩下的距離之速度規劃還能是梯形
			{
				if (m_dCurrentDistance <= S1) // 此小線段都在加速區域
				{
					m_dVEnd = sqrt(pow(m_dVStart, 2) + 2.0 * m_dAmax * m_dCurrentDistance);
					if (m_dVEnd > dNextVmax) // Vend設為Vmax,next，重算Vm
					{
						m_dVEnd = dNextVmax;
						Vm = sqrt((2.0 * m_dAmax * m_dCurrentDistance + pow(m_dVStart, 2) + pow(m_dVEnd, 2)) / 2.0);
						S1 = (pow(Vm, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
						T1 = 2.0 * S1 / (m_dVStart + Vm);
						T3 = (2.0 * (m_dCurrentDistance - S1)) / (Vm + m_dVEnd);
						dTimeTotal = T1 + T3;
						while (dDistanceSum < S1)
						{
							dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v += m_dAmax * m_dt;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if (dDistanceSum > S1)
							{
								dDistanceSum = dDistanceSum - (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - Vm);
								m_OutVT.v = Vm - m_dAmax * (m_OutVT.t - m_dTimeStart - T1);
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= S1 && dDistanceSum < m_dCurrentDistance)
						{
							dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v -= m_dAmax * m_dt;
							if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
							{
								break;
							}
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
					}
					else
					{
						dTimeTotal = 2.0 * m_dCurrentDistance / (m_dVStart + m_dVEnd);
						while (dDistanceSum < S1)
						{
							dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v += m_dAmax * m_dt;
							if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v >= m_dVEnd)
							{
								break;
							}
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
					}
				}
				else if (m_dCurrentDistance > S1 && m_dCurrentDistance <= (S1 + S2)) // 此小線段會到均速區域
				{
					m_dVEnd = m_dVmax;
					if (m_dVEnd > dNextVmax) // Vend設為Vmax,next，提早開始減速
					{
						m_dVEnd = dNextVmax;
						S = m_dCurrentDistance;
						S1 = (pow(m_dVmax, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
						S3 = (pow(m_dVmax, 2) - pow(m_dVEnd, 2)) / (2.0 * m_dAmax);
						S2 = S - S1 - S3;
						T1 = (2.0 * S1) / (m_dVStart + m_dVmax);
						T2 = S2 / m_dVmax;
						T3 = (2.0 * (m_dCurrentDistance - S1 - S2)) / (m_dVmax + m_dVEnd);
						dTimeTotal = T1 + T2 + T3;
						while (dDistanceSum < S1)
						{
							dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v += m_dAmax * m_dt;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if (dDistanceSum > S1)
							{
								dDistanceSum = dDistanceSum - 0.5 * (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - m_dVmax);
								m_OutVT.v = m_dVmax;
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
						{
							dDistanceSum += m_dVmax * m_dt;
							m_OutVT.v = m_dVmax;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if ((S - dDistanceSum) < S3)
							{
								m_OutVT.v = m_dVmax - m_dAmax * (m_OutVT.t - m_dTimeStart - T1 - T2);
								dDistanceSum = dDistanceSum - 0.5 * (m_dVmax - m_OutVT.v) * (m_OutVT.t - m_dTimeStart - T1 - T2);
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= (S1 + S2) && dDistanceSum < m_dCurrentDistance)
						{
							dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v -= m_dAmax * m_dt;
							if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
							{
								break;
							}
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
					}
					else
					{
						T1 = (2.0 * S1) / (m_dVStart + m_dVmax);
						T2 = (m_dCurrentDistance - S1) / m_dVmax;
						dTimeTotal = T1 + T2;
						while (dDistanceSum < S1)
						{
							dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v += m_dAmax * m_dt;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if (dDistanceSum > S1)
							{
								dDistanceSum = dDistanceSum - 0.5 * (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - m_dVmax);
								m_OutVT.v = m_dVmax;
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= S1 && dDistanceSum < m_dCurrentDistance)
						{
							dDistanceSum += m_dVmax * m_dt;
							m_OutVT.v = m_dVmax;
							if (dDistanceSum >= m_dCurrentDistance)
							{
								break;
							}
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
					}
				}
				else // 此小線段會到減速區域
				{
					m_dVEnd = sqrt(2.0 * m_dAmax * (S - m_dCurrentDistance));
					if (m_dVEnd > dNextVmax) // Vend設為Vmax,next，提早開始減速
					{
						m_dVEnd = dNextVmax;
						S = m_dCurrentDistance;
						S1 = (pow(m_dVmax, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
						S3 = (pow(m_dVmax, 2) - pow(m_dVEnd, 2)) / (2.0 * m_dAmax);
						S2 = S - S1 - S3;
						T1 = (2.0 * S1) / (m_dVStart + m_dVmax);
						T2 = S2 / m_dVmax;
						T3 = (2.0 * (m_dCurrentDistance - S1 - S2)) / (m_dVmax + m_dVEnd);
						dTimeTotal = T1 + T2 + T3;
						while (dDistanceSum < S1)
						{
							dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v += m_dAmax * m_dt;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if (dDistanceSum > S1)
							{
								dDistanceSum = dDistanceSum - 0.5 * (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - m_dVmax);
								m_OutVT.v = m_dVmax;
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
						{
							dDistanceSum += m_dVmax * m_dt;
							m_OutVT.v = m_dVmax;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if ((S - dDistanceSum) < S3)
							{
								m_OutVT.v = m_dVmax - m_dAmax * (m_OutVT.t - m_dTimeStart - T1 - T2);
								dDistanceSum = dDistanceSum - 0.5 * (m_dVmax - m_OutVT.v) * (m_OutVT.t - m_dTimeStart - T1 - T2);
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= (S1 + S2) && dDistanceSum < m_dCurrentDistance)
						{
							dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v -= m_dAmax * m_dt;
							if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
							{
								break;
							}
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
					}
					else
					{
						T1 = (2.0 * S1) / (m_dVStart + m_dVmax);
						T2 = S2 / m_dVmax;
						T3 = (2.0 * (m_dCurrentDistance - S1 - S2)) / (m_dVmax + m_dVEnd);
						dTimeTotal = T1 + T2 + T3;
						while (dDistanceSum < S1)
						{
							dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v += m_dAmax * m_dt;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if (dDistanceSum > S1)
							{
								dDistanceSum = dDistanceSum - 0.5 * (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - m_dVmax);
								m_OutVT.v = m_dVmax;
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
						{
							dDistanceSum += m_dVmax * m_dt;
							m_OutVT.v = m_dVmax;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if ((S - dDistanceSum) < S3)
							{
								m_OutVT.v = m_dVmax - m_dAmax * (m_OutVT.t - m_dTimeStart - T1 - T2);
								dDistanceSum = dDistanceSum - 0.5 * (m_dVmax - m_OutVT.v) * (m_OutVT.t - m_dTimeStart - T1 - T2);
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= (S1 + S2) && dDistanceSum < m_dCurrentDistance)
						{
							dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v -= m_dAmax * m_dt;
							if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
							{
								break;
							}
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
					}
				}
			}
		}
		else // 沒有下一段, Vend必須為0
		{
			S = m_dCurrentDistance;
			m_dVEnd = 0.0;
			S1 = (pow(m_dVmax, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
			S3 = (pow(m_dVmax, 2) - pow(m_dVEnd, 2)) / (2.0 * m_dAmax);
			S2 = S - S1 - S3;
			if (S2 <= 0.0) // 沒有均速階段, 剩下的距離之速度規劃為三角形
			{
				S1 = (2.0 * m_dAmax * S - pow(m_dVStart, 2)) / (4.0 * m_dAmax);
				if (S1 < 0.0) // 只有減速
				{
					dTimeTotal = 2.0 * m_dCurrentDistance / (m_dVStart + m_dVEnd);
					while (dDistanceSum < m_dCurrentDistance)
					{
						dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
						m_OutVT.v -= m_dAmax * m_dt;
						if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
						{
							break;
						}
						m_OutVT.t += m_dt;
						m_dt = m_dSampleTime * pow(10, -3);
						AddOutData(dXRatio, dYRatio, dDistanceSum, i);
						GetResultMaxMin();
					}
				}
				else // 加速到Vm再減速
				{
					Vm = sqrt((2.0 * m_dAmax * S + pow(m_dVStart, 2)) / 2.0);
					S1 = (pow(Vm, 2) - pow(m_dVStart, 2)) / (2.0 * m_dAmax);
					S3 = (pow(Vm, 2) - pow(m_dVEnd, 2)) / (2.0 * m_dAmax);
					S2 = 0.0;
					T1 = 2.0 * S1 / (m_dVStart + Vm);
					T2 = 0.0;
					T3 = 2.0 * S3 / (m_dVEnd + Vm);
					dTimeTotal = T1 + T2 + T3;
					if (Vm > m_dVmax)
					{
						MessageBox(_T("Vm 不可大於 Vmax, 計算結果錯誤"));
					}
					else // 三角形
					{
						while (dDistanceSum < S1)
						{
							dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v += m_dAmax * m_dt;
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							if (dDistanceSum > S1)
							{
								dDistanceSum = dDistanceSum - (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - Vm);
								m_OutVT.v = Vm - m_dAmax * (m_OutVT.t - m_dTimeStart - T1);
							}
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
						while (dDistanceSum >= S1 && dDistanceSum < m_dCurrentDistance)
						{
							dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
							m_OutVT.v -= m_dAmax * m_dt;
							if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
							{
								break;
							}
							m_OutVT.t += m_dt;
							m_dt = m_dSampleTime * pow(10, -3);
							AddOutData(dXRatio, dYRatio, dDistanceSum, i);
							GetResultMaxMin();
						}
					}
				}
			}
			else // 剩下的距離之速度規劃還能是梯形
			{
				T1 = 2.0 * S1 / (m_dVStart + m_dVmax);
				T2 = S2 / m_dVmax;
				T3 = 2.0 * S3 / (m_dVEnd + m_dVmax);
				dTimeTotal = T1 + T2 + T3;
				while (dDistanceSum < S1)
				{
					dDistanceSum += m_OutVT.v * m_dt + 0.5 * m_dAmax * pow(m_dt, 2);
					m_OutVT.v += m_dAmax * m_dt;
					m_OutVT.t += m_dt;
					m_dt = m_dSampleTime * pow(10, -3);
					if (dDistanceSum > S1)
					{
						dDistanceSum = dDistanceSum - 0.5 * (m_OutVT.t - m_dTimeStart - T1) * (m_OutVT.v - m_dVmax);
						m_OutVT.v = m_dVmax;
					}
					AddOutData(dXRatio, dYRatio, dDistanceSum, i);
					GetResultMaxMin();
				}
				while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
				{
					dDistanceSum += m_dVmax * m_dt;
					m_OutVT.v = m_dVmax;
					m_OutVT.t += m_dt;
					m_dt = m_dSampleTime * pow(10, -3);
					if ((S - dDistanceSum) < S3)
					{
						m_OutVT.v = m_dVmax - m_dAmax * (m_OutVT.t - m_dTimeStart - T1 - T2);
						dDistanceSum = dDistanceSum - 0.5 * (m_dVmax - m_OutVT.v) * (m_OutVT.t - m_dTimeStart - T1 - T2);
					}
					AddOutData(dXRatio, dYRatio, dDistanceSum, i);
					GetResultMaxMin();
				}
				while (dDistanceSum >= (S1 + S2) && dDistanceSum < m_dCurrentDistance)
				{
					dDistanceSum += m_OutVT.v * m_dt - 0.5 * m_dAmax * pow(m_dt, 2);
					m_OutVT.v -= m_dAmax * m_dt;
					if (dDistanceSum >= m_dCurrentDistance || m_OutVT.v <= m_dVEnd)
					{
						break;
					}
					m_OutVT.t += m_dt;
					m_dt = m_dSampleTime * pow(10, -3);
					AddOutData(dXRatio, dYRatio, dDistanceSum, i);
					GetResultMaxMin();
				}
			}
		}

		// 輸出此線段最後一點的參數
		if (m_OutVT.t + m_dt == m_dTimeStart + dTimeTotal)
		{
			m_OutVT.v = m_dVEnd; // 終止速度
			m_OutVT.t = m_dTimeStart + dTimeTotal; // 終止時間
			m_OutVxVy.x = m_OutVT.v * dXRatio;
			m_OutVxVy.y = m_OutVT.v * dYRatio;
			m_OutPoint.x = m_arrPathPointArray.GetAt(i + 1).x;
			m_OutPoint.y = m_arrPathPointArray.GetAt(i + 1).y;
			m_arrOutVTArray.Add(m_OutVT);
			m_arrOutXYArray.Add(m_OutPoint);
			GetResultMaxMin();
		}
		else
		{
			m_dt = m_OutVT.t + m_dt - (m_dTimeStart + dTimeTotal);
			m_OutVT.v = m_dVEnd; // 終止速度
			m_OutVT.t = m_dTimeStart + dTimeTotal; // 終止時間
		}

		m_dTimeStart += dTimeTotal;
		m_dVStart = m_dVEnd;
		dDistanceNow += m_dCurrentDistance;
	}
}


void CpathspeedDlg::AddOutData(double XRatio, double YRatio, double DistanceSum, int i)
{
	m_OutVxVy.x = m_OutVT.v * XRatio;
	m_OutVxVy.y = m_OutVT.v * YRatio;
	m_OutPoint.x = DistanceSum * XRatio + m_arrPathPointArray.GetAt(i).x;
	m_OutPoint.y = DistanceSum * YRatio + m_arrPathPointArray.GetAt(i).y;
	m_arrOutVTArray.Add(m_OutVT);
	m_arrOutXYArray.Add(m_OutPoint);
}


void CpathspeedDlg::OnBnClickedButtonCaculate()
{
	UpdateData(TRUE);
	if(m_dSampleTime <= 0) 
	{
		MessageBox(_T("Error: Sample Time <= 0"));
	}
	else if (m_cInputPathName == _T(""))
	{
		MessageBox(_T("Please select command file!"));
	}
	else 
	{
		// Read command
		ReadCommand(m_cInputPathName);

		FILE* OutFile;
		OutFile = fopen(CT2A(m_cOutTmpPathName), "w");

		m_dTimeStart = 0.0;
		m_dVStart = 0.0;
		m_dt = m_dSampleTime * pow(10, -3);

		// Command classification
		for (int i = 0; i < m_arrCmdArray.GetSize(); i++)
		{
			
			m_iCurrentCmd = m_arrCmdArray[i].m_iID;

			switch (m_iCurrentCmd)
			{
				case BEGIN:
				{
					m_iCmdFlag = i;
					m_CurrentCmd = m_arrCmdArray[i];
					m_dBeginPoint[0] = m_CurrentCmd.m_dParams[0];
					m_dBeginPoint[1] = m_CurrentCmd.m_dParams[1];
					m_OutPoint.x = m_dBeginPoint[0];
					m_OutPoint.y = m_dBeginPoint[1];
					m_dMaxOutX = m_OutPoint.x;
					m_dMaxOutY = m_OutPoint.y;
					m_dMinOutX = m_OutPoint.x;
					m_dMinOutY = m_OutPoint.y;
					m_OutVT.t = m_dTimeStart; // 起始時間為0
					m_OutVT.v = m_dVStart; // 起始速度為0
					m_OutVxVy.x = 0;
					m_OutVxVy.y = 0;
					fprintf(OutFile, "%lf,%lf,%lf\n", m_OutVT.t, m_OutPoint.x, m_OutPoint.y);
					break;
				}
				case SPEED:
				{
					m_iCmdFlag = i;
					m_CurrentCmd = m_arrCmdArray[i];
					m_dSpeed[0] = m_CurrentCmd.m_dParams[0];
					m_dSpeed[1] = m_CurrentCmd.m_dParams[1];
					break;
				}
				case ACC:
				{
					m_iCmdFlag = i;
					m_CurrentCmd = m_arrCmdArray[i];
					m_dAcc[0] = m_CurrentCmd.m_dParams[0];
					m_dAcc[1] = m_CurrentCmd.m_dParams[1];
					break;
				}
				case END:
				{
					fclose(OutFile);
					break;
				}
				case LINEXY:
				case FLINEXY:
				case ARCXY:
				case FARCXY:
				{
					m_iCmdFlag = i;
					if (m_iCmdFlag < 3)
					{
						MessageBox(_T("條件不足，直接輸出已計算結果"));
						i = m_arrCmdArray.GetSize();
						break;
					}

					// 讀這條命令，取得這條命令的資訊，加入線段起終點
					GetCurrentCMDinfo(m_iCmdFlag);

					if (m_arrPathPointArray.IsEmpty())
					{
						break;
					}

					// 讀下一條命令，判斷夾角，符合就加入線段
					CheckPathAngle();

					// 線段結束, Vend為0
					m_VAmax.m_dVmax = 0.0;
					m_VAmax.m_dAmax = 0.0;
					m_arrPathVAMaxArray.Add(m_VAmax);

					// 計算此段總距離
					GetPathDistance();
					
					// 速度規劃
					GetResultPath();
					
					// 輸出此線段結果
					for (int i = 0; i < m_arrOutXYArray.GetSize(); i++)
					{
						fprintf(OutFile, "%lf,%lf,%lf\n", m_arrOutVTArray.GetAt(i).t, m_arrOutXYArray.GetAt(i).x, m_arrOutXYArray.GetAt(i).y);
					}

					// 計算完成, 更新參數
					m_arrOutXYArray.RemoveAll();
					m_arrOutVTArray.RemoveAll();
					
					i = m_iCmdFlag;
					m_dBeginPoint[0] = m_arrPathPointArray[m_arrPathPointArray.GetSize() - 1].x;
					m_dBeginPoint[1] = m_arrPathPointArray[m_arrPathPointArray.GetSize() - 1].y;
					m_arrPathPointArray.RemoveAll();
					m_arrPathVAMaxArray.RemoveAll();
					break;
				}
			
				default:
				{
					MessageBox(_T("計算中出現未定義的命令，計算暫停，輸出已計算結果。"));
					break;
				}
			}	
		}

		// close file
		m_arrOutXYArray.RemoveAll();
		m_arrOutVTArray.RemoveAll();
		m_arrPathPointArray.RemoveAll();
		m_arrPathVAMaxArray.RemoveAll();
		fclose(OutFile);
		OutFile = fopen(CT2A(m_cOutTmpPathName), "r");

		if (MessageBox(_T("目前結果將會儲存在: ")+ m_cOutputPathName+_T("\n若需改變存檔位置請按否"), _T("計算完成"), MB_YESNO) == IDNO)
		{
			TCHAR szFilters[] = _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
			CFileDialog fileDlg(FALSE, _T("csv"), _T("*.csv"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
			if (fileDlg.DoModal() == IDOK)
			{
				m_cOutputPathName = fileDlg.GetPathName();
			}
		}

		FILE* ResultFile;
		ResultFile = fopen(CT2A(m_cOutputPathName), "w");
		fprintf(ResultFile, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", m_dTimeStart, m_dMaxOutV, m_dMaxOutX, m_dMinOutX, m_dMaxOutY, m_dMinOutY, m_dMaxOutVx, m_dMinOutVx, m_dMaxOutVy, m_dMinOutVy, m_dMaxOutA, m_dMinOutA);
		fclose(ResultFile);
		ResultFile = fopen(CT2A(m_cOutputPathName), "a");
		char tmp[1000];
		while ((fgets(tmp, 1000, OutFile)) != NULL)
		{
			fputs(tmp, ResultFile);
		}
		fclose(ResultFile);
		fclose(OutFile);
	}
}


void CpathspeedDlg::OnBnClickedButtonPlot()
{
	if (m_cPlotPathName == _T(""))
	{
		MessageBox(_T("Please select plot file!"));
	}
	else if (m_bSimulationFlag)
	{
		KillTimer(0);
		fclose(fpResult);
		m_dcMem.DeleteDC();
		m_bmp.DeleteObject();
		m_bSimulationFlag = FALSE;
		GetDlgItem(IDC_BUTTON_SIMULATION)->SetWindowTextW(_T("simulation"));
		m_bPlotFlag = TRUE;
		m_bZoomFlag = TRUE;
		PlotResult();
		m_dcMem.DeleteDC();
		m_cSimuTime.Format(_T("%.3f"), 0.0);
		GetDlgItem(IDC_STATIC_SIMU_TIME)->SetWindowTextW(m_cSimuTime);
	}
	else
	{
		m_bPlotFlag = TRUE;
		m_bZoomFlag = FALSE;
		PlotResult();
		m_dcMem.DeleteDC();
		m_cSimuTime.Format(_T("%.3f"), 0.0);
		GetDlgItem(IDC_STATIC_SIMU_TIME)->SetWindowTextW(m_cSimuTime);
	}
}


void CpathspeedDlg::OnBnClickedButtonZoom()
{
	if (m_cPlotPathName == _T(""))
	{
		MessageBox(_T("Please select plot file!"));
	}
	else if (m_bSimulationFlag)
	{
		KillTimer(0);
		fclose(fpResult);
		m_dcMem.DeleteDC();
		m_bmp.DeleteObject();
		m_bSimulationFlag = FALSE;
		GetDlgItem(IDC_BUTTON_SIMULATION)->SetWindowTextW(_T("simulation"));
		m_bPlotFlag = TRUE;
		m_bZoomFlag = TRUE;
		PlotResult();
		m_dcMem.DeleteDC();
		m_cSimuTime.Format(_T("%.3f"), 0.0);
		GetDlgItem(IDC_STATIC_SIMU_TIME)->SetWindowTextW(m_cSimuTime);
	}
	else
	{
		m_bPlotFlag = TRUE;
		m_bZoomFlag = TRUE;
		PlotResult();
		m_dcMem.DeleteDC();
		m_cSimuTime.Format(_T("%.3f"), 0.0);
		GetDlgItem(IDC_STATIC_SIMU_TIME)->SetWindowTextW(m_cSimuTime);
	}
}


BOOL CpathspeedDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		UINT nKey = (int)pMsg->wParam;
		if (VK_RETURN == nKey)
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CpathspeedDlg::OnTimer(UINT_PTR nIDEvent)
{	
	double len;

	if (nIDEvent == 0)
	{
		m_tSimuEnd = clock();
		CClientDC dc(this);
		double dResultData[3];
		POINTXY PlotBeginPoint, PlotEndPoint;

		CPen* pOldPen;
		CPen penLine(PS_SOLID, 2, RGB(99, 184, 255));
		pOldPen = m_dcMem.SelectObject(&penLine);

		len = 1.0 * (m_tSimuEnd - m_tSimuStart) / m_dSampleTime;

		for (int i = 0; i < len; i++)
		{
			if (fscanf(fpResult, "%lf,%lf,%lf\n", &dResultData[0], &dResultData[1], &dResultData[2]) != EOF)
			{
				PlotEndPoint.x = dResultData[m_iXlabel] * m_dXscale + m_dXscaleConst;
				PlotEndPoint.y = dResultData[m_iYlabel] * m_dYscale + m_dYscaleConst;
				m_dcMem.LineTo(PlotEndPoint.x, PlotEndPoint.y);
				PlotBeginPoint.x = PlotEndPoint.x;
				PlotBeginPoint.y = PlotEndPoint.y;
			}
			else
			{
				dc.BitBlt(m_rectPlotSpace.left, m_rectPlotSpace.top, m_rectPlotSpace.right, m_rectPlotSpace.bottom, &m_dcMem, m_rectPlotSpace.left, m_rectPlotSpace.top, SRCCOPY);
				m_cSimuTime.Format(_T("%.3f"), dResultData[0]);
				GetDlgItem(IDC_STATIC_SIMU_TIME)->SetWindowTextW(m_cSimuTime);
				
				KillTimer(0);
				fclose(fpResult);
				m_dcMem.DeleteDC();
				m_bmp.DeleteObject();

				m_bSimulationFlag = FALSE;
				GetDlgItem(IDC_BUTTON_SIMULATION)->SetWindowTextW(_T("simulation"));
			}
		}
		dc.BitBlt(m_rectPlotSpace.left, m_rectPlotSpace.top, m_rectPlotSpace.right, m_rectPlotSpace.bottom, &m_dcMem, m_rectPlotSpace.left, m_rectPlotSpace.top, SRCCOPY);
		m_tSimuStart = clock();
		m_cSimuTime.Format(_T("%.3f"), dResultData[0]);
		GetDlgItem(IDC_STATIC_SIMU_TIME)->SetWindowTextW(m_cSimuTime);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CpathspeedDlg::OnBnClickedButtonSelectPlot()
{
	SelectOutputFile(m_cPlotPathName);
	GetDlgItem(IDC_STATIC_SELT_PLOT)->SetWindowTextW(m_cPlotPathName);
}


void CpathspeedDlg::OnBnClickedButtonSimulation()
{
	if (m_cPlotPathName == _T(""))
	{
		MessageBox(_T("Please select plot file!"));
	}
	else if (!m_bSimulationFlag)
	{
		m_bPlotFlag = FALSE;
		m_bZoomFlag = FALSE;
		m_bSimulationFlag = TRUE;

		GetNewDCMem();
		GetResultHead(0);
		m_bPlotVflag = FALSE;
		m_bPlotAflag = FALSE;
		m_iXlabel = 1;
		m_iYlabel = 2;
		m_dPlotXmin = m_dResultData[3]; // Xmin
		m_dPlotXmax = m_dResultData[2]; // Xmax
		m_dPlotYmin = m_dResultData[5]; // Ymin
		m_dPlotYmax = m_dResultData[4]; // Ymax
		m_strPlotXlabel = _T("X");
		m_strPlotYlabel = _T("Y");
		UpdateData(FALSE);
		PlotGrid();

		double dResultData[3];
		POINTXY PlotBeginPoint;
		
		if (fscanf(fpResult, "%lf,%lf,%lf\n", &dResultData[0], &dResultData[1], &dResultData[2]) != EOF)
		{
			PlotBeginPoint.x = dResultData[m_iXlabel] * m_dXscale + m_dXscaleConst;
			PlotBeginPoint.y = dResultData[m_iYlabel] * m_dYscale + m_dYscaleConst;
			m_dcMem.MoveTo(PlotBeginPoint.x, PlotBeginPoint.y);
		}

		m_tSimuStart = clock();
		SetTimer(0, 50, NULL);

		GetDlgItem(IDC_BUTTON_SIMULATION)->SetWindowTextW(_T("stop"));
	}
	else
	{
		KillTimer(0);
		fclose(fpResult);
		m_dcMem.DeleteDC();
		m_bmp.DeleteObject();
		m_bSimulationFlag = FALSE;
		GetDlgItem(IDC_BUTTON_SIMULATION)->SetWindowTextW(_T("simulation"));
	}
}


void CpathspeedDlg::OnCbnSelchangeComboPlotType()
{
	OnBnClickedButtonPlot();
}

