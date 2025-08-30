
// NavFuseDemoDlg.h: 头文件
//

#pragma once


// CNavFuseDemoDlg 对话框
class CNavFuseDemoDlg : public CDialogEx
{
// 构造
public:
	CNavFuseDemoDlg(CWnd* pParent = nullptr);	// 标准构造函数
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVFUSEDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedLine();
	afx_msg void OnBnClickedArc();
	afx_msg void OnBnClickedSCurve();
	afx_msg void OnStnClickedDraw();
	int Timer_speed=50;
	afx_msg void OnEnChangeEditspeed();
	BOOL is_GPS;
	BOOL is_INS;
	afx_msg void OnBnClickedGps();
	afx_msg void OnBnClickedIns();
	double edit_step;
	afx_msg void OnEnChangeStep();
	double GPS_Freq;
	double GPS_ACC;
	double INS_Freq;
	double INS_ACC;
	double INS_Drift;
	afx_msg void OnEnChangeGpsFreq();
	afx_msg void OnEnChangeGpsAcc();
	afx_msg void OnEnChangeInsFreq();
	afx_msg void OnEnChangeInsAcc();
	afx_msg void OnEnChangeInsDrift();
};

