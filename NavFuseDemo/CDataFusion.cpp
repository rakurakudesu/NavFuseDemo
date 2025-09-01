// CDataFusion.cpp ʵ�������ںϵĺ����㷨
#include "pch.h"
#include "CDataFusion.h"
#include <cmath>

CDataFusion::CDataFusion()
    : m_alg(WEIGHTED), m_gpsMSE(0), m_insMSE(0), m_fuseMSE(0)
{
    // ��ʼ���������˲�������
    Eigen::MatrixXd F(6, 6); // ״̬ת�ƾ���
    Eigen::MatrixXd H(2, 6); // �۲����
    Eigen::MatrixXd Q(6, 6); // ��������Э����
    Eigen::MatrixXd R(2, 2); // �۲�����Э����

    // ��ʼ��״̬ת�ƾ��� (6x6) - ����λ�á��ٶȡ����ٶ�ģ��
    F << 1, 0.01, 0.00005, 0, 0, 0,
        0, 1, 0.01, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0.01, 0.00005,
        0, 0, 0, 0, 1, 0.01,
        0, 0, 0, 0, 0, 1;

    // ��ʼ���۲���� (2x6) - ֻ�۲�λ��
    H << 1, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0;

    // ��ʼ����������Э���INS������
    Q = Eigen::MatrixXd::Identity(6, 6) * 0.01;

    // ��ʼ���۲�����Э���GPS������
    R = Eigen::MatrixXd::Identity(2, 2) * 1.0;

    m_kalman.SetParam(F, H, Q, R);
}

CDataFusion::~CDataFusion() {}

void CDataFusion::SetAlgorithm(AlgType alg)
{
    m_alg = alg;
}

void CDataFusion::CalcSensorMSE(double trueX, double trueY, 
    double gpsX, double gpsY, 
    double insX, double insY) 
{
    // ����GPS��MSE
    double gpsErrX = gpsX - trueX;
    double gpsErrY = gpsY - trueY;
    m_gpsMSE = (gpsErrX * gpsErrX + gpsErrY * gpsErrY) / 2.0;

    // ����INS��MSE
    double insErrX = insX - trueX;
    double insErrY = insY - trueY;
    m_insMSE = (insErrX * insErrX + insErrY * insErrY) / 2.0;
}

void CDataFusion::FuseData(double trueX, double trueY, 
    double gpsX, double gpsY, 
    double insX, double insY, 
    double& fuseX, double& fuseY)
{
    switch (m_alg) 
    {
    case KALMAN:
        KalmanFusion(gpsX, gpsY, insX, insY, 0.01, fuseX, fuseY);
        break;
    case WEIGHTED:
        WeightedFusion(gpsX, gpsY, insX, insY, fuseX, fuseY);
        break;
    default:
        WeightedFusion(gpsX, gpsY, insX, insY, fuseX, fuseY);
    }

    // �����ںϽ����MSE
    double fuseErrX = fuseX - trueX;
    double fuseErrY = fuseY - trueY;
    m_fuseMSE = (fuseErrX * fuseErrX + fuseErrY * fuseErrY) / 2.0;
}

void CDataFusion::GetMSE(double& gpsMSE, double& insMSE, double& fuseMSE) {
    gpsMSE = m_gpsMSE;
    insMSE = m_insMSE;
    fuseMSE = m_fuseMSE;
}

void CDataFusion::WeightedFusion(double gpsX, double gpsY, double insX, double insY, double& fuseX, double& fuseY) {
    // ��Ȩ�ں�ʵ��
    double wGps = 1.0 / (m_gpsMSE + 1e-6); // ��ֹ����
    double wIns = 1.0 / (m_insMSE + 1e-6);
    double total = wGps + wIns;

    fuseX = (wGps * gpsX + wIns * insX) / total;
    fuseY = (wGps * gpsY + wIns * insY) / total;
}

void CDataFusion::KalmanFusion(double gpsX, double gpsY,
    double insX, double insY,
    double dt,
    double& fuseX, double& fuseY)
{
    // 1. ��INS��Ƶ����������ǰ״̬��INSƵ�ʸ���GPS���ṩʵʱ�˶����ƣ�
        Eigen::VectorXd currentState = m_kalman.GetState();
    // ����INS�����λ�ø�ʵʱ����INS��x/y������ǰ״̬��λ��
    currentState[0] = insX;  // ����xλ��
    currentState[3] = insY;  // ����yλ��
    // ������ǰЭ���������״̬����
    m_kalman.Init(currentState, m_kalman.GetStateCovariance());

    // 2. ִ��Ԥ�⣨�����������״̬���˶�ģ�ͣ�
    m_kalman.Predict();

    // 3. ��GPS��Ƶ�߾������ݸ��£�У׼Ԥ��ƫ�
    m_kalman.Update(gpsX, gpsY);

    // 4. ����ںϽ����ȡ״̬�е�x��yλ�ã�
    Eigen::VectorXd fusedState = m_kalman.GetState();
    fuseX = fusedState[0];
    fuseY = fusedState[3];
}


CDataFusion::AlgType CDataFusion::GetAlgorithm() const {
    return m_alg;  // ���ص�ǰ�㷨����
}