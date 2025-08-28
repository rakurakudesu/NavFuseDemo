/**
 * @file CMotionModel.h
 * @brief �������˶�ģ���࣬��������������ʵλ�����ݣ�ֱ��/Բ��/S�����ߣ�
 * @note ��ӦPPT������Ҫ��-1. 3���˶���ʽѡ�񡱣�Ϊ������ģ���ṩ��׼λ��
 */
#ifndef CMOTIONMODEL_H
#define CMOTIONMODEL_H

class CMotionModel {
public:
    /**
     * @brief �˶�����ö�٣�����PPTҪ���3���˶���ʽ��
     * @enum LINE��ֱ���˶������ٶȲ�����
     * @enum ARC��Բ���˶�����뾶�����ٶȲ�����
     * @enum S_CURVE��S�������˶��������������
     */
    enum MotionType { LINE, ARC, S_CURVE };

    /**
     * @brief ���캯������ʼ���˶�Ĭ��״̬
     * @param ��
     * @return ��
     * @���ó������Ի�����CNavFuseDemoDlg��ʼ��ʱ����ʵ��
     * @���˼·��Ĭ���˶�Ϊֱ�ߣ���ʼλ��(0,0)���ۼ�ʱ���0s��ʼ
     */
    CMotionModel();

    /**
     * @brief �����������ͷ��˶�ģ����Դ����ǰ�޶�̬��Դ�������淶��
     * @param ��
     * @return ��
     * @���ó�����ʵ������ʱ����Ի���رգ�
     */
    ~CMotionModel();

    /**
     * @brief ��ȡ��ǰʵ����ָ�루����������
     * @return CMotionModel*����ǰ�����ָ��
     * @���ó������ⲿ����Ҫֱ�ӷ��ʵ�ǰʵ��ʱʹ��
     * @ע�⣺ָ����Ч���뵱ǰʵ��һ�£�ʵ�����ٺ�ָ��ʧЧ
     */
    CMotionModel* GetInstancePtr() { return this; }

    /**
     * @brief �����˶����ͼ���Ӧ�������������ýӿڣ�
     * @param type���˶����ͣ�LINE/ARC/S_CURVE�����Խ��浥ѡ��ť��
     * @param param1����������LINE=�ٶ�(m/s)��ARC=�뾶(m)��S_CURVE=���(m)��
     * @param param2������������ARC����ٶ�(rad/s)�������˶���0���ɣ�
     * @return ��
     * @���ó������Ի��򡰿�ʼ����ť���ʱ����ȡ������������
     * @���˼·��ͳһ������ڣ�����Ϊ��ͬ�˶�����������ú���
     */
    void SetMotionParam(MotionType type, double param1, double param2 = 0.0);

    /**
     * @brief ��ʱ�䲽��������ʵλ�ã����ļ���ӿڣ�
     * @param dt��ʱ�䲽��(s)�����ڶ�ʱ�������Ĭ��0.1s����10Hzˢ�£�
     * @return ��
     * @���ó������Ի���OnTimer�¼��У�ÿ�ζ�ʱ����������
     * @���˼·��
     *  - ֱ���˶���x = �ٶ� * �ۼ�ʱ�䣬y=0�����٣�
     *  - Բ���˶���x=�뾶*cos(���ٶ�*�ۼ�ʱ��)��y=�뾶*sin(���ٶ�*�ۼ�ʱ��)
     *  - S�����ߣ�x=�ۼ�ʱ�䣬y=���*sin(�ۼ�ʱ��)�����ҹ켣��
     */
    void UpdateTruePos(double dt);

    /**
     * @brief ��ȡ��ǰ��ʵλ�ã���������ӿڣ�
     * @param x�������������ʵX����(m)�����ô��ݣ����������ഫ��׼ֵ��
     * @param y�������������ʵY����(m)�����ô��ݣ����������ഫ��׼ֵ��
     * @return ��
     * @���ó������Ի���OnTimer�У�����λ�ú���ã���CGPS/CINS����ʵֵ
     * @���˼·�������ڲ�λ�ñ�����ͨ���ӿڰ�ȫ��������Ϸ�װԭ��
     */
    void GetTruePos(double& x, double& y);

    MotionType m_type;    ///< ��ǰ�˶����ͣ�LINE/ARC/S_CURVE��
    double m_time;        ///< �˶��ۼ�ʱ��(s)�����ڼ���λ��
    double m_x;           ///< ��ʵX����(m)
    double m_y;           ///< ��ʵY����(m)
    double m_radius;      ///< Բ���˶��뾶(m)����ARC����ʹ�ã�
    double m_omega;       ///< Բ���˶����ٶ�(rad/s)����ARC����ʹ�ã�
    double m_sAmp;        ///< S���������(m)����S_CURVE����ʹ�ã�
    double m_lineSpeed;   ///< ֱ���˶��ٶ�(m/s)����LINE����ʹ�ã�
};

#endif // CMOTIONMODEL_H