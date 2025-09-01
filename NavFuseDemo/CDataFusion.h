#ifndef DATAFUSION_H
#define DATAFUSION_H

#include "CSensor.h"
#include "CKalmanFilter.h"

/**
 * @file DataFusion.h
 * @brief �����ں��࣬ʵ��PPTҪ���4���ں��㷨������+���ף�
 * @note ��������Ȩ��͡��������˲������ף��޼��������������˲�
 * @��������STL�⣬�������Զ��崫������Ϳ�������
 */
    class CDataFusion {
    public:
        /**
         * @brief �ں��㷨ö�٣�����PPTҪ��Ļ���+�����㷨��
         * @enum WEIGHTED����Ȩ��ͣ�������Ȩ���봫����������أ�
         * @enum KALMAN���������˲�������������ϵͳ���Ź��ƣ�
         * @enum UKF���޼��������˲������ף����������ϵͳ��Բ��/S���˶���
         * @enum PARTICLE�������˲������ף�����Ǹ�˹������
         */
        enum AlgType { WEIGHTED, KALMAN, UKF, PARTICLE };

        /**
         * @brief ���캯������ʼ���ں�Ĭ��״̬
         * @param ��
         * @return ��
         * @���ó������Ի�����CNavFuseDemoDlg��ʼ��ʱ����ʵ��
         * @���˼·��Ĭ���㷨Ϊ��Ȩ��ͣ�MSE��ʼΪ0����ʼ���������˲�ʵ��
         */
        CDataFusion();

        /**
         * @brief �����������ͷ��ں���Դ���޶�̬��Դ��
         * @param ��
         * @return ��
         */
        ~CDataFusion();

        /**
         * @brief �����ں��㷨���������ýӿڣ�
         * @param alg���㷨���ͣ�WEIGHTED/KALMAN/UKF/PARTICLE�����Խ�����Ͽ�
         * @return ��
         * @���ó������Ի��򡰿�ʼ����ť���ʱ����ȡ�����㷨ѡ������
         * @���˼·��ͳһ�㷨��ڣ�����FuseData�Զ�ƥ���Ӧ�߼�
         */


         /**
          * @brief ����GPS/INS�ľ�����MSE�������ڼ�Ȩ��͵�Ȩ��
          * @param trueX����ʵX����(m)������CMotionModel��
          * @param trueY����ʵY����(m)������CMotionModel��
          * @param gpsX��GPSģ��X����(m)������CGPS��
          * @param gpsY��GPSģ��Y����(m)������CGPS��
          * @param insX��INSģ��X����(m)������CINS��
          * @param insY��INSģ��Y����(m)������CINS��
          * @return ��
          * @���ó������Ի���OnTimer�У�FuseData֮ǰ����
          * @���˼·��MSE=(ģ��ֵ-��ʵֵ)?��X+Y�ϼƣ���MSEԽСȨ��Խ��
          */
        void CalcSensorMSE(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY);

        /**
         * @brief ִ�������ںϣ����ļ���ӿڣ����ö�Ӧ�㷨��
         * @param trueX����ʵX����(m)�����ڼ����ں�MSE��
         * @param trueY����ʵY����(m)�����ڼ����ں�MSE��
         * @param gpsX��GPSģ��X����(m)����������1��
         * @param gpsY��GPSģ��Y����(m)����������1��
         * @param insX��INSģ��X����(m)����������2��
         * @param insY��INSģ��Y����(m)����������2��
         * @param fuseX������ں�X����(m)������ʾ�ࣩ
         * @param fuseY������ں�Y����(m)������ʾ�ࣩ
         * @return ��
         * @���ó������Ի���OnTimer�У�CalcSensorMSE֮�����
         * @���˼·������m_alg���ö�Ӧ�㷨������ںϽ���������ں�MSE
         */
        void FuseData(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief ��ȡ�������ݵ�MSE����������ʾ�Աȣ�
         * @param gpsMSE�����GPS��MSE���������б�ؼ���
         * @param insMSE�����INS��MSE���������б�ؼ���
         * @param fuseMSE������ں����ݵ�MSE���������б�ؼ���
         * @return ��
         * @���ó������Ի���OnTimer�У�FuseData֮�����
         * @���˼·�������ڲ�MSE������ͨ���ӿڰ�ȫ��������ڽ���Ա��㷨Ч��
         */
         // �����ں��㷨
        void SetAlgorithm(AlgType alg);

        // ��ȡ��ǰ�ں��㷨
        AlgType GetAlgorithm() const;  // �����һ��



        void GetMSE(double& gpsMSE, double& insMSE, double& fuseMSE);
    private:
        AlgType m_alg;              ///< ��ǰ�ں��㷨����
        double m_gpsMSE;            ///< GPS�ľ�����MSE��
        double m_insMSE;            ///< INS�ľ�����MSE��
        double m_fuseMSE;           ///< �ں����ݵľ�����MSE��
        CKalmanFilter m_kalman;     ///< �������˲�ʵ������Ϲ�ϵ������KALMAN�㷨��

        /**
         * @brief ��Ȩ����㷨�������㷨��PPTҪ��
         * @param gpsX/gpsY��GPS����
         * @param insX/insY��INS����
         * @param fuseX/fuseY������ںϽ��
         * @return ��
         * @���ó�����FuseData�У�m_alg=WEIGHTEDʱ����
         * @���˼·��Ȩ��=1/MSE���ں�ֵ=GPS*w_gps + INS*w_ins
         */
        void WeightedFusion(double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief �������˲��㷨�������㷨��PPT��ʽ��أ�
         * @param trueX/trueY����ʵֵ�����ڼ����ں�MSE��
         * @param gpsX/gpsY��GPS���ݣ��۲�ֵ��
         * @param insX/insY��INS���ݣ�״̬Ԥ�����룩
         * @param fuseX/fuseY������ںϽ�������������Ƶ�λ�ã�
         * @return ��
         * @���ó�����FuseData�У�m_alg=KALMANʱ����
         * @���˼·����INS���ٶ�Ԥ�⣬GPS�۲���������ȡ״̬�����е�λ����Ϊ���
         */
        void KalmanFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief �޼��������˲��������㷨�����������ϵͳ��
         * @param trueX/trueY����ʵֵ�����ڼ����ں�MSE��
         * @param gpsX/gpsY��GPS���ݣ��۲�ֵ��
         * @param insX/insY��INS���ݣ�״̬Ԥ�����룩
         * @param fuseX/fuseY������ںϽ��
         * @return ��
         * @���ó�����FuseData�У�m_alg=UKFʱ����
         * @���˼·����UT�任���Ƹ�˹�ֲ������PPT���������˶���������
         */
        void UKFFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);

        /**
         * @brief �����˲��������㷨������Ǹ�˹������
         * @param trueX/trueY����ʵֵ�����ڼ����ں�MSE��
         * @param gpsX/gpsY��GPS���ݣ��۲�ֵ��
         * @param insX/insY��INS���ݣ�״̬Ԥ�����룩
         * @param fuseX/fuseY������ںϽ��
         * @return ��
         * @���ó�����FuseData�У�m_alg=PARTICLEʱ����
         * @���˼·�������Ӽ�����״̬�ֲ����ʺϷǸ�˹��������
         */
        void ParticleFusion(double trueX, double trueY, double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY);
};

#endif // DATAFUSION_H