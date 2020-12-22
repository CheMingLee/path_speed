
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
#define PLOTVT 0
#define PLOTXY 1
#define PLOTXT 2
#define PLOTYT 3
#define PLOTVXT 4
#define PLOTVYT 5
#define PLOTGAP 25
#define PLOTXLABELGAP 10
#define PLOTYLABELGAP 20

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
	double m_dSampleTime = 1;
	double m_dThetaMax = 10;
	bool m_bPlotFlag = FALSE;
	double CheckTheta(double CosTheta, double SinTheta, double Theta);
	double CheckMax(double a, double b);
	double CheckMin(double a, double b);
	void SelectFile(CString& pathName);
	void ReadCommand(CString pathName, CArray<CMD, CMD&>& CmdArray);
	void SelectPathVAmax(int cmd, double speed[2], double acc[2], CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&>& PathVAMaxArray);
	void CalArcPoint(POINTXY BeginP, CMD Cmd, CArray<POINTXY, POINTXY&>& PathPointArray, double speed[2], double acc[2], CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&>& PathVAMaxArray);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonPlot();
	afx_msg void OnBnClickedButtonSelectOutfile();
	CString m_cInputPathName;
	CString m_cOutputPathName = _T("D:\\result.txt");
	CString m_cPlotPathName = m_cOutputPathName;
	CArray<CMD, CMD&> m_arrCmdArray;
	CArray<POINTXY, POINTXY&> m_arrOutXYArray;
	CArray<SPEEDVT, SPEEDVT&> m_arrOutVTArray;
	CArray<POINTXY, POINTXY&> m_arrOutVxVyArray;
	CArray<POINTXY, POINTXY&> m_arrPathPointArray;
	CArray<PARAMS_VA_MAX, PARAMS_VA_MAX&> m_arrPathVAMaxArray;
	CRect m_rectPlotSpace;
	CComboBox m_cbPlotType;
};