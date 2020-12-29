
// path_speedDlg.h: 標頭檔
//

#pragma once

#define BEGIN 0
#define SPEED 1
#define ACC 2
#define LINEXY 3
#define FLINEXY 4
#define ARCXY 5
#define FARCXY 6
#define END 7
#define PI acos(-1)
#define PLOTXY 0
#define PLOTXT 1
#define PLOTYT 2
#define PLOTVXT 3
#define PLOTVYT 4
#define PLOTAXT 5
#define PLOTAYT 6
#define PLOTGAP 70
#define PLOTXLABELGAP 50
#define PLOTYLABELGAP 30
#define PLOTORGGAP 20
#define PLOTYMAXMINGAP 60
#define XMIN 0
#define XMAX 1
#define YMIN 2
#define YMAX 3

typedef struct {
	int m_iID;
	double m_dParams[5];
}CMD;

typedef struct {
	double x;
	double y;
}POINTXY;

typedef struct {
	double v;
	double t;
}SPEEDVT;

typedef struct {
	double m_dVmax;
	double m_dAmax;
}PARAMS_VA_MAX;

// CpathspeedDlg 對話方塊
class CpathspeedDlg : public CDialogEx
{
// 建構
public:
	CpathspeedDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PATH_SPEED_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	double m_dSampleTime, m_dThetaMax;
	bool m_bPlotFlag, m_bZoomFlag, m_bSimulationFlag;
	double m_dPlotXmin, m_dPlotXmax, m_dPlotYmin, m_dPlotYmax;
	double CheckTheta(double CosTheta, double SinTheta, double Theta);
	double CheckMax(double a, double b);
	double CheckMin(double a, double b);
	void PlotResult();
	void SelectInputFile(CString& pathName);
	void SelectOutputFile(CString& pathName);
	void ReadCommand(CString pathName, CArray<CMD, CMD&>& CmdArray);
	void SelectPathVAmax(int cmd, double speed[2], double acc[2], CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&>& PathVAMaxArray);
	void CalArcPoint(POINTXY BeginP, CMD Cmd, CArray<POINTXY, POINTXY&>& PathPointArray, double speed[2], double acc[2], CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&>& PathVAMaxArray);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSelectCommand();
	afx_msg void OnBnClickedButtonCaculate();
	afx_msg void OnBnClickedButtonPlot();
	afx_msg void OnBnClickedButtonZoom();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonSelectPlot();
	afx_msg void OnBnClickedButtonSimulation();
	CDC m_dcMem;
	CBitmap m_bmp;
	CRect m_rectPlotSpace, m_rectChartSpace;
	CString m_cInputPathName, m_cOutputPathName, m_cPlotPathName;
	CArray<CMD, CMD&> m_arrCmdArray;
	CArray<POINTXY, POINTXY&> m_arrOutXYArray, m_arrOutVxVyArray, m_arrPathPointArray;
	CArray<SPEEDVT, SPEEDVT&> m_arrOutVTArray;
	CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&> m_arrPathVAMaxArray;
	CComboBox m_cbPlotType;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeComboPlotType();
	FILE* fpResult;
	clock_t m_tSimuStart, m_tSimuEnd, m_tSimuStartCheck, m_tSimuEndCheck;
};