#pragma once
#include "afxdialogex.h"

// CMotionDlg 对话框
class CNavFuseDemoView;

class CMotionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMotionDlg)

public:
	CMotionDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMotionDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MotionDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMotion();
	afx_msg void OnEnChangeEdit2();
	double LineSpeed;
	int ArcRadius;
	int S_Curve_Am;
	CNavFuseDemoView* m_pview1;
	afx_msg void OnEnChangeLinespeed();
	afx_msg void OnEnChangeArcradius();
	afx_msg void OnEnChangeAm();

};
