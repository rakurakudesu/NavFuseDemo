// CNavFuseDemoView.cpp: 实现文件
//

#include "pch.h"
#include "NavFuseDemo.h"
#include "CNavFuseDemoView.h"
#include "CMotionModel.h"
#include "NavFuseDemoDlg.h"
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
CINS ins;
double simx, simy;
double x, y;
// CNavFuseDemoView 绘图

void CNavFuseDemoView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();

    // 1. 设置画笔和画刷
    CPen tracePen(PS_SOLID, 3, RGB(255, 0, 0)); // 红色轨迹线（2px宽）
    CPen* pOldPen = pDC->SelectObject(&tracePen);
    CBrush startBrush(RGB(0, 255, 0)); 
    CBrush* pOldBrush = pDC->SelectObject(&startBrush);

    // 2. 绘制轨迹
    if (!m_tracePoints.empty())
    {
        // 2.1 绘制起点
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

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);

    // 1. 获取应用程序实例指针
    CNavFuseDemoApp* pApp = (CNavFuseDemoApp*)AfxGetApp();
    if (pApp == nullptr)
        return; // 应用程序指针无效，直接返回

    // 2. 从应用程序类获取对话框指针
    CNavFuseDemoDlg* pDlg = pApp->m_pMainDlg;
    if (pDlg == nullptr)
        return; // 对话框指针无效，直接返回

    // 2. 绘制GPS轨迹（修改为圆点）
    if (!m_gpsTracePoints.empty() && pDlg->is_GPS)
    {
        // 设置GPS圆点画笔和画刷（蓝色）
        CPen gpsPen(PS_SOLID, 2, RGB(0, 0, 255));
        CPen* pGpsOldPen = pDC->SelectObject(&gpsPen);
        CBrush gpsBrush(RGB(0, 0, 255)); // 填充蓝色
        CBrush* pGpsOldBrush = pDC->SelectObject(&gpsBrush);

        // 遍历所有GPS点，逐个绘制圆点（半径2px，视觉上更像点）
        for (const auto& gpsPoint : m_gpsTracePoints)
        {
            // 绘制圆点：以gpsPoint为中心，宽高4px（半径2px）的椭圆
            pDC->Ellipse(
                gpsPoint.x - 2,  // 左
                gpsPoint.y - 2,  // 上
                gpsPoint.x + 2,  // 右
                gpsPoint.y + 2   // 下
            );
        }

        // 恢复画笔和画刷
        pDC->SelectObject(pGpsOldPen);
        pDC->SelectObject(pGpsOldBrush);
    }
    // 绘制INS连续轨迹
    if (!m_insTracePoints.empty() && pDlg->is_INS)
    {
        // 设置INS画笔（青色，1px宽，与真实轨迹区分）
        CPen insPen(PS_SOLID, 2, RGB(100, 150, 100));  
        CPen* pOldInsPen = pDC->SelectObject(&insPen);

        // 绘制连续线段（从第一个点到最后一个点）
        if (m_insTracePoints.size() >= 2)
        {
            pDC->MoveTo(m_insTracePoints[0]);  // 起点
            for (size_t i = 1; i < m_insTracePoints.size(); ++i)
            {
                pDC->LineTo(m_insTracePoints[i]);  // 连续连接
            }
        }

        // 恢复画笔
        pDC->SelectObject(pOldInsPen);
    }
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
    m_mot.SetMotionParam(m_mot.m_type, 200.0, 10);

    // 初始化GPS参数（频率10Hz，精度1.0m）
    gps.SetParam(10, 10.0);

    // 初始化INS：更高频率（如50Hz），更高短期精度（如1m）
    ins.SetParam(50, 10.0);
    ins.SetDriftRate(0.02);  // 自定义漂移率（如0.02m/s）

    // 初始化起点坐标（从运动模型获取初始位置）
    double initX, initY;
    m_mot.GetTruePos(initX, initY);
    m_lastX = initX;
    m_lastY = initY;

    // 首次绘制标记置为true
    m_isFirstDraw = true;
    // 清空历史轨迹
    m_tracePoints.clear();
    m_gpsTracePoints.clear();
    m_insTracePoints.clear();
    CView::OnInitialUpdate();

}

double currentTime = 2;
// CNavFuseDemoView 消息处理程序
void CNavFuseDemoView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
   // 1. 时间步长固定为定时器间隔（50ms = 0.05s）
    const double dt = 0.02;
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

    // 4. 存储轨迹点
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

    // 生成GPS模拟数据并存储轨迹
    double gpsX, gpsY; // GPS模拟坐标
    // 调用GPS生成数据（传入当前时间、真实坐标，获取模拟坐标）
    bool gpsValid = gps.GenerateData(currentTime, currentX, currentY, gpsX, gpsY);
    if (gpsValid) // 仅当GPS生成新数据时才记录轨迹
    {
        // 将GPS坐标转换为绘图坐标（与真实轨迹使用相同转换逻辑）
        int gpsDrawX = max(0, min((int)gpsX, clientRect.right - 1));
        int gpsDrawY = max(0, min((int)gpsY, clientRect.bottom - 1));
        m_gpsTracePoints.push_back(CPoint(gpsDrawX, gpsDrawY));
    }

    // 生成INS模拟数据并存储轨迹（与GPS逻辑对称）
    double insX, insY;  // INS模拟坐标
    bool insValid = ins.GenerateData(currentTime, currentX, currentY, insX, insY);
    if (insValid) {  // 仅当INS生成新数据时记录轨迹
        int insDrawX = max(0, min((int)insX, clientRect.right - 1));
        int insDrawY = max(0, min((int)insY, clientRect.bottom - 1));
        m_insTracePoints.push_back(CPoint(insDrawX, insDrawY));  // 需在CNavFuseDemoView中添加m_insTracePoints成员
    }

    // 7. 触发重绘
    CRect updateRect(0, 0, clientRect.right, clientRect.bottom); // 整个客户区
    InvalidateRect(updateRect, FALSE); 

    CView::OnTimer(nIDEvent);
}


void CNavFuseDemoView::ResetTrace()
{
    m_tracePoints.clear(); // 清空历史轨迹
    m_gpsTracePoints.clear();
    m_insTracePoints.clear();
    m_isFirstDraw = true;  // 重新标记首次绘制

    // 重置上一帧位置为运动模型的初始位置
    double initX, initY;
    m_mot.GetTruePos(initX, initY);
    m_lastX = initX;
    m_lastY = initY;
    OnInitialUpdate();
    Invalidate();
}