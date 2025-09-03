#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <afxwin.h> 

class C7Segment {
public:
    C7Segment(CStatic* pStatic);
    ~C7Segment();

    void SetValues(double gpsX, double gpsY,
        double insX, double insY,
        double fuseX, double fuseY,
        double trueX, double trueY);  
    void Draw();
    void DrawHorizontal();
    void SetVarianceValues(double gpsVariance, double insVariance, double fuseVariance);

private:
    CStatic* m_pStatic;
    int m_digitWidth;
    int m_digitHeight;
    CString m_gpsXStr;
    CString m_gpsYStr;
    CString m_insXStr;
    CString m_insYStr;
    CString m_fuseXStr;
    CString m_fuseYStr;
    CString m_trueXStr;       // 新增：真实X坐标显示字符串
    CString m_trueYStr;       // 新增：真实Y坐标显示字符串
    CString m_gpsVarianceStr;    // GPS方差字符串
    CString m_insVarianceStr;    // INS方差字符串
    CString m_fuseVarianceStr;   // 融合方差字符串

    // 声明数码管段映射（类内静态成员）
    static const bool digitSegments[10][7];

    void DrawDigit(CDC* pDC, int x, int y, int d, bool showDot, COLORREF color);
    void DrawString(CDC* pDC, const CString& str, int x, int y, COLORREF color);
    void SplitNumber(double val, int& intPart, int& decPart);
    
};

#endif // SEVENSEGMENT_H