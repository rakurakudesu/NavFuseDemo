// CKalmanFilter.cpp ʵ�ֿ������˲����ĺ����߼�
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
    // ��ʼ������6ά״̬��x, vx, ax, y, vy, ay��
    m_F = Eigen::MatrixXd::Identity(6, 6);
    m_H = Eigen::MatrixXd(2, 6);
    m_H << 1, 0, 0, 0, 0, 0,  // �۲�x
        0, 0, 0, 1, 0, 0; // �۲�y
    m_Q = Eigen::MatrixXd::Identity(6, 6) * 0.1;  // ��������
    m_R = Eigen::MatrixXd::Identity(2, 2) * 1.0;  // �۲�����
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
    // �����۲�����
    Eigen::VectorXd z(2);
    z << zX, zY;

    // ���㿨��������: K = P * H^T * (H * P * H^T + R)^-1
    Eigen::MatrixXd Ht = m_H.transpose();
    Eigen::MatrixXd S = m_H * m_P * Ht + m_R;
    Eigen::MatrixXd K = m_P * Ht * S.inverse();

    // ����״̬����
    Eigen::VectorXd y = z - m_H * m_x;
    m_x += K * y;

    // ����Э�������
    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(m_x.size(), m_x.size());
    m_P = (I - K * m_H) * m_P;
}

Eigen::VectorXd CKalmanFilter::GetState() const
 {
    return m_x;
}