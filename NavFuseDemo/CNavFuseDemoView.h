#pragma once
#include <vector>
#include "CDataFusion.h"
// CNavFuseDemoView 视图
class CNavFuseDemoView : public CView
{
	DECLARE_DYNCREATE(CNavFuseDemoView)

protected:
	CNavFuseDemoView();           // 动态创建所使用的受保护的构造函数
	virtual ~CNavFuseDemoView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();

	void ResetTrace();  // 重置轨迹

	double m_lastX = 0.0;  // 上一帧X坐标
	double m_lastY = 0.0;  // 上一帧Y坐标
	double paramL = 200.0;
	double paramA = 200.0;
	double paramS = 200.0;
	double gpsX, gpsY; // GPS模拟坐标
	double insX, insY;  // INS模拟坐标
	bool m_isFirstDraw = true;
	CDataFusion m_fusion;
	CGPS gps;
	CINS ins;
	std::vector<CPoint> m_tracePoints;    // 真实轨迹点
	std::vector<CPoint> m_gpsTracePoints;  // GPS轨迹点
	std::vector<CPoint> m_insTracePoints;  // INS轨迹点
	std::vector<CPoint> m_fuseTracePoints;  // 原始融合轨迹点
	std::vector<CPoint> m_smoothedFuseTracePoints;  // 平滑后的融合轨迹
	static const int SMOOTH_WINDOW_SIZE = 5;        // 平滑窗口大小
	CPoint SmoothPoint(const std::vector<CPoint>& points, int index);  // 计算平滑点

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};