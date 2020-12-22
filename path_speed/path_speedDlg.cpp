
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
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpathspeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dSampleTime);
	DDX_Text(pDX, IDC_EDIT4, m_dThetaMax);
	DDX_Control(pDX, IDC_COMBO_PLOT_TYPE, m_cbPlotType);
}

BEGIN_MESSAGE_MAP(CpathspeedDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CpathspeedDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CpathspeedDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CpathspeedDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CpathspeedDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_PLOT, &CpathspeedDlg::OnBnClickedButtonPlot)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_OUTFILE, &CpathspeedDlg::OnBnClickedButtonSelectOutfile)
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

	GetDlgItem(IDC_CHART)->GetWindowRect(&m_rectPlotSpace);
	ScreenToClient(&m_rectPlotSpace);
	
	m_cbPlotType.SetCurSel(PLOTVT);
	
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
		CPaintDC ChartDC(this);
		CRect rectChartSpace;
		rectChartSpace.left = m_rectPlotSpace.left + PLOTGAP;
		rectChartSpace.top = m_rectPlotSpace.top + PLOTGAP;
		rectChartSpace.right = m_rectPlotSpace.right - PLOTGAP;
		rectChartSpace.bottom = m_rectPlotSpace.bottom - PLOTGAP;

		// 雙緩衝
		CDC dcMem;
		CBitmap bmp;
		dcMem.CreateCompatibleDC(&ChartDC);
		bmp.CreateCompatibleBitmap(&ChartDC, m_rectPlotSpace.right, m_rectPlotSpace.bottom);
		dcMem.SelectObject(&bmp);
		dcMem.FillSolidRect(m_rectPlotSpace, RGB(255, 255, 255));
		dcMem.Rectangle(rectChartSpace);

		// 讀檔繪圖
		POINTXY PlotBeginPoint;
		POINTXY PlotEndPoint;
		int iPlotFlag = 0;
		double dResultData[6];
		double dXscale = 1;
		double dYscale = 1;
		int iXlabel;
		int iYlabel;
		POINTXY PlotOrgPoint;
		CString strPlotStr;
		int iPlotType = m_cbPlotType.GetCurSel();

		//抓行數換算時間
		/*FILE* fpResult;
		fpResult = fopen(CT2A(m_cPlotPathName), "r");
		int iFileLineCounts = 0;
		char strTmp;
		for (strTmp = getc(fpResult); strTmp != EOF; strTmp = getc(fpResult)) {
			if (strTmp == '\n')
			{
				iFileLineCounts += 1;
			}
		}
		dXscale = (1.0 * rectChartSpace.Width() - 2.0 * PLOTXLABELGAP) / (iFileLineCounts * m_dSampleTime * pow(10, -3));
		fclose(fpResult);*/

		//抓head,取max參數
		FILE* fpResult;
		fpResult = fopen(CT2A(m_cPlotPathName), "r");
		//maxTime, maxV, maxX, minX, maxY, minY
		if (fscanf(fpResult, "%lf,%lf,%lf,%lf,%lf,%lf\n", &dResultData[0], &dResultData[1], &dResultData[2], &dResultData[3], &dResultData[4], &dResultData[5]) != EOF)
		{
			dXscale = (1.0 * rectChartSpace.Width()) / dResultData[0];
			dYscale = (1.0 * rectChartSpace.Height()) / dResultData[1];
			iPlotFlag++;
		}

		switch (iPlotType)
		{
			case(PLOTVT):
			{
				iXlabel = 0;
				iYlabel = 3;
				PlotOrgPoint.x = rectChartSpace.left;
				PlotOrgPoint.y = rectChartSpace.bottom;
				strPlotStr = _T("0");
				dcMem.TextOutW(rectChartSpace.left, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("T");
				dcMem.TextOutW(rectChartSpace.right + PLOTXLABELGAP, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("V");
				dcMem.TextOutW(rectChartSpace.left - 15, rectChartSpace.top - PLOTYLABELGAP, strPlotStr);
				break;
			}

			case(PLOTXY):
			{
				dXscale = (1.0 * rectChartSpace.Width()) / (dResultData[2] - dResultData[3]);
				dYscale = (1.0 * rectChartSpace.Height()) / (dResultData[4] - dResultData[5]);
				iXlabel = 1;
				iYlabel = 2;
				PlotOrgPoint.x = rectChartSpace.CenterPoint().x;
				PlotOrgPoint.y = rectChartSpace.CenterPoint().y;
				strPlotStr = _T("0");
				dcMem.TextOutW(rectChartSpace.left, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("X");
				dcMem.TextOutW(rectChartSpace.right + PLOTXLABELGAP, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("Y");
				dcMem.TextOutW(rectChartSpace.left - 15, rectChartSpace.top - PLOTYLABELGAP, strPlotStr);
				break;
			}

			case(PLOTXT):
			{
				dYscale = (1.0 * rectChartSpace.Height()) / (2.0 * dResultData[2]);
				iXlabel = 0;
				iYlabel = 1;
				PlotOrgPoint.x = rectChartSpace.left;
				PlotOrgPoint.y = rectChartSpace.top + (rectChartSpace.Height() / 2.0);
				strPlotStr = _T("0");
				dcMem.TextOutW(rectChartSpace.left, rectChartSpace.bottom + 2, strPlotStr);
				dcMem.TextOutW(rectChartSpace.left - 12, PlotOrgPoint.y, strPlotStr);
				strPlotStr = _T("T");
				dcMem.TextOutW(rectChartSpace.right + PLOTXLABELGAP, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("X");
				dcMem.TextOutW(rectChartSpace.left - 15, rectChartSpace.top - PLOTYLABELGAP, strPlotStr);
				break;
			}

			case(PLOTYT):
			{
				dYscale = (1.0 * rectChartSpace.Height()) / (2.0 * dResultData[4]);
				iXlabel = 0;
				iYlabel = 2;
				PlotOrgPoint.x = rectChartSpace.left;
				PlotOrgPoint.y = rectChartSpace.top + (rectChartSpace.Height() / 2.0);
				strPlotStr = _T("0");
				dcMem.TextOutW(rectChartSpace.left, rectChartSpace.bottom + 2, strPlotStr);
				dcMem.TextOutW(rectChartSpace.left - 12, PlotOrgPoint.y, strPlotStr);
				strPlotStr = _T("T");
				dcMem.TextOutW(rectChartSpace.right + PLOTXLABELGAP, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("Y");
				dcMem.TextOutW(rectChartSpace.left - 15, rectChartSpace.top - PLOTYLABELGAP, strPlotStr);
				break;
			}

			case(PLOTVXT):
			{
				dYscale = (1.0 * rectChartSpace.Height()) / (2.0 * dResultData[1]);
				iXlabel = 0;
				iYlabel = 4;
				PlotOrgPoint.x = rectChartSpace.left;
				PlotOrgPoint.y = rectChartSpace.top + (rectChartSpace.Height() / 2.0);
				strPlotStr = _T("0");
				dcMem.TextOutW(rectChartSpace.left, rectChartSpace.bottom + 2, strPlotStr);
				dcMem.TextOutW(rectChartSpace.left - 12, PlotOrgPoint.y, strPlotStr);
				strPlotStr = _T("T");
				dcMem.TextOutW(rectChartSpace.right + PLOTXLABELGAP, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("Vx");
				dcMem.TextOutW(rectChartSpace.left - 20, rectChartSpace.top - PLOTXLABELGAP, strPlotStr);
				break;
			}

			case(PLOTVYT):
			{
				dYscale = (1.0 * rectChartSpace.Height()) / (2.0 * dResultData[1]);
				iXlabel = 0;
				iYlabel = 5;
				PlotOrgPoint.x = rectChartSpace.left;
				PlotOrgPoint.y = rectChartSpace.top + (rectChartSpace.Height() / 2.0);
				strPlotStr = _T("0");
				dcMem.TextOutW(rectChartSpace.left, rectChartSpace.bottom + 2, strPlotStr);
				dcMem.TextOutW(rectChartSpace.left - 12, PlotOrgPoint.y, strPlotStr);
				strPlotStr = _T("T");
				dcMem.TextOutW(rectChartSpace.right + PLOTXLABELGAP, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("Vy");
				dcMem.TextOutW(rectChartSpace.left - 20, rectChartSpace.top - PLOTXLABELGAP, strPlotStr);
				break;
			}

			default:
			{
				iXlabel = 0;
				iYlabel = 3;
				PlotOrgPoint.x = rectChartSpace.left;
				PlotOrgPoint.y = rectChartSpace.bottom;
				strPlotStr = _T("0");
				dcMem.TextOutW(rectChartSpace.left, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("T");
				dcMem.TextOutW(rectChartSpace.right + PLOTXLABELGAP, rectChartSpace.bottom + 2, strPlotStr);
				strPlotStr = _T("V");
				dcMem.TextOutW(rectChartSpace.left - 15, rectChartSpace.top - PLOTXLABELGAP, strPlotStr);
				break;
			}
		}

		CPen* pOldPen;
		CPen penLine(PS_SOLID, 1.5, RGB(99, 184, 255));
		pOldPen = dcMem.SelectObject(&penLine);

		while (fscanf(fpResult, "%lf, %lf, %lf, %lf, %lf, %lf\n", &dResultData[0], &dResultData[1], &dResultData[2], &dResultData[3], &dResultData[4], &dResultData[5]) != EOF)
		{
			if (iPlotFlag == 1)
			{
				PlotBeginPoint.x = dResultData[iXlabel] * dXscale + PlotOrgPoint.x;
				PlotBeginPoint.y = -dResultData[iYlabel] * dYscale + PlotOrgPoint.y;
				dcMem.MoveTo(PlotBeginPoint.x, PlotBeginPoint.y);
			}
			else
			{
				PlotEndPoint.x = dResultData[iXlabel] * dXscale + PlotOrgPoint.x;
				PlotEndPoint.y = -dResultData[iYlabel] * dYscale + PlotOrgPoint.y;
				dcMem.LineTo(PlotEndPoint.x, PlotEndPoint.y);
				PlotBeginPoint.x = PlotEndPoint.x;
				PlotBeginPoint.y = PlotEndPoint.y;
			}
			iPlotFlag++;
		}
		fclose(fpResult);

		ChartDC.BitBlt(m_rectPlotSpace.left, m_rectPlotSpace.top, m_rectPlotSpace.right, m_rectPlotSpace.bottom, &dcMem, m_rectPlotSpace.left, m_rectPlotSpace.top, SRCCOPY);
		bmp.DeleteObject();
		dcMem.DeleteDC();
	}
	else
	{
		CDialogEx::OnPaint();
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


void CpathspeedDlg::SelectFile(CString& pathName)
{
	// select file
	TCHAR szFilters[] = _T("Text Files (*.txt)|*.txt|CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("txt"), _T("*.txt"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		pathName = fileDlg.GetPathName();
	}
}

void CpathspeedDlg::ReadCommand(CString pathName, CArray<CMD, CMD&>& CmdArray)
{
	CFile FileCMD;
	CString strFileStr = _T("");
	char strFileChar;
	CString strFileCmd;
	int iflag; // 逗點flag
	CMD Cmd;

	CmdArray.RemoveAll();
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
				OnBnClickedCancel();
			}
			strFileStr = strFileStr.TrimLeft(strFileCmd).TrimLeft(_T(" ,\n"));
			iflag = 0;
			while (strFileStr.IsEmpty() == 0 && iflag < 5)
			{
				Cmd.m_dParams[iflag] = _tstof(strFileStr.SpanExcluding(_T(" ,\n")));
				strFileStr = strFileStr.TrimLeft(strFileStr.SpanExcluding(_T(" ,\n"))).TrimLeft(_T(" ,\n"));
				iflag++;
			}
			CmdArray.Add(Cmd);
			strFileStr.Empty();
		}
	}
	FileCMD.Close();
}

void CpathspeedDlg::SelectPathVAmax(int cmd, double speed[2], double acc[2], CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&>& PathVAMaxArray)
{
	PARAMS_VA_MAX VAmax;

	if (cmd == LINEXY || cmd == ARCXY)
	{
		VAmax.m_dVmax = speed[0];
		VAmax.m_dAmax = acc[0];
		PathVAMaxArray.Add(VAmax);
	}
	else if (cmd == FLINEXY || cmd == FARCXY)
	{
		VAmax.m_dVmax = speed[1];
		VAmax.m_dAmax = acc[1];
		PathVAMaxArray.Add(VAmax);
	}
	else
	{
		// do nothing
	}
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

void CpathspeedDlg::CalArcPoint(POINTXY BeginP, CMD Cmd, CArray<POINTXY, POINTXY&>& PathPointArray, double speed[2], double acc[2], CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&>& PathVAMaxArray)
{
	if (Cmd.m_iID == ARCXY || Cmd.m_iID == FARCXY)
	{
		POINTXY ArcEndPoint;
		POINTXY ArcCenterPoint;
		POINTXY ArcPoint;

		double dDirection;
		double dRadius;
		double dCosThetaStart;
		double dSinThetaStart;
		double dCosThetaEnd;
		double dSinThetaEnd;
		double dThetaStart;
		double dThetaEnd;
		double dThetaPath;
		double dTheta = 1.0 * PI / 180.0;

		ArcEndPoint.x = Cmd.m_dParams[0];
		ArcEndPoint.y = Cmd.m_dParams[1];
		ArcCenterPoint.x = Cmd.m_dParams[2];
		ArcCenterPoint.y = Cmd.m_dParams[3];
		dDirection = Cmd.m_dParams[4];
		dRadius = sqrt(pow(ArcEndPoint.x - ArcCenterPoint.x, 2) + pow(ArcEndPoint.y - ArcCenterPoint.y, 2));
		if (dRadius == 0.0)
		{
			// do nothing
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
				if (dThetaStart == dThetaEnd) // 逆時針完整圓
				{
					dThetaPath = 2.0 * PI;
				}
				else
				{
					dThetaPath = dThetaEnd - dThetaStart;
					if (dThetaPath < 0.0)
					{
						dThetaPath = dThetaPath + 2.0 * PI;
					}

				}
				while (dTheta < dThetaPath)
				{
					ArcPoint.x = ArcCenterPoint.x + dRadius * cos(dThetaStart + dTheta);
					ArcPoint.y = ArcCenterPoint.y + dRadius * sin(dThetaStart + dTheta);
					dTheta += 1.0 * PI / 180.0;
					if (dTheta >= dThetaPath)
					{
						break;
					}
					PathPointArray.Add(ArcPoint);
					SelectPathVAmax(Cmd.m_iID, speed, acc, PathVAMaxArray);
				}

			}
			else // 順時針
			{
				if (dThetaStart == dThetaEnd) // 順時針完整圓
				{
					dThetaPath = 2.0 * PI;
				}
				else
				{
					dThetaPath = dThetaStart - dThetaEnd;
					if (dThetaPath < 0.0)
					{
						dThetaPath = dThetaPath + 2.0 * PI;
					}
				}
				while (dTheta < dThetaPath)
				{
					ArcPoint.x = ArcCenterPoint.x + dRadius * cos(dThetaStart - dTheta);
					ArcPoint.y = ArcCenterPoint.y + dRadius * sin(dThetaStart - dTheta);
					dTheta += 1.0 * PI / 180.0;
					if (dTheta >= dThetaPath)
					{
						break;
					}
					PathPointArray.Add(ArcPoint);
					SelectPathVAmax(Cmd.m_iID, speed, acc, PathVAMaxArray);
				}
			}
		}
	}
	else
	{
		// do nothing
	}
}

void CpathspeedDlg::OnBnClickedButton1()
{
	SelectFile(m_cInputPathName);
}

void CpathspeedDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);
	if(m_dSampleTime <= 0) {
		MessageBox(_T("Error: Sample Time <= 0"));
	}
	else {
		// Read command
		ReadCommand(m_cInputPathName, m_arrCmdArray);

		FILE* OutFile;
		OutFile = fopen("D:\\tmpResult.txt", "w");

		double dTimeStart = 0; // 起始時間
		double dBeginPoint[2]; // 起點位置
		double dEndPoint[2]; // 終點位置
		dBeginPoint[0] = 0; // 起點 x 座標
		dBeginPoint[1] = 0; // 起點 y 座標
		double dSpeed[2]; // 速度
		dSpeed[0] = 0; // 最大移動速度
		dSpeed[1] = 0; // 最大快速移動速度
		double dAcc[2]; // 加速度
		dAcc[0] = 0; // 最大移動加速度
		dAcc[1] = 0; // 最大快速移動加速度

		int iCmdFlag; // 目前執行的cmd
		int iCurrentCmd;
		int iNextCmd;
		
		double dVStart = 0; // 起始速度
		double dVEnd = 0; // 終止速度
		double dVmax; // 最大速度
		double dAmax; // 最大加速度

		CMD CurrentCmd;
		CMD NextCmd;

		POINTXY OutPoint;
		SPEEDVT OutVT;
		POINTXY OutVxVy;

		POINTXY ArcCenterPoint; // 圓心
		double dDirection; // 圓弧方向, >=0 逆時間, <0 順時針

		double dCosTheta;
		double dCurrentDistance;
		double dNextDistance;

		POINTXY BeginPoint;
		POINTXY CurrentPoint;
		POINTXY NextPoint;
		PARAMS_VA_MAX VAmax;

		double dMaxOutX = 0;
		double dMaxOutY = 0;
		double dMaxOutV = 0;
		double dMinOutX = 0;
		double dMinOutY = 0;

		// Command classification
		for (int i = 0; i < m_arrCmdArray.GetSize(); i++) {
			
			iCurrentCmd = m_arrCmdArray[i].m_iID;

			switch (iCurrentCmd)
			{
			case BEGIN: {
				iCmdFlag = i;
				CurrentCmd = m_arrCmdArray[i];
				dBeginPoint[0] = CurrentCmd.m_dParams[0];
				dBeginPoint[1] = CurrentCmd.m_dParams[1];
				OutPoint.x = dBeginPoint[0];
				OutPoint.y = dBeginPoint[1];
				dMaxOutX = OutPoint.x;
				dMaxOutY = OutPoint.y;
				dMinOutX = OutPoint.x;
				dMinOutY = OutPoint.y;
				OutVT.t = dTimeStart; // 起始時間為0
				OutVT.v = dVStart; // 起始速度為0
				OutVxVy.x = 0;
				OutVxVy.y = 0;
				fprintf(OutFile, "%lf,%lf,%lf,%lf,%lf,%lf\n", OutVT.t, OutPoint.x, OutPoint.y, OutVT.v, OutVxVy.x, OutVxVy.y);
				break;
			}
			case SPEED: {
				iCmdFlag = i;
				CurrentCmd = m_arrCmdArray[i];
				dSpeed[0] = CurrentCmd.m_dParams[0];
				dSpeed[1] = CurrentCmd.m_dParams[1];
				dMaxOutV = fabs(dSpeed[1]);
				if (dMaxOutV < fabs(dSpeed[0]))
				{
					dMaxOutV = fabs(dSpeed[0]);
				}
				break;
			}
			case ACC: {
				iCmdFlag = i;
				CurrentCmd = m_arrCmdArray[i];
				dAcc[0] = CurrentCmd.m_dParams[0];
				dAcc[1] = CurrentCmd.m_dParams[1];
				break;
			}
			case END: {
				fclose(OutFile);
				break;
			}
			// LineXY, FLineXY, ArcXY, FArcXY
			default: {
				iCmdFlag = i;
				if (iCmdFlag < 3)
				{
					MessageBox(_T("條件不足"));
					i = m_arrCmdArray.GetSize();
					break;
				}

				CurrentCmd = m_arrCmdArray[i];

				dEndPoint[0] = CurrentCmd.m_dParams[0];
				dEndPoint[1] = CurrentCmd.m_dParams[1];
				if (iCurrentCmd == ARCXY || iCurrentCmd == FARCXY)
				{
					ArcCenterPoint.x = m_arrCmdArray[i].m_dParams[2];
					ArcCenterPoint.y = m_arrCmdArray[i].m_dParams[3];
					dDirection = m_arrCmdArray[i].m_dParams[4];
					if (dEndPoint[0] == ArcCenterPoint.x && dEndPoint[1] == ArcCenterPoint.y) // 半徑為0
					{
						i = iCmdFlag;
						dBeginPoint[0] = dEndPoint[0];
						dBeginPoint[1] = dEndPoint[1];
						m_arrPathPointArray.RemoveAll();
						MessageBox(_T("半徑為0"));
						break;
					}
				}

				BeginPoint.x = dBeginPoint[0];
				BeginPoint.y = dBeginPoint[1];
				CurrentPoint.x = dEndPoint[0];
				CurrentPoint.y = dEndPoint[1];

				// 加入起點
				m_arrPathPointArray.Add(BeginPoint);
				dCurrentDistance = sqrt(pow((CurrentPoint.x - BeginPoint.x), 2) + pow((CurrentPoint.y - BeginPoint.y), 2));

				// 起終點同點處理
				if (dCurrentDistance == 0.0)
				{
					if (iCurrentCmd == LINEXY || iCurrentCmd == FLINEXY)
					{
						i = iCmdFlag;
						dBeginPoint[0] = dEndPoint[0];
						dBeginPoint[1] = dEndPoint[1];
						m_arrPathPointArray.RemoveAll();
						MessageBox(_T("同點直線"));
						break;
					}
					else // Arc 正圓
					{
						// 加入路徑點與Vmax, Amax
						CalArcPoint(BeginPoint, CurrentCmd, m_arrPathPointArray, dSpeed, dAcc, m_arrPathVAMaxArray);
					}
				}
				else
				{
					// 確認是否是Arc
					if (iCurrentCmd == ARCXY || iCurrentCmd == FARCXY)
					{
						CalArcPoint(BeginPoint, CurrentCmd, m_arrPathPointArray, dSpeed, dAcc, m_arrPathVAMaxArray);
					}
				}

				// 加入終點
				m_arrPathPointArray.Add(CurrentPoint);
				// 加入終點最大速度與加速度
				SelectPathVAmax(CurrentCmd.m_iID, dSpeed, dAcc, m_arrPathVAMaxArray);

				// 讀下一條命令，判斷夾角
				while (m_arrCmdArray[iCmdFlag + 1].m_iID == LINEXY || m_arrCmdArray[iCmdFlag + 1].m_iID == FLINEXY || m_arrCmdArray[iCmdFlag + 1].m_iID == ARCXY || m_arrCmdArray[iCmdFlag + 1].m_iID == FARCXY)
				{
					NextCmd = m_arrCmdArray[iCmdFlag + 1];
					iNextCmd = NextCmd.m_iID;
					NextPoint.x = NextCmd.m_dParams[0];
					NextPoint.y = NextCmd.m_dParams[1];
					dNextDistance = sqrt(pow((NextPoint.x - CurrentPoint.x), 2) + pow((NextPoint.y - CurrentPoint.y), 2));
					if (dNextDistance == 0.0)
					{
						if (iNextCmd == LINEXY || iNextCmd == FLINEXY)
						{
							iCmdFlag += 1;
						}
						else // Arc
						{
							if (NextPoint.x == NextCmd.m_dParams[3] && NextPoint.y == NextCmd.m_dParams[4]) // 半徑為0
							{
								iCmdFlag += 1;
							}
							else
							{
								BeginPoint.x = CurrentPoint.x;
								BeginPoint.y = CurrentPoint.y;
								CurrentPoint.x = NextPoint.x;
								CurrentPoint.y = NextPoint.y;
								CalArcPoint(BeginPoint, NextCmd, m_arrPathPointArray, dSpeed, dAcc, m_arrPathVAMaxArray);
								m_arrPathPointArray.Add(NextPoint);
								SelectPathVAmax(iNextCmd, dSpeed, dAcc, m_arrPathVAMaxArray);
								iCmdFlag += 1;
							}
						}
					}
					else
					{
						dCosTheta = ((CurrentPoint.x - BeginPoint.x) * (NextPoint.x - CurrentPoint.x) + (CurrentPoint.y - BeginPoint.y) * (NextPoint.y - CurrentPoint.y)) / (dCurrentDistance * dNextDistance);

						if (dCosTheta > cos(m_dThetaMax * PI / 180.0))
						{
							BeginPoint.x = CurrentPoint.x;
							BeginPoint.y = CurrentPoint.y;
							CurrentPoint.x = NextPoint.x;
							CurrentPoint.y = NextPoint.y;
							// 更新終點
							if (iNextCmd == LINEXY || iNextCmd == FLINEXY)
							{
								m_arrPathPointArray.Add(NextPoint);
								SelectPathVAmax(iNextCmd, dSpeed, dAcc, m_arrPathVAMaxArray);
							}
							else // Arc
							{
								if (NextPoint.x == NextCmd.m_dParams[3] && NextPoint.y == NextCmd.m_dParams[4]) // 半徑為0
								{
									MessageBox(_T("Arc起點終點不同, 半徑不可為0"));
									break;
								}
								else
								{
									CalArcPoint(BeginPoint, NextCmd, m_arrPathPointArray, dSpeed, dAcc, m_arrPathVAMaxArray);
									m_arrPathPointArray.Add(NextPoint);
									SelectPathVAmax(iNextCmd, dSpeed, dAcc, m_arrPathVAMaxArray);
								}
							}
							iCmdFlag += 1;
						}
						else
						{
							break;
						}
					}
				}

				// 線段結束, Vend為0
				VAmax.m_dVmax = 0.0;
				VAmax.m_dAmax = 0.0;
				m_arrPathVAMaxArray.Add(VAmax);

				// 計算此段總距離
				double dDistance = 0.0; // 總移動距離, Vs=0, Vend =0, L=L1+L2+...+Ln
				for (int i = 0; i < m_arrPathPointArray.GetSize() - 1; i++)
				{
					dDistance += sqrt(pow((m_arrPathPointArray.GetAt(i + 1).x - m_arrPathPointArray.GetAt(i).x), 2) + pow((m_arrPathPointArray.GetAt(i + 1).y - m_arrPathPointArray.GetAt(i).y), 2));
				}

				// 開始計算線段
				double dTimeTotal = 0.0; // 總時間
				double dXRatio; // 換算X
				double dYRatio; // 換算Y
				double dDistanceSum;
				double dt = m_dSampleTime * pow(10, -3);

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

				OutVT.v = dVStart;
				OutVT.t = dTimeStart;

				/*CArray<POINTXY, POINTXY&> arrSortX;
				CArray<POINTXY, POINTXY&> arrSortY;
				CArray<SPEEDVT, SPEEDVT&> arrSortV;*/

				for (int i = 0; i < m_arrPathPointArray.GetSize() - 1; i++)
				{
					dCurrentDistance = sqrt(pow((m_arrPathPointArray.GetAt(i + 1).x - m_arrPathPointArray.GetAt(i).x), 2) + pow((m_arrPathPointArray.GetAt(i + 1).y - m_arrPathPointArray.GetAt(i).y), 2));
					dXRatio = (m_arrPathPointArray.GetAt(i + 1).x - m_arrPathPointArray.GetAt(i).x) / dCurrentDistance;
					dYRatio = (m_arrPathPointArray.GetAt(i + 1).y - m_arrPathPointArray.GetAt(i).y) / dCurrentDistance;
					
					dVmax = m_arrPathVAMaxArray.GetAt(i).m_dVmax;
					dAmax = m_arrPathVAMaxArray.GetAt(i).m_dAmax;

					dNextVmax = m_arrPathVAMaxArray.GetAt(i + 1).m_dVmax;
					
					dDistanceSum = 0;

					if (dNextVmax != 0.0) // 還有下一段, 因此 Vend != 0
					{
						S = dDistance - dDistanceNow;
						dVEnd = 0.0;
						S1 = (pow(dVmax, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
						S3 = (pow(dVmax, 2) - pow(dVEnd, 2)) / (2.0 * dAmax);
						S2 = S - S1 - S3;
						if (S2 <= 0.0) // 沒有均速階段, 剩下的距離之速度規劃為三角形
						{
							S1 = (2.0 * dAmax * S - pow(dVStart, 2)) / (4.0 * dAmax);
							if (S1 < 0.0) // 只有減速
							{
								dVEnd = sqrt(pow(dVStart, 2) - 2.0 * dAmax * dCurrentDistance);
								dTimeTotal = 2.0 * dCurrentDistance / (dVStart + dVEnd);
								while (dDistanceSum < dCurrentDistance)
								{
									dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
									OutVT.v -= dAmax * dt;
									if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
									{
										break;
									}
									OutVT.t += dt;
									OutVxVy.x = OutVT.v * dXRatio;
									OutVxVy.y = OutVT.v * dYRatio;
									OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
									OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
									m_arrOutVTArray.Add(OutVT);
									m_arrOutVxVyArray.Add(OutVxVy);
									m_arrOutXYArray.Add(OutPoint);
									dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
									dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
									dMinOutX = CheckMin(dMinOutX, OutPoint.x);
									dMinOutY = CheckMin(dMinOutY, OutPoint.y);
								}
							}
							else // 加速到Vm再減速
							{
								Vm = sqrt((2.0 * dAmax * S + pow(dVStart, 2)) / 2.0);
								S1 = (pow(Vm, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
								T1 = 2.0 * S1 / (dVStart + Vm);
								if (dCurrentDistance <= S1) // 此小線段都在加速區域
								{
									dVEnd = sqrt(pow(dVStart, 2) + 2.0 * dAmax * dCurrentDistance);
									if (dVEnd > dNextVmax) // Vend設為Vmax,next，重算Vm，此小線段會進減速區域
									{
										dVEnd = dNextVmax;
										Vm = sqrt((2.0 * dAmax * dCurrentDistance + pow(dVStart, 2) + pow(dVEnd, 2)) / 2.0);
										S1 = (pow(Vm, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
										T1 = 2.0 * S1 / (dVStart + Vm);
										T3 = (2.0 * (dCurrentDistance - S1)) / (Vm + dVEnd);
										dTimeTotal = T1 + T3;
										while (dDistanceSum < S1)
										{
											dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
											OutVT.v += dAmax * dt;
											OutVT.t += dt;
											if (dDistanceSum > S1)
											{
												dDistanceSum = dDistanceSum - (OutVT.t - dTimeStart - T1) * (OutVT.v - Vm);
												OutVT.v = Vm - dAmax * (OutVT.t - dTimeStart - T1);
											}
											OutVxVy.x = OutVT.v * dXRatio;
											OutVxVy.y = OutVT.v * dYRatio;
											OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
											OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
											m_arrOutVTArray.Add(OutVT);
											m_arrOutVxVyArray.Add(OutVxVy);
											m_arrOutXYArray.Add(OutPoint);
											dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
											dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
											dMinOutX = CheckMin(dMinOutX, OutPoint.x);
											dMinOutY = CheckMin(dMinOutY, OutPoint.y);
										}
										while (dDistanceSum >= S1 && dDistanceSum < dCurrentDistance)
										{
											dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
											OutVT.v -= dAmax * dt;
											if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
											{
												break;
											}
											OutVT.t += dt;
											OutVxVy.x = OutVT.v * dXRatio;
											OutVxVy.y = OutVT.v * dYRatio;
											OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
											OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
											m_arrOutVTArray.Add(OutVT);
											m_arrOutVxVyArray.Add(OutVxVy);
											m_arrOutXYArray.Add(OutPoint);
											dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
											dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
											dMinOutX = CheckMin(dMinOutX, OutPoint.x);
											dMinOutY = CheckMin(dMinOutY, OutPoint.y);
										}
									}
									else // 此小線段都在加速區域
									{
										dTimeTotal = 2.0 * dCurrentDistance / (dVStart + dVEnd);
										while (dDistanceSum < S1)
										{
											dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
											OutVT.v += dAmax * dt;
											if (dDistanceSum >= dCurrentDistance || OutVT.v >= dVEnd)
											{
												break;
											}
											OutVT.t += dt;
											OutVxVy.x = OutVT.v * dXRatio;
											OutVxVy.y = OutVT.v * dYRatio;
											OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
											OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
											m_arrOutVTArray.Add(OutVT);
											m_arrOutVxVyArray.Add(OutVxVy);
											m_arrOutXYArray.Add(OutPoint);
											dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
											dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
											dMinOutX = CheckMin(dMinOutX, OutPoint.x);
											dMinOutY = CheckMin(dMinOutY, OutPoint.y);
										}
									}
								}
								else // 此小線段會進減速區域
								{
									dVEnd = sqrt(2.0 * dAmax * (S - dCurrentDistance));
									if (dVEnd > dNextVmax) // Vend設為Vmax,next，重算Vm
									{
										dVEnd = dNextVmax;
										Vm = sqrt((2.0 * dAmax * dCurrentDistance + pow(dVStart, 2) + pow(dVEnd, 2)) / 2.0);
										S1 = (pow(Vm, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
										T1 = 2.0 * S1 / (dVStart + Vm);
										T3 = (2.0 * (dCurrentDistance - S1)) / (Vm + dVEnd);
										dTimeTotal = T1 + T3;
										while (dDistanceSum < S1)
										{
											dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
											OutVT.v += dAmax * dt;
											OutVT.t += dt;
											if (dDistanceSum > S1)
											{
												dDistanceSum = dDistanceSum - (OutVT.t - dTimeStart - T1) * (OutVT.v - Vm);
												OutVT.v = Vm - dAmax * (OutVT.t - dTimeStart - T1);
											}
											OutVxVy.x = OutVT.v * dXRatio;
											OutVxVy.y = OutVT.v * dYRatio;
											OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
											OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
											m_arrOutVTArray.Add(OutVT);
											m_arrOutVxVyArray.Add(OutVxVy);
											m_arrOutXYArray.Add(OutPoint);
											dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
											dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
											dMinOutX = CheckMin(dMinOutX, OutPoint.x);
											dMinOutY = CheckMin(dMinOutY, OutPoint.y);
										}
										while (dDistanceSum >= S1 && dDistanceSum < dCurrentDistance)
										{
											dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
											OutVT.v -= dAmax * dt;
											if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
											{
												break;
											}
											OutVT.t += dt;
											OutVxVy.x = OutVT.v * dXRatio;
											OutVxVy.y = OutVT.v * dYRatio;
											OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
											OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
											m_arrOutVTArray.Add(OutVT);
											m_arrOutVxVyArray.Add(OutVxVy);
											m_arrOutXYArray.Add(OutPoint);
											dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
											dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
											dMinOutX = CheckMin(dMinOutX, OutPoint.x);
											dMinOutY = CheckMin(dMinOutY, OutPoint.y);
										}
									}
									else // 此小線段會進減速區域
									{
										T3 = (2.0 * (dCurrentDistance - S1)) / (Vm + dVEnd);
										dTimeTotal = T1 + T3;
										while (dDistanceSum < S1)
										{
											dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
											OutVT.v += dAmax * dt;
											OutVT.t += dt;
											if (dDistanceSum > S1)
											{
												dDistanceSum = dDistanceSum - (OutVT.t - dTimeStart - T1) * (OutVT.v - Vm);
												OutVT.v = Vm - dAmax * (OutVT.t - dTimeStart - T1);
											}
											OutVxVy.x = OutVT.v * dXRatio;
											OutVxVy.y = OutVT.v * dYRatio;
											OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
											OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
											m_arrOutVTArray.Add(OutVT);
											m_arrOutVxVyArray.Add(OutVxVy);
											m_arrOutXYArray.Add(OutPoint);
											dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
											dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
											dMinOutX = CheckMin(dMinOutX, OutPoint.x);
											dMinOutY = CheckMin(dMinOutY, OutPoint.y);
										}
										while (dDistanceSum >= S1 && dDistanceSum < dCurrentDistance)
										{
											dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
											OutVT.v -= dAmax * dt;
											if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
											{
												break;
											}
											OutVT.t += dt;
											OutVxVy.x = OutVT.v * dXRatio;
											OutVxVy.y = OutVT.v * dYRatio;
											OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
											OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
											m_arrOutVTArray.Add(OutVT);
											m_arrOutVxVyArray.Add(OutVxVy);
											m_arrOutXYArray.Add(OutPoint);
											dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
											dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
											dMinOutX = CheckMin(dMinOutX, OutPoint.x);
											dMinOutY = CheckMin(dMinOutY, OutPoint.y);
										}
									}
								}
							}
						}
						else // 剩下的距離之速度規劃還能是梯形
						{
							if (dCurrentDistance <= S1) // 此小線段都在加速區域
							{
								dVEnd = sqrt(pow(dVStart, 2) + 2.0 * dAmax * dCurrentDistance);
								if (dVEnd > dNextVmax) // Vend設為Vmax,next，重算Vm
								{
									dVEnd = dNextVmax;
									Vm = sqrt((2.0 * dAmax * dCurrentDistance + pow(dVStart, 2) + pow(dVEnd, 2)) / 2.0);
									S1 = (pow(Vm, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
									T1 = 2.0 * S1 / (dVStart + Vm);
									T3 = (2.0 * (dCurrentDistance - S1)) / (Vm + dVEnd);
									dTimeTotal = T1 + T3;
									while (dDistanceSum < S1)
									{
										dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
										OutVT.v += dAmax * dt;
										OutVT.t += dt;
										if (dDistanceSum > S1)
										{
											dDistanceSum = dDistanceSum - (OutVT.t - dTimeStart - T1) * (OutVT.v - Vm);
											OutVT.v = Vm - dAmax * (OutVT.t - dTimeStart - T1);
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= S1 && dDistanceSum < dCurrentDistance)
									{
										dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
										OutVT.v -= dAmax * dt;
										if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
										{
											break;
										}
										OutVT.t += dt;
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
								}
								else
								{
									dTimeTotal = 2.0 * dCurrentDistance / (dVStart + dVEnd);
									while (dDistanceSum < S1)
									{
										dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
										OutVT.v += dAmax * dt;
										if (dDistanceSum >= dCurrentDistance || OutVT.v >= dVEnd)
										{
											break;
										}
										OutVT.t += dt;
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
								}
							}
							else if (dCurrentDistance > S1 && dCurrentDistance <= (S1 + S2)) // 此小線段會到均速區域
							{
								dVEnd = dVmax;
								if (dVEnd > dNextVmax) // Vend設為Vmax,next，提早開始減速
								{
									dVEnd = dNextVmax;
									S = dCurrentDistance;
									S1 = (pow(dVmax, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
									S3 = (pow(dVmax, 2) - pow(dVEnd, 2)) / (2.0 * dAmax);
									S2 = S - S1 - S3;
									T1 = (2.0 * S1) / (dVStart + dVmax);
									T2 = S2 / dVmax;
									T3 = (2.0 * (dCurrentDistance - S1 - S2)) / (dVmax + dVEnd);
									dTimeTotal = T1 + T2 + T3;
									while (dDistanceSum < S1)
									{
										dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
										OutVT.v += dAmax * dt;
										OutVT.t += dt;
										if (dDistanceSum > S1)
										{
											dDistanceSum = dDistanceSum - 0.5 * (OutVT.t - dTimeStart - T1) * (OutVT.v - dVmax);
											OutVT.v = dVmax;
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
									{
										dDistanceSum += dVmax * dt;
										OutVT.v = dVmax;
										OutVT.t += dt;
										if ((S - dDistanceSum) < S3)
										{
											OutVT.v = dVmax - dAmax * (OutVT.t - dTimeStart - T1 - T2);
											dDistanceSum = dDistanceSum - 0.5 * (dVmax - OutVT.v) * (OutVT.t - dTimeStart - T1 - T2);
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= (S1 + S2) && dDistanceSum < dCurrentDistance)
									{
										dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
										OutVT.v -= dAmax * dt;
										if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
										{
											break;
										}
										OutVT.t += dt;
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
								}
								else
								{
									T1 = (2.0 * S1) / (dVStart + dVmax);
									T2 = (dCurrentDistance - S1) / dVmax;
									dTimeTotal = T1 + T2;
									while (dDistanceSum < S1)
									{
										dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
										OutVT.v += dAmax * dt;
										OutVT.t += dt;
										if (dDistanceSum > S1)
										{
											dDistanceSum = dDistanceSum - 0.5 * (OutVT.t - dTimeStart - T1) * (OutVT.v - dVmax);
											OutVT.v = dVmax;
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= S1 && dDistanceSum < dCurrentDistance)
									{
										dDistanceSum += dVmax * dt;
										OutVT.v = dVmax;
										if (dDistanceSum >= dCurrentDistance)
										{
											break;
										}
										OutVT.t += dt;
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
								}
							}
							else // 此小線段會到減速區域
							{
								dVEnd = sqrt(2.0 * dAmax * (S - dCurrentDistance));
								if (dVEnd > dNextVmax) // Vend設為Vmax,next，提早開始減速
								{
									dVEnd = dNextVmax;
									S = dCurrentDistance;
									S1 = (pow(dVmax, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
									S3 = (pow(dVmax, 2) - pow(dVEnd, 2)) / (2.0 * dAmax);
									S2 = S - S1 - S3;
									T1 = (2.0 * S1) / (dVStart + dVmax);
									T2 = S2 / dVmax;
									T3 = (2.0 * (dCurrentDistance - S1 - S2)) / (dVmax + dVEnd);
									dTimeTotal = T1 + T2 + T3;
									while (dDistanceSum < S1)
									{
										dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
										OutVT.v += dAmax * dt;
										OutVT.t += dt;
										if (dDistanceSum > S1)
										{
											dDistanceSum = dDistanceSum - 0.5 * (OutVT.t - dTimeStart - T1) * (OutVT.v - dVmax);
											OutVT.v = dVmax;
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
									{
										dDistanceSum += dVmax * dt;
										OutVT.v = dVmax;
										OutVT.t += dt;
										if ((S - dDistanceSum) < S3)
										{
											OutVT.v = dVmax - dAmax * (OutVT.t - dTimeStart - T1 - T2);
											dDistanceSum = dDistanceSum - 0.5 * (dVmax - OutVT.v) * (OutVT.t - dTimeStart - T1 - T2);
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= (S1 + S2) && dDistanceSum < dCurrentDistance)
									{
										dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
										OutVT.v -= dAmax * dt;
										if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
										{
											break;
										}
										OutVT.t += dt;
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
								}
								else
								{
									T1 = (2.0 * S1) / (dVStart + dVmax);
									T2 = S2 / dVmax;
									T3 = (2.0 * (dCurrentDistance - S1 - S2)) / (dVmax + dVEnd);
									dTimeTotal = T1 + T2 + T3;
									while (dDistanceSum < S1)
									{
										dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
										OutVT.v += dAmax * dt;
										OutVT.t += dt;
										if (dDistanceSum > S1)
										{
											dDistanceSum = dDistanceSum - 0.5 * (OutVT.t - dTimeStart - T1) * (OutVT.v - dVmax);
											OutVT.v = dVmax;
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
									{
										dDistanceSum += dVmax * dt;
										OutVT.v = dVmax;
										OutVT.t += dt;
										if ((S - dDistanceSum) < S3)
										{
											OutVT.v = dVmax - dAmax * (OutVT.t - dTimeStart - T1 - T2);
											dDistanceSum = dDistanceSum - 0.5 * (dVmax - OutVT.v) * (OutVT.t - dTimeStart - T1 - T2);
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= (S1 + S2) && dDistanceSum < dCurrentDistance)
									{
										dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
										OutVT.v -= dAmax * dt;
										if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
										{
											break;
										}
										OutVT.t += dt;
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
								}
							}
						}
					}
					else // 沒有下一段, Vend必須為0
					{
						S = dCurrentDistance;
						dVEnd = 0.0;
						S1 = (pow(dVmax, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
						S3 = (pow(dVmax, 2) - pow(dVEnd, 2)) / (2.0 * dAmax);
						S2 = S - S1 - S3;
						if (S2 <= 0.0) // 沒有均速階段, 剩下的距離之速度規劃為三角形
						{
							S1 = (2.0 * dAmax * S - pow(dVStart, 2)) / (4.0 * dAmax);
							if (S1 < 0.0) // 只有減速
							{
								dTimeTotal = 2.0 * dCurrentDistance / (dVStart + dVEnd);
								while (dDistanceSum < dCurrentDistance)
								{
									dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
									OutVT.v -= dAmax * dt;
									if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
									{
										break;
									}
									OutVT.t += dt;
									OutVxVy.x = OutVT.v * dXRatio;
									OutVxVy.y = OutVT.v * dYRatio;
									OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
									OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
									m_arrOutVTArray.Add(OutVT);
									m_arrOutVxVyArray.Add(OutVxVy);
									m_arrOutXYArray.Add(OutPoint);
									dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
									dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
									dMinOutX = CheckMin(dMinOutX, OutPoint.x);
									dMinOutY = CheckMin(dMinOutY, OutPoint.y);
								}
							}
							else // 加速到Vm再減速
							{
								Vm = sqrt((2.0 * dAmax * S + pow(dVStart, 2)) / 2.0);
								S1 = (pow(Vm, 2) - pow(dVStart, 2)) / (2.0 * dAmax);
								S3 = (pow(Vm, 2) - pow(dVEnd, 2)) / (2.0 * dAmax);
								S2 = 0.0;
								T1 = 2.0 * S1 / (dVStart + Vm);
								T2 = 0.0;
								T3 = 2.0 * S3 / (dVEnd + Vm);
								dTimeTotal = T1 + T2 + T3;
								if (Vm > dVmax)
								{
									MessageBox(_T("Vm 不可大於 Vmax, 計算結果錯誤"));
								}
								else // 三角形
								{
									while (dDistanceSum < S1)
									{
										dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
										OutVT.v += dAmax * dt;
										OutVT.t += dt;
										if (dDistanceSum > S1)
										{
											dDistanceSum = dDistanceSum - (OutVT.t - dTimeStart - T1) * (OutVT.v - Vm);
											OutVT.v = Vm - dAmax * (OutVT.t - dTimeStart - T1);
										}
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
									while (dDistanceSum >= S1 && dDistanceSum < dCurrentDistance)
									{
										dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
										OutVT.v -= dAmax * dt;
										if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
										{
											break;
										}
										OutVT.t += dt;
										OutVxVy.x = OutVT.v * dXRatio;
										OutVxVy.y = OutVT.v * dYRatio;
										OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
										OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
										m_arrOutVTArray.Add(OutVT);
										m_arrOutVxVyArray.Add(OutVxVy);
										m_arrOutXYArray.Add(OutPoint);
										dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
										dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
										dMinOutX = CheckMin(dMinOutX, OutPoint.x);
										dMinOutY = CheckMin(dMinOutY, OutPoint.y);
									}
								}
							}
						}
						else // 剩下的距離之速度規劃還能是梯形
						{
							T1 = 2.0 * S1 / (dVStart + dVmax);
							T2 = S2 / dVmax;
							T3 = 2.0 * S3 / (dVEnd + dVmax);
							dTimeTotal = T1 + T2 + T3;
							while (dDistanceSum < S1)
							{
								dDistanceSum += OutVT.v * dt + 0.5 * dAmax * pow(dt, 2);
								OutVT.v += dAmax * dt;
								OutVT.t += dt;
								if (dDistanceSum > S1)
								{
									dDistanceSum = dDistanceSum - 0.5 * (OutVT.t - dTimeStart - T1) * (OutVT.v - dVmax);
									OutVT.v = dVmax;
								}
								OutVxVy.x = OutVT.v * dXRatio;
								OutVxVy.y = OutVT.v * dYRatio;
								OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
								OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
								m_arrOutVTArray.Add(OutVT);
								m_arrOutVxVyArray.Add(OutVxVy);
								m_arrOutXYArray.Add(OutPoint);
								dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
								dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
								dMinOutX = CheckMin(dMinOutX, OutPoint.x);
								dMinOutY = CheckMin(dMinOutY, OutPoint.y);
							}
							while (dDistanceSum >= S1 && dDistanceSum < (S1 + S2))
							{
								dDistanceSum += dVmax * dt;
								OutVT.v = dVmax;
								OutVT.t += dt;
								if ((S - dDistanceSum) < S3)
								{
									OutVT.v = dVmax - dAmax * (OutVT.t - dTimeStart - T1 - T2);
									dDistanceSum = dDistanceSum - 0.5 * (dVmax - OutVT.v) * (OutVT.t - dTimeStart - T1 - T2);
								}
								OutVxVy.x = OutVT.v * dXRatio;
								OutVxVy.y = OutVT.v * dYRatio;
								OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
								OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
								m_arrOutVTArray.Add(OutVT);
								m_arrOutVxVyArray.Add(OutVxVy);
								m_arrOutXYArray.Add(OutPoint);
								dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
								dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
								dMinOutX = CheckMin(dMinOutX, OutPoint.x);
								dMinOutY = CheckMin(dMinOutY, OutPoint.y);
							}
							while (dDistanceSum >= (S1 + S2) && dDistanceSum < dCurrentDistance)
							{
								dDistanceSum += OutVT.v * dt - 0.5 * dAmax * pow(dt, 2);
								OutVT.v -= dAmax * dt;
								if (dDistanceSum >= dCurrentDistance || OutVT.v <= dVEnd)
								{
									break;
								}
								OutVT.t += dt;
								OutVxVy.x = OutVT.v * dXRatio;
								OutVxVy.y = OutVT.v * dYRatio;
								OutPoint.x = dDistanceSum * dXRatio + m_arrPathPointArray.GetAt(i).x;
								OutPoint.y = dDistanceSum * dYRatio + m_arrPathPointArray.GetAt(i).y;
								m_arrOutVTArray.Add(OutVT);
								m_arrOutVxVyArray.Add(OutVxVy);
								m_arrOutXYArray.Add(OutPoint);
								dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
								dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
								dMinOutX = CheckMin(dMinOutX, OutPoint.x);
								dMinOutY = CheckMin(dMinOutY, OutPoint.y);
							}
						}
					}

					// 輸出此線段最後一點的參數
					OutVT.v = dVEnd; // 終止速度
					OutVT.t = dTimeStart + dTimeTotal; // 終止時間
					OutVxVy.x = OutVT.v * dXRatio;
					OutVxVy.y = OutVT.v * dYRatio;
					OutPoint.x = m_arrPathPointArray.GetAt(i + 1).x;
					OutPoint.y = m_arrPathPointArray.GetAt(i + 1).y;
					m_arrOutVTArray.Add(OutVT);
					m_arrOutVxVyArray.Add(OutVxVy);
					m_arrOutXYArray.Add(OutPoint);
					dMaxOutX = CheckMax(dMaxOutX, OutPoint.x);
					dMaxOutY = CheckMax(dMaxOutY, OutPoint.y);
					dMinOutX = CheckMin(dMinOutX, OutPoint.x);
					dMinOutY = CheckMin(dMinOutY, OutPoint.y);

					dTimeStart += dTimeTotal;
					dVStart = dVEnd;
					dDistanceNow += dCurrentDistance;
				}

				// 輸出此線段結果
				for (int i = 0; i < m_arrOutXYArray.GetSize(); i++)
				{
					fprintf(OutFile, "%lf,%lf,%lf,%lf,%lf,%lf\n", m_arrOutVTArray.GetAt(i).t, m_arrOutXYArray.GetAt(i).x, m_arrOutXYArray.GetAt(i).y, m_arrOutVTArray.GetAt(i).v, m_arrOutVxVyArray.GetAt(i).x, m_arrOutVxVyArray.GetAt(i).y);
				}

				// 計算完成, 更新參數
				m_arrOutXYArray.RemoveAll();
				m_arrOutVTArray.RemoveAll();
				m_arrOutVxVyArray.RemoveAll();
				i = iCmdFlag;
				dBeginPoint[0] = m_arrPathPointArray[m_arrPathPointArray.GetSize() - 1].x;
				dBeginPoint[1] = m_arrPathPointArray[m_arrPathPointArray.GetSize() - 1].y;
				m_arrPathPointArray.RemoveAll();
				m_arrPathVAMaxArray.RemoveAll();				
				break;
			}
			}
		}

		// close file
		m_arrOutXYArray.RemoveAll();
		m_arrOutVTArray.RemoveAll();
		m_arrOutVxVyArray.RemoveAll();
		m_arrPathPointArray.RemoveAll();
		m_arrPathVAMaxArray.RemoveAll();
		fclose(OutFile);
		OutFile = fopen("D:\\tmpResult.txt", "r");

		FILE* ResultFile;
		ResultFile = fopen(CT2A(m_cOutputPathName), "w");
		fprintf(ResultFile, "%lf,%lf,%lf,%lf,%lf,%lf\n", dTimeStart, dMaxOutV, dMaxOutX, dMinOutX, dMaxOutY, dMinOutY);
		fclose(ResultFile);
		ResultFile = fopen(CT2A(m_cOutputPathName), "a");
		char tmp[1000];
		while ((fgets(tmp, 1000, OutFile)) != NULL)
		{
			fputs(tmp, ResultFile);
		}
		fclose(ResultFile);
		fclose(OutFile);

		MessageBox(_T("計算完成"));
	}
}


void CpathspeedDlg::OnBnClickedButtonPlot()
{
	UpdateData(TRUE);
	m_bPlotFlag = TRUE;
	InvalidateRect(m_rectPlotSpace, TRUE);
	UpdateWindow();
}


void CpathspeedDlg::OnBnClickedButtonSelectOutfile()
{
	SelectFile(m_cPlotPathName);
}
