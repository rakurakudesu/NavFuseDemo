#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include <afxwin.h> // MFC����ͷ�ļ���CStatic��CPoint��CRect�ȣ�

/**
 * @file Oscilloscope.h
 * @brief ʾ������ʾ�࣬��Picture�ؼ��϶�̬����3����������
 * @note ��ӦPPT������Ҫ��-4. ģ��ʾ������ʾ���ݡ���֧��GPS/INS/�ں����ݿ��ӻ�
 * @��������STL�⣬���Զ���ṹ��+�̶��������vector<tuple>
 */

 // �Զ������ݵ�ṹ�壨���std::tuple���洢�������ݣ�
struct DataPoint {
    double time;   ///< ʱ��(s)��X�ᣩ
    double gpsX;   ///< GPS X����(m)
    double gpsY;   ///< GPS Y����(m)
    double insX;   ///< INS X����(m)
    double insY;   ///< INS Y����(m)
    double fuseX;  ///< �ں� X����(m)
    double fuseY;  ///< �ں� Y����(m)
};

class COscilloscope {
public:
    // �̶�������������ݵ����������vector��̬���ݣ��㹻ʵ��ʹ�ã�
    static const int MAX_DATA_POINTS = 100;

    /**
     * @brief ���캯�����󶨻�ͼ���壨MFC Picture�ؼ���
     * @param pStatic��ָ��IDC_STATIC_OSC�ؼ���ָ�루���ԶԻ����ࣩ
     * @return ��
     * @���ó������Ի���OnInitDialog�У�������ʾʵ��ʱ����
     * @���˼·����ؼ���ȷ����ͼ�����ù̶�����洢���ݣ����100�㣩
     */
    COscilloscope(CStatic* pStatic);

    /**
     * @brief �����������ͷ�ʾ������Դ���޶�̬��Դ��
     * @param ��
     * @return ��
     */
    ~COscilloscope();

    /**
     * @brief ������ݵ㵽���棨���ڻ������ߣ�
     * @param t����ǰʱ��(s)��X�����꣩
     * @param x1/y1��GPS���ݣ������ߣ�
     * @param x2/y2��INS���ݣ������ߣ�
     * @param x3/y3���ں����ݣ������ߣ�
     * @return ��
     * @���ó������Ի���OnTimer�У�ÿ��FuseData�����
     * @���˼·����ѭ������洢������������ʱ�����������ݣ�FIFO��
     */
    void AddDataPoint(double t, double x1, double y1, double x2, double y2, double x3, double y3);

    /**
     * @brief ����ʾ�������ߣ�˫���������˸��
     * @param ��
     * @return ��
     * @���ó������Ի���OnTimer�У�AddDataPoint�󣩡�OnPaint�У��ػ棩
     * @���˼·��
     *  1. ˫�����ͼ���Ȼ����ڴ�DC���ٸ��Ƶ���ĻDC��������˸
     *  2. ����Ԫ�أ������ᡢ�����ߡ�3���������ߣ�GPS��/INS��/�ں��̣�
     */
    void Draw();

private:
    CStatic* m_pStatic;         ///< �󶨵�MFC Picture�ؼ�����ͼ���壩
    DataPoint m_data[MAX_DATA_POINTS]; ///< �̶�����洢���ݵ㣨���vector��
    int m_dataCount;            ///< ��ǰ���ݵ�������0~MAX_DATA_POINTS��
    int m_dataIndex;            ///< ��һ�����ݵ�Ĵ洢������ѭ�������ã�

    /**
     * @brief �����������������ӳ�䣨˽�й��߽ӿڣ�
     * @param t������ʱ�䣨X�����ݣ�
     * @param val������λ�ã�Y�����ݣ�
     * @param rect���ؼ��ͻ������Σ����ط�Χ��
     * @param tMin/tMax��ʱ�䷶Χ��X�����ݷ�Χ��
     * @param valMin/valMax��λ�÷�Χ��Y�����ݷ�Χ��
     * @return CPoint��ӳ�����������꣨x,y��
     * @���ó�����Draw�����У������ݵ�תΪ�ؼ����ص�
     * @���˼·��ȷ���������ݵ�����ؼ���С����������ʾ����
     */
    CPoint MapToPixel(double t, double val, CRect& rect, double tMin, double tMax, double valMin, double valMax);

    /**
     * @brief ��ȡ���ݻ����е�ʱ��/λ�÷�Χ��˽�й��߽ӿڣ�
     * @param tMin/tMax�����ʱ�䷶Χ��X�ᣩ
     * @param valMin/valMax�����λ�÷�Χ��Y�ᣬ��GPS/INS/�ںϣ�
     * @return ��
     * @���ó�����Draw�����У���������ӳ��ķ�Χ
     * @���˼·�����������ȡ���/��Сֵ��ȷ��������������ʾ��Χ��
     */
    void GetDataRange(double& tMin, double& tMax, double& valMin, double& valMax);
};

#endif // OSCILLOSCOPE_H