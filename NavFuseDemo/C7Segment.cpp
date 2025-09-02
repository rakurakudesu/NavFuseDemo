#include "pch.h"
#include "C7Segment.h"

// ����0-9��Ӧ���߶�����ӳ�䣨�Ż��β��ֱ�����
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
    // ��ʼ����ʾ�ַ����������ظ���ʼ�����⣩
    m_gpsXStr = _T("GPS X:0.00");
    m_gpsYStr = _T("GPS Y:0.00");
    m_insXStr = _T("INS X:0.00");
    m_insYStr = _T("INS Y:0.00");
    m_fuseXStr = _T("FUSE X:0.00");
    m_fuseYStr = _T("FUSE Y:0.00");
    m_trueXStr = _T("TRUE X:0.00");  // ��������ʵ�����ʼ��
    m_trueYStr = _T("TRUE Y:0.00");
}

C7Segment::~C7Segment() {}

// ������ʵ�����������
void C7Segment::SetValues(double gpsX, double gpsY,
    double insX, double insY,
    double fuseX, double fuseY,
    double trueX, double trueY) {  // ������ʵ�������
    int intPart, decPart;

    // ��ʽ��GPS����
    SplitNumber(gpsX, intPart, decPart);
    m_gpsXStr.Format(_T("GPS X:%d.%02d"), intPart, decPart);
    SplitNumber(gpsY, intPart, decPart);
    m_gpsYStr.Format(_T("GPS Y:%d.%02d"), intPart, decPart);

    // ��ʽ��INS����
    SplitNumber(insX, intPart, decPart);
    m_insXStr.Format(_T("INS X:%d.%02d"), intPart, decPart);
    SplitNumber(insY, intPart, decPart);
    m_insYStr.Format(_T("INS Y:%d.%02d"), intPart, decPart);

    // ��ʽ���ں�����
    SplitNumber(fuseX, intPart, decPart);
    m_fuseXStr.Format(_T("FUSE X:%d.%02d"), intPart, decPart);
    SplitNumber(fuseY, intPart, decPart);
    m_fuseYStr.Format(_T("FUSE Y:%d.%02d"), intPart, decPart);

    // ��������ʽ����ʵ��������
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
    pDC->FillSolidRect(rect, RGB(50, 50, 50));  // ��ɫ����

    // �����иߣ�������ʵ������ʾ����
    int yPos = 20;
    const int lineHeight = m_digitHeight + 15;  // �Ż��м��

    // ����GPS���ݣ���ɫ��
    DrawString(pDC, m_gpsXStr, 20, yPos, RGB(255, 0, 0));
    yPos += lineHeight;
    DrawString(pDC, m_gpsYStr, 20, yPos, RGB(255, 0, 0));
    yPos += lineHeight;

    // ����INS���ݣ���ɫ��
    DrawString(pDC, m_insXStr, 20, yPos, RGB(0, 255, 0));
    yPos += lineHeight;
    DrawString(pDC, m_insYStr, 20, yPos, RGB(0, 255, 0));
    yPos += lineHeight;

    // �����ں����ݣ���ɫ��
    DrawString(pDC, m_fuseXStr, 20, yPos, RGB(0, 0, 255));
    yPos += lineHeight;
    DrawString(pDC, m_fuseYStr, 20, yPos, RGB(0, 0, 255));
    yPos += lineHeight;

    // ������������ʵ�������ݣ���ɫ���������������֣�
    DrawString(pDC, m_trueXStr, 20, yPos, RGB(200, 200, 0));
    yPos += lineHeight;
    DrawString(pDC, m_trueYStr, 20, yPos, RGB(255, 255, 0));

    m_pStatic->ReleaseDC(pDC);
}

void C7Segment::DrawString(CDC* pDC, const CString& str, int x, int y, COLORREF color) {
    int currentX = x;

    // ���Ʊ�ʶ�ı����Ż������С��
    CFont font;
    font.CreatePointFont(100, _T("SimHei"));  // ��С�������ӵ��
    CFont* pOldFont = pDC->SelectObject(&font);
    pDC->SetTextColor(RGB(255, 255, 255));
    CString label = str.Left(str.Find(':')) + _T(":");
    pDC->TextOut(currentX, y + m_digitHeight / 2 - 5, label);

    // �����ʶ��ȣ��ƶ���������ʼλ��
    CSize textSize = pDC->GetTextExtent(label);
    currentX += textSize.cx + 10;
    pDC->SelectObject(pOldFont);

    // ��ȡ���ֲ���
    CString numStr = str.Mid(str.Find(':') + 1);

    // ����������ֺ�С����
    for (int i = 0; i < numStr.GetLength(); ++i) {
        TCHAR c = numStr[i];
        if (c == '.') {
            // �Ż�С����λ�ã����ж��룩
            pDC->FillSolidRect(currentX - 3, y + m_digitHeight - 8, 6, 6, color);
        }
        else if (_istdigit(c)) {
            DrawDigit(pDC, currentX, y, c - _T('0'), false, color);
            currentX += m_digitWidth + 3;  // ��С���ּ��
        }
    }
}

// �Ż�������״�������߶α���������ƽ����
void C7Segment::DrawDigit(CDC* pDC, int x, int y, int d, bool showDot, COLORREF color) {
    if (d < 0 || d > 9) return;

    CPen pen(PS_SOLID, 2, color);
    CPen* pOldPen = pDC->SelectObject(&pen);

    // �Ż���������Э���Ŀ�߱Ⱥ��߶γ���
    int w = m_digitWidth;    // 30px
    int h = m_digitHeight;   // 50px
    int segLen = w - 8;      // �γ��ȣ���������4px��
    int midY = y + h / 2;    // �м���Y����
    const bool* segments = digitSegments[d];

    // ��a���������ߣ�ˮƽ���У��������˾��룩
    if (segments[0]) {
        pDC->MoveTo(x + 4, y + 2);          // ����㣨�����ƣ�
        pDC->LineTo(x + 4 + segLen, y + 2); // ���յ�
    }

    // ��b���������ߣ���б�Ƕ��Ż���
    if (segments[1]) {
        pDC->MoveTo(x + w - 4, y + 4);          // �����
        pDC->LineTo(x + w - 1, midY - 5);       // ���յ㣨�������ߣ�
    }

    // ��c���������ߣ����b�Գƣ�
    if (segments[2]) {
        pDC->MoveTo(x + w - 1, midY + 5);       // ����㣨�������ߣ�
        pDC->LineTo(x + w - 4, y + h - 4);      // ���յ�
    }

    // ��d���ײ����ߣ����a�Գƣ�
    if (segments[3]) {
        pDC->MoveTo(x + 4, y + h - 4);
        pDC->LineTo(x + 4 + segLen, y + h - 4);
    }

    // ��e���������ߣ����c�Գƣ�
    if (segments[4]) {
        pDC->MoveTo(x + 4, y + h - 4);
        pDC->LineTo(x + 1, midY + 5);
    }

    // ��f���������ߣ����b�Գƣ�
    if (segments[5]) {
        pDC->MoveTo(x + 1, midY - 5);
        pDC->LineTo(x + 4, y + 4);
    }

    // ��g���м���ߣ��Ӵ��Ӿ�Ч����
    if (segments[6]) {
        CPen midPen(PS_SOLID, 3, color);  // �м����Դ�
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
    // ������
    bool isNegative = val < 0;
    double absVal = fabs(val);

    // ��������
    intPart = static_cast<int>(absVal);
    // С������
    decPart = static_cast<int>((absVal - intPart) * 100 + 0.5);

    // ����С����λ
    if (decPart >= 100) 
    {
        decPart -= 100;
        intPart += 1;
    }

    // �ָ�������ʶ
    if (isNegative) {
        intPart = -intPart;
    }
}