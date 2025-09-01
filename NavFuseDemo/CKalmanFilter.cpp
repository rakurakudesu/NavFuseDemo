// CKalmanFilter.cpp ʵ�ֿ������˲����ĺ����㷨
#include "pch.h"
#include "CKalmanFilter.h"

CKalmanFilter::CKalmanFilter()
{
    // ��ʼ��״̬������Э�������
    m_x = Eigen::VectorXd(6);
    m_x.setZero();

    m_P = Eigen::MatrixXd(6, 6);
    m_P.setIdentity();
    m_P *= 10.0; // ��ʼЭ����
}

CKalmanFilter::~CKalmanFilter() {}

void CKalmanFilter::SetParam(Eigen::MatrixXd F,
    Eigen::MatrixXd H,
    Eigen::MatrixXd Q,
    Eigen::MatrixXd R)
{
    m_F = F;
    m_H = H;
    m_Q = Q;
    m_R = R;
}

void CKalmanFilter::Init(const Eigen::VectorXd& initialState, const Eigen::MatrixXd& initialP) {
    m_x = initialState;
    m_P = initialP;

}

void CKalmanFilter::Predict()
{
    // Ԥ��״̬: x = F * x
    m_x = m_F * m_x;

    // Ԥ��Э����: P = F * P * F^T + Q
    m_P = m_F * m_P * m_F.transpose() + m_Q;
}

void CKalmanFilter::Update(double zX, double zY)
{
    // ����۲�����
    Eigen::VectorXd z(2);
    z << zX, zY;

    // ���㿨��������: K = P * H^T * (H * P * H^T + R)^-1
    Eigen::MatrixXd Ht = m_H.transpose();
    Eigen::MatrixXd S = m_H * m_P * Ht + m_R;
    // ʹ��Cholesky�ֽ������ֵ�ȶ���
    Eigen::MatrixXd K = m_P * Ht * S.ldlt().solve(Eigen::MatrixXd::Identity(S.rows(), S.cols()));

    // ����״̬����
    Eigen::VectorXd y = z - m_H * m_x;
    m_x += K * y;

    // ����Э�������
    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(m_x.size(), m_x.size());
    m_P = (I - K * m_H) * m_P;
}

// ������ʹ��INS���ݽ���״̬���£�Ԥ��׶�������
void CKalmanFilter::UpdateINS(double insX, double insY, double dt)
{
    // ����INS���ݵ���״̬��������Ҫ����λ�ú��ٶȣ�
    Eigen::VectorXd insState(6);
    insState << insX, (insX - m_x[0]) / dt, 0,  // �򻯼��ٶȼ���
        insY, (insY - m_x[3]) / dt, 0;

    // �ں�INS���ݵ�Ԥ��״̬��������Ȩ�أ�
    m_x = 0.7 * m_x + 0.3 * insState;
}

Eigen::VectorXd CKalmanFilter::GetState() const
{
    return m_x;
}