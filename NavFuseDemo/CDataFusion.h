#ifndef DATAFUSION_H
#define DATAFUSION_H

#include "CSensor.h"
#include "CKalmanFilter.h"

/**
 * @file DataFusion.h
 * @brief �����ں��࣬ʵ��PPTҪ���4���ں��㷨����Ȩ+��������
 * @note Ŀǰ������Ȩƽ�����������˲�
 */
class CDataFusion {
public:
    /**
     * @brief �ں��㷨ö�٣�����PPTҪ��ļ�Ȩ+�������㷨
     * @enum WEIGHTED����Ȩƽ����������������Ȩ��
     * @enum KALMAN���������˲������ڶ�̬ϵͳģ�͵����Ź���
     */
    enum AlgType { WEIGHTED, KALMAN, GPS_ONLY};

    /**
     * @brief ���캯������ʼ���ںϵ�Ĭ��״̬
     * @param ��
     * @return ��
     * @���ó����������ں���ʾ�Ի����ʼ��ʱʵ����
     * @���˼·��Ĭ���㷨Ϊ��Ȩƽ����MSE��ʼΪ0����ʼ���������˲�ʵ��
     */
    CDataFusion();

    /**
     * @brief �����������ͷ��ں�����Դ���޶�̬��Դ��
     * @param ��
     * @return ��
     */
    ~CDataFusion();

    /**
     * @brief ����GPS/INS�ľ�����MSE�������ڼ�Ȩ�ںϵ�Ȩ��
     * @param trueX/trueY����ʵX/Y����(m)������CMotionModel��
     * @param gpsX/gpsY��GPSģ��X/Y����(m)������CGPS��
     * @param insX/insY��INSģ��X/Y����(m)������CINS��
     * @return ��
     * @���ó������Ի���OnTimer�У�FuseData֮ǰ����
     */
    void CalcSensorMSE(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY,double &gpsmse,double &insmse);

    /**
     * @brief ִ�������ںϣ����Ľӿڣ����ö�Ӧ�㷨
     * @param trueX/trueY����ʵ����(���ڼ����ں�MSE)
     * @param gpsX/gpsY��GPS����(������1)
     * @param insX/insY��INS����(������2)
     * @param fuseX/fuseY������ںϽ��
     * @return ��
     * @���ó������Ի���OnTimer�У�CalcSensorMSE֮�����
     */
    void CDataFusion::FuseData(double trueX, double trueY,
        double gpsX, double gpsY,
        double insX, double insY,
        double& fuseX, double& fuseY,double & fusemse);

    /**
     * @brief ��ȡ�����ݵ�MSE�����ڽ�����ʾ
     * @param gpsMSE/insMSE/fuseMSE����������������ݵ�MSE
     * @return ��
     * @���ó������Ի���OnTimer�У�FuseData֮�����
     */
    void GetMSE(double& gpsMSE, double& insMSE, double& fuseMSE);

    /**
     * @brief �����ں��㷨
     * @param alg���㷨���ͣ�WEIGHTED/KALMAN�ȣ�
     * @return ��
     */
    void SetAlgorithm(AlgType alg);

    /**
     * @brief ��ȡ��ǰ�ں��㷨
     * @return ��ǰ�㷨����
     */
    AlgType GetAlgorithm() const;

    /**
     * @brief ���ÿ������˲���
     * @param initialState����ʼ״̬����
     * @param initialP����ʼЭ�������
     * @return ��
     */
    void ResetKalman(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP)
    {
        m_kalman.Init(initialState, initialP);
    }

private:
    AlgType m_alg;              ///< ��ǰ�ں��㷨����
    double m_gpsMSE;            ///< GPS�ľ�����MSE��
    double m_insMSE;            ///< INS�ľ�����MSE��
    double m_fuseMSE;           ///< �ں����ݵľ�����MSE��
    CKalmanFilter m_kalman;     ///< �������˲���ʵ��������KALMAN�㷨��

    /**
     * @brief ��Ȩ�ں��㷨ʵ�֣�PPTҪ��
     * @param gpsX/gpsY��GPS����
     * @param insX/insY��INS����
     * @param fuseX/fuseY������ںϽ��
     * @return ��
     * @���ó�����FuseData�У�m_alg=WEIGHTEDʱ����
     */
    void WeightedFusion(double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

    /**
     * @brief �������˲��㷨ʵ�֣�PPT��ʽ��
     * @param gpsX/gpsY��GPS����(�۲�ֵ)
     * @param insX/insY��INS����(״̬Ԥ��Դ)
     * @param dt��ʱ����
     * @param fuseX/fuseY������ںϽ��(״̬�е�λ��)
     * @return ��
     * @���ó�����FuseData�У�m_alg=KALMANʱ����
     */
    void CDataFusion::KalmanFusion(double gpsX, double gpsY,
        double insX, double insY,
        double dt,
        double& fuseX, double& fuseY);
};

#endif // DATAFUSION_H