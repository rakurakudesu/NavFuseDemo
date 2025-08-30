#pragma once
#include <vector>

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

	void ClearTrace();  //清空轨迹
	void ResetTrace();  //重置轨迹

	double m_lastX = 0.0;  // 上一帧X坐标
	double m_lastY = 0.0;  // 上一帧Y坐标
	bool m_isFirstDraw = true;  
	std::vector<CPoint> m_tracePoints;    // 真实轨迹点
	std::vector<CPoint> m_gpsTracePoints;  // GPS轨迹点
	std::vector<CPoint> m_insTracePoints;  // INS轨迹点

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


