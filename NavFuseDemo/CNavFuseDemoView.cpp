// CNavFuseDemoView.cpp: 实现文件
//

#include "pch.h"
#include "NavFuseDemo.h"
#include "CNavFuseDemoView.h"
#include "CMotionModel.h"
#include "CSensor.h"
#include <vector>

// CNavFuseDemoView

IMPLEMENT_DYNCREATE(CNavFuseDemoView, CView)

CNavFuseDemoView::CNavFuseDemoView()
{

}

CNavFuseDemoView::~CNavFuseDemoView()
{
}

BEGIN_MESSAGE_MAP(CNavFuseDemoView, CView)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CMotionModel m_mot;
CGPS gps;
double simx, simy;
double x, y;
// CNavFuseDemoView 绘图

void CNavFuseDemoView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();

    // 1. 设置画笔和画刷
    CPen tracePen(PS_SOLID, 2, RGB(255, 0, 0)); // 红色轨迹线（2px宽）
    CPen* pOldPen = pDC->SelectObject(&tracePen);
    CBrush startBrush(RGB(0, 255, 0)); // 绿色起点
    CBrush* pOldBrush = pDC->SelectObject(&startBrush);

    // 2. 绘制轨迹（确保至少有一个点）
    if (!m_tracePoints.empty())
    {
        // 2.1 绘制起点（突出显示）
        CPoint startPoint = m_tracePoints[0];
        pDC->Ellipse(startPoint.x - 3, startPoint.y - 3,
            startPoint.x + 3, startPoint.y + 3);

        // 2.2 绘制连续线段（从第一个点到最后一个点，逐段连接）
        if (m_tracePoints.size() >= 2)
        {
            pDC->MoveTo(m_tracePoints[0]); // 移动到起点
            for (size_t i = 1; i < m_tracePoints.size(); ++i)
            {
                pDC->LineTo(m_tracePoints[i]); // 从上一点绘制到当前点
            }
        }
    }

    // 3. 还原GDI资源（避免泄漏）
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
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
void CNavFuseDemoView::OnInitialUpdate()
{

    // 初始化定时器（50ms间隔，即20Hz刷新）
    SetTimer(1, 5, NULL);

    // 关键：设置为直线运动模式（LINE），速度1m/s（参数1为速度）
    m_mot.SetMotionParam(CMotionModel::S_CURVE, 200.0, 10); 

    // 初始化GPS参数（频率10Hz，精度1.0m）
    gps.SetParam(10, 1.0);

    // 初始化起点坐标（从运动模型获取初始位置）
    double initX, initY;
    m_mot.GetTruePos(initX, initY);
    m_lastX = initX;
    m_lastY = initY;

    // 首次绘制标记置为true
    m_isFirstDraw = true;
    // 清空历史轨迹
    m_tracePoints.clear();
    CView::OnInitialUpdate();

}

double currentTime = 2;
// CNavFuseDemoView 消息处理程序
void CNavFuseDemoView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
   // 1. 时间步长固定为定时器间隔（50ms = 0.05s）
    const double dt = 0.05;
    m_mot.UpdateTruePos(dt); // 更新运动模型的真实位置
    currentTime += dt;

    // 2. 获取当前真实位置
    double currentX, currentY;
    m_mot.GetTruePos(currentX, currentY);

    // 3. 坐标转换为绘图坐标（确保在视图范围内）
    CRect clientRect;
    GetClientRect(&clientRect);
    int drawX = max(0, min((int)currentX, clientRect.right - 1));
    int drawY = max(0, min((int)currentY, clientRect.bottom - 1));

    // 4. 存储轨迹点（首次绘制时直接记录起点，后续记录当前点并更新上一帧坐标）
    if (m_isFirstDraw)
    {
        // 首次绘制：记录起点，作为轨迹的第一个点
        m_tracePoints.push_back(CPoint(drawX, drawY));
        m_isFirstDraw = false; // 标记为非首次
    }
    else
    {
        // 非首次绘制：记录当前点（用于绘制线段）
        m_tracePoints.push_back(CPoint(drawX, drawY));
    }

    // 5. 更新上一帧坐标为当前坐标（供后续绘制线段使用）
    m_lastX = currentX;
    m_lastY = currentY;

    // 6. 生成GPS模拟数据（原有逻辑保留）
    gps.GenerateData(currentTime, m_mot.m_x, m_mot.m_y, simx, simy);

    // 7. 触发重绘
    CRect updateRect(0, 0, clientRect.right, clientRect.bottom); // 整个客户区
    InvalidateRect(updateRect, FALSE); // FALSE表示不擦除背景（配合双缓冲）

    // 注意：移除原代码中的 CView::OnInitialUpdate(); 避免重复初始化
    CView::OnTimer(nIDEvent);
}
