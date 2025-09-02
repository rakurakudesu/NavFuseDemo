#include "pch.h"
#include "C7Segment.h"

// 数字0-9对应的七段亮灭映射（优化段布局比例）
const bool C7Segment::digitSegments[10][7] =
{
    {1,1,1,1,1,1,0}, // 0
    {0,1,1,0,0,0,0}, // 1
    {1,1,0,1,1,0,1}, // 2
    {1,1,1,1,0,0,1}, // 3
    {0,1,1,0,0,1,1}, // 4
    {1,0,1,1,0,1,1}, // 5
    {1,0,1,1,1,1,1}, // 6
    {1,1,1,0,0,0,0}, // 7
    {1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1}  // 9
};

C7Segment::C7Segment(CStatic* pStatic)
    : m_pStatic(pStatic), m_digitWidth(30), m_digitHeight(50)
{
    // 初始化显示字符串（修正重复初始化问题）
    m_gpsXStr = _T("GPS X:0.00");
    m_gpsYStr = _T("GPS Y:0.00");
    m_insXStr = _T("INS X:0.00");
    m_insYStr = _T("INS Y:0.00");
    m_fuseXStr = _T("FUSE X:0.00");
    m_fuseYStr = _T("FUSE Y:0.00");
    m_trueXStr = _T("TRUE X:0.00");  // 新增：真实坐标初始化
    m_trueYStr = _T("TRUE Y:0.00");
}

C7Segment::~C7Segment() {}

// 新增真实坐标参数处理
void C7Segment::SetValues(double gpsX, double gpsY,
    double insX, double insY,
    double fuseX, double fuseY,
    double trueX, double trueY) {  // 新增真实坐标参数
    int intPart, decPart;

    // 格式化GPS数据
    SplitNumber(gpsX, intPart, decPart);
    m_gpsXStr.Format(_T("GPS X:%d.%02d"), intPart, decPart);
    SplitNumber(gpsY, intPart, decPart);
    m_gpsYStr.Format(_T("GPS Y:%d.%02d"), intPart, decPart);

    // 格式化INS数据
    SplitNumber(insX, intPart, decPart);
    m_insXStr.Format(_T("INS X:%d.%02d"), intPart, decPart);
    SplitNumber(insY, intPart, decPart);
    m_insYStr.Format(_T("INS Y:%d.%02d"), intPart, decPart);

    // 格式化融合数据
    SplitNumber(fuseX, intPart, decPart);
    m_fuseXStr.Format(_T("FUSE X:%d.%02d"), intPart, decPart);
    SplitNumber(fuseY, intPart, decPart);
    m_fuseYStr.Format(_T("FUSE Y:%d.%02d"), intPart, decPart);

    // 新增：格式化真实坐标数据
    SplitNumber(trueX, intPart, decPart);
    m_trueXStr.Format(_T("TRUE X:%d.%02d"), intPart, decPart);
    SplitNumber(trueY, intPart, decPart);
    m_trueYStr.Format(_T("TRUE Y:%d.%02d"), intPart, decPart);
}

void C7Segment::Draw() {
    if (!m_pStatic) return;

    CDC* pDC = m_pStatic->GetDC();
    CRect rect;
    m_pStatic->GetClientRect(&rect);
    pDC->FillSolidRect(rect, RGB(50, 50, 50));  // 黑色背景

    // 调整行高，增加真实坐标显示区域
    int yPos = 20;
    const int lineHeight = m_digitHeight + 15;  // 优化行间距

    // 绘制GPS数据（红色）
    DrawString(pDC, m_gpsXStr, 20, yPos, RGB(255, 0, 0));
    yPos += lineHeight;
    DrawString(pDC, m_gpsYStr, 20, yPos, RGB(255, 0, 0));
    yPos += lineHeight;

    // 绘制INS数据（绿色）
    DrawString(pDC, m_insXStr, 20, yPos, RGB(0, 255, 0));
    yPos += lineHeight;
    DrawString(pDC, m_insYStr, 20, yPos, RGB(0, 255, 0));
    yPos += lineHeight;

    // 绘制融合数据（蓝色）
    DrawString(pDC, m_fuseXStr, 20, yPos, RGB(0, 0, 255));
    yPos += lineHeight;
    DrawString(pDC, m_fuseYStr, 20, yPos, RGB(0, 0, 255));
    yPos += lineHeight;

    // 新增：绘制真实坐标数据（黄色，与其他数据区分）
    DrawString(pDC, m_trueXStr, 20, yPos, RGB(200, 200, 0));
    yPos += lineHeight;
    DrawString(pDC, m_trueYStr, 20, yPos, RGB(255, 255, 0));

    m_pStatic->ReleaseDC(pDC);
}

void C7Segment::DrawString(CDC* pDC, const CString& str, int x, int y, COLORREF color) {
    int currentX = x;

    // 绘制标识文本（优化字体大小）
    CFont font;
    font.CreatePointFont(100, _T("SimHei"));  // 稍小字体避免拥挤
    CFont* pOldFont = pDC->SelectObject(&font);
    pDC->SetTextColor(RGB(255, 255, 255));
    CString label = str.Left(str.Find(':')) + _T(":");
    pDC->TextOut(currentX, y + m_digitHeight / 2 - 5, label);

    // 计算标识宽度，移动到数字起始位置
    CSize textSize = pDC->GetTextExtent(label);
    currentX += textSize.cx + 10;
    pDC->SelectObject(pOldFont);

    // 提取数字部分
    CString numStr = str.Mid(str.Find(':') + 1);

    // 逐个绘制数字和小数点
    for (int i = 0; i < numStr.GetLength(); ++i) {
        TCHAR c = numStr[i];
        if (c == '.') {
            // 优化小数点位置（居中对齐）
            pDC->FillSolidRect(currentX - 3, y + m_digitHeight - 8, 6, 6, color);
        }
        else if (_istdigit(c)) {
            DrawDigit(pDC, currentX, y, c - _T('0'), false, color);
            currentX += m_digitWidth + 3;  // 缩小数字间距
        }
    }
}

// 优化数字形状：调整线段比例和连接平滑度
void C7Segment::DrawDigit(CDC* pDC, int x, int y, int d, bool showDot, COLORREF color) {
    if (d < 0 || d > 9) return;

    CPen pen(PS_SOLID, 2, color);
    CPen* pOldPen = pDC->SelectObject(&pen);

    // 优化比例：更协调的宽高比和线段长度
    int w = m_digitWidth;    // 30px
    int h = m_digitHeight;   // 50px
    int segLen = w - 8;      // 段长度（两侧留边4px）
    int midY = y + h / 2;    // 中间线Y坐标
    const bool* segments = digitSegments[d];

    // 段a：顶部横线（水平居中，缩短两端距离）
    if (segments[0]) {
        pDC->MoveTo(x + 4, y + 2);          // 左起点（稍右移）
        pDC->LineTo(x + 4 + segLen, y + 2); // 右终点
    }

    // 段b：右上竖线（倾斜角度优化）
    if (segments[1]) {
        pDC->MoveTo(x + w - 4, y + 4);          // 上起点
        pDC->LineTo(x + w - 1, midY - 5);       // 下终点（靠近中线）
    }

    // 段c：右下竖线（与段b对称）
    if (segments[2]) {
        pDC->MoveTo(x + w - 1, midY + 5);       // 上起点（靠近中线）
        pDC->LineTo(x + w - 4, y + h - 4);      // 下终点
    }

    // 段d：底部横线（与段a对称）
    if (segments[3]) {
        pDC->MoveTo(x + 4, y + h - 4);
        pDC->LineTo(x + 4 + segLen, y + h - 4);
    }

    // 段e：左下竖线（与段c对称）
    if (segments[4]) {
        pDC->MoveTo(x + 4, y + h - 4);
        pDC->LineTo(x + 1, midY + 5);
    }

    // 段f：左上竖线（与段b对称）
    if (segments[5]) {
        pDC->MoveTo(x + 1, midY - 5);
        pDC->LineTo(x + 4, y + 4);
    }

    // 段g：中间横线（加粗视觉效果）
    if (segments[6]) {
        CPen midPen(PS_SOLID, 3, color);  // 中间线稍粗
        CPen* pOldMidPen = pDC->SelectObject(&midPen);
        pDC->MoveTo(x + 4, midY);
        pDC->LineTo(x + 4 + segLen, midY);
        pDC->SelectObject(pOldMidPen);
    }

    if (showDot) {
        pDC->FillSolidRect(x + w + 2, y + h - 8, 6, 6, color);
    }

    pDC->SelectObject(pOldPen);
}

void C7Segment::SplitNumber(double val, int& intPart, int& decPart) {
    // 处理负数
    bool isNegative = val < 0;
    double absVal = fabs(val);

    // 整数部分
    intPart = static_cast<int>(absVal);
    // 小数部分
    decPart = static_cast<int>((absVal - intPart) * 100 + 0.5);

    // 处理小数进位
    if (decPart >= 100) 
    {
        decPart -= 100;
        intPart += 1;
    }

    // 恢复负数标识
    if (isNegative) {
        intPart = -intPart;
    }
}