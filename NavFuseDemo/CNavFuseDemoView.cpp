// CNavFuseDemoView.cpp: 实现文件
//

#include "pch.h"
#include "NavFuseDemo.h"
#include "CNavFuseDemoView.h"
#include "CDataFusion.h"
#include "CMotionModel.h"
#include "NavFuseDemoDlg.h"
#include "CSensor.h"
#include "CKalmanFilter.h"
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
CKalmanFilter kalman;
double gpsmse = 0, insmse = 0,fusemse = 0;

double simx, simy;
double x, y;
// CNavFuseDemoView 绘图

void CNavFuseDemoView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
        // 1. 设置画笔和画刷
        CPen tracePen(PS_SOLID, 3, RGB(255, 0, 0)); // 红色轨迹线
        CPen* pOldPen = pDC->SelectObject(&tracePen);
        CBrush startBrush(RGB(0, 255, 0));
        CBrush* pOldBrush = pDC->SelectObject(&startBrush);

        //2. 绘制轨迹
        if (!m_tracePoints.empty())
        {
            // 2.1 绘制起点
            CPoint startPoint = m_tracePoints[0];
            pDC->Ellipse(startPoint.x - 3, startPoint.y - 3,
                startPoint.x + 3, startPoint.y + 3);

            // 2.2 绘制连续线段
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
            return;

        // 2. 从应用程序类获取对话框指针
        CNavFuseDemoDlg* pDlg = pApp->m_pMainDlg;
        if (pDlg == nullptr)
            return;

        // 2. 绘制GPS轨迹
        if (!m_gpsTracePoints.empty() && pDlg->is_GPS)
        {
            // 设置GPS圆点画笔和画刷
            CPen gpsPen(PS_SOLID, 2, RGB(0, 0, 255));
            CPen* pGpsOldPen = pDC->SelectObject(&gpsPen);
            CBrush gpsBrush(RGB(0, 0, 255));
            CBrush* pGpsOldBrush = pDC->SelectObject(&gpsBrush);

            // 遍历所有GPS点，逐个绘制圆点
            for (const auto& gpsPoint : m_gpsTracePoints)
            {
                // 绘制圆点
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
            // 设置INS画笔
            CPen insPen(PS_SOLID, 2, RGB(100, 150, 100));
            CPen* pOldInsPen = pDC->SelectObject(&insPen);

            // 绘制连续线段
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

        if (!m_smoothedFuseTracePoints.empty() && pDlg->is_Filter)
        {
            CPen smoothPen(PS_SOLID, 3, RGB(0, 255, 255));
            pOldPen = pDC->SelectObject(&smoothPen);

            pDC->MoveTo(m_smoothedFuseTracePoints[0]);
            for (size_t i = 1; i < m_smoothedFuseTracePoints.size(); ++i)
            {
                pDC->LineTo(m_smoothedFuseTracePoints[i]);
            }

            pDC->SelectObject(pOldPen);
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
    CNavFuseDemoApp* pApp = (CNavFuseDemoApp*)AfxGetApp();
    CNavFuseDemoDlg* pDlg = pApp ? pApp->m_pMainDlg : nullptr;

    int initialSpeed = (pDlg != nullptr) ? pDlg->Timer_speed : 50;  
    double initialGpsFreq = (pDlg != nullptr) ? pDlg->GPS_Freq : 10;
    double initialGpsAcc = (pDlg != nullptr) ? pDlg->GPS_ACC : 10.0;
    double initialInsFreq = (pDlg != nullptr) ? pDlg ->INS_Freq : 50;
    double initialInsAcc = (pDlg != nullptr) ? pDlg->INS_ACC : 10.0;
    double initialInsDrift = (pDlg != nullptr) ? pDlg->INS_Drift : 0.02;
    // 使用初始间隔设置定时器
    if(m_begin==TRUE)
    SetTimer(1, initialSpeed, NULL);

    // 设置运动模式以及对应的参数
    switch (m_mot.m_type)
    {
    case CMotionModel::LINE:  
        m_mot.SetMotionParam(m_mot.m_type, paramL); break;
    case CMotionModel::ARC:
        m_mot.SetMotionParam(m_mot.m_type, paramA, 10); break;
    case CMotionModel::S_CURVE:
        m_mot.SetMotionParam(m_mot.m_type, paramS); break;

    }
    // 初始化GPS参数
    gps.SetParam(initialGpsFreq, initialGpsAcc);

    // 初始化INS
    ins.SetParam(initialInsFreq, initialInsAcc);
    ins.SetDriftRate(initialInsDrift); 
    ins.ResetDrift();

    //初始化滤波
    double initTrueX = 0.0;
    double initTrueY = 300.0;  // 对应 CMotionModel 中 LINE 模式的初始 y 值

    // 2. 构造初始状态向量 [x, vx, ax, y, vy, ay]
    // 假设初始速度和加速度为0（运动刚开始）
    Eigen::VectorXd initialState(6);
    initialState << initTrueX, 0.0, 0.0, initTrueY, 0.0, 0.0;

    // 3. 构造初始协方差矩阵（位置不确定性小，速度/加速度不确定性大）
    Eigen::MatrixXd initialP(6, 6);
    initialP.setZero();
    initialP.diagonal() << 1.0,   // x 位置误差（1m内）
        10.0,   // x 速度误差
        100.0,   // x 加速度误差
        1.0,   // y 位置误差（1m内）
        10.0,   // y 速度误差
        100.0;   // y 加速度误差

    // 4. 初始化卡尔曼滤波器（通过 CDataFusion 的接口）
    m_fusion.ResetKalman(initialState, initialP);

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
    if (m_isReplaying)
    {
        CView::OnTimer(nIDEvent);
        return;
    }
    double dt = 0.01;
    m_mot.UpdateTruePos(dt); // 更新运动模型的真实位置
    currentTime += dt;

    // 2. 获取当前真实位置
    double currentX, currentY;
    m_mot.GetTruePos(currentX, currentY);

    // 3. 坐标转换为绘图坐标
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
        // 非首次绘制：记录当前点
        m_tracePoints.push_back(CPoint(drawX, drawY));
    }

    // 5. 更新上一帧坐标为当前坐标
    m_lastX = currentX;
    m_lastY = currentY;

    // 生成GPS模拟数据并存储轨迹

    // 调用GPS生成数据（传入当前时间、真实坐标，获取模拟坐标）
    bool gpsValid = gps.GenerateData(currentTime, currentX, currentY, gpsX, gpsY);
    if (gpsValid) // 仅当GPS生成新数据时才记录轨迹
    {
        // 将GPS坐标转换为绘图坐标（与真实轨迹使用相同转换逻辑）
        int gpsDrawX = max(0, min((int)gpsX, clientRect.right - 1));
        int gpsDrawY = max(0, min((int)gpsY, clientRect.bottom - 1));
        m_gpsTracePoints.push_back(CPoint(gpsDrawX, gpsDrawY));
    }

    // 生成INS模拟数据并存储轨迹
   
    bool insValid = ins.GenerateData(currentTime, currentX, currentY, insX, insY);
    if (insValid) {  // 仅当INS生成新数据时记录轨迹
        int insDrawX = max(0, min((int)insX, clientRect.right - 1));
        int insDrawY = max(0, min((int)insY, clientRect.bottom - 1));
        m_insTracePoints.push_back(CPoint(insDrawX, insDrawY)); 
    }

    double fuseX, fuseY;  // 融合后的坐标
    // 6.1 计算传感器MSE（用于加权融合的权重计算）
    m_fusion.CalcSensorMSE(currentX, currentY, gpsX, gpsY, insX, insY,gpsmse,insmse);

    // 6.2 从对话框获取当前选择的融合算法（如卡尔曼、加权等）
    CNavFuseDemoApp* pApp = (CNavFuseDemoApp*)AfxGetApp();
    CNavFuseDemoDlg* pDlg = pApp ? pApp->m_pMainDlg : nullptr;
    if (pDlg != nullptr) 
    {
        m_fusion.SetAlgorithm(pDlg->FuseType);  
    }

    // 6.3 执行融合
    m_fusion.FuseData(currentX, currentY, gpsX, gpsY, insX, insY, fuseX, fuseY,fusemse);

    // 6.4 存储融合轨迹
    if (m_fuseTracePoints.empty()) {
        int fuseDrawX = max(0, min((int)(fuseX + 0.5), clientRect.right - 1));
        int fuseDrawY = max(0, min((int)(fuseY + 0.5), clientRect.bottom - 1));
        m_fuseTracePoints.push_back(CPoint(fuseDrawX, fuseDrawY));
    }
    else 
    {
        // 非第一个点：计算与上一点的距离
        CPoint lastPoint = m_fuseTracePoints.back();  // 上一个点
        int dx = abs((int)fuseX - lastPoint.x);
        int dy = abs((int)fuseY - lastPoint.y);
        int distance = (int)sqrt(dx * dx + dy * dy);  // 欧氏距离

        // 仅保留距离小于阈值的点（如10单位，可调整）
        if (distance < 120) {
            int fuseDrawX = max(0, min((int)(fuseX + 0.5), clientRect.right - 1));
            int fuseDrawY = max(0, min((int)(fuseY + 0.5), clientRect.bottom - 1));
            m_fuseTracePoints.push_back(CPoint(fuseDrawX, fuseDrawY));
        }
    }
    if (pDlg != nullptr)
    {
        pDlg->Update7Segment(gpsX, gpsY, insX, insY, fuseX, fuseY, currentX, currentY);
    }
    // 6.5 计算平滑后的融合轨迹
    if (!m_fuseTracePoints.empty())
    {
        CPoint smoothedPoint = SmoothPoint(m_fuseTracePoints, m_fuseTracePoints.size() - 1);
        m_smoothedFuseTracePoints.push_back(smoothedPoint);
    }
   
    // 7. 触发重绘
    CRect updateRect(0, 0, clientRect.right, clientRect.bottom); // 整个客户区
    InvalidateRect(updateRect, FALSE); 

    CView::OnTimer(nIDEvent);
}

CPoint CNavFuseDemoView::SmoothPoint(const std::vector<CPoint>& points, int index)
{
    if (points.empty()) return CPoint(0, 0);

    int start = max(0, index - SMOOTH_WINDOW_SIZE / 2);
    int end = min((int)points.size() - 1, index + SMOOTH_WINDOW_SIZE+1 / 2);

    int sumX = 0, sumY = 0;
    int count = 0;

    for (int i = start; i <= end; ++i)
    {
        sumX += points[i].x;
        sumY += points[i].y;
        count++;
    }

    return CPoint(sumX / count, sumY / count);
}

void CNavFuseDemoView::ResetTrace()
{
    m_tracePoints.clear(); // 清空历史轨迹
    m_gpsTracePoints.clear();
    m_insTracePoints.clear();
    m_fuseTracePoints.clear();
    m_smoothedFuseTracePoints.clear();  

    m_isFirstDraw = true;  // 重新标记首次绘制

    // 重置上一帧位置为运动模型的初始位置
    double initX, initY;
    m_mot.GetTruePos(initX, initY);
    m_lastX = initX;
    m_lastY = initY;

    // 3. 重置卡尔曼滤波器初始状态（基于运动模型初始位置）
    Eigen::VectorXd initialState(6);
    initialState << 0.0, 0.0, 0.0, 300.0, 0.0, 0.0;  // [x, vx, ax, y, vy, ay]
    Eigen::MatrixXd initialP(6, 6);
    initialP.setZero();
    initialP.diagonal() << 1.0, 10.0, 100.0, 1.0, 10.0, 100.0;  // 初始协方差
    m_fusion.ResetKalman(initialState, initialP);

    OnInitialUpdate();
    Invalidate();
}