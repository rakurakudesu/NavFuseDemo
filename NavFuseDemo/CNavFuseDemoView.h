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
	virtual void CNavFuseDemoView::OnInitialUpdate();
private:
	double m_lastX = 0.0;  // 上一帧X坐标
	double m_lastY = 0.0;  // 上一帧Y坐标
	bool m_isFirstDraw = true;  // 是否首次绘制（用于初始化起点）
	std::vector<CPoint> m_tracePoints; // 新增：存储所有轨迹点
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


