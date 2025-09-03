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
    CString m_trueXStr;       // ��������ʵX������ʾ�ַ���
    CString m_trueYStr;       // ��������ʵY������ʾ�ַ���
    CString m_gpsVarianceStr;    // GPS�����ַ���
    CString m_insVarianceStr;    // INS�����ַ���
    CString m_fuseVarianceStr;   // �ںϷ����ַ���

    // ��������ܶ�ӳ�䣨���ھ�̬��Ա��
    static const bool digitSegments[10][7];

    void DrawDigit(CDC* pDC, int x, int y, int d, bool showDot, COLORREF color);
    void DrawString(CDC* pDC, const CString& str, int x, int y, COLORREF color);
    void SplitNumber(double val, int& intPart, int& decPart);
    
};

#endif // SEVENSEGMENT_H