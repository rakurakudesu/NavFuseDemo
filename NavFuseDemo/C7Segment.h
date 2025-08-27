#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <afxwin.h> // MFC����ͷ�ļ���CStatic��CDC��CPen�ȣ�

/**
 * @file SevenSegment.h
 * @brief �������ʾ�࣬��Picture�ؼ���ģ��7������ܣ���ʾʵʱλ��
 * @note ��ӦPPT������Ҫ��-4. �������ʾ���ݡ���֧��GPS/INS/�ں����ݵ���ֵ��ʾ
 * @��������STL�⣬��MFC�����ؼ�
 */
class C7Segment {
public:
    /**
     * @brief ���캯�����󶨻�ͼ���壨MFC Picture�ؼ���
     * @param pStatic��ָ��IDC_STATIC_SEG�ؼ���ָ�루���ԶԻ����ࣩ
     * @return ��
     * @���ó������Ի���OnInitDialog�У�������ʾʵ��ʱ����
     * @���˼·�����õ�������ܳߴ磨��30px����50px��������ؼ�����
     */
    C7Segment(CStatic* pStatic);

    /**
     * @brief �����������ͷ��������Դ���޶�̬��Դ��
     * @param ��
     * @return ��
     */
    ~C7Segment();

    /**
     * @brief ����Ҫ��ʾ����ֵ��6����ֵ��GPSx/y��INSx/y���ں�x/y��
     * @param gpsX��GPS X���꣨����2λС����
     * @param gpsY��GPS Y���꣨����2λС����
     * @param insX��INS X���꣨����2λС����
     * @param insY��INS Y���꣨����2λС����
     * @param fuseX���ں� X���꣨����2λС����
     * @param fuseY���ں� Y���꣨����2λС����
     * @return ��
     * @���ó������Ի���OnTimer�У�ÿ��FuseData�����
     * @���˼·������ֵ��ʽ��Ϊ����ǩ��CString��MFC�ࣩ����Draw����ʹ��
     */
    void SetValues(double gpsX, double gpsY, double insX, double insY, double fuseX, double fuseY);

    /**
     * @brief ��������ܣ�ģ��7����ʾ��
     * @param ��
     * @return ��
     * @���ó������Ի���OnTimer�У�SetValues�󣩡�OnPaint�У��ػ棩
     * @���˼·��
     *  1. �ָ�ؼ�����Ϊ6����ʾ�飨��Ӧ6����ֵ��
     *  2. ��7�ι������ÿ�����֣�������Ӧ�Σ�
     *  3. ������ֵ��ǩ���硰GPS X:����������ɫ���ֲ�ͬ����
     */
    void Draw();

private:
    CStatic* m_pStatic;       ///< �󶨵�MFC Picture�ؼ�����ͼ���壩
    int m_digitWidth;         ///< ��������ܿ�ȣ����أ�Ĭ��30��
    int m_digitHeight;        ///< ��������ܸ߶ȣ����أ�Ĭ��50��
    CString m_gpsXStr;        ///< GPS X������ʾ�ַ���������ǩ��
    CString m_gpsYStr;        ///< GPS Y������ʾ�ַ���������ǩ��
    CString m_insXStr;        ///< INS X������ʾ�ַ���������ǩ��
    CString m_insYStr;        ///< INS Y������ʾ�ַ���������ǩ��
    CString m_fuseXStr;       ///< �ں� X������ʾ�ַ���������ǩ��
    CString m_fuseYStr;       ///< �ں� Y������ʾ�ַ���������ǩ��

    /**
     * @brief ���Ƶ�������ܣ�˽�й��߽ӿڣ�
     * @param pDC���豸������ָ�루MFC CDC��
     * @param x����������Ͻ�X���꣨���أ�
     * @param y����������Ͻ�Y���꣨���أ�
     * @param d��Ҫ��ʾ�����֣�0-9��
     * @param showDot���Ƿ���ʾС���㣨true=��ʾ��
     * @return ��
     * @���ó�����Draw�����У�����ÿ������ʱ����
     * @���˼·����7�ε���������ƶ�Ӧ�Σ���ɫ������ʾ
     */
    void DrawDigit(CDC* pDC, int x, int y, int d, bool showDot);

    /**
     * @brief ��ȡ���ֵ�����/С�����֣�˽�й��߽ӿڣ�
     * @param val��Ҫ��������ֵ����2.34��
     * @param intPart������������֣�2��
     * @param decPart�����С�����֣�34������2λ��
     * @return ��
     * @���ó�����SetValues�У���ʽ����ֵΪ����ܿ���ʾ������
     * @���˼·��ȷ��С�����ֱ���2λ�������������ʾ
     */
    void SplitNumber(double val, int& intPart, int& decPart) {
        intPart = (int)val;                  // �������֣���2.34��2��
        decPart = (int)((val - intPart) * 100 + 0.5); // С�����֣��������뵽2λ��
    }
};

#endif // SEVENSEGMENT_H
