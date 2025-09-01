// CMotionDlg.cpp: 实现文件
//

#include "pch.h"
#include "NavFuseDemo.h"
#include "afxdialogex.h"
#include "CMotionDlg.h"
#include "CNavFuseDemoView.h"


// CMotionDlg 对话框

IMPLEMENT_DYNAMIC(CMotionDlg, CDialogEx)

CMotionDlg::CMotionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MotionDlg, pParent)
	, LineSpeed(200.0)
	, ArcRadius(200.0)
	, S_Curve_Am(200.0)
{

}

CMotionDlg::~CMotionDlg()
{
}

void CMotionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, EDIT_LineSpeed, LineSpeed);
	DDX_Text(pDX, EDIT_ArcRadius, ArcRadius);
	DDX_Text(pDX, EDIT_Am, S_Curve_Am);
}


BEGIN_MESSAGE_MAP(CMotionDlg, CDialogEx)
	ON_EN_CHANGE(EDIT_LineSpeed, &CMotionDlg::OnEnChangeLinespeed)
	ON_EN_CHANGE(EDIT_ArcRadius, &CMotionDlg::OnEnChangeArcradius)
	ON_EN_CHANGE(EDIT_Am, &CMotionDlg::OnEnChangeAm)
END_MESSAGE_MAP()


// CMotionDlg 消息处理程序

void CMotionDlg::OnEnChangeLinespeed()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_pview1->paramL = LineSpeed;
	m_pview1->ResetTrace();
}

void CMotionDlg::OnEnChangeArcradius()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_pview1->paramA = ArcRadius;
	m_pview1->ResetTrace();
}

void CMotionDlg::OnEnChangeAm()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_pview1->paramS = S_Curve_Am;
	UpdateData(FALSE);
	m_pview1->ResetTrace();
}

