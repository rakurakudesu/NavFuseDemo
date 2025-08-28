// CNavFuseDemoView.cpp: 实现文件
//

#include "pch.h"
#include "NavFuseDemo.h"
#include "CNavFuseDemoView.h"
#include "CMotionModel.h"

// CNavFuseDemoView

IMPLEMENT_DYNCREATE(CNavFuseDemoView, CView)

CNavFuseDemoView::CNavFuseDemoView()
{

}

CNavFuseDemoView::~CNavFuseDemoView()
{
}

BEGIN_MESSAGE_MAP(CNavFuseDemoView, CView)
END_MESSAGE_MAP()

// CNavFuseDemoView 绘图
void CNavFuseDemoView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	CMotionModel m_mot;
	m_mot.SetMotionParam(CMotionModel::LINE, 100);
	m_mot.UpdateTruePos(1.0);
	// TODO:  在此添加绘制代码
	CPen Red_pen(PS_SOLID, 3, RGB(255, 0, 0));
	CPen* Old_pen = pDC->SelectObject(&Red_pen);
	pDC->LineTo(m_mot.m_x, m_mot.m_y+100);
}



// CNavFuseDemoView 诊断

#ifdef _DEBUG
void CNavFuseDemoView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CNavFuseDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNavFuseDemoView 消息处理程序
